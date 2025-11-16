#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
从 1.txt 解析寄存器详细说明
提取每个寄存器和位域的详细描述
"""

import re
import json
import os

def parse_register_docs(file_path):
    """解析 1.txt 中的寄存器详细说明"""
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    registers = {}
    current_reg = None
    current_description = []
    
    # 匹配寄存器标题：### 寄存器 X (0xXX) - 描述
    reg_pattern = r'###\s*寄存器\s*(\d+)\s*\(0x([0-9A-Fa-f]+)\)\s*-\s*(.+?)(?=\n|$)'
    
    # 匹配位域：- **位[X:Y] FIELD_NAME**: 描述
    bit_field_pattern = r'-?\s*\*\*位\[(\d+)(?::(\d+))?\]\s*([A-Z_0-9]+)\*\*:\s*(.+?)(?=\n|$)'
    
    # 匹配选项：- X: 描述
    option_pattern = r'^\s*-\s*(\d+|[A-Z_0-9]+)\s*[：:]\s*(.+?)(?=\n|$)'
    
    lines = content.split('\n')
    i = 0
    
    while i < len(lines):
        line = lines[i]
        
        # 检查是否是寄存器标题
        reg_match = re.match(reg_pattern, line)
        if reg_match:
            # 保存上一个寄存器
            if current_reg is not None:
                registers[current_reg['number']] = current_reg
            
            # 开始新寄存器
            reg_num = int(reg_match.group(1))
            hex_addr = reg_match.group(2)
            reg_name = reg_match.group(3).strip()
            
            current_reg = {
                'number': reg_num,
                'hex_address': f'0x{hex_addr.upper()}',
                'name': reg_name,
                'description': '',
                'bit_fields': {},
                'notes': []
            }
            current_description = []
            i += 1
            continue
        
        # 检查是否是位域定义
        bit_match = re.match(bit_field_pattern, line)
        if bit_match and current_reg is not None:
            bit_high = int(bit_match.group(1))
            bit_low = int(bit_match.group(2)) if bit_match.group(2) else bit_high
            field_name = bit_match.group(3)
            field_desc = bit_match.group(4).strip()
            
            # 读取后续的选项
            options = []
            j = i + 1
            while j < len(lines) and lines[j].strip() and not lines[j].strip().startswith('**'):
                opt_match = re.match(option_pattern, lines[j])
                if opt_match:
                    opt_value = opt_match.group(1)
                    opt_desc = opt_match.group(2).strip()
                    options.append({
                        'value': opt_value,
                        'description': opt_desc
                    })
                j += 1
            
            current_reg['bit_fields'][field_name] = {
                'name': field_name,
                'bits': f'[{bit_high}:{bit_low}]' if bit_high != bit_low else f'[{bit_high}]',
                'bit_high': bit_high,
                'bit_low': bit_low,
                'description': field_desc,
                'options': options
            }
            i = j
            continue
        
        # 检查是否是说明或注意事项
        if current_reg is not None:
            if line.strip().startswith('**说明**') or line.strip().startswith('**注意**'):
                note_text = line.strip()
                j = i + 1
                while j < len(lines) and lines[j].strip() and not lines[j].strip().startswith('###'):
                    note_text += ' ' + lines[j].strip()
                    j += 1
                current_reg['notes'].append(note_text)
                i = j
                continue
            
            # 收集描述文本
            if line.strip() and not line.strip().startswith('-') and not line.strip().startswith('|'):
                current_description.append(line.strip())
        
        i += 1
    
    # 保存最后一个寄存器
    if current_reg is not None:
        registers[current_reg['number']] = current_reg
    
    # 合并描述
    for reg_num, reg_data in registers.items():
        reg_data['description'] = ' '.join([d for d in current_description if d])
    
    return registers


def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.dirname(script_dir)
    
    # 查找 1.txt 文件（可能在项目根目录或上一级目录）
    txt_file = os.path.join(project_root, '1.txt')
    if not os.path.exists(txt_file):
        # 尝试上一级目录
        parent_dir = os.path.dirname(project_root)
        txt_file = os.path.join(parent_dir, '1.txt')
        if not os.path.exists(txt_file):
            print(f"错误: 文件不存在: {txt_file}")
            print(f"已尝试路径:")
            print(f"  1. {os.path.join(project_root, '1.txt')}")
            print(f"  2. {txt_file}")
            return
    
    registers = parse_register_docs(txt_file)
    
    # 保存为 JSON
    output_file = os.path.join(script_dir, 'register_docs.json')
    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump(registers, f, ensure_ascii=False, indent=2)
    
    print(f"成功解析 {len(registers)} 个寄存器的详细说明")
    print(f"输出文件: {output_file}")


if __name__ == '__main__':
    main()

