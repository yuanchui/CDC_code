#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
消息日志显示组件
显示收发消息内容，支持命令响应追踪
"""

from PyQt5.QtWidgets import (QTextEdit, QVBoxLayout, QWidget, QLabel, QPushButton, 
                             QHBoxLayout, QSizePolicy, QDialog)
from PyQt5.QtCore import Qt, pyqtSignal
from PyQt5.QtGui import QColor, QTextCharFormat, QTextCursor, QFont
from datetime import datetime

class MessageLog(QWidget):
    """消息日志组件"""
    
    def __init__(self):
        super().__init__()
        # 设置大小策略，允许随窗口缩放
        self.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.init_ui()
        self.last_command = None  # 追踪最后发送的命令
        
    def init_ui(self):
        """初始化UI"""
        layout = QVBoxLayout(self)
        
        # 标题和按钮 - Mac风格
        header_layout = QHBoxLayout()
        title = QLabel("消息日志")
        title.setStyleSheet("""
            QLabel {
                font-size: 12pt;
                font-weight: 600;
                color: #333333;
            }
        """)
        clear_btn = QPushButton("清除")
        clear_btn.clicked.connect(self.clear)
        clear_btn.setMaximumWidth(60)
        
        header_layout.addWidget(title)
        header_layout.addStretch()
        
        # 弹出窗口按钮
        self.popout_btn = QPushButton("弹出")
        self.popout_btn.setMaximumWidth(50)
        self.popout_btn.clicked.connect(self.on_popout_clicked)
        header_layout.addWidget(self.popout_btn)
        
        header_layout.addWidget(clear_btn)
        layout.addLayout(header_layout)
        
        # 独立的窗口实例（初始为None）
        self.popout_window = None
        
        # 文本显示区域 - 可自适应高度
        self.text_edit = QTextEdit()
        self.text_edit.setReadOnly(True)
        # 设置大小策略，允许随窗口缩放
        self.text_edit.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        # 设置等宽字体
        font = QFont("Consolas", 8)
        if not font.exactMatch():
            font = QFont("Courier New", 8)
        self.text_edit.setFont(font)
        layout.addWidget(self.text_edit, 1)  # 设置拉伸因子
        
        # 消息类型颜色
        self.colors = {
            "sent": QColor(0, 100, 200),      # 深蓝色 - 发送命令
            "received": QColor(0, 150, 0),    # 深绿色 - 接收响应
            "info": QColor(0, 0, 0),          # 黑色 - 信息
            "warning": QColor(255, 140, 0),   # 橙色 - 警告
            "error": QColor(220, 0, 0),       # 红色 - 错误
            "system": QColor(100, 100, 100),  # 灰色 - 系统
            "ok": QColor(0, 150, 0),          # 绿色 - OK响应
            "matrix": QColor(100, 50, 200),   # 紫色 - 矩阵数据
            "status": QColor(50, 100, 200)   # 蓝色 - 状态信息
        }
        
    def add_command_sent(self, command):
        """添加发送的命令"""
        self.last_command = command
        timestamp = datetime.now().strftime("%H:%M:%S.%f")[:-3]  # 毫秒精度
        
        # 格式化命令显示
        formatted_cmd = f"➤ 发送命令: {command}"
        
        cursor = self.text_edit.textCursor()
        cursor.movePosition(QTextCursor.End)
        
        # 时间戳（灰色）
        fmt = QTextCharFormat()
        fmt.setForeground(QColor(128, 128, 128))
        fmt.setFontWeight(QFont.Normal)
        cursor.setCharFormat(fmt)
        cursor.insertText(f"[{timestamp}] ")
        
        # 命令内容（蓝色，加粗）
        fmt = QTextCharFormat()
        fmt.setForeground(self.colors["sent"])
        fmt.setFontWeight(QFont.Bold)
        cursor.setCharFormat(fmt)
        cursor.insertText(f"{formatted_cmd}\n")
        
        # 分隔线
        fmt = QTextCharFormat()
        fmt.setForeground(QColor(200, 200, 200))
        cursor.setCharFormat(fmt)
        cursor.insertText("─" * 60 + "\n")
        
        self.scroll_to_bottom()
        
        # 同步到弹出窗口（如果存在）
        self._sync_to_popout()
        
    def add_response(self, response, response_type="received"):
        """添加接收到的响应"""
        timestamp = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        
        # 解析响应类型
        parsed_response, response_category = self.parse_response(response)
        
        cursor = self.text_edit.textCursor()
        cursor.movePosition(QTextCursor.End)
        
        # 时间戳
        fmt = QTextCharFormat()
        fmt.setForeground(QColor(128, 128, 128))
        cursor.setCharFormat(fmt)
        cursor.insertText(f"[{timestamp}] ")
        
        # 如果上一个命令存在，显示关联
        if self.last_command:
            fmt = QTextCharFormat()
            fmt.setForeground(QColor(150, 150, 150))
            fmt.setFontItalic(True)
            cursor.setCharFormat(fmt)
            cursor.insertText(f"↳ 响应命令: {self.last_command}\n")
            cursor.insertText("   ")
            
        # 响应内容（根据类型选择颜色）
        color = self.colors.get(response_category, self.colors["received"])
        fmt = QTextCharFormat()
        fmt.setForeground(color)
        
        if response_category in ["ok", "error", "warning"]:
            fmt.setFontWeight(QFont.Bold)
        
        cursor.setCharFormat(fmt)
        cursor.insertText(f"{parsed_response}\n")
        cursor.insertText("\n")
        
        self.scroll_to_bottom()
        
        # 同步到弹出窗口（如果存在）
        self._sync_to_popout()
        
    def parse_response(self, response):
        """解析响应，识别类型和格式化"""
        response_clean = response.strip()
        
        # OK响应
        if response_clean.startswith("OK:"):
            return f"✓ {response_clean}", "ok"
            
        # ERROR响应
        if response_clean.startswith("ERROR:") or "ERROR" in response_clean.upper():
            return f"✗ {response_clean}", "error"
            
        # 状态信息
        if "Status:" in response_clean or "status" in response_clean.lower():
            return self.format_status_response(response_clean), "status"
            
        # 矩阵信息
        if "Matrix Information:" in response_clean or "Capacitance Matrix" in response_clean:
            return self.format_matrix_response(response_clean), "matrix"
            
        # HELP信息
        if "Available Commands:" in response_clean or "帮助" in response_clean:
            return "📖 帮助信息 (已收到)", "info"
            
        # 矩阵数据行
        if response_clean.startswith("Row") and any(c.isdigit() for c in response_clean):
            return f"📊 {response_clean[:80]}..." if len(response_clean) > 80 else f"📊 {response_clean}", "matrix"
            
        # 警告
        if "warning" in response_clean.lower() or "WARNING" in response_clean:
            return f"⚠ {response_clean}", "warning"
            
        # 默认响应
        return response_clean, "received"
        
    def format_status_response(self, response):
        """格式化状态响应"""
        lines = response.split('\n')
        formatted = "📋 状态响应:\n"
        for line in lines:
            if line.strip():
                formatted += f"   {line.strip()}\n"
        return formatted.strip()
        
    def format_matrix_response(self, response):
        """格式化矩阵响应"""
        if "Capacitance Matrix" in response:
            # 提取矩阵标题行
            lines = response.split('\n')
            title_line = ""
            for line in lines:
                if "Capacitance Matrix" in line:
                    title_line = line.strip()
                    break
            
            if title_line:
                return f"📊 {title_line}"
        
        return f"矩阵数据 (16x16)"
        
    def add_message(self, source, message, msg_type="info"):
        """添加普通消息（兼容旧接口）"""
        timestamp = datetime.now().strftime("%H:%M:%S")
        prefix = f"[{timestamp}] [{source}]"
        
        color = self.colors.get(msg_type, QColor(0, 0, 0))
        
        cursor = self.text_edit.textCursor()
        cursor.movePosition(QTextCursor.End)
        
        # 时间戳
        fmt = QTextCharFormat()
        fmt.setForeground(QColor(128, 128, 128))
        cursor.setCharFormat(fmt)
        cursor.insertText(f"[{timestamp}] ")
        
        # 消息内容
        fmt = QTextCharFormat()
        fmt.setForeground(color)
        if msg_type in ["error", "warning"]:
            fmt.setFontWeight(QFont.Bold)
        cursor.setCharFormat(fmt)
        cursor.insertText(f"[{source}]: {message}\n")
        
        self.scroll_to_bottom()
        
        # 同步到弹出窗口（如果存在）
        self._sync_to_popout()
        
    def scroll_to_bottom(self):
        """滚动到底部"""
        self.text_edit.moveCursor(QTextCursor.End)
        
        # 限制最大行数（防止内存占用过大）
        document = self.text_edit.document()
        if document.blockCount() > 1000:
            cursor = self.text_edit.textCursor()
            cursor.movePosition(QTextCursor.Start)
            cursor.movePosition(QTextCursor.Down, QTextCursor.MoveAnchor, 100)
            cursor.movePosition(QTextCursor.Start, QTextCursor.KeepAnchor)
            cursor.removeSelectedText()
    
    def _sync_to_popout(self):
        """同步内容到弹出窗口（内部方法）"""
        if self.popout_window and self.popout_window.isVisible():
            # 同步完整内容
            content = self.text_edit.toPlainText()
            self.popout_window.log_widget.text_edit.setPlainText(content)
            # 同步最后一条命令
            self.popout_window.log_widget.last_command = self.last_command
            # 同步滚动位置
            popout_cursor = self.popout_window.log_widget.text_edit.textCursor()
            popout_cursor.movePosition(QTextCursor.End)
            self.popout_window.log_widget.text_edit.setTextCursor(popout_cursor)
            
    def clear(self):
        """清除日志"""
        self.text_edit.clear()
        self.last_command = None
        # 如果弹出窗口存在，也清除
        if self.popout_window:
            self.popout_window.log_widget.clear()
    
    def on_popout_clicked(self):
        """弹出/关闭独立窗口"""
        if self.popout_window is None or not self.popout_window.isVisible():
            # 创建并显示独立窗口，传入self作为父日志组件
            self.popout_window = MessageLogWindow(self)
            self.popout_window.show()
            self.popout_btn.setText("关闭窗口")
            # 同步当前内容到独立窗口
            self.sync_content_to_popout()
            # 隐藏主界面中的消息日志组件
            self.setVisible(False)
        else:
            # 关闭独立窗口
            if self.popout_window:
                self.popout_window.close()
                self.popout_window = None
            self.popout_btn.setText("弹出")
            # 显示主界面中的消息日志组件
            self.setVisible(True)
    
    def sync_content_to_popout(self):
        """同步内容到弹出窗口"""
        if self.popout_window:
            # 获取当前内容
            content = self.text_edit.toPlainText()
            # 设置到弹出窗口
            self.popout_window.log_widget.text_edit.setPlainText(content)
            # 同步最后一条命令
            self.popout_window.log_widget.last_command = self.last_command
    
    def ensure_popout_sync(self):
        """确保弹出窗口同步（如果存在）"""
        if self.popout_window and self.popout_window.isVisible():
            # 同步内容
            content = self.text_edit.toPlainText()
            self.popout_window.log_widget.text_edit.setPlainText(content)
            # 同步滚动位置
            cursor = self.text_edit.textCursor()
            popout_cursor = self.popout_window.log_widget.text_edit.textCursor()
            popout_cursor.setPosition(cursor.position())
            self.popout_window.log_widget.text_edit.setTextCursor(popout_cursor)


class MessageLogWindow(QDialog):
    """消息日志独立窗口"""
    
    def __init__(self, parent_log_widget=None):
        """
        初始化独立窗口
        :param parent_log_widget: 主窗口中的MessageLog组件实例
        """
        super().__init__(None)  # 设置为无父窗口，使其独立
        self.parent_log_widget = parent_log_widget  # 保存主窗口的日志组件引用
        self.setWindowTitle("消息日志 - PCap04 监控系统")
        self.setMinimumSize(600, 400)
        self.resize(800, 500)
        
        layout = QVBoxLayout(self)
        layout.setContentsMargins(5, 5, 5, 5)
        
        # 创建消息日志组件（用于显示）
        self.log_widget = MessageLog()
        # 隐藏弹出窗口按钮（避免重复）
        if hasattr(self.log_widget, 'popout_btn'):
            self.log_widget.popout_btn.setVisible(False)
        layout.addWidget(self.log_widget)
        
        # 应用Mac风格样式
        self.setStyleSheet("""
            QDialog {
                background-color: white;
            }
            QPushButton {
                background-color: #007AFF;
                color: white;
                border: none;
                border-radius: 6px;
                padding: 4px 12px;
                font-size: 10pt;
                min-height: 22px;
                max-height: 28px;
            }
            QPushButton:hover {
                background-color: #0051D5;
            }
        """)
    
    def closeEvent(self, event):
        """窗口关闭事件"""
        # 通知父组件窗口已关闭
        if self.parent_log_widget:
            if self.parent_log_widget.popout_window == self:
                self.parent_log_widget.popout_window = None
                self.parent_log_widget.popout_btn.setText("弹出")
                # 显示主界面中的消息日志组件
                self.parent_log_widget.setVisible(True)
        super().closeEvent(event)

