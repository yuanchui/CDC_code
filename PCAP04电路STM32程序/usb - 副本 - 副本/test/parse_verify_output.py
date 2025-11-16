#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
解析 verify_config.exe 的输出，提取 PCAP04_Config 的详细信息
"""

import json
import re
import subprocess
import os
import sys

def parse_verify_output(output_text):
    """解析 verify_config.exe 的输出"""
    # 查找 JSON 部分（使用标记）
    json_start_marker = '===JSON_START==='
    json_end_marker = '===JSON_END==='
    
    start_idx = output_text.find(json_start_marker)
    end_idx = output_text.find(json_end_marker)
    
    if start_idx == -1 or end_idx == -1:
        # 如果没有标记，尝试查找 JSON 对象
        json_start = output_text.find('{')
        json_end = output_text.rfind('}') + 1
        
        if json_start == -1 or json_end == 0:
            print("错误: 未找到 JSON 数据")
            return None
        
        json_text = output_text[json_start:json_end]
    else:
        # 提取标记之间的内容
        json_text = output_text[start_idx + len(json_start_marker):end_idx].strip()
    
    try:
        config = json.loads(json_text)
        return config
    except json.JSONDecodeError as e:
        print(f"JSON 解析错误: {e}")
        print(f"JSON 文本片段: {json_text[:200]}...")
        return None

def extract_bit_fields(reg_num, value):
    """根据寄存器编号和值提取位域信息"""
    bit_fields = {}
    
    # 寄存器 0: I2C_A[7:6], OLF_FTUNE[5:2], OLF_CTUNE[1:0]
    if reg_num == 0:
        bit_fields['I2C_A'] = {
            'bits': '[7:6]',
            'value': (value >> 6) & 0x03,
            'description': 'I²C设备地址补码配置'
        }
        bit_fields['OLF_FTUNE'] = {
            'bits': '[5:2]',
            'value': (value >> 2) & 0x0F,
            'description': '低频时钟微调'
        }
        bit_fields['OLF_CTUNE'] = {
            'bits': '[1:0]',
            'value': value & 0x03,
            'description': '低频时钟粗调'
        }
    
    # 寄存器 1: OX_DIS[7], OX_DIV4[5], OX_RUN[2:0]
    elif reg_num == 1:
        bit_fields['OX_DIS'] = {
            'bits': '[7]',
            'value': (value >> 7) & 0x01,
            'description': '禁用OX时钟'
        }
        bit_fields['OX_DIV4'] = {
            'bits': '[5]',
            'value': (value >> 5) & 0x01,
            'description': 'OX时钟4分频'
        }
        bit_fields['OX_RUN'] = {
            'bits': '[2:0]',
            'value': value & 0x07,
            'description': '控制OX发生器的持续性或延迟'
        }
    
    # 寄存器 2: RDCHG_INT_SEL1[7:6], RDCHG_INT_SEL0[5:4], RDCHG_INT_EN[3], RDCHG_EXT_EN[1]
    elif reg_num == 2:
        bit_fields['RDCHG_INT_SEL1'] = {
            'bits': '[7:6]',
            'value': (value >> 6) & 0x03,
            'description': 'PC4~PC5的片上放电电阻选择'
        }
        bit_fields['RDCHG_INT_SEL0'] = {
            'bits': '[5:4]',
            'value': (value >> 4) & 0x03,
            'description': 'PC0~PC3和内部端口PC6的片上放电电阻选择'
        }
        bit_fields['RDCHG_INT_EN'] = {
            'bits': '[3]',
            'value': (value >> 3) & 0x01,
            'description': '内部放电电阻使能'
        }
        bit_fields['RDCHG_EXT_EN'] = {
            'bits': '[1]',
            'value': (value >> 1) & 0x01,
            'description': '外部放电电阻使能'
        }
    
    # 寄存器 6: C_PORT_EN[5:0]
    elif reg_num == 6:
        bit_fields['C_PORT_EN'] = {
            'bits': '[5:0]',
            'value': value & 0x3F,
            'description': '位使能CDC端口（PC0到PC5）',
            'ports': []
        }
        for port in range(6):
            if (value >> port) & 0x01:
                bit_fields['C_PORT_EN']['ports'].append(f'PC{port}')
    
    # 可以继续添加其他寄存器的位域解析...
    
    return bit_fields

def run_verify_config():
    """运行 verify_config.exe 并获取输出"""
    script_dir = os.path.dirname(os.path.abspath(__file__))
    exe_path = os.path.join(script_dir, 'verify_config.exe')
    
    if not os.path.exists(exe_path):
        print(f"错误: verify_config.exe 不存在: {exe_path}")
        print("请先编译: gcc -o verify_config.exe verify_config.c Core/Src/pcap04_reg.c -I Core/Inc -D__GNUC__")
        return None
    
    try:
        result = subprocess.run([exe_path], capture_output=True, text=True, encoding='utf-8', errors='ignore')
        # 合并 stdout 和 stderr（JSON 输出到 stderr）
        output = result.stdout + result.stderr
        return output
    except Exception as e:
        print(f"运行错误: {e}")
        return None

def main():
    # 运行 verify_config.exe
    output = run_verify_config()
    if not output:
        return
    
    # 解析输出
    config = parse_verify_output(output)
    if not config:
        print("无法解析输出")
        return
    
    # 增强配置信息（添加位域解析）
    enhanced_config = {
        'summary': {
            'total_registers': 64,
            'non_zero_count': sum(1 for r in config['registers'] if r['value_decimal'] != 0),
            'zero_count': sum(1 for r in config['registers'] if r['value_decimal'] == 0)
        },
        'registers': []
    }
    
    for reg in config['registers']:
        reg_num = reg['reg_num']
        value = reg['value_decimal']
        
        enhanced_reg = reg.copy()
        enhanced_reg['bit_fields'] = extract_bit_fields(reg_num, value)
        
        enhanced_config['registers'].append(enhanced_reg)
    
    # 保存结果
    script_dir = os.path.dirname(os.path.abspath(__file__))
    output_file = os.path.join(script_dir, 'parsed_config_detailed.json')
    
    with open(output_file, 'w', encoding='utf-8') as f:
        json.dump(enhanced_config, f, ensure_ascii=False, indent=2)
    
    print(f"详细配置已保存到: {output_file}")
    print(f"非零寄存器: {enhanced_config['summary']['non_zero_count']}/64")

if __name__ == '__main__':
    main()

