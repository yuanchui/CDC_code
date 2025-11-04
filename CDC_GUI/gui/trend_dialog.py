#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
趋势曲线对话框
点击矩阵单元格时弹出的趋势图窗口
"""

from PyQt5.QtWidgets import (QDialog, QVBoxLayout, QHBoxLayout, QLabel, 
                             QPushButton, QGroupBox, QSizePolicy)
from PyQt5.QtCore import Qt, pyqtSignal
from PyQt5.QtGui import QFont
from gui.trend_chart import TrendChart
from database.database_manager import DatabaseManager

class TrendDialog(QDialog):
    """趋势曲线对话框"""
    
    def __init__(self, parent=None, row=0, col=0, db_manager=None):
        super().__init__(parent)
        self.row = row
        self.col = col
        self.db_manager = db_manager or parent.db_manager if parent else None
        
        self.setWindowTitle(f"Single Point Trend - Position [{row}, {col}]")
        self.setMinimumSize(600, 400)
        self.resize(800, 500)
        
        # Set window flags to ensure single-threaded dialog
        self.setWindowFlags(Qt.Dialog | Qt.WindowCloseButtonHint | Qt.WindowTitleHint)
        
        self.init_ui()
        
        # Load historical data from database (load all, not limited)
        self.load_history_data()
        
    def init_ui(self):
        """Initialize UI"""
        layout = QVBoxLayout(self)
        
        # Header
        header_layout = QHBoxLayout()
        
        title = QLabel(f"Trend Curve for Position [{self.row}, {self.col}]")
        title.setStyleSheet("font-size: 14pt; font-weight: bold;")
        header_layout.addWidget(title)
        
        header_layout.addStretch()
        
        close_btn = QPushButton("Close")
        close_btn.clicked.connect(self.close)
        close_btn.setMaximumWidth(80)
        header_layout.addWidget(close_btn)
        
        layout.addLayout(header_layout)
        
        # Trend chart
        chart_group = QGroupBox("Value Trend")
        chart_layout = QVBoxLayout()
        
        self.trend_chart = TrendChart()
        self.trend_chart.setMinimumHeight(350)
        chart_layout.addWidget(self.trend_chart)
        
        chart_group.setLayout(chart_layout)
        layout.addWidget(chart_group)
        
        # Info bar
        info_layout = QHBoxLayout()
        
        self.info_label = QLabel("Waiting for data...")
        self.info_label.setStyleSheet("color: #666; font-size: 10pt;")
        info_layout.addWidget(self.info_label)
        
        info_layout.addStretch()
        
        clear_btn = QPushButton("Clear Data")
        clear_btn.clicked.connect(self.clear_chart)
        clear_btn.setMaximumWidth(100)
        info_layout.addWidget(clear_btn)
        
        layout.addLayout(info_layout)
        
    def load_history_data(self):
        """Load historical data from database"""
        if self.db_manager:
            # 加载全部历史（不限条目）
            history = self.db_manager.get_point_history(self.row, self.col, limit=None)
            if history:
                self.trend_chart.update_data(history)
                self.info_label.setText(f"Loaded {len(history)} historical data points")
            else:
                self.info_label.setText("No historical data, waiting for real-time data...")
        else:
            self.info_label.setText("Database not initialized")
            
    def add_point(self, value):
        """Add new data point"""
        self.trend_chart.add_point(value)
        
        # Update info
        buffer_size = len(self.trend_chart.data_buffer) if hasattr(self.trend_chart, 'data_buffer') else 0
        self.info_label.setText(f"Real-time tracking - Current value: {value}, Data points: {buffer_size}")
        
    def clear_chart(self):
        """Clear chart data"""
        self.trend_chart.clear()
        self.info_label.setText("Chart cleared")
        
    def closeEvent(self, event):
        """Close event"""
        # Can save some state here if needed
        event.accept()

