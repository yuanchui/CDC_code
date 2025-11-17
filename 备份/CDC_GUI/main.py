#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
PCap04 USB CDC GUI Application
主程序入口
"""

import sys
import os
from PyQt5.QtWidgets import QApplication
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QIcon
from gui.main_window import MainWindow

def main():
    """主函数"""
    # Windows 任务栏图标与分组：设置 AppUserModelID，确保使用自定义图标而非 python.exe 图标
    try:
        if sys.platform.startswith('win'):
            import ctypes  # 延迟导入，避免非 Windows 平台报错
            ctypes.windll.shell32.SetCurrentProcessExplicitAppUserModelID(
                'PCap04Tools.PCap04MatrixMonitor.1.0'
            )
    except Exception:
        pass

    app = QApplication(sys.argv)
    app.setStyle('Fusion')  # 使用Fusion风格，更现代化
    
    # 设置应用信息
    app.setApplicationName("PCap04 Matrix Monitor")
    app.setApplicationVersion("1.0.0")
    app.setOrganizationName("PCap04 Tools")
    # 应用图标（任务栏）
    try:
        base_dir = os.path.dirname(os.path.abspath(__file__))
        icon_path = os.path.join(base_dir, "app.ico")
        if os.path.exists(icon_path):
            app.setWindowIcon(QIcon(icon_path))
    except Exception:
        pass
    
    # 创建主窗口
    window = MainWindow()
    # 窗口左上角图标
    try:
        if 'icon_path' in locals() and os.path.exists(icon_path):
            window.setWindowIcon(QIcon(icon_path))
    except Exception:
        pass
    window.show()
    
    sys.exit(app.exec_())

if __name__ == '__main__':
    main()

