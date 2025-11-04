#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
命令发送面板
支持所有USB命令的发送
"""

from PyQt5.QtWidgets import (QWidget, QVBoxLayout, QHBoxLayout, QPushButton, 
                             QLineEdit, QComboBox, QGroupBox, QGridLayout, QLabel, QSizePolicy, QSpacerItem)
from PyQt5.QtCore import pyqtSignal, Qt
import os
import json

class CommandPanel(QWidget):
    """命令发送面板"""
    
    command_sent = pyqtSignal(str)  # 发送命令信号
    
    def __init__(self):
        super().__init__()
        # 设置大小策略，允许随窗口缩放
        self.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        # 配置相关
        self.config_path = None
        self.config = {

        }
        # 预先尝试加载默认配置文件
        try:
            base_dir = os.path.dirname(os.path.dirname(__file__))
            default_cfg = os.path.join(base_dir, "command_config.json")
            if os.path.exists(default_cfg):
                self.load_config_file(default_cfg)
        except Exception:
            pass
        self.init_ui()
        
    def init_ui(self):
        """初始化UI"""
        layout = QVBoxLayout(self)
        layout.setContentsMargins(5, 5, 5, 5)
        layout.setSpacing(8)
        # 不设置固定大小，允许根据分割器大小调整
        
        # 快速命令按钮 - 紧凑布局（基于配置构建）
        self.quick_group = QGroupBox("快速命令")
        self.quick_layout = QGridLayout()
        self.quick_layout.setSpacing(5)
        self._rebuild_quick_buttons()
        self.quick_group.setLayout(self.quick_layout)
        layout.addWidget(self.quick_group)
        
        # 参数命令 - 紧凑布局
        param_group = QGroupBox("参数命令")
        param_layout = QVBoxLayout()
        param_layout.setSpacing(2)
        
        # 设置速率
        rate_layout = QHBoxLayout()
        rate_layout.addWidget(QLabel("速率(ms):"))
        self.rate_edit = QLineEdit(str(self.config.get("defaults", {}).get("rate_ms", 100)))
        self.rate_edit.setMaximumWidth(70)
        rate_layout.addWidget(self.rate_edit)
        rate_btn = QPushButton("SET_RATE")
        rate_btn.setMaximumWidth(85)
        rate_btn.clicked.connect(self.send_rate_command)
        rate_layout.addWidget(rate_btn)
        rate_layout.addStretch()
        param_layout.addLayout(rate_layout)
        
        # 设置行列（Y/X格式）- 同一行
        yx_layout = QHBoxLayout()
        yx_layout.addWidget(QLabel("Y:"))
        self.row_edit = QLineEdit(str(self.config.get("defaults", {}).get("row", 0)))
        self.row_edit.setMaximumWidth(45)
        yx_layout.addWidget(self.row_edit)
        row_btn = QPushButton("SET_Y")
        row_btn.setMaximumWidth(65)
        row_btn.clicked.connect(self.send_row_command)
        yx_layout.addWidget(row_btn)
        
        yx_layout.addSpacing(10)  # 间距
        
        yx_layout.addWidget(QLabel("X:"))
        self.col_edit = QLineEdit(str(self.config.get("defaults", {}).get("col", 0)))
        self.col_edit.setMaximumWidth(45)
        yx_layout.addWidget(self.col_edit)
        col_btn = QPushButton("SET_X")
        col_btn.setMaximumWidth(65)
        col_btn.clicked.connect(self.send_col_command)
        yx_layout.addWidget(col_btn)
        
        yx_layout.addStretch()
        param_layout.addLayout(yx_layout)
        
        # 扫描指定点
        point_layout = QHBoxLayout()
        point_layout.addWidget(QLabel("扫描点(Y:X):"))
        self.point_row_edit = QLineEdit(str(self.config.get("defaults", {}).get("point_row", 0)))
        self.point_row_edit.setMaximumWidth(35)
        point_layout.addWidget(self.point_row_edit)
        point_layout.addWidget(QLabel(":"))
        self.point_col_edit = QLineEdit(str(self.config.get("defaults", {}).get("point_col", 0)))
        self.point_col_edit.setMaximumWidth(35)
        point_layout.addWidget(self.point_col_edit)
        point_btn = QPushButton("SCAN")
        point_btn.setMaximumWidth(65)
        point_btn.clicked.connect(self.send_point_command)
        point_layout.addWidget(point_btn)
        point_layout.addStretch()
        param_layout.addLayout(point_layout)
        
        param_group.setLayout(param_layout)
        layout.addWidget(param_group)
        
        # 自定义命令
        custom_group = QGroupBox("自定义命令")
        custom_layout = QVBoxLayout()
        
        self.custom_edit = QLineEdit()
        self.custom_edit.setPlaceholderText("输入命令，如: SET_RATE:50")
        self.custom_edit.returnPressed.connect(self.send_custom_command)
        
        custom_btn = QPushButton("发送")
        custom_btn.clicked.connect(self.send_custom_command)
        
        custom_layout.addWidget(self.custom_edit)
        custom_layout.addWidget(custom_btn)
        custom_group.setLayout(custom_layout)
        layout.addWidget(custom_group)
        
        # 移除stretch，让组件可以根据分割器大小调整
        
    def load_config_file(self, path):
        """从JSON文件加载配置"""
        try:
            with open(path, "r", encoding="utf-8") as f:
                cfg = json.load(f)
            # 基本校验与合并
            quick = cfg.get("quick_commands")
            if isinstance(quick, list) and all(isinstance(item, dict) for item in quick):
                self.config["quick_commands"] = [
                    {"label": str(item.get("label", "未命名")), "command": str(item.get("command", ""))}
                    for item in quick
                ]
            defaults = cfg.get("defaults", {})
            if isinstance(defaults, dict):
                merged = dict(self.config.get("defaults", {}))
                merged.update({
                    "rate_ms": defaults.get("rate_ms", merged.get("rate_ms", 100)),
                    "row": defaults.get("row", merged.get("row", 0)),
                    "col": defaults.get("col", merged.get("col", 0)),
                    "point_row": defaults.get("point_row", merged.get("point_row", 0)),
                    "point_col": defaults.get("point_col", merged.get("point_col", 0)),
                })
                self.config["defaults"] = merged
            self.config_path = path
            return True
        except Exception:
            return False

    def _rebuild_quick_buttons(self):
        """根据配置重建快捷命令按钮"""
        try:
            # 清理现有按钮
            while self.quick_layout.count():
                item = self.quick_layout.takeAt(0)
                w = item.widget()
                if w is not None:
                    w.setParent(None)
            # 重新添加
            commands = self.config.get("quick_commands", [])
            for idx, item in enumerate(commands):
                label = item.get("label", "未命名")
                cmd = item.get("command", "")
                btn = QPushButton(label)
                btn.setMaximumHeight(28)
                btn.clicked.connect(lambda checked, c=cmd: self.send_quick_command(c))
                row = idx // 3
                col = idx % 3
                self.quick_layout.addWidget(btn, row, col)
        except Exception:
            pass

    def send_quick_command(self, command):
        """发送快速命令"""
        # 短暂禁用触发按钮，避免重复快速点击导致队列积压或卡顿
        try:
            sender = self.sender()
            if sender is not None:
                sender.setEnabled(False)
                from PyQt5.QtCore import QTimer
                QTimer.singleShot(150, lambda: sender.setEnabled(True))
        except Exception:
            pass
        self.command_sent.emit(command)
        
    def send_rate_command(self):
        """发送速率命令"""
        rate = self.rate_edit.text()
        if rate:
            self._disable_sender_briefly()
            self.command_sent.emit(f"SET_RATE:{rate}")
            
    def send_row_command(self):
        """发送行设置命令"""
        row = self.row_edit.text()
        if row:
            self._disable_sender_briefly()
            self.command_sent.emit(f"SET_ROW:{row}")
            
    def send_col_command(self):
        """发送列设置命令"""
        col = self.col_edit.text()
        if col:
            self._disable_sender_briefly()
            self.command_sent.emit(f"SET_COL:{col}")
            
    def send_point_command(self):
        """发送扫描点命令"""
        row = self.point_row_edit.text()
        col = self.point_col_edit.text()
        if row and col:
            self._disable_sender_briefly()
            self.command_sent.emit(f"SCAN_POINT:{row}:{col}")
            
    def send_custom_command(self):
        """发送自定义命令"""
        command = self.custom_edit.text().strip()
        if command:
            self._disable_sender_briefly()
            self.command_sent.emit(command)
            self.custom_edit.clear()

    def _disable_sender_briefly(self):
        """短暂禁用触发按钮，避免UI卡顿或误触"""
        try:
            sender = self.sender()
            if sender is not None:
                sender.setEnabled(False)
                from PyQt5.QtCore import QTimer
                QTimer.singleShot(150, lambda: sender.setEnabled(True))
        except Exception:
            pass

