#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
趋势图表组件
显示单点的变化趋势
"""

from PyQt5.QtWidgets import QWidget, QVBoxLayout, QLabel
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import numpy as np
from collections import deque

class TrendChart(QWidget):
    """趋势图表组件"""
    
    def __init__(self):
        super().__init__()
        self.init_ui()
        
        # 数据缓冲区（不限制长度）
        self.data_buffer = deque()
        self.time_buffer = deque()
        
    def init_ui(self):
        """Initialize UI"""
        layout = QVBoxLayout(self)
        
        title = QLabel("Single Point Trend")
        title.setStyleSheet("font-weight: bold;")
        layout.addWidget(title)
        
        # Matplotlib chart
        self.figure = Figure(figsize=(5, 3))
        self.canvas = FigureCanvas(self.figure)
        self.ax = self.figure.add_subplot(111)
        
        self.line, = self.ax.plot([], [], 'b-', linewidth=1.5)
        self.ax.set_xlabel('Time (Samples)', fontsize=10)
        self.ax.set_ylabel('Value', fontsize=10)
        self.ax.grid(True, alpha=0.3)
        self.ax.set_title('Click Matrix to Select Tracking Point', fontsize=10)
        
        self.figure.tight_layout()
        
        layout.addWidget(self.canvas)
        
    def add_point(self, value):
        """添加数据点"""
        if not self.data_buffer:
            # 初始化时间轴
            self.time_buffer.clear()
            self.data_buffer.clear()
            
        self.data_buffer.append(value)
        self.time_buffer.append(len(self.data_buffer) - 1)
        
        # 更新图表
        if len(self.data_buffer) > 0:
            self.line.set_data(list(self.time_buffer), list(self.data_buffer))
            self.ax.relim()
            self.ax.autoscale_view()
            self.canvas.draw()
            
    def update_data(self, data_list):
        """更新数据（从数据库加载）"""
        self.data_buffer.clear()
        self.time_buffer.clear()
        
        for i, value in enumerate(data_list):
            self.data_buffer.append(value)
            self.time_buffer.append(i)
            
        if len(self.data_buffer) > 0:
            self.line.set_data(list(self.time_buffer), list(self.data_buffer))
            self.ax.relim()
            self.ax.autoscale_view()
            self.canvas.draw()
            
    def clear(self):
        """清除数据"""
        self.data_buffer.clear()
        self.time_buffer.clear()
        self.line.set_data([], [])
        self.ax.relim()
        self.ax.autoscale_view()
        self.canvas.draw()

