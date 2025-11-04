#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
16x16矩阵显示组件
支持颜色映射和点击选择
"""

from PyQt5.QtWidgets import QWidget, QTableWidgetItem, QTableWidget, QVBoxLayout, QLabel, QSizePolicy
from PyQt5.QtCore import pyqtSignal, Qt, QSize, QTimer
from PyQt5.QtGui import QColor, QBrush
from gui.matrix_table import MatrixTableWidget
import numpy as np

class MatrixWidget(QWidget):
    """16x16矩阵显示组件"""
    
    cell_clicked = pyqtSignal(int, int)  # (row, col)
    cell_double_clicked = pyqtSignal(int, int)  # (row, col) - 双击弹出趋势图
    
    def __init__(self):
        super().__init__()
        # 设置大小策略，允许随窗口缩放
        self.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.init_ui()
        
    def init_ui(self):
        """初始化UI"""
        layout = QVBoxLayout(self)
        layout.setContentsMargins(10, 10, 10, 10)
        layout.setSpacing(8)
        # 确保布局没有固定高度限制
        
        # Mac风格标题
        title = QLabel("16×16 电容矩阵")
        title.setStyleSheet("""
            QLabel {
                font-size: 16pt;
                font-weight: 600;
                color: #333333;
                padding: 5px;
            }
        """)
        title.setSizePolicy(QSizePolicy.Preferred, QSizePolicy.Fixed)
        layout.addWidget(title)
        
        # 提示文字
        hint = QLabel("双击单元格查看趋势图")
        hint.setStyleSheet("""
            QLabel {
                font-size: 10pt;
                color: #666666;
                padding-left: 5px;
            }
        """)
        hint.setSizePolicy(QSizePolicy.Preferred, QSizePolicy.Fixed)
        layout.addWidget(hint)
        
        self.table = MatrixTableWidget()
        self.table.setRowCount(16)
        self.table.setColumnCount(16)
        
        # 禁用交替行颜色，避免干扰背景色
        self.table.setAlternatingRowColors(False)
        
        # 设置为无选择模式，使用自定义边框显示选中
        self.table.setSelectionMode(QTableWidget.NoSelection)
        self.table.cellClicked.connect(self.on_cell_clicked)
        self.table.cellDoubleClicked.connect(self.on_cell_double_clicked)
        
        # 设置表格属性
        self.table.verticalHeader().setVisible(True)
        self.table.horizontalHeader().setVisible(True)
        # 使用X0-X15表示列，Y0-Y15表示行
        self.table.setHorizontalHeaderLabels([f"X{i}" for i in range(16)])
        self.table.setVerticalHeaderLabels([f"Y{i}" for i in range(16)])
        
        # 初始化所有单元格为黑色（默认未连接状态）
        black_brush = QBrush(QColor(0, 0, 0))
        white_brush = QBrush(QColor(255, 255, 255))
        for row in range(16):
            for col in range(16):
                item = QTableWidgetItem("0")
                item.setTextAlignment(Qt.AlignCenter)
                item.setData(Qt.BackgroundRole, black_brush)
                item.setData(Qt.ForegroundRole, white_brush)
                item.setBackground(black_brush)
                item.setForeground(white_brush)
                self.table.setItem(row, col, item)
        
        # 设置表格自适应大小
        self.table.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        
        # 设置表头大小
        self.table.verticalHeader().setDefaultSectionSize(30)
        self.table.horizontalHeader().setDefaultSectionSize(30)
        
        # 禁用表格边框，使用Mac风格
        self.table.setShowGrid(True)
        self.table.setGridStyle(Qt.SolidLine)
        
        # 初始计算单元格大小（在resizeEvent中动态调整）
        self.update_cell_sizes()
            
        # 设置只读
        self.table.setEditTriggers(QTableWidget.NoEditTriggers)
        
        # 自定义样式：保持选中时底色不变，只有边框高亮
        # 注意：完全不设置item的背景色相关样式，让item的背景色通过代码动态设置
        # 移除所有可能覆盖背景色的样式规则
        self.table.setStyleSheet("""
            QTableWidget {
                border: 1px solid #D0D0D0;
                border-radius: 6px;
                background-color: #F5F5F5;
                gridline-color: #E0E0E0;
                alternate-background-color: transparent;
            }
            /* 完全移除item的样式，避免覆盖背景色 */
            QHeaderView::section {
                background-color: #F5F5F5;
                border: 1px solid #E0E0E0;
                padding: 4px;
                font-weight: 600;
                color: #333333;
            }
        """)
        
        layout.addWidget(self.table, 1)  # 设置拉伸因子为1，使表格占据剩余空间
        layout.setStretchFactor(self.table, 1)  # 确保表格可以伸缩
        
    def on_cell_clicked(self, row, col):
        """单元格单击事件 - 仅更新选中边框"""
        self.table.set_selected_cell(row, col)
        
        # 发送信号
        self.cell_clicked.emit(row, col)
        
    def on_cell_double_clicked(self, row, col):
        """单元格双击事件 - 弹出趋势图"""
        self.table.set_selected_cell(row, col)
        
        # 发送双击信号，由主窗口处理弹出对话框
        self.cell_double_clicked.emit(row, col)
        
    def get_selected_cell(self):
        """获取当前选中的单元格"""
        if self.table.selected_row is not None and self.table.selected_col is not None:
            return (self.table.selected_row, self.table.selected_col)
        return None
        
    def update_matrix(self, matrix_data, output_mode="raw", quant_min=0, quant_max=100000, quant_level=255, color_enabled=True):
        """更新矩阵显示（优化版本：只更新变化的单元格）
        :param color_enabled: 为False时，跳过颜色映射，使用白底黑字
        """
        # 使用blockSignals批量更新，减少信号发射
        self.table.blockSignals(True)
        
        try:
            # 颜色映射关闭：直接白底黑字显示，减少计算
            if not color_enabled:
                WHITE = QColor(255, 255, 255)
                BLACK = QColor(0, 0, 0)
                WHITE_BRUSH = QBrush(WHITE)
                BLACK_BRUSH = QBrush(BLACK)
                for row in range(16):
                    for col in range(16):
                        val = matrix_data[row][col]
                        if val is None:
                            val = 0
                        item = self.table.item(row, col)
                        if item is None:
                            item = QTableWidgetItem()
                            item.setTextAlignment(Qt.AlignCenter)
                            item.setFlags(Qt.ItemIsEnabled | Qt.ItemIsSelectable)
                            self.table.setItem(row, col, item)
                        # 文本
                        item.setText("0" if val == 0 else str(val))
                        # 白底黑字
                        item.setBackground(WHITE_BRUSH)
                        item.setForeground(BLACK_BRUSH)
                        item.setData(Qt.BackgroundRole, WHITE_BRUSH)
                        item.setData(Qt.ForegroundRole, BLACK_BRUSH)
                self.table.viewport().update()
                return

            # 首先检查是否所有值都是0或None（未连接USB的情况）
            all_zero = True
            has_data = False
            for r in range(16):
                for c in range(16):
                    val = matrix_data[r][c]
                    # 如果值不是None，检查是否不为0
                    if val is not None:
                        has_data = True
                        try:
                            val_float = float(val)
                            if abs(val_float) >= 0.0001:  # 不为0
                                all_zero = False
                                break
                        except (ValueError, TypeError):
                            # 如果无法转换为float，尝试直接比较
                            if val != 0 and val != "0":
                                all_zero = False
                                break
                if not all_zero:
                    break
            
            # 如果所有值都是0或None（未初始化），直接全部显示为黑色
            if all_zero or not has_data:
                black_color = QColor(0, 0, 0)
                white_color = QColor(255, 255, 255)
                black_brush = QBrush(black_color)
                white_brush = QBrush(white_color)
                
                # 强制设置所有16x16单元格为黑色
                for row in range(16):
                    for col in range(16):
                        item = self.table.item(row, col)
                        if item is None:
                            item = QTableWidgetItem("0")
                            item.setTextAlignment(Qt.AlignCenter)
                            item.setFlags(Qt.ItemIsEnabled | Qt.ItemIsSelectable)
                            self.table.setItem(row, col, item)
                        else:
                            item.setText("0")
                        
                        # 设置背景色和前景色
                        item.setBackground(black_brush)
                        item.setForeground(white_brush)
                        item.setData(Qt.BackgroundRole, black_brush)
                        item.setData(Qt.ForegroundRole, white_brush)
                
                # 只更新一次视图（减少重绘次数）
                self.table.viewport().update()
                
                return  # 提前返回，不需要后续的颜色映射计算
            
            # 找到最小值和最大值用于颜色映射
            if output_mode == "quant":
                # 量化模式：值范围是 0 到 quant_level
                min_val = 0
                max_val = quant_level
            else:
                # 原始值模式：计算实际范围
                all_values = [matrix_data[r][c] for r in range(16) for c in range(16) if matrix_data[r][c] is not None]
                if all_values:
                    min_val = min(all_values)
                    max_val = max(all_values)
                    if max_val == min_val:
                        max_val = min_val + 1
                else:
                    min_val = 0
                    max_val = 1
                    
            # 预计算量化参数和常量（避免重复计算）
            if output_mode == "quant":
                quant_range = quant_max - quant_min if quant_max > quant_min else 1
                quant_range_inv = quant_level / quant_range if quant_range > 0 else 0
            else:
                value_range = max_val - min_val if max_val > min_val else 1
                value_range_inv = 1.0 / value_range if value_range > 0 else 0
            
            # 预定义颜色常量（避免重复创建对象）
            BLACK = QColor(0, 0, 0)
            WHITE = QColor(255, 255, 255)
            BLACK_BRUSH = QBrush(BLACK)
            WHITE_BRUSH = QBrush(WHITE)
            
            # 批量更新每个单元格（高度优化版本）
            for row in range(16):
                for col in range(16):
                    value = matrix_data[row][col]
                    # 快速处理None值
                    if value is None:
                        value = 0
                    
                    # 获取或创建单元格（避免每次都创建新对象）
                    item = self.table.item(row, col)
                    if item is None:
                        item = QTableWidgetItem()
                        item.setTextAlignment(Qt.AlignCenter)
                        item.setFlags(Qt.ItemIsEnabled | Qt.ItemIsSelectable)
                        self.table.setItem(row, col, item)
                    
                    # 快速量化计算
                    if output_mode == "quant":
                        if quant_min >= quant_max or value < quant_min:
                            display_value = 0
                        elif value > quant_max:
                            display_value = quant_level
                        else:
                            display_value = int((value - quant_min) * quant_range_inv)
                    else:
                        display_value = value
                    
                    # 快速更新文本（避免重复转换）
                    if display_value != 0:
                        item.setText(str(display_value))
                    else:
                        item.setText("0")
                    
                    # 快速颜色计算（优化版）
                    if display_value == 0:
                        # 值为0，直接使用预定义颜色
                        color = BLACK
                        text_color = WHITE
                    elif output_mode == "quant":
                        # 量化模式：ratio = display_value / quant_level
                        ratio = display_value * (1.0 / quant_level)
                        r, g, b = self._fast_color_calc(ratio)
                        color = QColor(r, g, b)
                        # 快速亮度计算（使用整数运算）
                        brightness = (r * 299 + g * 587 + b * 114) // 1000
                        text_color = WHITE if brightness <= 128 else BLACK
                    else:
                        # 原始值模式：ratio = (display_value - min_val) / value_range
                        ratio = (display_value - min_val) * value_range_inv
                        if ratio <= 0:
                            r, g, b = 0, 0, 0
                            color = BLACK
                            text_color = WHITE
                        elif ratio >= 1:
                            r, g, b = 255, 255, 255
                            color = WHITE
                            text_color = BLACK
                        else:
                            r, g, b = self._fast_color_calc(ratio)
                            color = QColor(r, g, b)
                            # 快速亮度计算
                            brightness = (r * 299 + g * 587 + b * 114) // 1000
                            text_color = WHITE if brightness <= 128 else BLACK
                    
                    # 批量设置颜色（使用预定义对象）
                    if color == BLACK:
                        item.setBackground(BLACK_BRUSH)
                        item.setForeground(WHITE_BRUSH)
                    elif color == WHITE:
                        item.setBackground(WHITE_BRUSH)
                        item.setForeground(BLACK_BRUSH)
                    else:
                        color_brush = QBrush(color)
                        item.setBackground(color_brush)
                        item.setForeground(WHITE_BRUSH if text_color == WHITE else BLACK_BRUSH)
                    item.setData(Qt.BackgroundRole, QBrush(color))
                    item.setData(Qt.ForegroundRole, QBrush(text_color))
            
            # 一次性更新视图（减少重绘次数）
            self.table.viewport().update()
            
        finally:
            self.table.blockSignals(False)
    
    def _fast_color_calc(self, ratio):
        """快速颜色计算（使用整数运算优化）"""
        # 限制ratio在0-1范围并转换为整数
        if ratio <= 0:
            return (0, 0, 0)
        if ratio >= 1:
            return (255, 255, 255)
        
        # 使用整数运算（ratio * 8 将0-1映射到0-8的8个区间）
        ratio_scaled = ratio * 8.0
        
        if ratio_scaled < 1.0:  # ratio < 0.125: 黑色 -> 深紫
            t = ratio_scaled * 8.0
            r = b = int(128 * t)
            return (r, 0, b)
        elif ratio_scaled < 2.0:  # ratio < 0.25: 深紫 -> 蓝色
            t = (ratio_scaled - 1.0) * 8.0
            r = int(128 * (1 - t))
            b = int(128 + 127 * t)
            return (max(0, r), 0, min(255, b))
        elif ratio_scaled < 3.0:  # ratio < 0.375: 蓝色 -> 绿色
            t = (ratio_scaled - 2.0) * 8.0
            g = int(255 * t)
            b = int(255 * (1 - t))
            return (0, min(255, g), max(0, b))
        elif ratio_scaled < 4.0:  # ratio < 0.5: 绿色 -> 青色
            return (0, 255, 255)
        elif ratio_scaled < 5.0:  # ratio < 0.625: 青色 -> 黄色
            t = (ratio_scaled - 4.0) * 8.0
            r = int(255 * t)
            b = int(255 * (1 - t))
            return (min(255, r), 255, max(0, b))
        elif ratio_scaled < 6.0:  # ratio < 0.75: 黄色 -> 红色
            t = (ratio_scaled - 5.0) * 8.0
            g = int(255 * (1 - t))
            return (255, max(0, g), 0)
        elif ratio_scaled < 7.0:  # ratio < 0.875: 红色 -> 洋红
            t = (ratio_scaled - 6.0) * 8.0
            b = int(255 * t)
            return (255, 0, min(255, b))
        else:  # ratio < 1.0: 洋红 -> 白色
            t = (ratio_scaled - 7.0) * 8.0
            g = int(255 * t)
            return (255, min(255, g), 255)
                
    def highlight_cell(self, row, col):
        """高亮显示某个单元格（通过边框）"""
        self.table.set_selected_cell(row, col)
        
    def update_cell_sizes(self):
        """根据表格大小动态调整单元格大小，确保16x16完整显示，保持1:1比例"""
        if not self.table or not self.table.isVisible():
            return
            
        # 获取表格可用大小（viewport大小）
        table_size = self.table.viewport().size()
        
        # 计算可用空间
        available_width = table_size.width()
        available_height = table_size.height()
        
        # 如果表格还没初始化或大小无效，使用默认值
        if available_width <= 0 or available_height <= 0:
            # 尝试从widget大小计算
            widget_size = self.size()
            if widget_size.width() > 0 and widget_size.height() > 0:
                # 减去标题和提示的高度（大约80像素）
                available_width = widget_size.width() - 40  # 减去边距
                available_height = widget_size.height() - 80  # 减去标题、提示和边距
            else:
                available_width = 800
                available_height = 800
        
        # 计算每个单元格的理想大小（确保16x16完整显示，保持1:1比例）
        # 取宽度和高度的较小值，确保单元格是正方形
        cell_size = max(20, min(available_width, available_height) // 16)  # 最小20像素，保持1:1比例
        
        # 设置列宽和行高相同，保持1:1比例
        for i in range(16):
            self.table.setColumnWidth(i, cell_size)
            self.table.setRowHeight(i, cell_size)
            
    def resizeEvent(self, event):
        """窗口大小改变时重新计算单元格大小"""
        super().resizeEvent(event)
        # 延迟更新，确保布局已完成
        QTimer.singleShot(50, self.update_cell_sizes)
        
    def showEvent(self, event):
        """窗口显示时初始化单元格大小"""
        super().showEvent(event)
        QTimer.singleShot(100, self.update_cell_sizes)

