#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
USB CDC串口通信模块
双线程双缓冲机制
"""

import serial
import serial.tools.list_ports
from PyQt5.QtCore import QThread, pyqtSignal, QMutex, QMutexLocker
from collections import deque
import time

class SerialCommunication(QThread):
    """串口通信类 - 使用双线程和双缓冲"""
    
    # 信号
    data_received = pyqtSignal(str)  # 接收到数据
    status_changed = pyqtSignal(str, str)  # 状态变化 (status, message)
    
    def __init__(self):
        super().__init__()
        self.serial_port = None
        self.is_running = False
        self.is_connected_flag = False
        
        # 双缓冲：接收缓冲区和处理缓冲区
        self.receive_buffer = deque(maxlen=5000)  # 接收缓冲区（线程安全）- 增大以处理高速数据
        self.process_buffer = deque(maxlen=5000)  # 处理缓冲区 - 增大以处理高速数据
        
        # 发送缓冲区
        self.send_buffer = deque(maxlen=100)
        
        # 互斥锁
        self.receive_lock = QMutex()
        self.send_lock = QMutex()
        
        # 接收线程标志
        self.receive_thread_running = False
        
        # 连接状态监控线程
        self.monitor_thread = None
        
    def get_available_ports(self):
        """获取可用串口列表（返回设备名列表）"""
        ports = serial.tools.list_ports.comports()
        return [port.device for port in ports]
    
    def get_available_ports_with_info(self):
        """获取可用串口列表（包含详细信息）"""
        ports = serial.tools.list_ports.comports()
        port_list = []
        for port in ports:
            # 格式：设备名 - 描述信息
            if port.description:
                display_name = f"{port.device} - {port.description}"
            else:
                display_name = port.device
            port_list.append((port.device, display_name))
        return port_list
        
    def connect(self, port, baudrate=115200):
        """连接串口"""
        try:
            self.serial_port = serial.Serial(
                port=port,
                baudrate=baudrate,
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                timeout=0,              # 非阻塞读取
                write_timeout=0.2,      # 写入超时，防止卡死
                inter_byte_timeout=0.02 # 字节间超时
            )
            
            self.is_connected_flag = True
            self.is_running = True
            
            # 启动主通信线程
            self.start()
            
            # 启动连接状态监控线程（检查间隔200ms，更频繁检测）
            self.monitor_thread = ConnectionMonitorThread(self.serial_port, check_interval=200)
            self.monitor_thread.connection_lost.connect(self.on_connection_lost)
            self.monitor_thread.start_monitoring()
            
            # 确保监控线程启动
            import time
            time.sleep(0.01)  # 短暂延迟确保线程启动
            
            # 获取端口描述信息（如果可用）
            try:
                ports = serial.tools.list_ports.comports()
                for p in ports:
                    if p.device == port:
                        if p.description:
                            port_display = f"{port} - {p.description}"
                        else:
                            port_display = port
                        break
                else:
                    port_display = port
            except:
                port_display = port
            
            self.status_changed.emit("connected", f"已连接到 {port_display}")
            return True
            
        except Exception as e:
            self.status_changed.emit("error", f"连接失败: {str(e)}")
            return False
            
    def disconnect(self):
        """断开串口"""
        self.is_running = False
        self.is_connected_flag = False
        
        # 停止监控线程
        if self.monitor_thread:
            self.monitor_thread.stop_monitoring()
            self.monitor_thread = None
        
        # 等待主线程结束（在关闭串口之前）
        self.wait(1000)  # 等待线程结束
        
        # 安全关闭串口，捕获可能的异常
        if self.serial_port:
            try:
                if self.serial_port.is_open:
                    self.serial_port.close()
            except Exception as e:
                # 忽略关闭时的错误（串口可能已经被关闭或占用）
                # 这些错误通常不影响程序运行
                pass
            finally:
                self.serial_port = None
        
        self.status_changed.emit("disconnected", "已断开连接")
        
    def is_connected(self):
        """检查是否已连接（主动检测，触发异常）"""
        if not self.is_connected_flag:
            return False
        if not self.serial_port:
            return False
        try:
            # 主动尝试读取操作来检测连接（USB拔掉时会抛出异常）
            # 使用reset_input_buffer()会触发底层IO操作
            self.serial_port.reset_input_buffer()
            return self.serial_port.is_open
        except (serial.SerialException, OSError, AttributeError, ValueError):
            # 串口已断开（读取操作抛出异常）
            self.is_connected_flag = False
            return False
        
    def send_command(self, command):
        """发送命令（线程安全）"""
        if not self.is_connected():
            return False
            
        with QMutexLocker(self.send_lock):
            # 添加换行符
            cmd = command.strip() + '\r\n'
            self.send_buffer.append(cmd)
            
        return True
        
    def run(self):
        """线程运行函数 - 处理接收和发送"""
        receive_thread = None
        
        try:
            # 启动接收线程（子线程）
            if self.serial_port:
                receive_thread = ReceiveThread(self.serial_port, self.receive_buffer, self.receive_lock)
                receive_thread.data_received.connect(self.on_receive_data)
                receive_thread.connection_lost.connect(lambda: self.on_connection_lost())
                receive_thread.start()
                
            # 主循环：处理发送和缓冲区交换
            # 注意：连接状态检测由专门的监控线程负责
            
            while self.is_running and self.is_connected():
                # 发送数据
                with QMutexLocker(self.send_lock):
                    # 发送队列过长时丢弃最旧命令，避免堆积
                    if len(self.send_buffer) > 90:
                        drop_to = 60
                        while len(self.send_buffer) > drop_to:
                            try:
                                self.send_buffer.popleft()
                            except IndexError:
                                break
                    # 批量发送，减少系统调用，控制每次批量大小
                    batch = []
                    total_len = 0
                    max_cmds = 32
                    max_bytes = 1024
                    while self.send_buffer and len(batch) < max_cmds and total_len < max_bytes:
                        cmd = self.send_buffer.popleft()
                        batch.append(cmd)
                        total_len += len(cmd)
                # 实际写入在锁外，避免阻塞其他入队
                if batch:
                    try:
                        if not self.serial_port or not self.serial_port.is_open:
                            raise serial.SerialException("Port not open")
                        payload = ("".join(batch)).encode('utf-8')
                        try:
                            self.serial_port.write(payload)
                        except serial.SerialTimeoutException:
                            # 写入超时：丢弃本批，避免阻塞
                            pass
                        # 控制台打印发送的每条命令
                        try:
                            for c in batch:
                                print(f">>> {c.strip()}")
                        except Exception:
                            pass
                    except (serial.SerialException, OSError, AttributeError, ValueError):
                        # 端口问题：让监控线程处理断开
                        pass
                            
                # 交换缓冲区（双缓冲）- 激进版：最大化处理速度
                with QMutexLocker(self.receive_lock):
                    # 一次性交换所有可用数据，减少锁竞争
                    while self.receive_buffer:
                        data = self.receive_buffer.popleft()
                        self.process_buffer.append(data)
                        
                # 批量处理接收到的数据（最大化吞吐量）
                batch_data = []
                # 处理所有可用数据，不限制批次大小
                while self.process_buffer:
                    data = self.process_buffer.popleft()
                    batch_data.append(data)
                    # 如果批次太大，分批发送（避免一次性发送太多导致Qt事件队列阻塞）
                    if len(batch_data) >= 500:
                        # 发送当前批次
                        for d in batch_data:
                            self.data_received.emit(d)
                        batch_data = []
                
                # 发送剩余数据
                if batch_data:
                    for data in batch_data:
                        self.data_received.emit(data)
                    
                # 根据数据量动态调整：有数据时不休眠，无数据时短暂休眠
                if not batch_data and len(self.receive_buffer) == 0:
                    self.msleep(1)  # 完全没有数据时才休眠1ms
                # 否则不休眠，立即继续处理
                
        except (serial.SerialException, OSError) as e:
            # 串口异常断开
            self.is_running = False
            self.is_connected_flag = False
            self.status_changed.emit("disconnected", "串口已断开，请重新连接")
        except Exception as e:
            self.status_changed.emit("error", f"通信错误: {str(e)}")
        finally:
            if receive_thread:
                receive_thread.stop()
                receive_thread.wait(1000)
                
    def on_receive_data(self, data):
        """接收数据回调（在主线程中）"""
        pass  # 数据已通过信号发送
    
    def on_connection_lost(self):
        """连接丢失回调（在主线程中）"""
        # 串口意外断开，更新状态
        if self.is_connected_flag:  # 只在确实连接时才处理
            self.is_running = False
            self.is_connected_flag = False
            # 停止监控线程
            if self.monitor_thread:
                try:
                    self.monitor_thread.stop_monitoring()
                except:
                    pass
            self.status_changed.emit("disconnected", "串口已断开，请重新连接")
        
        
class ReceiveThread(QThread):
    """接收线程（子线程）"""
    
    data_received = pyqtSignal(str)
    connection_lost = pyqtSignal()  # 连接丢失信号
    
    def __init__(self, serial_port, buffer, lock):
        super().__init__()
        self.serial_port = serial_port
        self.buffer = buffer
        self.lock = lock
        self.running = True
        
    def run(self):
        """接收线程运行"""
        line_buffer = ""
        
        while self.running and self.serial_port:
            waiting = 0
            try:
                # 读取数据
                # 注意：连接状态检测由专门的监控线程负责
                try:
                    # 检查串口是否打开（基本检查）
                    if not self.serial_port or not self.serial_port.is_open:
                        if self.running:
                            self.running = False
                            self.connection_lost.emit()
                        break
                    
                    # 读取数据
                    try:
                        waiting = self.serial_port.in_waiting
                    except (serial.SerialException, OSError, AttributeError, ValueError):
                        # 访问in_waiting时出错，串口可能已断开（监控线程会处理）
                        break
                    
                    if waiting > 0:
                        # 读取数据 - 一次性读取所有可用数据
                        try:
                            data = self.serial_port.read(waiting)
                        except serial.SerialTimeoutException:
                            data = b""
                        line_buffer += data.decode('utf-8', errors='ignore')
                        
                        # 批量处理多行数据，提高效率
                        # 注意：确保完整行才发送，避免半行数据
                        lines_to_send = []
                        while '\n' in line_buffer:
                            line, line_buffer = line_buffer.split('\n', 1)
                            line = line.strip()
                            if line:
                                lines_to_send.append(line)
                        
                        # 批量写入接收缓冲区（线程安全）
                        if lines_to_send:
                            with QMutexLocker(self.lock):
                                # 如果缓冲区接近满，先清理旧数据
                                if len(self.buffer) > 4500:
                                    # 缓冲区快满时，清理到3000
                                    while len(self.buffer) > 3000:
                                        self.buffer.popleft()
                                self.buffer.extend(lines_to_send)
                            # 批量发送信号（减少Qt信号开销）- 每行单独发送
                            for line in lines_to_send:
                                # 控制台打印接收内容（不阻塞）
                                try:
                                    print(f"{line}")
                                except Exception:
                                    pass
                                self.data_received.emit(line)
                        
                        # 如果有数据，继续读取（不休眠）
                        continue  # 立即进入下一轮循环，继续读取
                    
                except (serial.SerialException, OSError, AttributeError, ValueError):
                    # 读取时发生错误，串口可能已断开（监控线程会处理）
                    break
                            
            except (serial.SerialException, OSError, AttributeError, ValueError) as e:
                # 串口断开错误，通过信号通知主线程
                if self.running:
                    self.running = False
                    self.connection_lost.emit()  # 发送连接丢失信号
                break
            except Exception as e:
                if self.running:
                    # 如果是串口关闭相关的错误，不打印（这是正常的）
                    error_str = str(e)
                    if "ClearCommError" not in error_str and "设备不识别此命令" not in error_str:
                        print(f"接收错误: {e}")
                    self.msleep(1)  # 异常时短暂休眠
                    
            # 只有在没有数据时才休眠（减少延迟）
            if waiting == 0:
                self.msleep(0)  # 即使没有数据也尽量不休眠，但给其他线程让出CPU
            
    def stop(self):
        """停止接收线程"""
        self.running = False


class ConnectionMonitorThread(QThread):
    """串口连接状态监控线程"""
    
    connection_lost = pyqtSignal()  # 连接丢失信号
    
    def __init__(self, serial_port, check_interval=500):
        """
        初始化监控线程
        :param serial_port: 串口对象
        :param check_interval: 检查间隔（毫秒）
        """
        super().__init__()
        self.serial_port = serial_port
        self.check_interval = check_interval
        self.running = False
        
    def start_monitoring(self):
        """开始监控"""
        self.running = True
        if not self.isRunning():
            self.start()
    
    def stop_monitoring(self):
        """停止监控"""
        self.running = False
        self.wait(1000)  # 等待线程结束
    
    def run(self):
        """监控线程运行"""
        while self.running:
            if not self.serial_port:
                # 串口对象不存在，停止监控
                self.running = False
                break
            
            try:
                # 检查串口是否打开
                port_open = False
                try:
                    port_open = self.serial_port.is_open
                except (serial.SerialException, OSError, AttributeError, ValueError):
                    # 访问is_open时出错，串口已断开
                    if self.running:
                        self.running = False
                        self.connection_lost.emit()
                    break
                
                if not port_open:
                    # 串口已关闭
                    if self.running:
                        self.running = False
                        self.connection_lost.emit()
                    break
                
                # 主动尝试多种IO操作来检测连接（USB拔掉时会抛出异常）
                try:
                    # 方法1: 尝试重置输入缓冲区
                    self.serial_port.reset_input_buffer()
                    
                    # 方法2: 尝试获取等待的字节数（会访问底层驱动）
                    _ = self.serial_port.in_waiting
                    
                    # 方法3: 尝试访问串口属性
                    _ = self.serial_port.port
                    
                except (serial.SerialException, OSError, AttributeError, ValueError) as e:
                    # IO操作失败，串口已断开
                    if self.running:
                        self.running = False
                        self.connection_lost.emit()
                    break
                    
            except (serial.SerialException, OSError, AttributeError, ValueError):
                # 访问串口属性时出错，串口已断开
                if self.running:
                    self.running = False
                    self.connection_lost.emit()
                break
            except Exception as e:
                # 其他异常，记录但继续监控
                # print(f"监控线程异常: {e}")
                pass
            
            # 休眠指定时间后再次检查
            self.msleep(self.check_interval)

