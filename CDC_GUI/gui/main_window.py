#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
主窗口
"""

from PyQt5.QtWidgets import (QMainWindow, QWidget, QVBoxLayout, QHBoxLayout, 
                             QSplitter, QStatusBar, QLabel, QPushButton,
                             QComboBox, QLineEdit, QGroupBox, QGridLayout, QFrame, QSizePolicy)
from PyQt5.QtCore import Qt, QTimer
from PyQt5.QtGui import QFont, QPalette, QColor, QIcon

from gui.matrix_widget import MatrixWidget
from gui.message_log import MessageLog
from gui.command_panel import CommandPanel
from communication.serial_communication import SerialCommunication
from database.database_manager import DatabaseManager

class MainWindow(QMainWindow):
    """主窗口类"""
    
    def __init__(self):
        super().__init__()
        self.setWindowTitle("PCap04 16x16 电容矩阵监控系统")
        # Mac风格：宽度较宽，高度适中（最小尺寸，可调整）
        self.setMinimumSize(850, 850)  # 设置最小尺寸
        self.resize(1200, 900)  # 初始大小，可调整
        # 保障窗口图标（如果应用层已设置，这里也设置一次）
        try:
            import os
            base_dir = os.path.dirname(os.path.dirname(__file__))
            icon_path = os.path.join(base_dir, "app.ico")
            if os.path.exists(icon_path):
                self.setWindowIcon(QIcon(icon_path))
        except Exception:
            pass
        
        # 初始化组件
        self.serial_comm = SerialCommunication()
        self.db_manager = DatabaseManager()
        # 数据库线程已在DatabaseManager中启动
        
        # 当前矩阵数据（双缓冲）
        self.matrix_data = [[0 for _ in range(16)] for _ in range(16)]
        self.matrix_lock = False  # 简单的锁标志
        
        # 数据接收状态（用于识别START/END标记和格式）
        self.matrix_receiving = False  # 是否正在接收矩阵数据块
        self.current_format = None  # 当前格式：'simple' 或 'table'
        self.matrix_buffer = []  # 矩阵数据缓冲区（存储完整的数据块）
        self.start_time = None  # START标记的时间戳（用于计算传输时间）
        self.table_col_headers = []  # TABLE格式的列标题（用于流式解析）
        self.received_rows = set()  # 已接收的行号（用于检查完整性）
        self.expected_rows = set(range(16))  # 期望的行号（Y00-Y15）
        # 统计START/END期间的字节数
        self.transfer_byte_count = 0
        
        # 追踪的点（行，列）
        self.tracked_point = None
        self.track_history = {}  # {(row, col): [values...]}
        self.trend_dialogs = {}  # 趋势对话框字典 {(row, col): TrendDialog}
        
        # 输出模式
        self.output_mode = "raw"  # "raw" 或 "quant"
        self.quant_min = 0
        self.quant_max = 100000
        self.quant_level = 255  # 255 或 1023
        # 颜色映射开关（开：按数值上色；关：白底黑字）
        self.color_mapping_enabled = True
        
        # 测试模式标志
        self.test_mode_active = False
        # 自动记录设置：连接串口后自动记录每帧（START-END）
        self.auto_record_enabled = True
        # 当前帧ID（用于数据库分帧）
        self.current_frame_id = 0
        
        # 数据更新标志（用于触发立即更新）- 必须在update_matrix_display之前初始化
        self.matrix_data_changed = False
        
        # 当前连接的端口（显示名称）
        self.current_port = None
        # 端口映射：显示名称 -> 实际设备名
        self.port_map = {}
        
        # 应用Mac风格样式
        self.apply_mac_style()
        
        # 初始化UI
        self.init_ui()
        
        # 初始化矩阵显示（显示黑色背景，因为所有值都是0）
        self.update_matrix_display()
        
        # 连接信号
        self.connect_signals()
        
        # 定时器：批量更新矩阵显示（提高性能）
        self.update_timer = QTimer()
        self.update_timer.timeout.connect(self.update_matrix_display)
        self.update_timer.start(50)  # 50ms更新一次（更频繁的更新）
        
        # 注意：串口连接状态由专门的监控线程维护，不再需要定时器检查
        
    def apply_mac_style(self):
        """应用Mac风格样式"""
        # 设置全局样式表
        self.setStyleSheet("""
            /* 主窗口 */
            QMainWindow {
                background-color: #F5F5F5;
            }
            
            /* 分组框 - Mac风格圆角 */
            QGroupBox {
                font-weight: bold;
                border: 1px solid #D0D0D0;
                border-radius: 8px;
                margin-top: 10px;
                padding-top: 15px;
                background-color: #FFFFFF;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 10px;
                padding: 0 5px;
                color: #333333;
            }
            
            /* 按钮 - Mac风格 */
            QPushButton {
                background-color: #007AFF;
                color: white;
                border: none;
                border-radius: 6px;
                padding: 3px 10px;
                font-size: 9pt;
                min-height: 18px;
                max-height: 24px;
            }
            QPushButton:hover {
                background-color: #0051D5;
            }
            QPushButton:pressed {
                background-color: #0040B0;
            }
            QPushButton:disabled {
                background-color: #C0C0C0;
                color: #808080;
            }
            
            /* 下拉框 - Mac风格 */
            QComboBox {
                border: 1px solid #D0D0D0;
                border-radius: 6px;
                padding: 4px 8px;
                background-color: white;
                min-height: 24px;
                color: #00AA00;  /* 绿色文字 */
            }
            QComboBox:hover {
                border-color: #007AFF;
            }
            QComboBox::drop-down {
                border: none;
                width: 20px;
            }
            QComboBox::down-arrow {
                image: none;
                border-left: 4px solid transparent;
                border-right: 4px solid transparent;
                border-top: 5px solid #666666;
                width: 0;
                height: 0;
            }
            QComboBox QAbstractItemView {
                color: #00AA00;  /* 下拉列表文字也是绿色 */
                background-color: white;
                selection-background-color: #007AFF;
                selection-color: white;
            }
            
            /* 输入框 - Mac风格 */
            QLineEdit {
                border: 1px solid #D0D0D0;
                border-radius: 6px;
                padding: 4px 8px;
                background-color: white;
                min-height: 24px;
            }
            QLineEdit:focus {
                border-color: #007AFF;
                border-width: 2px;
            }
            
            /* 标签 */
            QLabel {
                color: #333333;
            }
            
            /* 状态栏 */
            QStatusBar {
                background-color: #E8E8E8;
                border-top: 1px solid #D0D0D0;
            }
            
            /* 分割器 */
            QSplitter::handle {
                background-color: #E0E0E0;
            }
            QSplitter::handle:horizontal {
                width: 3px;
            }
            QSplitter::handle:vertical {
                height: 3px;
            }
            QSplitter::handle:hover {
                background-color: #007AFF;
            }
        """)
        
    def init_ui(self):
        """初始化UI"""
        central_widget = QWidget()
        central_widget.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        self.setCentralWidget(central_widget)
        
        main_layout = QVBoxLayout(central_widget)
        main_layout.setContentsMargins(5, 5, 5, 5)
        main_layout.setSpacing(0)  # 移除间距，确保充分利用空间
        
        # 使用水平分割器（可拖拽调整）
        splitter = QSplitter(Qt.Horizontal)
        splitter.setChildrenCollapsible(False)  # 防止子组件被折叠为0
        # 确保分割器可以垂直和水平缩放
        splitter.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
        
        # 左侧：矩阵显示
        self.matrix_widget = MatrixWidget()
        splitter.addWidget(self.matrix_widget)
        
        # 右侧：使用垂直分割器（只允许垂直拖拽，不允许水平拖拽）
        right_splitter = QSplitter(Qt.Vertical)  # 垂直分割器，只有垂直拖拽
        right_splitter.setChildrenCollapsible(False)  # 防止子组件被折叠为0
        # 注意：右侧面板整体可以通过主水平分割器与矩阵区域调整宽度
        # 但右侧面板内部只有垂直分割（上下拖拽），没有水平分割（左右拖拽）
        
        # 右侧顶部：USB连接和输出模式控制
        right_top_widget = QWidget()
        # 使用Preferred而不是Fixed，允许有一定伸缩性
        right_top_widget.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Preferred)
        right_top_layout = QVBoxLayout(right_top_widget)
        right_top_layout.setSpacing(4)
        right_top_layout.setContentsMargins(5, 5, 5, 5)
        # 不要添加stretch，让控制面板保持紧凑但可调整
        
        # USB连接控制
        conn_group = QGroupBox("USB连接")
        conn_layout = QVBoxLayout()
        conn_layout.setSpacing(6)
        
        port_layout = QHBoxLayout()
        port_layout.addWidget(QLabel("端口:"))
        self.port_combo = QComboBox()
        self.port_combo.setMinimumWidth(120)
        port_layout.addWidget(self.port_combo)
        self.refresh_btn = QPushButton("刷新")
        self.refresh_btn.setMaximumWidth(60)
        port_layout.addWidget(self.refresh_btn)
        conn_layout.addLayout(port_layout)
        
        btn_layout = QHBoxLayout()
        self.connect_btn = QPushButton("连接")
        self.disconnect_btn = QPushButton("断开")
        self.disconnect_btn.setEnabled(False)
        btn_layout.addWidget(self.connect_btn)
        btn_layout.addWidget(self.disconnect_btn)
        conn_layout.addLayout(btn_layout)
        
        conn_group.setLayout(conn_layout)
        right_top_layout.addWidget(conn_group)
        
        # 输出模式控制
        mode_group = QGroupBox("输出模式")
        mode_layout = QVBoxLayout()
        mode_layout.setSpacing(6)
        
        # 模式选择
        mode_row = QHBoxLayout()
        mode_row.addWidget(QLabel("模式:"))
        self.mode_combo = QComboBox()
        self.mode_combo.addItems(["原始值 (RAW)", "量化值 (QUANT)"])
        self.mode_combo.setMinimumWidth(150)
        self.mode_combo.currentTextChanged.connect(self.on_mode_changed)  # 模式变化时启用/禁用
        mode_row.addWidget(self.mode_combo)
        mode_row.addStretch()
        mode_layout.addLayout(mode_row)
        
        # 量化参数（最小/最大/档位）一行
        quant_row = QHBoxLayout()
        quant_row.addWidget(QLabel("最小值:"))
        self.min_spin = QLineEdit("0")
        self.min_spin.setMaximumWidth(70)
        quant_row.addWidget(self.min_spin)
        
        quant_row.addWidget(QLabel("最大值:"))
        self.max_spin = QLineEdit("100000")
        self.max_spin.setMaximumWidth(80)
        quant_row.addWidget(self.max_spin)
        
        quant_row.addWidget(QLabel("档位:"))
        self.level_combo = QComboBox()
        self.level_combo.addItems(["255", "1023"])
        self.level_combo.setMaximumWidth(70)
        quant_row.addWidget(self.level_combo)
        
        quant_row.addStretch()
        mode_layout.addLayout(quant_row)
        
        # 应用按钮
        self.apply_mode_btn = QPushButton("应用设置")
        self.apply_mode_btn.setMaximumWidth(100)
        mode_layout.addWidget(self.apply_mode_btn, alignment=Qt.AlignHCenter)

        # 颜色映射开关按钮
        color_row = QHBoxLayout()
        self.color_toggle_btn = QPushButton("色彩映射: 开")
        self.color_toggle_btn.setCheckable(True)
        self.color_toggle_btn.setChecked(True)
        self.color_toggle_btn.setMaximumWidth(120)
        self.color_toggle_btn.setMaximumHeight(22)
        color_row.addWidget(self.color_toggle_btn)
        color_row.addStretch()
        mode_layout.addLayout(color_row)

        # 当前数据格式显示
        fmt_row = QHBoxLayout()
        fmt_label = QLabel("当前格式:")
        fmt_label.setStyleSheet("font-size: 9pt;")
        fmt_label.setMaximumHeight(18)
        fmt_row.addWidget(fmt_label)
        self.format_display = QLabel("未知")
        self.format_display.setStyleSheet("color: #333333;")
        self.format_display.setMaximumHeight(18)
        fmt_row.addWidget(self.format_display)
        fmt_row.addStretch()
        mode_layout.addLayout(fmt_row)
        
        # 初始状态：根据当前模式启用/禁用量化参数
        self.update_quant_controls_enabled()
        
        mode_group.setLayout(mode_layout)
        right_top_layout.addWidget(mode_group)
        
        # 移除stretch，让控制面板保持紧凑
        right_splitter.addWidget(right_top_widget)
        
        # 命令面板
        self.command_panel = CommandPanel()
        right_splitter.addWidget(self.command_panel)
        
        # 消息日志
        self.message_log = MessageLog()
        right_splitter.addWidget(self.message_log)
        
        splitter.addWidget(right_splitter)
        
        # 设置主分割器比例（矩阵65%，右侧面板35%）
        splitter.setStretchFactor(0, 75)
        splitter.setStretchFactor(1, 25)
        
        # 将分割器添加到主布局，设置拉伸因子使它能占据所有可用空间
        # 使用addWidget的第二个参数设置拉伸因子
        main_layout.addWidget(splitter)
        # 设置布局的拉伸因子
        main_layout.setStretch(0, 1)  # 第一个子组件（splitter）占据所有空间
        
        # 延迟设置右侧分割器的初始大小，确保窗口已经显示
        QTimer.singleShot(100, lambda: self.setup_right_splitter_sizes(right_splitter))
        
        # 状态栏
        self.statusBar().showMessage("就绪 - 未连接")
        
    def setup_right_splitter_sizes(self, right_splitter):
        """设置右侧分割器的初始大小"""
        try:
            total_height = right_splitter.height()
            if total_height > 100:  # 确保有足够的高度
                # 控制面板约180像素（但不超过总高度的30%），其余平均分配
                control_height = min(180, int(total_height * 0.3))
                remaining = total_height - control_height
                if remaining > 0:
                    command_height = remaining // 2
                    log_height = remaining - command_height
                    right_splitter.setSizes([control_height, command_height, log_height])
                    # 设置拉伸因子，允许用户调整
                    right_splitter.setStretchFactor(0, 0)  # 控制面板不拉伸
                    right_splitter.setStretchFactor(1, 1)  # 命令面板可拉伸
                    right_splitter.setStretchFactor(2, 1)  # 消息日志可拉伸
        except:
            pass  # 如果设置失败，使用默认行为
        
    def connect_signals(self):
        """连接信号和槽"""
        # USB连接
        self.refresh_btn.clicked.connect(self.refresh_ports)
        self.connect_btn.clicked.connect(self.connect_usb)
        self.disconnect_btn.clicked.connect(self.disconnect_usb)
        
        # 模式设置
        self.apply_mode_btn.clicked.connect(self.apply_mode_settings)
        # 模式变化信号已在init_ui中连接
        # 颜色映射切换
        self.color_toggle_btn.toggled.connect(self.on_color_toggle)
        
        # 串口通信信号
        self.serial_comm.data_received.connect(self.on_data_received)
        self.serial_comm.status_changed.connect(self.on_serial_status_changed)
        
        # 矩阵点击信号
        self.matrix_widget.cell_clicked.connect(self.on_matrix_cell_clicked)
        self.matrix_widget.cell_double_clicked.connect(self.on_matrix_cell_double_clicked)
        
        # 命令面板信号
        self.command_panel.command_sent.connect(self.send_command)
        
        # 趋势对话框字典已在__init__中初始化
        
        # 初始刷新端口列表
        self.refresh_ports()
        
    def refresh_ports(self):
        """刷新串口列表"""
        port_list = self.serial_comm.get_available_ports_with_info()
        self.port_combo.clear()
        self.port_map.clear()
        
        # 添加端口，显示名称和实际设备名的映射
        for device, display_name in port_list:
            self.port_combo.addItem(display_name)
            self.port_map[display_name] = device
        
    def connect_usb(self):
        """连接USB"""
        display_name = self.port_combo.currentText()
        if display_name:
            # 从映射中获取实际设备名
            port = self.port_map.get(display_name, display_name)
            self.current_port = display_name  # 保存显示名称
            
            if self.serial_comm.connect(port):
                self.connect_btn.setEnabled(False)
                self.disconnect_btn.setEnabled(True)
                self.statusBar().showMessage(f"已连接到 {display_name}")
                self.message_log.add_message("系统", f"连接成功: {display_name}", "info")
            else:
                self.message_log.add_message("系统", f"连接失败: {display_name}", "error")
        else:
            self.message_log.add_message("系统", "请选择端口", "warning")
            
    def disconnect_usb(self):
        """断开USB"""
        self.serial_comm.disconnect()
        self.connect_btn.setEnabled(True)
        self.disconnect_btn.setEnabled(False)
        self.statusBar().showMessage("已断开连接")
        self.message_log.add_message("系统", "已断开连接", "info")
        
    def on_mode_changed(self, mode_text):
        """模式变化时更新量化参数控件的启用状态"""
        self.update_quant_controls_enabled()
        
    def update_quant_controls_enabled(self):
        """根据当前模式更新量化参数控件的启用状态"""
        mode = self.mode_combo.currentText()
        is_quant = "QUANT" in mode
        
        # 量化模式下启用，原始值模式下禁用
        self.min_spin.setEnabled(is_quant)
        self.max_spin.setEnabled(is_quant)
        self.level_combo.setEnabled(is_quant)
        
        # 更新视觉效果
        style_disabled = "background-color: #F0F0F0; color: #888888;"
        style_enabled = "background-color: white; color: #00AA00;"
        
        self.min_spin.setStyleSheet(style_enabled if is_quant else style_disabled)
        self.max_spin.setStyleSheet(style_enabled if is_quant else style_disabled)
        self.level_combo.setStyleSheet(style_enabled if is_quant else style_disabled)
        
    def apply_mode_settings(self):
        """应用模式设置"""
        mode = self.mode_combo.currentText()
        if "QUANT" in mode:
            self.output_mode = "quant"
        else:
            self.output_mode = "raw"
            
        try:
            if self.output_mode == "quant":
                self.quant_min = int(self.min_spin.text())
                self.quant_max = int(self.max_spin.text())
                level_str = self.level_combo.currentText()
                self.quant_level = int(level_str)
            
            # 发送命令到设备
            if self.serial_comm.is_connected():
                self.send_command(f"SET_MODE:{self.output_mode}")
                if self.output_mode == "quant":
                    self.send_command(f"SET_RANGE:{self.quant_min}:{self.quant_max}")
                    self.send_command(f"SET_LEVEL:{self.quant_level}")
                # 不再显示翻译后的消息，只显示原始命令（已在send_command中通过add_command_sent显示）
            else:
                self.message_log.add_message("系统", "请先连接USB设备", "warning")
        except ValueError:
            self.message_log.add_message("系统", "参数格式错误", "error")
            
    def send_command(self, command):
        """发送命令"""
        # Handle TEST_MODE command locally
        if command == "TEST_MODE":
            # Toggle test mode
            self.test_mode_active = not self.test_mode_active
            
            if self.test_mode_active:
                # Set all matrix data to 0 to display black
                for row in range(16):
                    for col in range(16):
                        self.matrix_data[row][col] = 0
                
                # Stop automatic updates during test mode
                if self.update_timer.isActive():
                    self.update_timer.stop()
                
                # Force update display immediately - call update_matrix directly
                # This will detect all zeros and set all 16x16 cells to black
                self.matrix_widget.update_matrix(
                    self.matrix_data,
                    self.output_mode,
                    self.quant_min if self.output_mode == "quant" else 0,
                    self.quant_max if self.output_mode == "quant" else 100000,
                    self.quant_level if self.output_mode == "quant" else 255
                )
                
                # Force multiple repaints to ensure all cells are updated
                self.matrix_widget.update()
                self.matrix_widget.table.update()
                self.matrix_widget.table.viewport().update()
                self.matrix_widget.table.repaint()
                
                # Use QTimer to force update after a short delay (ensure all 256 cells are updated)
                from PyQt5.QtCore import QTimer
                QTimer.singleShot(10, lambda: (
                    self.matrix_widget.table.viewport().update(),
                    self.matrix_widget.table.update()
                ))
                QTimer.singleShot(50, lambda: self.matrix_widget.table.repaint())
                QTimer.singleShot(100, lambda: (
                    self.matrix_widget.table.viewport().update(),
                    self.matrix_widget.table.repaint()
                ))
                
                self.message_log.add_message("系统", "测试模式已激活 - 矩阵已设置为黑色", "info")
            else:
                # Exit test mode, resume automatic updates
                if not self.update_timer.isActive():
                    self.update_timer.start(100)
                self.message_log.add_message("系统", "测试模式已关闭 - 恢复正常更新", "info")
            return
        
        if self.serial_comm.is_connected():
            self.serial_comm.send_command(command)
            # 使用新的命令显示方法
            self.message_log.add_command_sent(command)
        else:
            # Allow TEST_MODE without connection
            if command != "TEST_MODE":
                self.message_log.add_message("系统", "设备未连接，无法发送命令", "error")
            
    def on_data_received(self, data):
        """处理接收到的数据（支持流式传输）- 优化版"""
        # Skip data updates in test mode
        if self.test_mode_active:
            return
            
        # 快速处理单行数据（避免split开销）
        line = data.strip() if isinstance(data, str) else data.decode('utf-8', errors='ignore').strip()
        if not line:
            return
            
        has_matrix = False
        rows_updated = set()  # 跟踪更新的行
        
        # 处理单行数据（大部分情况下是单行）
        self._process_single_line(line, rows_updated)
        
        # 标记数据变化（如果有矩阵数据）
        if self.matrix_data_changed:
            has_matrix = True
                
        # 如果包含矩阵数据，更新数据库（界面由定时器更新）
        if has_matrix:
            # 更新数据库（非阻塞）
            if self.tracked_point:
                row, col = self.tracked_point
                raw_value = self.matrix_data[row][col]
                if self.output_mode == "quant":
                    display_value = self.quantize_value(raw_value)
                    self.db_manager.add_data_point(row, col, display_value, raw_value=raw_value, non_blocking=True)
                else:
                    self.db_manager.add_data_point(row, col, raw_value, raw_value=raw_value, non_blocking=True)
        
        # 不显示矩阵数据日志（提升性能）
        # 只在非矩阵数据时显示响应消息
        if not self.matrix_receiving and data.strip() and not (line.startswith('Y') or line.startswith('X')):
            # 也不显示START/END，完全静默处理矩阵数据
            if line not in ["START", "END"]:
                self.message_log.add_response(data, "received")
    
    def _process_single_line(self, line, rows_updated):
        """处理单行数据（优化版本，减少函数调用开销）"""
        line = line.strip()
        if not line:
            return
        
        # 检测START标记（记录开始时间，用于计算时差）
        if line == "START":
            import time
            self.start_time = time.time() * 1000  # 转换为毫秒
            self.matrix_receiving = True
            self.current_format = None
            self.matrix_buffer = []
            self.table_col_headers = []  # 重置列标题
            self.received_rows = set()  # 重置已接收行号
            self.expected_rows = set(range(16))  # 期望16行（Y00-Y15）
            self.transfer_byte_count = 0
            # 显示未知格式，待自动识别
            try:
                self.update_format_display()
            except Exception:
                pass
            # 进入新帧
            self.current_frame_id += 1
            return
        
        # 检测END标记（只计算时差，不打印日志以提升性能）
        if line == "END":
            if self.start_time is not None:
                import time
                end_time = time.time() * 1000
                elapsed_ms = end_time - self.start_time
                # 控制台打印传输耗时与速率（保留用户逻辑）
                try:
                    duration_sec = elapsed_ms / 1000.0
                    bytes_count = self.transfer_byte_count
                    rate_bps = bytes_count / duration_sec if duration_sec > 0 else 0
                    rate_kbps = rate_bps / 1024.0
                    total_bits = bytes_count * 8
                    print(f"Time duration: {elapsed_ms:.4f} ms")
                    print(f"Data rate: {rate_kbps:.4f} KB/s")
                    print(f"{bytes_count} byte ({total_bits} bit)")
                except Exception:
                    pass
                # 记录本帧到数据库
                try:
                    if self.auto_record_enabled and self.db_manager:
                        from datetime import datetime
                        ts = datetime.now()
                        frame_points = []
                        for r in range(16):
                            for c in range(16):
                                raw_val = self.matrix_data[r][c]
                                if raw_val is None:
                                    raw_val = 0
                                if self.output_mode == "quant":
                                    disp_val = self.quantize_value(raw_val)
                                else:
                                    disp_val = raw_val
                                frame_points.append((ts, r, c, disp_val, raw_val, self.current_frame_id))
                        self.db_manager.add_frame_points(frame_points)
                except Exception as e:
                    print(f"记录帧到数据库失败: {e}")
                self.start_time = None
            
            # 处理缓冲区中剩余的数据（确保不遗漏）
            # 注意：此时 self.matrix_receiving 还是 True，所以可以处理
            if self.matrix_buffer:
                for buffered_line in self.matrix_buffer:
                    parsed = self.parse_stream_line(buffered_line, rows_updated)
                    if parsed:
                        self.matrix_data_changed = True
            
            self.matrix_receiving = False
            self.current_format = None
            self.matrix_buffer = []
            self.table_col_headers = []  # 重置列标题
            self.received_rows = set()  # 重置已接收行号
            return
        
        # 流式处理：如果在接收矩阵数据块中，立即解析
        if self.matrix_receiving:
            # 将数据行加入缓冲区（确保顺序）
            self.matrix_buffer.append(line)
            # 统计字节数（保留用户逻辑：计算除START/END外的内容长度）
            try:
                self.transfer_byte_count += len(line.encode('utf-8'))
            except Exception:
                pass
            
            # 流式解析：每收到一行数据就立即处理（但不立即更新界面）
            parsed = self.parse_stream_line(line, rows_updated)
            if parsed:
                self.matrix_data_changed = True  # 标记数据已变化
        else:
            # 不在矩阵数据块中，尝试解析旧的格式（向后兼容）
            if self.parse_matrix_data_old_format(line, rows_updated):
                self.matrix_data_changed = True  # 标记数据已变化
        
    def parse_stream_line(self, line, rows_updated=None):
        """流式解析单行数据（立即解析并显示，不等待完整数据块）"""
        line = line.strip()
        if not line:
            return False
        
        # 自动检测格式（第一次遇到数据时）
        if self.current_format is None:
            if line.startswith('X') and ',' in line:
                # TABLE格式：列标题行
                self.current_format = 'table'
                # 解析列标题并保存
                self.table_col_headers = []
                for col_name in line.split(','):
                    col_name = col_name.strip()
                    if col_name.startswith('X'):
                        try:
                            col_num = int(col_name[1:])
                            if 0 <= col_num < 16:
                                self.table_col_headers.append(col_num)
                        except ValueError:
                            pass
                # 列标题行不包含数据，返回False
                try:
                    self.update_format_display()
                except Exception:
                    pass
                return False
            elif 'X' in line and 'Y' in line and ':' in line:
                # SIMPLE格式
                self.current_format = 'simple'
                try:
                    self.update_format_display()
                except Exception:
                    pass
            else:
                # 无法识别格式，等待更多数据
                return False
        
        # 根据格式解析
        if self.current_format == 'simple':
            return self.parse_simple_format_line(line, rows_updated)
        elif self.current_format == 'table':
            return self.parse_table_format_line(line, rows_updated)
        
        return False

    def update_format_display(self):
        """更新界面上的当前数据格式显示"""
        try:
            if hasattr(self, 'format_display') and self.format_display is not None:
                if self.current_format == 'simple':
                    self.format_display.setText("SIMPLE (逐点)")
                elif self.current_format == 'table':
                    self.format_display.setText("TABLE (按行)")
                else:
                    self.format_display.setText("未知")
        except Exception:
            pass
    
    def parse_simple_format_line(self, line, rows_updated=None):
        """解析简洁格式的单行：X00Y00:值"""
        line = line.strip()
        if not line or ':' not in line:
            return False
        
        try:
            # 分离坐标和值
            coord_part, value_str = line.split(':', 1)
            coord_part = coord_part.strip()
            value_str = value_str.strip()
            
            # 解析坐标 X00Y00
            if coord_part.startswith('X') and 'Y' in coord_part:
                # 找到Y的位置
                y_idx = coord_part.index('Y')
                x_str = coord_part[1:y_idx]  # X后面的数字
                y_str = coord_part[y_idx+1:]  # Y后面的数字
                
                try:
                    col = int(x_str)
                    row = int(y_str)
                    value = int(value_str)
                    
                    if 0 <= row < 16 and 0 <= col < 16:
                        self.matrix_data[row][col] = value
                        
                        if rows_updated is not None:
                            rows_updated.add(row)
                        
                        return True
                except (ValueError, IndexError):
                    return False
        except (ValueError, IndexError, AttributeError):
            return False
        
        return False
    
    def parse_table_format_line(self, line, rows_updated=None):
        """解析表格格式的单行：Y00,值,值,值,...,值"""
        line = line.strip()
        if not line or not line.startswith('Y'):
            return False
        
        # 检查是否已解析列标题
        if len(self.table_col_headers) != 16:
            return False
        
        try:
            # 分离行号和值
            parts = line.split(',')
            if len(parts) < 2:
                return False
            
            # 第一个元素是行号 Y00
            row_str = parts[0].strip()
            if row_str.startswith('Y'):
                row_num = int(row_str[1:])
                if 0 <= row_num < 16:
                    # 记录已接收的行号
                    self.received_rows.add(row_num)
                    
                    # 解析该行的所有值
                    found = False
                    for col_idx, val_str in enumerate(parts[1:17]):  # 最多16列
                        if col_idx < len(self.table_col_headers):
                            col = self.table_col_headers[col_idx]
                            try:
                                value = int(val_str.strip())
                                if 0 <= col < 16:
                                    self.matrix_data[row_num][col] = value
                                    found = True
                            except (ValueError, IndexError):
                                continue
                    
                    if rows_updated is not None and found:
                        rows_updated.add(row_num)
                    
                    return found
        except (ValueError, IndexError, AttributeError):
            return False
        
        return False
    
    def process_matrix_block(self, lines, rows_updated=None):
        """处理完整的矩阵数据块（保留用于向后兼容，流式模式下不使用）"""
        if not lines:
            return False
        
        # 检测数据格式
        first_line = lines[0].strip()
        
        # 判断是TABLE格式还是SIMPLE格式
        # TABLE格式：第一行是列标题 X00,X01,X02,...,X15
        # SIMPLE格式：每行都是 X00Y00:值
        
        if first_line.startswith('X') and ',' in first_line:
            # TABLE格式
            return self.parse_table_format(lines, rows_updated)
        elif 'X' in first_line and 'Y' in first_line and ':' in first_line:
            # SIMPLE格式
            return self.parse_simple_format(lines, rows_updated)
        else:
            # 无法识别，返回False
            return False
    
    def parse_simple_format(self, lines, rows_updated=None):
        """解析简洁格式：X00Y00:值"""
        found = False
        for line in lines:
            line = line.strip()
            if not line:
                continue
            
            # 格式：X00Y00:值 或 X00Y00:值\r
            if ':' not in line:
                continue
            
            try:
                # 分离坐标和值
                if ':' in line:
                    coord_part, value_str = line.split(':', 1)
                    coord_part = coord_part.strip()
                    value_str = value_str.strip()
                else:
                    continue
                
                # 解析坐标 X00Y00
                if coord_part.startswith('X') and 'Y' in coord_part:
                    # 找到Y的位置
                    y_idx = coord_part.index('Y')
                    x_str = coord_part[1:y_idx]  # X后面的数字
                    y_str = coord_part[y_idx+1:]  # Y后面的数字
                    
                    try:
                        col = int(x_str)
                        row = int(y_str)
                        value = int(value_str)
                        
                        if 0 <= row < 16 and 0 <= col < 16:
                            self.matrix_data[row][col] = value
                            found = True
                            
                            if rows_updated is not None:
                                rows_updated.add(row)
                    except (ValueError, IndexError):
                        continue
            except (ValueError, IndexError, AttributeError):
                continue
        
        return found
    
    def parse_table_format(self, lines, rows_updated=None):
        """解析表格格式：第一行是列标题，后续行是数据"""
        if not lines:
            return False
        
        found = False
        
        # 第一行：列标题 X00,X01,X02,...,X15
        header_line = lines[0].strip()
        if not header_line.startswith('X'):
            return False
        
        # 解析列标题（确定列的顺序）
        col_headers = []
        for col_name in header_line.split(','):
            col_name = col_name.strip()
            if col_name.startswith('X'):
                try:
                    col_num = int(col_name[1:])
                    if 0 <= col_num < 16:
                        col_headers.append(col_num)
                except ValueError:
                    pass
        
        if len(col_headers) != 16:
            # 列标题不完整，可能解析失败
            return False
        
        # 后续行：Y00,值,值,值,...,值
        for line in lines[1:]:
            line = line.strip()
            if not line:
                continue
            
            if not line.startswith('Y'):
                continue
            
            try:
                # 分离行号和值
                parts = line.split(',')
                if len(parts) < 2:
                    continue
                
                # 第一个元素是行号 Y00
                row_str = parts[0].strip()
                if row_str.startswith('Y'):
                    row_num = int(row_str[1:])
                    if 0 <= row_num < 16:
                        # 解析该行的所有值
                        for col_idx, val_str in enumerate(parts[1:17]):  # 最多16列
                            if col_idx < len(col_headers):
                                col = col_headers[col_idx]
                                try:
                                    value = int(val_str.strip())
                                    if 0 <= col < 16:
                                        self.matrix_data[row_num][col] = value
                                        found = True
                                except (ValueError, IndexError):
                                    continue
                        
                        if rows_updated is not None:
                            rows_updated.add(row_num)
            except (ValueError, IndexError, AttributeError):
                continue
        
        return found
    
    def parse_matrix_data_old_format(self, line, rows_updated=None):
        """解析旧的矩阵数据格式（向后兼容）"""
        # 匹配 RowXX 开头且包含数值的行
        if not line.startswith('Row') or 'Row\\Col' in line or 'Capacitance Matrix' in line:
            return False
            
        try:
            # 解析: Row00   12345  23456 ... 或 Row00: 12345 23456 ...
            # 支持多种格式: "Row00:", "Row00 ", "Row00  " 等
            line_clean = line.replace(':', ' ').strip()
            parts = line_clean.split()
            
            if len(parts) >= 2:
                row_str = parts[0]
                if row_str.startswith('Row'):
                    # 提取行号 (Row00 -> 0, Row15 -> 15, Row0 -> 0)
                    try:
                        # 支持Row00和Row0两种格式
                        if len(row_str) >= 5:
                            row = int(row_str[3:5])  # Row00-Row15
                        elif len(row_str) >= 4:
                            row = int(row_str[3:4])  # Row0-Row9
                        else:
                            return False
                        
                        if row < 0 or row >= 16:
                            return False
                        
                        # 解析数值（跳过第一个元素RowXX）
                        # parts[1:17] 对应列0-15
                        found = False
                        for col_idx, val_str in enumerate(parts[1:17]):  # 最多16列
                            try:
                                value = int(val_str.strip())
                                if col_idx < 16:
                                    # row对应Y轴（行），col_idx对应X轴（列）
                                    self.matrix_data[row][col_idx] = value
                                    found = True
                            except (ValueError, IndexError):
                                # 如果某个值解析失败，继续处理下一个
                                continue
                        
                        # 记录更新的行
                        if rows_updated is not None:
                            rows_updated.add(row)
                            
                        return found
                    except (ValueError, IndexError):
                        pass
        except (ValueError, IndexError, AttributeError):
            pass
            
        return False
        
    def quantize_value(self, raw_value):
        """量化值"""
        if self.quant_min >= self.quant_max:
            return 0
            
        if raw_value < self.quant_min:
            return 0
        if raw_value > self.quant_max:
            return self.quant_level
            
        range_val = self.quant_max - self.quant_min
        quantized = int((raw_value - self.quant_min) * self.quant_level / range_val)
        return min(quantized, self.quant_level)
        
    def on_serial_status_changed(self, status, message):
        """串口状态变化"""
        self.statusBar().showMessage(message)
        
        # 如果串口断开，更新按钮状态并显示提示
        if status == "disconnected":
            # 强制更新按钮状态
            self.connect_btn.setEnabled(True)
            self.disconnect_btn.setEnabled(False)
            # 确保按钮状态正确显示
            self.connect_btn.update()
            self.disconnect_btn.update()
            # 显示断开消息（包含端口名称）
            port_name = self.current_port if self.current_port else "串口"
            self.message_log.add_message("系统", f"串口已断开 ({port_name})，请重新连接", "warning")
            # 清空当前端口
            self.current_port = None
        elif status == "connected":
            # 强制更新按钮状态
            self.connect_btn.setEnabled(False)
            self.disconnect_btn.setEnabled(True)
            # 确保按钮状态正确显示
            self.connect_btn.update()
            self.disconnect_btn.update()
        elif status == "error":
            # 连接错误时也允许重新连接
            self.connect_btn.setEnabled(True)
            self.disconnect_btn.setEnabled(False)
            self.connect_btn.update()
            self.disconnect_btn.update()
        
    def on_matrix_cell_clicked(self, row, col):
        """矩阵单元格单击"""
        self.tracked_point = (row, col)
        self.message_log.add_message("追踪", f"选择点 [{row}, {col}] - 双击可查看趋势图", "info")
            
    def on_matrix_cell_double_clicked(self, row, col):
        """矩阵单元格双击 - 弹出趋势图对话框"""
        from gui.trend_dialog import TrendDialog
        
        # 检查是否已有该点的对话框
        dialog_key = (row, col)
        if dialog_key in self.trend_dialogs:
            # 如果对话框已存在，激活并显示
            dialog = self.trend_dialogs[dialog_key]
            if dialog:
                dialog.raise_()
                dialog.activateWindow()
                return
        else:
            # 创建新的对话框
            dialog = TrendDialog(self, row, col, self.db_manager)
            dialog.finished.connect(lambda: self.on_trend_dialog_closed(dialog_key))
            self.trend_dialogs[dialog_key] = dialog
            dialog.show()
            
    def on_trend_dialog_closed(self, dialog_key):
        """趋势对话框关闭时的处理"""
        if dialog_key in self.trend_dialogs:
            del self.trend_dialogs[dialog_key]
        
    def update_matrix_display(self):
        """更新矩阵显示（批量更新，提高性能）"""
        # 如果数据没有变化且不是测试模式，跳过更新
        if not self.matrix_data_changed and not self.test_mode_active:
            return
        
        # In test mode, keep all values at 0 to display black
        if self.test_mode_active:
            # Ensure all values are 0 during test mode
            for row in range(16):
                for col in range(16):
                    if self.matrix_data[row][col] != 0:
                        self.matrix_data[row][col] = 0
        
        self.matrix_widget.update_matrix(
            self.matrix_data, 
            self.output_mode,
            self.quant_min if self.output_mode == "quant" else 0,
            self.quant_max if self.output_mode == "quant" else 100000,
            self.quant_level if self.output_mode == "quant" else 255,
            color_enabled=self.color_mapping_enabled
        )
        
        # 更新所有打开的对话框（实时更新）
        for dialog_key, dialog in list(self.trend_dialogs.items()):
            if dialog:
                row, col = dialog.row, dialog.col
                value = self.matrix_data[row][col]
                if self.output_mode == "quant":
                    value = self.quantize_value(value)
                dialog.add_point(value)
        
        # 重置变化标志
        self.matrix_data_changed = False

    def on_color_toggle(self, checked):
        """切换颜色映射开关"""
        self.color_mapping_enabled = bool(checked)
        self.color_toggle_btn.setText("色彩映射: 开" if checked else "色彩映射: 关")
        # 立即刷新显示，减少颜色计算或恢复颜色
        self.matrix_data_changed = True
        self.update_matrix_display()
            
    def closeEvent(self, event):
        """关闭事件"""
        # Close all trend dialogs
        for dialog_key, dialog in list(self.trend_dialogs.items()):
            if dialog:
                try:
                    dialog.close()
                except:
                    pass
        self.trend_dialogs.clear()
        
        # Close message log popout window if exists
        if self.message_log and hasattr(self.message_log, 'popout_window'):
            if self.message_log.popout_window:
                try:
                    self.message_log.popout_window.close()
                except:
                    pass
        
        # Stop all timers first
        if hasattr(self, 'update_timer') and self.update_timer.isActive():
            self.update_timer.stop()
        
        # Disconnect serial communication (with error handling)
        try:
            self.serial_comm.disconnect()
        except Exception as e:
            # Ignore disconnect errors (port may already be closed)
            pass
        
        # Close database
        if self.db_manager:
            try:
                self.db_manager.close()
            except:
                pass
            
        event.accept()

