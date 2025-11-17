#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
自定义表格组件，支持选中边框显示
"""

from PyQt5.QtWidgets import QTableWidget
from PyQt5.QtCore import Qt, QRect
from PyQt5.QtGui import QPainter, QPen, QColor

class MatrixTableWidget(QTableWidget):
    """自定义表格，支持选中单元格边框显示"""
    
    def __init__(self, parent=None):
        super().__init__(parent)
        self.selected_row = None
        self.selected_col = None
        
    def set_selected_cell(self, row, col):
        """设置选中的单元格"""
        self.selected_row = row
        self.selected_col = col
        self.viewport().update()
        
    def clear_selection(self):
        """清除选中"""
        self.selected_row = None
        self.selected_col = None
        self.viewport().update()
        
    def paintEvent(self, event):
        """绘制事件 - 绘制选中边框"""
        super().paintEvent(event)
        
        # 如果有选中的单元格，绘制黄色边框
        if self.selected_row is not None and self.selected_col is not None:
            painter = QPainter(self.viewport())
            pen = QPen(QColor(255, 255, 0), 3)  # 黄色边框，3像素宽
            painter.setPen(pen)
            
            # 获取单元格的位置和大小
            try:
                item = self.item(self.selected_row, self.selected_col)
                if item:
                    rect = self.visualItemRect(item)
                    if rect.isValid():
                        # 绘制矩形边框（留一点边距）
                        margin = 2
                        painter.drawRect(rect.adjusted(margin, margin, -margin, -margin))
            except Exception:
                pass

