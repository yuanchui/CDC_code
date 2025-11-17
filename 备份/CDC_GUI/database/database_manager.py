#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
数据库管理模块
非阻塞线程存储
"""

import sqlite3
import threading
from datetime import datetime
from queue import Queue
from PyQt5.QtCore import QThread, pyqtSignal
import os
import sys
import time

class DatabaseManager(QThread):
    """数据库管理器 - 非阻塞线程"""
    
    def __init__(self, db_path="pcap04_data.db"):
        super().__init__()
        # 确保数据库生成在当前工作目录（EXE 所在目录或启动目录）
        self.db_path = self._resolve_db_path(db_path)
        self.data_queue = Queue(maxsize=10000)  # 数据队列 - 增大以处理高速数据
        self.running = False
        self.db_lock = threading.Lock()  # 数据库操作锁
        
        # 初始化数据库
        self.init_database()
        
        # 启动存储线程
        self.start()
        
    def _resolve_db_path(self, db_path):
        """将数据库路径解析到当前工作目录（相对路径情况下）。"""
        # 默认文件名
        name = db_path or "pcap04_data.db"
        # 绝对路径则直接返回
        if os.path.isabs(name):
            return name
        # 相对路径：拼到当前工作目录
        cwd = os.getcwd()
        return os.path.join(cwd, name)

    def _get_connection(self, timeout=5.0, max_retries=3):
        """获取数据库连接（带超时和重试机制）"""
        for attempt in range(max_retries):
            try:
                conn = sqlite3.connect(self.db_path, timeout=timeout)
                # 设置同步模式为NORMAL（提高性能）
                conn.execute('PRAGMA synchronous=NORMAL')
                # 设置缓存大小（16MB）
                conn.execute('PRAGMA cache_size=-16384')
                # 设置繁忙超时（避免锁定）
                conn.execute('PRAGMA busy_timeout=5000')
                return conn
            except sqlite3.OperationalError as e:
                if "locked" in str(e).lower() and attempt < max_retries - 1:
                    # 如果是锁定错误且还有重试机会，等待后重试
                    time.sleep(0.05 * (attempt + 1))  # 递增等待时间
                    continue
                raise
        raise sqlite3.OperationalError("Database connection failed after retries")
    
    def init_database(self):
        """初始化数据库"""
        # 先尝试启用WAL模式（如果数据库文件已存在且未被锁定）
        self._enable_wal_mode()
        
        conn = self._get_connection()
        cursor = conn.cursor()
        
        # 创建表：存储每个点的历史数据
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS matrix_data (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                timestamp DATETIME NOT NULL,
                row INTEGER NOT NULL,
                col INTEGER NOT NULL,
                value REAL NOT NULL,
                raw_value REAL,
                frame_id INTEGER
            )
        ''')
        
        # 创建索引（单独执行）
        cursor.execute('''
            CREATE INDEX IF NOT EXISTS idx_timestamp ON matrix_data(timestamp)
        ''')
        
        cursor.execute('''
            CREATE INDEX IF NOT EXISTS idx_position ON matrix_data(row, col, timestamp)
        ''')
        # 迁移：如果缺少frame_id列则添加
        try:
            cursor.execute("PRAGMA table_info(matrix_data)")
            cols = [r[1] for r in cursor.fetchall()]
            if 'frame_id' not in cols:
                cursor.execute('ALTER TABLE matrix_data ADD COLUMN frame_id INTEGER')
        except Exception:
            pass
        
        conn.commit()
        conn.close()
    
    def _enable_wal_mode(self):
        """尝试启用WAL模式（如果数据库文件存在且未被锁定）"""
        if not os.path.exists(self.db_path):
            # 数据库文件不存在，创建时自动启用WAL
            try:
                conn = sqlite3.connect(self.db_path, timeout=5.0)
                conn.execute('PRAGMA journal_mode=WAL')
                conn.commit()
                conn.close()
            except Exception:
                pass  # 如果失败，继续使用默认模式
        else:
            # 数据库文件已存在，尝试启用WAL模式
            try:
                conn = sqlite3.connect(self.db_path, timeout=5.0)
                # 检查当前模式
                cursor = conn.cursor()
                cursor.execute('PRAGMA journal_mode')
                current_mode = cursor.fetchone()[0].upper()
                if current_mode != 'WAL':
                    # 尝试切换到WAL模式
                    cursor.execute('PRAGMA journal_mode=WAL')
                    conn.commit()
                conn.close()
            except sqlite3.OperationalError:
                # 如果数据库被锁定，跳过WAL模式设置，继续使用默认模式
                pass
            except Exception:
                pass  # 其他错误也跳过
        
    def add_data_point(self, row, col, value, raw_value=None, non_blocking=True):
        """添加数据点"""
        timestamp = datetime.now()
        
        if non_blocking:
            # 非阻塞：加入队列
            try:
                self.data_queue.put_nowait((timestamp, row, col, value, raw_value))
            except:
                pass  # 队列满时丢弃
        else:
            # 阻塞：直接写入
            self._write_to_db(timestamp, row, col, value, raw_value)
            
    def _write_to_db(self, timestamp, row, col, value, raw_value, frame_id=None):
        """写入数据库（内部方法）- 单条写入"""
        try:
            conn = self._get_connection()
            cursor = conn.cursor()
            
            if frame_id is None:
                cursor.execute('''
                    INSERT INTO matrix_data (timestamp, row, col, value, raw_value)
                    VALUES (?, ?, ?, ?, ?)
                ''', (timestamp, row, col, value, raw_value))
            else:
                cursor.execute('''
                    INSERT INTO matrix_data (timestamp, row, col, value, raw_value, frame_id)
                    VALUES (?, ?, ?, ?, ?, ?)
                ''', (timestamp, row, col, value, raw_value, frame_id))
            
            conn.commit()
            conn.close()
        except Exception as e:
            print(f"数据库写入错误: {e}")
    
    def _write_batch_to_db(self, data_batch):
        """批量写入数据库（提高性能）"""
        if not data_batch:
            return
        try:
            conn = self._get_connection()
            cursor = conn.cursor()
            
            # 根据数据元组长度选择SQL（兼容带frame_id与不带）
            if len(data_batch[0]) == 6:
                cursor.executemany('''
                    INSERT INTO matrix_data (timestamp, row, col, value, raw_value, frame_id)
                    VALUES (?, ?, ?, ?, ?, ?)
                ''', data_batch)
            else:
                cursor.executemany('''
                    INSERT INTO matrix_data (timestamp, row, col, value, raw_value)
                    VALUES (?, ?, ?, ?, ?)
                ''', data_batch)
            
            conn.commit()
            conn.close()
        except sqlite3.OperationalError as e:
            if "locked" in str(e).lower():
                # 如果是锁定错误，将数据重新放回队列等待下次处理
                print(f"数据库批量写入错误: database is locked, 将重试 {len(data_batch)} 条数据")
                # 将数据重新放回队列（非阻塞）
                for item in data_batch:
                    try:
                        self.data_queue.put_nowait(item)
                    except:
                        pass  # 队列满时丢弃
            else:
                print(f"数据库批量写入错误: {e}")
        except Exception as e:
            print(f"数据库批量写入错误: {e}")
            
    def get_point_history(self, row, col, limit=100):
        """获取点的历史数据"""
        try:
            conn = self._get_connection()
            cursor = conn.cursor()
            
            if limit is None:
                cursor.execute('''
                    SELECT value FROM matrix_data
                    WHERE row = ? AND col = ?
                    ORDER BY timestamp ASC
                ''', (row, col))
            else:
                cursor.execute('''
                    SELECT value FROM matrix_data
                    WHERE row = ? AND col = ?
                    ORDER BY timestamp DESC
                    LIMIT ?
                ''', (row, col, limit))
            
            results = cursor.fetchall()
            conn.close()
            
            # 当limit为None时已按ASC；否则需要反转为旧->新
            if limit is None:
                values = [r[0] for r in results]
            else:
                values = [r[0] for r in reversed(results)]
            return values
        except sqlite3.OperationalError as e:
            if "locked" in str(e).lower():
                # 如果是锁定错误，等待后重试一次
                time.sleep(0.05)
                try:
                    conn = self._get_connection()
                    cursor = conn.cursor()
                    if limit is None:
                        cursor.execute('''
                            SELECT value FROM matrix_data
                            WHERE row = ? AND col = ?
                            ORDER BY timestamp ASC
                        ''', (row, col))
                    else:
                        cursor.execute('''
                            SELECT value FROM matrix_data
                            WHERE row = ? AND col = ?
                            ORDER BY timestamp DESC
                            LIMIT ?
                        ''', (row, col, limit))
                    results = cursor.fetchall()
                    conn.close()
                    if limit is None:
                        values = [r[0] for r in results]
                    else:
                        values = [r[0] for r in reversed(results)]
                    return values
                except Exception:
                    print(f"数据库读取错误（重试失败）: {e}")
                    return []
            else:
                print(f"数据库读取错误: {e}")
                return []
        except Exception as e:
            print(f"数据库读取错误: {e}")
            return []

    def add_frame_points(self, points):
        """批量加入一个帧的点到队列（非阻塞）
        points: 列表[(timestamp,row,col,value,raw_value,frame_id), ...]
        """
        for item in points:
            try:
                self.data_queue.put_nowait(item)
            except Exception:
                pass
            
    def run(self):
        """线程运行函数（优化版本：批量写入）"""
        self.running = True
        batch = []
        batch_size = 50  # 每批写入50条数据
        batch_timeout = 0.5  # 0.5秒超时，强制写入
        
        last_write_time = time.time()
        
        while self.running:
            try:
                # 从队列取数据（超时0.1秒，快速响应）
                try:
                    item = self.data_queue.get(timeout=0.1)
                    # item 可能包含frame_id
                    if len(item) == 6:
                        timestamp, row, col, value, raw_value, frame_id = item
                        batch.append((timestamp, row, col, value, raw_value, frame_id))
                    else:
                        timestamp, row, col, value, raw_value = item
                        batch.append((timestamp, row, col, value, raw_value))
                    self.data_queue.task_done()
                except:
                    # 超时或队列为空
                    pass
                
                # 批量写入：达到批次大小或超时
                current_time = time.time()
                if len(batch) >= batch_size or (batch and (current_time - last_write_time) >= batch_timeout):
                    if batch:
                        self._write_batch_to_db(batch)
                        batch = []
                        last_write_time = current_time
                    
            except Exception as e:
                print(f"数据库线程错误: {e}")
                continue
        
        # 退出时写入剩余数据
        if batch:
            self._write_batch_to_db(batch)
                
    def close(self):
        """关闭数据库"""
        self.running = False
        
        # 等待队列数据处理完成
        while not self.data_queue.empty():
            try:
                item = self.data_queue.get_nowait()
                timestamp, row, col, value, raw_value = item
                self._write_to_db(timestamp, row, col, value, raw_value)
            except:
                break
                
        self.wait(2000)  # 等待线程结束

