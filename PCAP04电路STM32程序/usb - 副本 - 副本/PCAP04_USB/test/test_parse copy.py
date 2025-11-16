#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""快速测试解析脚本"""

import sys
import os

# 添加当前目录到路径
sys.path.insert(0, os.path.dirname(__file__))

from parse_config import parse_register_config, extract_current_config_values, format_output
import json

# 直接指定文件路径
script_dir = os.path.dirname(os.path.abspath(__file__))
project_root = os.path.dirname(os.path.dirname(script_dir))
file_path = os.path.join(project_root, '1.txt')

print(f"脚本目录: {script_dir}")
print(f"项目根目录: {project_root}")
print(f"尝试读取文件: {file_path}")
print(f"文件存在: {os.path.exists(file_path)}")

if os.path.exists(file_path):
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()
    print(f"成功读取文件 ({len(content)} 字符)\n")
    
    # 解析
    registers = parse_register_config(content)
    current_values = extract_current_config_values(content)
    
    # 补充当前值
    for reg_name, value in current_values.items():
        if reg_name in registers and not registers[reg_name]["current_value"]:
            registers[reg_name]["current_value"] = value
    
    # 保存JSON
    json_output = format_output(registers, "json")
    with open(os.path.join(script_dir, 'parsed_config.json'), 'w', encoding='utf-8') as f:
        f.write(json_output)
    
    # 保存Markdown
    md_output = format_output(registers, "markdown")
    with open(os.path.join(script_dir, 'parsed_config.md'), 'w', encoding='utf-8') as f:
        f.write(md_output)
    
    # 生成UI选项
    ui_options = {}
    for reg_name, reg in registers.items():
        if reg["bit_fields"]:
            ui_options[reg_name] = {
                "register": reg_name,
                "hex_address": reg["hex_address"],
                "current_value": reg["current_value"],
                "configurable_fields": []
            }
            for bit_field in reg["bit_fields"]:
                if bit_field["options"]:
                    ui_options[reg_name]["configurable_fields"].append({
                        "field_name": bit_field["name"],
                        "bits": bit_field["bits"],
                        "options": [
                            {
                                "value": opt["value"],
                                "label": opt["description"],
                                "is_default": opt["is_default"]
                            }
                            for opt in bit_field["options"]
                        ]
                    })
    
    with open(os.path.join(script_dir, 'ui_options.json'), 'w', encoding='utf-8') as f:
        json.dump(ui_options, f, ensure_ascii=False, indent=2)
    
    print(f"已解析 {len(registers)} 个寄存器")
    print(f"已生成配置文件:")
    print(f"  - {os.path.join(script_dir, 'parsed_config.json')}")
    print(f"  - {os.path.join(script_dir, 'parsed_config.md')}")
    print(f"  - {os.path.join(script_dir, 'ui_options.json')}")
else:
    print("错误: 文件不存在!")

