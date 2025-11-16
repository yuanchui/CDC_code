#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
PCAP04 配置 GUI 工具
提供可视化界面配置 PCAP04 寄存器，并生成 C 代码
"""

import tkinter as tk
from tkinter import ttk, scrolledtext, messagebox, filedialog
import json
import os
from typing import Dict, Any, List
from code_generator import CodeGenerator

class PCAP04ConfigGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("PCAP04 寄存器配置工具")
        self.root.geometry("1200x800")
        
        # 数据
        self.register_config = {}  # 从 JSON 加载的寄存器配置
        self.ui_config = {}  # GUI 配置选项
        self.current_values = {}  # 当前配置值
        self.register_docs = {}  # 从 1.txt 解析的详细说明
        self.parsed_config = {}  # 从 verify_config.exe 解析的当前配置
        
        # 创建界面
        self.create_widgets()
        
        # 加载配置
        self.load_configs()
    
    def create_widgets(self):
        """创建 GUI 组件"""
        # 主框架
        main_frame = ttk.Frame(self.root, padding="10")
        main_frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        
        # 配置网格权重
        self.root.columnconfigure(0, weight=1)
        self.root.rowconfigure(0, weight=1)
        main_frame.columnconfigure(1, weight=1)
        main_frame.rowconfigure(1, weight=1)
        
        # 工具栏
        toolbar = ttk.Frame(main_frame)
        toolbar.grid(row=0, column=0, columnspan=2, sticky=(tk.W, tk.E), pady=(0, 10))
        
        ttk.Button(toolbar, text="加载配置", command=self.load_configs).pack(side=tk.LEFT, padx=5)
        ttk.Button(toolbar, text="保存配置", command=self.save_config).pack(side=tk.LEFT, padx=5)
        ttk.Button(toolbar, text="生成代码", command=self.generate_code).pack(side=tk.LEFT, padx=5)
        ttk.Button(toolbar, text="导出 JSON", command=self.export_json).pack(side=tk.LEFT, padx=5)
        
        # 左侧：寄存器列表
        left_frame = ttk.LabelFrame(main_frame, text="寄存器列表", padding="5")
        left_frame.grid(row=1, column=0, sticky=(tk.W, tk.E, tk.N, tk.S), padx=(0, 10))
        left_frame.columnconfigure(0, weight=1)
        left_frame.rowconfigure(0, weight=1)
        
        # 寄存器列表树（通过列配置设置宽度）
        self.reg_tree = ttk.Treeview(left_frame, show="tree", height=30)
        self.reg_tree.column('#0', width=400, minwidth=300)  # 设置列宽度
        self.reg_tree.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        
        # 滚动条
        reg_scroll = ttk.Scrollbar(left_frame, orient=tk.VERTICAL, command=self.reg_tree.yview)
        reg_scroll.grid(row=0, column=1, sticky=(tk.N, tk.S))
        self.reg_tree.configure(yscrollcommand=reg_scroll.set)
        
        # 绑定选择事件
        self.reg_tree.bind('<<TreeviewSelect>>', self.on_register_select)
        
        # 右侧：配置面板
        right_frame = ttk.LabelFrame(main_frame, text="寄存器配置", padding="10")
        right_frame.grid(row=1, column=1, sticky=(tk.W, tk.E, tk.N, tk.S))
        right_frame.columnconfigure(0, weight=1)
        
        # 配置面板容器（使用 Canvas 实现滚动）
        canvas = tk.Canvas(right_frame)
        scrollbar = ttk.Scrollbar(right_frame, orient="vertical", command=canvas.yview)
        # 使用普通 Frame 而不是 ttk.Frame，以便支持 bg 选项
        self.config_frame = tk.Frame(canvas)
        
        self.config_frame.bind(
            "<Configure>",
            lambda e: canvas.configure(scrollregion=canvas.bbox("all"))
        )
        
        canvas.create_window((0, 0), window=self.config_frame, anchor="nw")
        canvas.configure(yscrollcommand=scrollbar.set)
        
        canvas.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        scrollbar.grid(row=0, column=1, sticky=(tk.N, tk.S))
        right_frame.rowconfigure(0, weight=1)
        right_frame.columnconfigure(0, weight=1)
        
        self.config_canvas = canvas
        
        # 底部：代码预览
        bottom_frame = ttk.LabelFrame(main_frame, text="生成的 C 代码预览", padding="5")
        bottom_frame.grid(row=2, column=0, columnspan=2, sticky=(tk.W, tk.E, tk.N, tk.S), pady=(10, 0))
        bottom_frame.columnconfigure(0, weight=1)
        bottom_frame.rowconfigure(0, weight=1)
        
        self.code_text = scrolledtext.ScrolledText(bottom_frame, height=15, font=('Consolas', 10))
        self.code_text.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
    
    def load_configs(self):
        """加载配置文件"""
        script_dir = os.path.dirname(os.path.abspath(__file__))
        
        # 加载 UI 配置
        ui_file = os.path.join(script_dir, 'ui_options.json')
        if os.path.exists(ui_file):
            with open(ui_file, 'r', encoding='utf-8') as f:
                self.ui_config = json.load(f)
        else:
            messagebox.showwarning("警告", f"未找到配置文件: {ui_file}")
            return
        
        # 加载寄存器详细说明（从 1.txt 解析）
        docs_file = os.path.join(script_dir, 'register_docs.json')
        if os.path.exists(docs_file):
            with open(docs_file, 'r', encoding='utf-8') as f:
                docs_data = json.load(f)
                # 转换为以 RegX 为键的格式
                for reg_num, reg_doc in docs_data.items():
                    self.register_docs[f'Reg{reg_num}'] = reg_doc
        
        # 加载从 verify_config.exe 解析的当前配置
        parsed_file = os.path.join(script_dir, 'parsed_config_detailed.json')
        if os.path.exists(parsed_file):
            with open(parsed_file, 'r', encoding='utf-8') as f:
                self.parsed_config = json.load(f)
        
        # 加载当前配置（如果存在）
        current_file = os.path.join(script_dir, 'current_config.json')
        if os.path.exists(current_file):
            with open(current_file, 'r', encoding='utf-8') as f:
                self.current_values = json.load(f)
        
        # 构建寄存器树
        self.build_register_tree()
    
    def build_register_tree(self):
        """构建寄存器树形列表"""
        self.reg_tree.delete(*self.reg_tree.get_children())
        
        # 按寄存器编号排序
        regs = sorted(self.ui_config.items(), key=lambda x: int(x[0].replace('Reg', '')))
        
        # 分组：CDC配置、RDC配置、固件配置
        cdc_node = self.reg_tree.insert('', 'end', 'CDC', text='CDC配置 (0-19)', open=True)
        rdc_node = self.reg_tree.insert('', 'end', 'RDC', text='RDC配置 (20-39)', open=True)
        fw_node = self.reg_tree.insert('', 'end', 'FW', text='固件配置 (40-63)', open=True)
        
        for reg_name, reg_data in regs:
            reg_num = int(reg_name.replace('Reg', ''))
            hex_addr = reg_data.get('hex_address', f'0x{reg_num:02X}')
            current_val = reg_data.get('current_value', 'N/A')
            
            # 获取当前配置值（从 parsed_config）
            bit_field_info = ""
            if self.parsed_config and 'registers' in self.parsed_config:
                for reg in self.parsed_config['registers']:
                    if reg['reg_num'] == reg_num:
                        bit_fields = reg.get('bit_fields', {})
                        # 对于寄存器0，显示OLF_FTUNE和OLF_CTUNE
                        if reg_num == 0:
                            olf_ftune = bit_fields.get('OLF_FTUNE', {}).get('value', '?')
                            olf_ctune = bit_fields.get('OLF_CTUNE', {}).get('value', '?')
                            # 将OLF_CTUNE值转换为频率描述
                            ctune_desc = {0: '10k', 1: '50k', 2: '100k', 3: '200k'}.get(olf_ctune, f'{olf_ctune}')
                            bit_field_info = f" | OLF_FTUNE:{olf_ftune} OLF_CTUNE:{ctune_desc}Hz"
                        break
            
            # 选择父节点
            if reg_num < 20:
                parent = cdc_node
            elif reg_num < 40:
                parent = rdc_node
            else:
                parent = fw_node
            
            # 插入寄存器节点
            display_text = f"寄存器 {reg_num} ({hex_addr}) - 值: {current_val}{bit_field_info}"
            node_id = self.reg_tree.insert(
                parent, 'end', reg_name,
                text=display_text
            )
    
    def on_register_select(self, event):
        """寄存器选择事件"""
        selection = self.reg_tree.selection()
        if not selection:
            return
        
        reg_name = selection[0]
        if reg_name in ['CDC', 'RDC', 'FW']:
            return
        
        # 清空配置面板
        for widget in self.config_frame.winfo_children():
            widget.destroy()
        
        # 显示寄存器配置
        if reg_name in self.ui_config:
            self.show_register_config(reg_name, self.ui_config[reg_name])
    
    def show_register_config(self, reg_name: str, reg_data: Dict[str, Any]):
        """显示寄存器配置选项"""
        row = 0
        
        # 寄存器信息
        reg_num = int(reg_name.replace('Reg', ''))
        hex_addr = reg_data.get('hex_address', f'0x{reg_num:02X}')
        
        # 获取当前配置值（从 parsed_config）
        current_value = None
        current_bit_fields = {}
        if self.parsed_config and 'registers' in self.parsed_config:
            for reg in self.parsed_config['registers']:
                if reg['reg_num'] == reg_num:
                    current_value = reg.get('value', 'N/A')
                    current_bit_fields = reg.get('bit_fields', {})
                    break
        
        # 寄存器标题
        title_text = f"寄存器 {reg_num} ({hex_addr})"
        if current_value:
            title_text += f" - 当前值: {current_value}"
        
        info_label = ttk.Label(
            self.config_frame,
            text=title_text,
            font=('Arial', 12, 'bold')
        )
        info_label.grid(row=row, column=0, columnspan=2, sticky=tk.W, pady=(0, 5))
        row += 1
        
        # 获取寄存器详细说明（从 1.txt）
        reg_doc = self.register_docs.get(reg_name)
        
        # 显示寄存器详细说明（从 1.txt）
        if reg_doc:
            # 寄存器名称和描述
            if reg_doc.get('name'):
                name_label = ttk.Label(
                    self.config_frame,
                    text=f"名称: {reg_doc['name']}",
                    font=('Arial', 10, 'italic'),
                    foreground='gray'
                )
                name_label.grid(row=row, column=0, columnspan=2, sticky=tk.W, pady=(0, 5))
                row += 1
            
            # 详细说明
            if reg_doc.get('description'):
                desc_text = reg_doc['description']
                # 使用 Text 组件支持多行文本和换行
                desc_frame = ttk.Frame(self.config_frame)
                desc_frame.grid(row=row, column=0, columnspan=2, sticky=(tk.W, tk.E), pady=(0, 10))
                # 获取系统默认背景色
                try:
                    bg_color = self.root.cget('bg')
                except:
                    bg_color = 'SystemButtonFace'  # Windows 默认背景色
                desc_label = tk.Text(
                    desc_frame,
                    height=3,
                    wrap=tk.WORD,
                    font=('Arial', 9),
                    bg=bg_color,
                    relief=tk.FLAT,
                    padx=5,
                    pady=5
                )
                desc_label.insert('1.0', desc_text)
                desc_label.config(state=tk.DISABLED)
                desc_label.pack(fill=tk.BOTH, expand=True)
                row += 1
            
            # 注意事项
            if reg_doc.get('notes'):
                notes_text = '\n'.join(reg_doc['notes'])
                notes_frame = ttk.LabelFrame(self.config_frame, text="注意事项", padding="5")
                notes_frame.grid(row=row, column=0, columnspan=2, sticky=(tk.W, tk.E), pady=(0, 10))
                notes_label = tk.Text(
                    notes_frame,
                    height=2,
                    wrap=tk.WORD,
                    font=('Arial', 9),
                    bg='#fffacd',  # 浅黄色背景
                    relief=tk.FLAT,
                    padx=5,
                    pady=5
                )
                notes_label.insert('1.0', notes_text)
                notes_label.config(state=tk.DISABLED)
                notes_label.pack(fill=tk.BOTH, expand=True)
                row += 1
        
        # 分隔线
        separator = ttk.Separator(self.config_frame, orient='horizontal')
        separator.grid(row=row, column=0, columnspan=2, sticky=(tk.W, tk.E), pady=(0, 10))
        row += 1
        
        # 配置字段
        configurable_fields = reg_data.get('configurable_fields', [])
        
        if not configurable_fields:
            ttk.Label(self.config_frame, text="此寄存器无可配置字段（固定值）").grid(
                row=row, column=0, columnspan=2, sticky=tk.W
            )
            return
        
        # 存储控件引用
        if not hasattr(self, 'field_widgets'):
            self.field_widgets = {}
        self.field_widgets[reg_name] = {}
        
        for field in configurable_fields:
            field_name = field['field_name']
            bits = field['bits']
            options = field.get('options', [])
            
            # 获取字段详细说明（从 register_docs）
            field_desc = None
            field_current_value = None
            if reg_doc and 'bit_fields' in reg_doc:
                bit_field_info = reg_doc['bit_fields'].get(field_name)
                if bit_field_info:
                    field_desc = bit_field_info.get('description', '')
                    # 获取当前值
                    if field_name in current_bit_fields:
                        field_current_value = current_bit_fields[field_name].get('value')
            
            # 字段标签和说明
            field_frame = ttk.Frame(self.config_frame)
            field_frame.grid(row=row, column=0, columnspan=2, sticky=(tk.W, tk.E), pady=5)
            
            field_label_text = f"{field_name} {bits}:"
            if field_current_value is not None:
                field_label_text += f" (当前值: {field_current_value})"
            
            field_label = ttk.Label(
                field_frame,
                text=field_label_text,
                font=('Arial', 10, 'bold')
            )
            field_label.grid(row=0, column=0, sticky=tk.W)
            
            # 字段详细说明
            if field_desc:
                desc_label = ttk.Label(
                    field_frame,
                    text=field_desc,
                    font=('Arial', 9),
                    foreground='gray',
                    wraplength=600
                )
                desc_label.grid(row=1, column=0, sticky=tk.W, pady=(2, 0))
            
            row += 1
            
            # 字段控件
            if options:
                # 下拉框
                var = tk.StringVar()
                combo = ttk.Combobox(
                    self.config_frame,
                    textvariable=var,
                    width=50,
                    state='readonly'
                )
                
                # 填充选项（包含详细说明）
                option_list = []
                default_idx = 0
                for idx, opt in enumerate(options):
                    label = opt.get('label', f"值: {opt.get('value', '')}")
                    option_list.append(label)
                    if opt.get('is_default', False):
                        default_idx = idx
                    # 如果当前值匹配，使用当前值
                    if field_current_value is not None:
                        opt_value = opt.get('value', '')
                        try:
                            if int(opt_value) == field_current_value:
                                default_idx = idx
                        except (ValueError, TypeError):
                            pass
                
                combo['values'] = option_list
                combo.current(default_idx)
                combo.grid(row=row, column=1, sticky=(tk.W, tk.E), pady=5, padx=(10, 0))
                
                # 存储引用
                self.field_widgets[reg_name][field_name] = {
                    'type': 'combobox',
                    'var': var,
                    'combo': combo,
                    'options': options
                }
            else:
                # 文本输入（用于数值输入）
                var = tk.StringVar()
                if field_current_value is not None:
                    var.set(str(field_current_value))
                entry = ttk.Entry(self.config_frame, textvariable=var, width=30)
                entry.grid(row=row, column=1, sticky=(tk.W, tk.E), pady=5, padx=(10, 0))
                
                self.field_widgets[reg_name][field_name] = {
                    'type': 'entry',
                    'var': var,
                    'entry': entry
                }
            
            row += 1
        
        # 更新画布滚动区域
        self.config_frame.update_idletasks()
        self.config_canvas.configure(scrollregion=self.config_canvas.bbox("all"))
    
    def get_current_config(self) -> Dict[str, Any]:
        """获取当前 GUI 配置"""
        config = {}
        
        if not hasattr(self, 'field_widgets'):
            return config
        
        for reg_name, fields in self.field_widgets.items():
            reg_config = {}
            for field_name, widget_info in fields.items():
                if widget_info['type'] == 'combobox':
                    var = widget_info['var']
                    current_text = var.get()
                    # 找到对应的选项值
                    for opt in widget_info['options']:
                        if opt.get('label', '') == current_text:
                            reg_config[field_name] = opt.get('value', '')
                            break
                elif widget_info['type'] == 'entry':
                    var = widget_info['var']
                    reg_config[field_name] = var.get()
            
            if reg_config:
                config[reg_name] = reg_config
        
        return config
    
    def save_config(self):
        """保存当前配置"""
        config = self.get_current_config()
        
        script_dir = os.path.dirname(os.path.abspath(__file__))
        output_file = os.path.join(script_dir, 'gui_config.json')
        
        with open(output_file, 'w', encoding='utf-8') as f:
            json.dump(config, f, indent=2, ensure_ascii=False)
        
        messagebox.showinfo("成功", f"配置已保存到: {output_file}")
    
    def export_json(self):
        """导出配置为 JSON"""
        config = self.get_current_config()
        
        file_path = filedialog.asksaveasfilename(
            defaultextension=".json",
            filetypes=[("JSON files", "*.json"), ("All files", "*.*")]
        )
        
        if file_path:
            with open(file_path, 'w', encoding='utf-8') as f:
                json.dump(config, f, indent=2, ensure_ascii=False)
            messagebox.showinfo("成功", f"配置已导出到: {file_path}")
    
    def generate_code(self):
        """生成 C 代码"""
        config = self.get_current_config()
        
        if not config:
            messagebox.showwarning("警告", "没有可生成的配置")
            return
        
        generator = CodeGenerator()
        code = generator.generate_c_code(config, self.ui_config)
        
        # 显示代码
        self.code_text.delete(1.0, tk.END)
        self.code_text.insert(1.0, code)
        
        # 询问是否保存
        if messagebox.askyesno("生成代码", "代码已生成，是否保存到文件？"):
            file_path = filedialog.asksaveasfilename(
                defaultextension=".c",
                filetypes=[("C files", "*.c"), ("All files", "*.*")]
            )
            if file_path:
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(code)
                messagebox.showinfo("成功", f"代码已保存到: {file_path}")


def main():
    root = tk.Tk()
    app = PCAP04ConfigGUI(root)
    root.mainloop()


if __name__ == '__main__':
    main()

