#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
从 pcap04_reg.c 解析当前配置
提取 PCAP04_DefaultConfigItems[] 数组中的配置值
"""

import re
import os
import json
from typing import Dict, List, Any, Optional

class CCodeParser:
    def __init__(self):
        self.config_items = {}
        self.macro_patterns = {
            'PCAP04_REG0_VALUE': r'PCAP04_REG0_VALUE\s*\(\s*(\d+)\s*,\s*(\d+)\s*,\s*([^)]+)\s*\)',
            'PCAP04_REG1_VALUE': r'PCAP04_REG1_VALUE\s*\(\s*([^,]+)\s*,\s*([^,]+)\s*,\s*([^)]+)\s*\)',
            'PCAP04_REG2_VALUE': r'PCAP04_REG2_VALUE\s*\(\s*([^,]+)\s*,\s*([^,]+)\s*,\s*([^,]+)\s*,\s*([^)]+)\s*\)',
            'PCAP04_REG3_VALUE': r'PCAP04_REG3_VALUE\s*\(\s*([^,]+)\s*,\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*,\s*([^)]+)\s*\)',
            'PCAP04_REG4_VALUE': r'PCAP04_REG4_VALUE\s*\(\s*([^,]+)\s*,\s*([^,]+)\s*,\s*([^,]+)\s*,\s*([^,]+)\s*,\s*([^)]+)\s*\)',
            'PCAP04_REG5_VALUE': r'PCAP04_REG5_VALUE\s*\(\s*([^,]+)\s*,\s*([^,]+)\s*,\s*([^,]+)\s*,\s*([^,]+)\s*,\s*([^,]+)\s*,\s*([^)]+)\s*\)',
            'PCAP04_REG6_VALUE': r'PCAP04_REG6_VALUE\s*\(\s*([^)]+)\s*\)',
            'PCAP04_C_AVRG_CONFIG': r'PCAP04_C_AVRG_CONFIG\s*\(\s*(\d+)\s*\)',
            'PCAP04_CONV_TIME_CONFIG': r'PCAP04_CONV_TIME_CONFIG\s*\(\s*(\d+)\s*\)',
            'PCAP04_DISCHARGE_TIME_AND_REG13_CONFIG': r'PCAP04_DISCHARGE_TIME_AND_REG13_CONFIG\s*\(\s*(\d+)\s*,\s*([^,]+)\s*,\s*([^)]+)\s*\)',
            'PCAP04_PRECHARGE_TIME_AND_REG15_CONFIG': r'PCAP04_PRECHARGE_TIME_AND_REG15_CONFIG\s*\(\s*(\d+)\s*,\s*(\d+)\s*\)',
            'PCAP04_FULLCHARGE_TIME_AND_REG17_CONFIG': r'PCAP04_FULLCHARGE_TIME_AND_REG17_CONFIG\s*\(\s*(\d+)\s*,\s*(\d+)\s*\)',
        }
    
    def parse_c_file(self, file_path: str) -> Dict[str, Any]:
        """解析 C 文件，提取配置项"""
        with open(file_path, 'r', encoding='utf-8') as f:
            content = f.read()
        
        # 找到 PCAP04_DefaultConfigItems 数组
        array_start = content.find('static const PCAP04_RegConfigItem_t PCAP04_DefaultConfigItems[] = {')
        if array_start == -1:
            raise ValueError("未找到 PCAP04_DefaultConfigItems 数组")
        
        array_end = content.find('};', array_start)
        if array_end == -1:
            raise ValueError("未找到数组结束标记")
        
        array_content = content[array_start:array_end]
        
        config = {}
        reg_num = 0
        
        # 解析每个配置项
        lines = array_content.split('\n')
        i = 0
        while i < len(lines):
            line = lines[i].strip()
            
            # 跳过注释和空行
            if not line or line.startswith('//'):
                i += 1
                continue
            
            # 查找寄存器地址
            reg_addr_match = re.search(r'PCAP04_REG_ADDR_(\d+)', line)
            if reg_addr_match:
                reg_addr = int(reg_addr_match.group(1))
                
                # 查找对应的值
                value_match = None
                value_line = line
                
                # 检查是否是宏展开
                for macro_name, pattern in self.macro_patterns.items():
                    match = re.search(pattern, value_line)
                    if match:
                        value_match = {
                            'type': macro_name,
                            'params': match.groups(),
                            'raw': match.group(0)
                        }
                        break
                
                # 如果是直接值
                if not value_match:
                    # 查找 0xXX 或数字
                    hex_match = re.search(r'0x([0-9A-Fa-f]+)', value_line)
                    if hex_match:
                        value_match = {
                            'type': 'direct',
                            'value': int(hex_match.group(1), 16),
                            'raw': hex_match.group(0)
                        }
                    else:
                        # 查找宏常量
                        const_match = re.search(r'PCAP04_[A-Z0-9_]+', value_line)
                        if const_match:
                            value_match = {
                                'type': 'constant',
                                'name': const_match.group(0),
                                'raw': const_match.group(0)
                            }
                
                if value_match:
                    config[f'Reg{reg_addr}'] = {
                        'register_number': reg_addr,
                        'hex_address': f'0x{reg_addr:02X}',
                        'parsed_value': value_match,
                        'raw_line': value_line
                    }
            
            i += 1
        
        return config
    
    def extract_port_enable(self, value_str: str) -> List[int]:
        """提取端口使能位（如 PCAP04_REG6_C_PORT_EN_PC0 | PCAP04_REG6_C_PORT_EN_PC1）"""
        ports = []
        for i in range(6):
            if f'PC{i}' in value_str or f'C_PORT_EN_PC{i}' in value_str:
                ports.append(i)
        return ports
    
    def parse_macro_constant(self, const_name: str) -> Optional[Any]:
        """解析宏常量值（需要读取头文件）"""
        # 这里可以扩展为读取头文件并解析宏定义
        # 暂时返回常量名
        return const_name
    
    def to_json(self, config: Dict[str, Any]) -> str:
        """转换为 JSON 格式"""
        return json.dumps(config, indent=2, ensure_ascii=False)


def main():
    # 获取脚本目录
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.dirname(script_dir)
    
    # C 文件路径
    c_file = os.path.join(project_root, 'Core', 'Src', 'pcap04_reg.c')
    
    if not os.path.exists(c_file):
        print(f"错误: 文件不存在: {c_file}")
        return
    
    parser = CCodeParser()
    try:
        config = parser.parse_c_file(c_file)
        
        # 保存 JSON
        output_file = os.path.join(script_dir, 'current_config.json')
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write(parser.to_json(config))
        
        print(f"成功解析 {len(config)} 个配置项")
        print(f"输出文件: {output_file}")
        
    except Exception as e:
        print(f"解析错误: {e}")
        import traceback
        traceback.print_exc()


if __name__ == '__main__':
    main()

