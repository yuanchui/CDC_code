#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PCAP04 配置工具主程序
整合解析、GUI 和代码生成功能
"""

import os
import sys
import argparse
import json

def main():
    parser = argparse.ArgumentParser(description='PCAP04 寄存器配置工具')
    parser.add_argument('--mode', choices=['gui', 'parse', 'generate'], 
                       default='gui', help='运行模式: gui(图形界面), parse(解析C代码), generate(生成代码)')
    parser.add_argument('--input', help='输入文件路径')
    parser.add_argument('--output', help='输出文件路径')
    parser.add_argument('--config', help='配置文件路径 (JSON)')
    
    args = parser.parse_args()
    
    if args.mode == 'gui':
        # 启动 GUI
        try:
            import tkinter as tk
            from config_gui import PCAP04ConfigGUI
            root = tk.Tk()
            app = PCAP04ConfigGUI(root)
            root.mainloop()
        except ImportError:
            print("错误: 需要 tkinter 库。请安装: pip install tk")
            sys.exit(1)
    
    elif args.mode == 'parse':
        # 解析 C 代码
        from parse_c_code import CCodeParser
        
        if not args.input:
            # 默认路径
            script_dir = os.path.dirname(os.path.abspath(__file__))
            project_root = os.path.dirname(script_dir)
            args.input = os.path.join(project_root, 'Core', 'Src', 'pcap04_reg.c')
        
        if not os.path.exists(args.input):
            print(f"错误: 文件不存在: {args.input}")
            sys.exit(1)
        
        parser = CCodeParser()
        try:
            config = parser.parse_c_file(args.input)
            
            if args.output:
                output_file = args.output
            else:
                script_dir = os.path.dirname(os.path.abspath(__file__))
                output_file = os.path.join(script_dir, 'current_config.json')
            
            with open(output_file, 'w', encoding='utf-8') as f:
                f.write(parser.to_json(config))
            
            print(f"成功解析 {len(config)} 个配置项")
            print(f"输出文件: {output_file}")
            
        except Exception as e:
            print(f"解析错误: {e}")
            import traceback
            traceback.print_exc()
            sys.exit(1)
    
    elif args.mode == 'generate':
        # 生成 C 代码
        from code_generator import CodeGenerator
        
        if not args.config:
            script_dir = os.path.dirname(os.path.abspath(__file__))
            args.config = os.path.join(script_dir, 'gui_config.json')
        
        if not os.path.exists(args.config):
            print(f"错误: 配置文件不存在: {args.config}")
            sys.exit(1)
        
        # 加载配置
        with open(args.config, 'r', encoding='utf-8') as f:
            gui_config = json.load(f)
        
        # 加载 UI 配置（用于获取寄存器信息）
        script_dir = os.path.dirname(os.path.abspath(__file__))
        ui_config_file = os.path.join(script_dir, 'ui_options.json')
        ui_config = {}
        if os.path.exists(ui_config_file):
            with open(ui_config_file, 'r', encoding='utf-8') as f:
                ui_config = json.load(f)
        
        generator = CodeGenerator()
        code = generator.generate_c_code(gui_config, ui_config)
        
        if args.output:
            output_file = args.output
        else:
            output_file = os.path.join(script_dir, 'generated_config.c')
        
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write(code)
        
        print(f"代码已生成: {output_file}")
        print("\n生成的代码预览:")
        print("=" * 80)
        print(code[:500] + "..." if len(code) > 500 else code)
        print("=" * 80)


if __name__ == '__main__':
    main()

