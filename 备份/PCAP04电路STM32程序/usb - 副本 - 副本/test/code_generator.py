#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
代码生成器
根据 GUI 配置生成 PCAP04_DefaultConfigItems[] 数组的 C 代码
"""

from typing import Dict, Any, List
import re
from constant_mapper import ConstantMapper

class CodeGenerator:
    def __init__(self):
        # 常量映射器
        self.mapper = ConstantMapper()
        
        # 寄存器宏映射
        self.reg_macro_map = {
            'Reg0': 'PCAP04_REG0_VALUE',
            'Reg1': 'PCAP04_REG1_VALUE',
            'Reg2': 'PCAP04_REG2_VALUE',
            'Reg3': 'PCAP04_REG3_VALUE',
            'Reg4': 'PCAP04_REG4_VALUE',
            'Reg5': 'PCAP04_REG5_VALUE',
            'Reg6': 'PCAP04_REG6_VALUE',
            'Reg7': 'PCAP04_C_AVRG_CONFIG',  # 特殊处理
            'Reg8': None,  # 与 Reg7 一起处理
            'Reg9': 'PCAP04_CONV_TIME_CONFIG',  # 特殊处理
            'Reg10': None,  # 与 Reg9 一起处理
            'Reg11': None,  # 与 Reg9 一起处理
            'Reg12': 'PCAP04_DISCHARGE_TIME_AND_REG13_CONFIG',  # 特殊处理
            'Reg13': None,  # 与 Reg12 一起处理
            'Reg14': 'PCAP04_PRECHARGE_TIME_AND_REG15_CONFIG',  # 特殊处理
            'Reg15': None,  # 与 Reg14 一起处理
            'Reg16': 'PCAP04_FULLCHARGE_TIME_AND_REG17_CONFIG',  # 特殊处理
            'Reg17': None,  # 与 Reg16 一起处理
            'Reg18': 'PCAP04_REG18_VALUE',
            'Reg19': 'PCAP04_REG19_VALUE',
            'Reg20': 'PCAP04_REG20_VALUE',
            'Reg21': 'PCAP04_R_TRIG_PREDIV_AND_REG22_CONFIG',  # 特殊处理
            'Reg22': None,  # 与 Reg21 一起处理
            'Reg23': 'PCAP04_REG23_VALUE',
            'Reg24': 'PCAP04_TDC_REG24_DEFAULT',  # 固定值
            'Reg25': 'PCAP04_TDC_REG25_DEFAULT',  # 固定值
            'Reg26': 'PCAP04_TDC_REG26_DEFAULT',  # 固定值
            'Reg27': 'PCAP04_REG27_VALUE',
            'Reg28': 'PCAP04_REG28_WD_DIS_bit',
            'Reg29': 'PCAP04_REG29_VALUE',
            'Reg30': 'PCAP04_REG30_VALUE',
            'Reg31': 'PCAP04_REG31_VALUE',
            'Reg32': 'PCAP04_REG32_VALUE',
            'Reg33': 'PCAP04_REG33_VALUE',
            'Reg34': 'PCAP04_REG34_VALUE',
            'Reg35': 'PCAP04_REG35_CDC_GAIN_CORR_bit',
            'Reg36': None,  # 固定值 0x00
            'Reg37': None,  # 固定值 0x00
            'Reg38': None,  # 固定值 0x00
            'Reg39': 'PCAP04_REG39_VALUE',
            'Reg40': None,  # 固定值 0x00
            'Reg41': None,  # 固定值 0x00
            'Reg42': 'PCAP04_REG42_VALUE',
            'Reg43': None,  # 固定值 0x00
            'Reg44': None,  # 固定值 0x00
            'Reg45': None,  # 固定值 0x00
            'Reg46': None,  # 固定值 0x00
            'Reg47': 'PCAP04_REG47_RUNBIT_bit',
            'Reg48': None,  # 固定值 0x00
            'Reg49': None,  # 固定值 0x00
            'Reg50': None,  # 固定值 0x00
            'Reg51': None,  # 固定值 0x00
            'Reg52': None,  # 固定值 0x00
            'Reg53': None,  # 固定值 0x00
            'Reg54': None,  # 固定值 0x00
            'Reg55': None,  # 固定值 0x00
            'Reg56': None,  # 固定值 0x00
            'Reg57': None,  # 固定值 0x00
            'Reg58': None,  # 固定值 0x00
            'Reg59': None,  # 固定值 0x00
            'Reg60': None,  # 固定值 0x00
            'Reg61': None,  # 固定值 0x00
            'Reg62': None,  # 固定值 0x00
            'Reg63': None,  # 固定值 0x00
        }
        
        # 常量映射（从选项值到宏常量）
        self.constant_map = self._build_constant_map()
    
    def _build_constant_map(self) -> Dict[str, str]:
        """构建常量映射表"""
        # 这里应该从头文件解析，暂时使用硬编码
        return {
            # Reg0
            'OLF_CTUNE_200k': 'PCAP04_REG0_OLF_CTUNE_200k',
            'OLF_CTUNE_100k': 'PCAP04_REG0_OLF_CTUNE_100k',
            'OLF_CTUNE_50k': 'PCAP04_REG0_OLF_CTUNE_50k',
            'OLF_CTUNE_10k': 'PCAP04_REG0_OLF_CTUNE_10k',
            
            # Reg1
            'OX_DIS_ENABLE': 'PCAP04_REG1_OX_DIS_ENABLE',
            'OX_DIS_DISABLE': 'PCAP04_REG1_OX_DIS_DISABLE',
            'OX_DIV4_NO_DIV': 'PCAP04_REG1_OX_DIV4_NO_DIV',
            'OX_DIV4_DIV4': 'PCAP04_REG1_OX_DIV4_DIV4',
            'OX_RUN_PERMANENT': 'PCAP04_REG1_OX_RUN_PERMANENT',
            'OX_RUN_OFF': 'PCAP04_REG1_OX_RUN_OFF',
            
            # Reg2
            'RDCHG_INT_SEL1_180K': 'PCAP04_REG2_RDCHG_INT_SEL1_180K',
            'RDCHG_INT_SEL1_90K': 'PCAP04_REG2_RDCHG_INT_SEL1_90K',
            'RDCHG_INT_SEL1_30K': 'PCAP04_REG2_RDCHG_INT_SEL1_30K',
            'RDCHG_INT_SEL1_10K': 'PCAP04_REG2_RDCHG_INT_SEL1_10K',
            'RDCHG_INT_SEL0_180K': 'PCAP04_REG2_RDCHG_INT_SEL0_180K',
            'RDCHG_INT_SEL0_90K': 'PCAP04_REG2_RDCHG_INT_SEL0_90K',
            'RDCHG_INT_SEL0_30K': 'PCAP04_REG2_RDCHG_INT_SEL0_30K',
            'RDCHG_INT_SEL0_10K': 'PCAP04_REG2_RDCHG_INT_SEL0_10K',
            'RDCHG_INT_EN_ENABLE': 'PCAP04_REG2_RDCHG_INT_EN_ENABLE',
            'RDCHG_INT_EN_DISABLE': 'PCAP04_REG2_RDCHG_INT_EN_DISABLE',
            'RDCHG_EXT_EN_ENABLE': 'PCAP04_REG2_RDCHG_EXT_EN_ENABLE',
            'RDCHG_EXT_EN_DISABLE': 'PCAP04_REG2_RDCHG_EXT_EN_DISABLE',
            
            # 更多常量...
        }
    
    def generate_c_code(self, gui_config: Dict[str, Any], ui_config: Dict[str, Any]) -> str:
        """生成 C 代码"""
        lines = []
        lines.append("static const PCAP04_RegConfigItem_t PCAP04_DefaultConfigItems[] = {")
        lines.append("    //******************CDC配置相关（0-19）******************** */")
        
        processed_regs = set()
        
        # 按寄存器编号排序
        regs = sorted(gui_config.items(), key=lambda x: int(x[0].replace('Reg', '')))
        
        for reg_name, reg_values in regs:
            if reg_name in processed_regs:
                continue
            
            reg_num = int(reg_name.replace('Reg', ''))
            
            # 处理特殊寄存器（跨多个寄存器的配置）
            if reg_name == 'Reg7':
                # C_AVRG 配置（Reg7-8）
                c_avrg = reg_values.get('C_AVRG', '2')
                lines.append(f"    // Register 7-8 (0x07-0x08) - C_AVRG采样平均配置（13位，跨两个寄存器）")
                lines.append(f"    PCAP04_C_AVRG_CONFIG({c_avrg}),  // C_AVRG = {c_avrg}")
                processed_regs.add('Reg7')
                processed_regs.add('Reg8')
                continue
            
            elif reg_name == 'Reg9':
                # CONV_TIME 配置（Reg9-11）
                conv_time = reg_values.get('CONV_TIME', '500')
                lines.append(f"    // Register 9-11 (0x09-0x0B) - CONV_TIME转换时间配置（23位，三个寄存器）")
                lines.append(f"    PCAP04_CONV_TIME_CONFIG({conv_time}),  // 转换时间 = {conv_time}")
                processed_regs.add('Reg9')
                processed_regs.add('Reg10')
                processed_regs.add('Reg11')
                continue
            
            elif reg_name == 'Reg12':
                # DISCHARGE_TIME 配置（Reg12-13）
                discharge_time = reg_values.get('DISCHARGE_TIME', '639')
                startonpin = reg_values.get('C_STARTONPIN', 'PCAP04_REG13_C_STARTONPIN_PG0')
                trig_sel = reg_values.get('C_TRIG_SEL', 'PCAP04_REG13_C_TRIG_SEL_TIMER_TRIGGER')
                
                # 使用常量映射器转换
                startonpin = self.mapper.map_reg13_startonpin(startonpin)
                trig_sel = self.mapper.map_reg13_trig_sel(trig_sel)
                
                lines.append(f"    // Register 12-13 (0x0C-0x0D) - DISCHARGE_TIME放电时间配置（10位，跨两个寄存器）")
                lines.append(f"    PCAP04_DISCHARGE_TIME_AND_REG13_CONFIG(")
                lines.append(f"        {discharge_time},                                    // 放电时间")
                lines.append(f"        {startonpin},         // 允许触发CDC启动的GPIO端口")
                lines.append(f"        {trig_sel}  // CDC触发模式")
                lines.append(f"    ),")
                processed_regs.add('Reg12')
                processed_regs.add('Reg13')
                continue
            
            elif reg_name == 'Reg14':
                # PRECHARGE_TIME 配置（Reg14-15）
                precharge_time = reg_values.get('PRECHARGE_TIME', '255')
                c_fake = reg_values.get('C_FAKE', '0')
                lines.append(f"    // Register 14-15 (0x0E-0x0F) - PRECHARGE_TIME预充电时间配置（10位，跨两个寄存器）")
                lines.append(f"    PCAP04_PRECHARGE_TIME_AND_REG15_CONFIG(")
                lines.append(f"        {precharge_time},  // 预充电时间")
                lines.append(f"        {c_fake}    // 无假测量")
                lines.append(f"    ),")
                processed_regs.add('Reg14')
                processed_regs.add('Reg15')
                continue
            
            elif reg_name == 'Reg16':
                # FULLCHARGE_TIME 配置（Reg16-17）
                fullcharge_time = reg_values.get('FULLCHARGE_TIME', '0')
                c_ref_sel = reg_values.get('C_REF_SEL', '5')
                lines.append(f"    // Register 16-17 (0x10-0x11) - FULLCHARGE_TIME满充电时间配置（10位，跨两个寄存器）")
                lines.append(f"    PCAP04_FULLCHARGE_TIME_AND_REG17_CONFIG(")
                lines.append(f"        {fullcharge_time},   // 满充电时间")
                lines.append(f"        {c_ref_sel}    // 内部参考电容大小")
                lines.append(f"    ),")
                processed_regs.add('Reg16')
                processed_regs.add('Reg17')
                continue
            
            elif reg_name == 'Reg21':
                # R_TRIG_PREDIV 配置（Reg21-22）
                r_trig_prediv = reg_values.get('R_TRIG_PREDIV', '1')
                r_avrg = reg_values.get('R_AVRG', 'PCAP04_REG22_R_AVRG_NO_AVG')
                r_trig_sel = reg_values.get('R_TRIG_SEL', 'PCAP04_REG22_R_TRIG_SEL_TIMER_TRIGGER')
                
                # 使用常量映射器转换
                r_avrg = self.mapper.map_reg22_r_avrg(r_avrg)
                r_trig_sel = self.mapper.map_reg22_r_trig_sel(r_trig_sel)
                
                lines.append(f"    // Register 21-22 (0x15-0x16) - R_TRIG_PREDIV RDC预分频配置（10位，跨两个寄存器）")
                lines.append(f"    PCAP04_R_TRIG_PREDIV_AND_REG22_CONFIG(")
                lines.append(f"        {r_trig_prediv},                                      // RDC预分频")
                lines.append(f"        {r_avrg},            // RDC测定的平均值配置")
                lines.append(f"        {r_trig_sel}  // RDC触发源选择")
                lines.append(f"    ),")
                processed_regs.add('Reg21')
                processed_regs.add('Reg22')
                continue
            
            # 处理普通寄存器
            macro_name = self.reg_macro_map.get(reg_name)
            if macro_name is None:
                # 固定值寄存器
                if reg_num < 64:
                    hex_val = reg_values.get('value', '0x00')
                    lines.append(f"    // Register {reg_num} (0x{reg_num:02X}) - 固定值")
                    lines.append(f"    {{PCAP04_REG_ADDR_{reg_num}, {hex_val}}},")
                processed_regs.add(reg_name)
                continue
            
            # 生成寄存器配置代码
            code = self._generate_register_code(reg_name, reg_num, reg_values, macro_name, ui_config)
            if code:
                lines.extend(code)
                processed_regs.add(reg_name)
        
        lines.append("};")
        
        return '\n'.join(lines)
    
    def _generate_register_code(self, reg_name: str, reg_num: int, 
                                reg_values: Dict[str, Any], macro_name: str,
                                ui_config: Dict[str, Any]) -> List[str]:
        """生成单个寄存器的配置代码"""
        lines = []
        
        # 获取寄存器信息
        reg_info = ui_config.get(reg_name, {})
        hex_addr = reg_info.get('hex_address', f'0x{reg_num:02X}')
        
        lines.append(f"    // Register {reg_num} ({hex_addr})")
        
        # 根据宏类型生成代码
        if macro_name == 'PCAP04_REG0_VALUE':
            i2c_a = reg_values.get('I2C_A', '3')
            olf_ftune = reg_values.get('OLF_FTUNE', '5')
            olf_ctune = reg_values.get('OLF_CTUNE', 'PCAP04_REG0_OLF_CTUNE_200k')
            
            # 使用常量映射器转换 OLF_CTUNE
            olf_ctune = self.mapper.map_reg0_olf_ctune(olf_ctune)
            
            lines.append(f"    {{PCAP04_REG_ADDR_{reg_num},")
            lines.append(f"     PCAP04_REG0_VALUE(")
            lines.append(f"         {i2c_a},                              // 地址位I2C_A")
            lines.append(f"         {olf_ftune},                              // 低频时钟微调 OLF_FTUNE")
            lines.append(f"         {olf_ctune}     // 低频时钟粗调 OLF_CTUNE")
            lines.append(f"     )}},")
        
        elif macro_name == 'PCAP04_REG1_VALUE':
            ox_dis = reg_values.get('OX_DIS', 'PCAP04_REG1_OX_DIS_ENABLE')
            ox_div4 = reg_values.get('OX_DIV4', 'PCAP04_REG1_OX_DIV4_NO_DIV')
            ox_run = reg_values.get('OX_RUN', 'PCAP04_REG1_OX_RUN_PERMANENT')
            
            # 使用常量映射器转换
            ox_dis = self.mapper.map_reg1_ox_dis(ox_dis)
            ox_div4 = self.mapper.map_reg1_ox_div4(ox_div4)
            ox_run = self.mapper.map_reg1_ox_run(ox_run)
            
            lines.append(f"    {{PCAP04_REG_ADDR_{reg_num},")
            lines.append(f"     PCAP04_REG1_VALUE(")
            lines.append(f"         {ox_dis},        // 禁用OX时钟 OX_DIS")
            lines.append(f"         {ox_div4},       // OX时钟4分频 OX_DIV4")
            lines.append(f"         {ox_run}      // 控制OX发生器的持续性或延迟 OX_RUN")
            lines.append(f"     )}},")
        
        elif macro_name == 'PCAP04_REG2_VALUE':
            sel1 = reg_values.get('RDCHG_INT_SEL1', 'PCAP04_REG2_RDCHG_INT_SEL1_180K')
            sel0 = reg_values.get('RDCHG_INT_SEL0', 'PCAP04_REG2_RDCHG_INT_SEL0_10K')
            int_en = reg_values.get('RDCHG_INT_EN', 'PCAP04_REG2_RDCHG_INT_EN_ENABLE')
            ext_en = reg_values.get('RDCHG_EXT_EN', 'PCAP04_REG2_RDCHG_EXT_EN_DISABLE')
            
            # 使用常量映射器转换
            sel1 = self.mapper.map_reg2_sel(sel1, 'SEL1')
            sel0 = self.mapper.map_reg2_sel(sel0, 'SEL0')
            int_en = self.mapper.map_reg2_en(int_en, 'RDCHG_INT_EN')
            ext_en = self.mapper.map_reg2_en(ext_en, 'RDCHG_EXT_EN')
            
            lines.append(f"    {{PCAP04_REG_ADDR_{reg_num},")
            lines.append(f"     PCAP04_REG2_VALUE(")
            lines.append(f"         {sel1},     // PC4~PC5的片上放电电阻选择 RDCHG_INT_SEL1")
            lines.append(f"         {sel0},      // PC0~PC3和内部端口PC6的片上放电电阻选择 RDCHG_INT_SEL0")
            lines.append(f"         {int_en},     // 内部放电电阻使能 RDCHG_INT_EN")
            lines.append(f"         {ext_en}     // 外部放电电阻使能 RDCHG_EXT_EN")
            lines.append(f"     )}},")
        
        elif macro_name == 'PCAP04_REG6_VALUE':
            # 端口使能（位掩码）
            port_en = reg_values.get('C_PORT_EN', '0x0F')
            port_en = self.mapper.map_reg6_port_en(port_en)
            lines.append(f"    {{PCAP04_REG_ADDR_{reg_num},")
            lines.append(f"     PCAP04_REG6_VALUE({port_en})}},  // 启用PC0~PC3的CDC端口")
        
        elif macro_name in ['PCAP04_TDC_REG24_DEFAULT', 'PCAP04_TDC_REG25_DEFAULT', 'PCAP04_TDC_REG26_DEFAULT']:
            lines.append(f"    {{PCAP04_REG_ADDR_{reg_num}, {macro_name}}},  // 固定值（ams内部配置）")
        
        elif macro_name == 'PCAP04_REG28_WD_DIS_bit':
            wd_dis = reg_values.get('WD_DIS', 'PCAP04_REG28_WD_DIS_DISABLE')
            wd_dis = self.mapper.map_reg28_wd_dis(wd_dis)
            lines.append(f"    {{PCAP04_REG_ADDR_{reg_num}, {macro_name}({wd_dis})}},  // 看门狗配置")
        
        elif macro_name == 'PCAP04_REG35_CDC_GAIN_CORR_bit':
            gain = reg_values.get('CDC_GAIN_CORR', 'PCAP04_REG35_CDC_GAIN_CORR_1_25')
            gain = self.mapper.map_reg35_gain_corr(gain)
            lines.append(f"    {{PCAP04_REG_ADDR_{reg_num}, {macro_name}({gain})}},")
        
        elif macro_name == 'PCAP04_REG47_RUNBIT_bit':
            runbit = reg_values.get('RUNBIT', 'PCAP04_REG47_RUNBIT_ON')
            runbit = self.mapper.map_reg47_runbit(runbit)
            lines.append(f"    {{PCAP04_REG_ADDR_{reg_num}, {macro_name}({runbit})}},  // = 0x01")
        
        elif macro_name == 'PCAP04_REG3_VALUE':
            aux_pd_dis = reg_values.get('AUX_PD_DIS', 'PCAP04_REG3_AUX_PD_DIS_ACTIVE')
            aux_cint = reg_values.get('AUX_CINT', '0')
            rdchg_perm_en = reg_values.get('RDCHG_PERM_EN', '0')
            rdchg_ext_perm = reg_values.get('RDCHG_EXT_PERM', '0')
            rchg_sel = reg_values.get('RCHG_SEL', 'PCAP04_REG3_RCHG_SEL_180K')
            
            # 使用常量映射器转换
            aux_pd_dis = self.mapper.map_reg3_aux_pd_dis(aux_pd_dis)
            rchg_sel = self.mapper.map_reg3_rchg_sel(rchg_sel)
            
            lines.append(f"    {{PCAP04_REG_ADDR_{reg_num},")
            lines.append(f"     PCAP04_REG3_VALUE(")
            lines.append(f"         {aux_pd_dis},      // 禁用PCAUX下拉电阻 AUX_PD_DIS")
            lines.append(f"         {aux_cint},                                  // 仅在内部参考转换期间激活辅助端口PCAUX AUX_CINT")
            lines.append(f"         {rdchg_perm_en},                                  // 保持芯片内部放电电阻永久连接 RDCHG_PERM_EN")
            lines.append(f"         {rdchg_ext_perm},                                  // 永久激活辅助端口PCAUX RDCHG_EXT_PERM")
            lines.append(f"         {rchg_sel}          // 选择充电电阻 RCHG_SEL")
            lines.append(f"     )}},")
        
        elif macro_name == 'PCAP04_REG4_VALUE':
            c_ref_int = reg_values.get('C_REF_INT', 'PCAP04_REG4_C_REF_INT_EXTERNAL')
            c_comp_ext = reg_values.get('C_COMP_EXT', 'PCAP04_REG4_C_COMP_EXT_ACTIVE')
            c_comp_int = reg_values.get('C_COMP_INT', 'PCAP04_REG4_C_COMP_INT_ACTIVE')
            c_differential = reg_values.get('C_DIFFERENTIAL', 'PCAP04_REG4_C_DIFFERENTIAL_SINGLE_ENDED')
            c_floating = reg_values.get('C_FLOATING', 'PCAP04_REG4_C_FLOATING_FLOATING')
            
            # 使用常量映射器转换
            c_ref_int = self.mapper.map_reg4_c_ref_int(c_ref_int)
            c_comp_ext = self.mapper.map_reg4_comp(c_comp_ext, 'EXT')
            c_comp_int = self.mapper.map_reg4_comp(c_comp_int, 'INT')
            c_differential = self.mapper.map_reg4_differential(c_differential)
            c_floating = self.mapper.map_reg4_floating(c_floating)
            
            lines.append(f"    {{PCAP04_REG_ADDR_{reg_num},")
            lines.append(f"     PCAP04_REG4_VALUE(")
            lines.append(f"         {c_ref_int},     // 在PC0/GND或PC0/PC1使用片上参比电容 C_REF_INT")
            lines.append(f"         {c_comp_ext},      // 激活外部寄生电容补偿机制 C_COMP_EXT")
            lines.append(f"         {c_comp_int},      // 激活片上寄生电容补偿机制和增益补偿 C_COMP_INT")
            lines.append(f"         {c_differential},  // 在单端测量和差分测量之间选择 C_DIFFERENTIAL")
            lines.append(f"         {c_floating}     // 选择接地测量或浮动测量 C_FLOATING")
            lines.append(f"     )}},")
        
        elif macro_name == 'PCAP04_REG5_VALUE':
            cy_pre_mr1_short = reg_values.get('CY_PRE_MR1_SHORT', 'PCAP04_REG5_CY_PRE_MR1_SHORT_NORMAL')
            c_port_pat = reg_values.get('C_PORT_PAT', 'PCAP04_REG5_C_PORT_PAT_NORMAL')
            cy_hfclk_sel = reg_values.get('CY_HFCLK_SEL', 'PCAP04_REG5_CY_HFCLK_SEL_OLF')
            cy_div4_dis = reg_values.get('CY_DIV4_DIS', 'PCAP04_REG5_CY_DIV4_DIS_DISABLE')
            cy_pre_long = reg_values.get('CY_PRE_LONG', 'PCAP04_REG5_CY_PRE_LONG_DISABLE')
            c_dc_balance = reg_values.get('C_DC_BALANCE', 'PCAP04_REG5_C_DC_BALANCE_DISABLE')
            
            # 使用常量映射器转换
            cy_pre_mr1_short = self.mapper.map_reg5_cy_pre_mr1_short(cy_pre_mr1_short)
            c_port_pat = self.mapper.map_reg5_c_port_pat(c_port_pat)
            cy_hfclk_sel = self.mapper.map_reg5_cy_hfclk_sel(cy_hfclk_sel)
            cy_div4_dis = self.mapper.map_reg5_cy_div4_dis(cy_div4_dis)
            cy_pre_long = self.mapper.map_reg5_cy_pre_long(cy_pre_long)
            c_dc_balance = self.mapper.map_reg5_c_dc_balance(c_dc_balance)
            
            lines.append(f"    {{PCAP04_REG_ADDR_{reg_num},")
            lines.append(f"     PCAP04_REG5_VALUE(")
            lines.append(f"         {cy_pre_mr1_short},  // 减少内部时钟路径之间的延迟 CY_PRE_MR1_SHORT")
            lines.append(f"         {c_port_pat},        // 端口测量顺序交替 C_PORT_PAT")
            lines.append(f"         {cy_hfclk_sel},        // 选择CDC的时钟源 CY_HFCLK_SEL")
            lines.append(f"         {cy_div4_dis},      // 改变时间周期 CY_DIV4_DIS")
            lines.append(f"         {cy_pre_long},      // 在内部时钟路径之间添加安全延迟 CY_PRE_LONG")
            lines.append(f"         {c_dc_balance}      // 仅用于差分浮动模式 C_DC_BALANCE")
            lines.append(f"     )}},")
        
        elif macro_name == 'PCAP04_REG18_VALUE':
            c_g_op_run = reg_values.get('C_G_OP_RUN', 'PCAP04_REG18_C_G_OP_RUN_PERMANENT')
            c_g_op_ext = reg_values.get('C_G_OP_EXT', 'PCAP04_REG18_C_G_OP_EXT_INTERNAL')
            c_g_en = reg_values.get('C_G_EN', 'PCAP04_REG18_C_G_EN_NONE')
            
            # 使用常量映射器转换
            c_g_op_run = self.mapper.map_reg18_c_g_op_run(c_g_op_run)
            c_g_op_ext = self.mapper.map_reg18_c_g_op_ext(c_g_op_ext)
            c_g_en = self.mapper.map_reg18_c_g_en(c_g_en)
            
            lines.append(f"    {{PCAP04_REG_ADDR_{reg_num},")
            lines.append(f"     PCAP04_REG18_VALUE(")
            lines.append(f"         {c_g_op_run},  // 保护运算放大器模式 = 永久")
            lines.append(f"         {c_g_op_ext},   // 保护外部运算放大器 = 内部OP")
            lines.append(f"         {c_g_en}            // 保护使能 = 未启用（所有端口禁用）")
            lines.append(f"     )}},  // = 0x00 (保护功能未启用)")
        
        elif macro_name == 'PCAP04_REG19_VALUE':
            c_g_op_vu = reg_values.get('C_G_OP_VU', 'PCAP04_REG19_C_G_OP_VU_1_00')
            c_g_op_attn = reg_values.get('C_G_OP_ATTN', 'PCAP04_REG19_C_G_OP_ATTN_0_5_AF')
            c_g_time = reg_values.get('C_G_TIME', '0')
            
            # 使用常量映射器转换
            c_g_op_vu = self.mapper.map_reg19_c_g_op_vu(c_g_op_vu)
            c_g_op_attn = self.mapper.map_reg19_c_g_op_attn(c_g_op_attn)
            
            lines.append(f"    {{PCAP04_REG_ADDR_{reg_num},")
            lines.append(f"     PCAP04_REG19_VALUE(")
            lines.append(f"         {c_g_op_vu},        // 保护运算放大器增益 = ×1.00")
            lines.append(f"         {c_g_op_attn},    // 保护运算放大器衰减 = 0.5 aF")
            lines.append(f"         {c_g_time}                                   // 保护端口切换时间 = 0 (默认值)")
            lines.append(f"     )}},  // = 0x00 (所有位为默认值)")
        
        elif macro_name == 'PCAP04_REG20_VALUE':
            r_cy = reg_values.get('R_CY', 'PCAP04_REG20_R_CY_SHORT')
            c_g_op_tr = reg_values.get('C_G_OP_TR', '0')
            
            # 使用常量映射器转换
            r_cy = self.mapper.map_reg20_r_cy(r_cy)
            
            lines.append(f"    {{PCAP04_REG_ADDR_{reg_num},")
            lines.append(f"     PCAP04_REG20_VALUE(")
            lines.append(f"         {r_cy},            // RDC循环时间 = 较短循环时间")
            lines.append(f"         {c_g_op_tr}                                   // 保护运算放大器电流调整 = 0 (默认值)")
            lines.append(f"     )}},  // = 0x00")
        
        elif macro_name == 'PCAP04_REG23_VALUE':
            r_port_en = reg_values.get('R_PORT_EN', 'PCAP04_REG23_R_PORT_EN_DISABLE')
            r_port_en_imes = reg_values.get('R_PORT_EN_IMES', 'PCAP04_REG23_R_PORT_EN_IMES_DISABLE')
            r_port_en_iref = reg_values.get('R_PORT_EN_IREF', 'PCAP04_REG23_R_PORT_EN_IREF_ACTIVE')
            r_fake = reg_values.get('R_FAKE', 'PCAP04_REG23_R_FAKE_2_CYCLES')
            r_startonpin = reg_values.get('R_STARTONPIN', 'PCAP04_REG23_R_STARTONPIN_PG0')
            
            # 使用常量映射器转换
            r_port_en = self.mapper.map_reg23_r_port_en(r_port_en)
            r_port_en_imes = self.mapper.map_reg23_r_port_en_imes(r_port_en_imes)
            r_port_en_iref = self.mapper.map_reg23_r_port_en_iref(r_port_en_iref)
            r_fake = self.mapper.map_reg23_r_fake(r_fake)
            r_startonpin = self.mapper.map_reg23_r_startonpin(r_startonpin)
            
            lines.append(f"    {{PCAP04_REG_ADDR_{reg_num},")
            lines.append(f"     PCAP04_REG23_VALUE(")
            lines.append(f"         {r_port_en},        // RDC部分的端口激活 R_PORT_EN")
            lines.append(f"         {r_port_en_imes},    // 内部铝温度传感器的端口激活 R_PORT_EN_IMES")
            lines.append(f"         {r_port_en_iref},    // 内部参考电阻的端口激活 R_PORT_EN_IREF")
            lines.append(f"         {r_fake},          // RDC的\"假\"或\"预热\"测量数量 R_FAKE")
            lines.append(f"         {r_startonpin}          // 选择允许触发RDC启动的GPIO端口 R_STARTONPIN")
            lines.append(f"     )}},  // = 0x30")
        
        elif macro_name == 'PCAP04_REG27_VALUE':
            dsp_moflo_en = reg_values.get('DSP_MOFLO_EN', 'PCAP04_REG27_DSP_MOFLO_EN_DISABLE')
            dsp_speed = reg_values.get('DSP_SPEED', 'PCAP04_REG27_DSP_SPEED_FASTEST')
            pg1xpg3 = reg_values.get('PG1xPG3', 'PCAP04_REG27_PG1xPG3_NORMAL')
            pg0xpg2 = reg_values.get('PG0xPG2', 'PCAP04_REG27_PG0xPG2_NORMAL')
            
            # 使用常量映射器转换
            dsp_moflo_en = self.mapper.map_reg27_dsp_moflo_en(dsp_moflo_en)
            dsp_speed = self.mapper.map_reg27_dsp_speed(dsp_speed)
            pg1xpg3 = self.mapper.map_reg27_pg_swap(pg1xpg3, 'PG1xPG3')
            pg0xpg2 = self.mapper.map_reg27_pg_swap(pg0xpg2, 'PG0xPG2')
            
            lines.append(f"    {{PCAP04_REG_ADDR_{reg_num},")
            lines.append(f"     PCAP04_REG27_VALUE(")
            lines.append(f"         {dsp_moflo_en},      // GPIO输出数据使能的动态范围溢出 DSP_MOFLO_EN")
            lines.append(f"         {dsp_speed},         // DSP速度 DSP_SPEED")
            lines.append(f"         {pg1xpg3},           // PDM/PWM输出引脚为PG3")
            lines.append(f"         {pg0xpg2}            // PDM/PWM输出引脚为PG2")
            lines.append(f"     )}},  // = 0x08")
        
        elif macro_name == 'PCAP04_REG29_VALUE':
            dsp_startonpin = reg_values.get('DSP_STARTONPIN', '0')
            dsp_ff_in = reg_values.get('DSP_FF_IN', '0')
            lines.append(f"    {{PCAP04_REG_ADDR_{reg_num},")
            lines.append(f"     PCAP04_REG29_VALUE(")
            lines.append(f"         {dsp_startonpin},  // 启动DSP的引脚掩码 DSP_STARTONPIN")
            lines.append(f"         {dsp_ff_in}   // 触发器激活的引脚掩码 DSP_FF_IN")
            lines.append(f"     )}},  // = 0x00")
        
        elif macro_name == 'PCAP04_REG30_VALUE':
            pg5_intn_en = reg_values.get('PG5_INTN_EN', 'PCAP04_REG30_PG5_INTN_EN_ROUTE')
            pg4_intn_en = reg_values.get('PG4_INTN_EN', 'PCAP04_REG30_PG4_INTN_EN_NORMAL')
            dsp_start_en = reg_values.get('DSP_START_EN', 'PCAP04_REG30_DSP_START_EN_RDC_END')
            
            # 使用常量映射器转换
            pg5_intn_en = self.mapper.map_reg30_pg_intn_en(pg5_intn_en, 5)
            pg4_intn_en = self.mapper.map_reg30_pg_intn_en(pg4_intn_en, 4)
            dsp_start_en = self.mapper.map_reg30_dsp_start_en(dsp_start_en)
            
            lines.append(f"    {{PCAP04_REG_ADDR_{reg_num},")
            lines.append(f"     PCAP04_REG30_VALUE(")
            lines.append(f"         {pg5_intn_en},        // 将INTN信号路由到PG5 PG5_INTN_EN")
            lines.append(f"         {pg4_intn_en},       // 将INTN信号路由到PG4 PG4_INTN_EN")
            lines.append(f"         {dsp_start_en}      // DSP触发器启用 DSP_START_EN")
            lines.append(f"     )}},  // = 0x82")
        
        elif macro_name == 'PCAP04_REG31_VALUE':
            pi1_toggle_en = reg_values.get('PI1_TOGGLE_EN', 'PCAP04_REG31_PI1_TOGGLE_EN_NORMAL')
            pio_toggle_en = reg_values.get('PIO_TOGGLE_EN', 'PCAP04_REG31_PIO_TOGGLE_EN_NORMAL')
            pio_res = reg_values.get('PIO_RES', 'PCAP04_REG31_PIO_RES_10BIT')
            pio_pdm_sel = reg_values.get('PIO_PDM_SEL', 'PCAP04_REG31_PIO_PDM_SEL_PWM')
            pio_clk_sel = reg_values.get('PIO_CLK_SEL', 'PCAP04_REG31_PIO_CLK_SEL_OFF')
            
            # 使用常量映射器转换
            pi1_toggle_en = self.mapper.map_reg31_toggle_en(pi1_toggle_en, 'PI1')
            pio_toggle_en = self.mapper.map_reg31_toggle_en(pio_toggle_en, 'PIO')
            pio_res = self.mapper.map_reg31_pio_res(pio_res)
            pio_pdm_sel = self.mapper.map_reg31_pio_pdm_sel(pio_pdm_sel)
            pio_clk_sel = self.mapper.map_reg31_pio_clk_sel(pio_clk_sel)
            
            lines.append(f"    {{PCAP04_REG_ADDR_{reg_num},")
            lines.append(f"     PCAP04_REG31_VALUE(")
            lines.append(f"         {pi1_toggle_en},     // 在脉冲接口1输出激活切换触发器")
            lines.append(f"         {pio_toggle_en},     // 在脉冲接口0输出激活切换触发器")
            lines.append(f"         {pio_res},            // 脉冲编码接口0的分辨率")
            lines.append(f"         {pio_pdm_sel},          // PWM/PDM")
            lines.append(f"         {pio_clk_sel}           // 脉冲接口0时钟选择")
            lines.append(f"     )}},  // = 0x08")
        
        elif macro_name == 'PCAP04_REG32_VALUE':
            pi1_res = reg_values.get('PI1_RES', 'PCAP04_REG32_PI1_RES_10BIT')
            pi1_pdm_sel = reg_values.get('PI1_PDM_SEL', 'PCAP04_REG32_PI1_PDM_SEL_PWM')
            pi1_clk_sel = reg_values.get('PI1_CLK_SEL', 'PCAP04_REG32_PI1_CLK_SEL_OFF')
            
            # 使用常量映射器转换
            pi1_res = self.mapper.map_reg32_pi1_res(pi1_res)
            pi1_pdm_sel = self.mapper.map_reg32_pi1_pdm_sel(pi1_pdm_sel)
            pi1_clk_sel = self.mapper.map_reg32_pi1_clk_sel(pi1_clk_sel)
            
            lines.append(f"    {{PCAP04_REG_ADDR_{reg_num},")
            lines.append(f"     PCAP04_REG32_VALUE(")
            lines.append(f"         {pi1_res},            // 脉冲接口1的分辨率")
            lines.append(f"         {pi1_pdm_sel},          // 脉冲接口1 PWM / PDM开关")
            lines.append(f"         {pi1_clk_sel}           // 脉冲接口1时钟选择")
            lines.append(f"     )}},  // = 0x08")
        
        elif macro_name == 'PCAP04_REG33_VALUE':
            pg_dir_in = reg_values.get('PG_DIR_IN', '0')
            pg_pu = reg_values.get('PG_PU', '0')
            
            # 使用常量映射器转换
            pg_dir_in = self.mapper.map_reg33_pg_dir_in(pg_dir_in)
            pg_pu = self.mapper.map_reg33_pg_pu(pg_pu)
            
            lines.append(f"    {{PCAP04_REG_ADDR_{reg_num},")
            lines.append(f"     PCAP04_REG33_VALUE(")
            lines.append(f"         {pg_dir_in},  // PG_DIR_IN = 0 (所有端口为输出模式，默认值)")
            lines.append(f"         {pg_pu}   // PG_PU = 0 (所有端口上拉禁用，默认值)")
            lines.append(f"     )}},  // = 0x00 (所有GPIO端口为输出模式，上拉禁用)")
        
        elif macro_name == 'PCAP04_REG34_VALUE':
            int_trig_bg = reg_values.get('INT_TRIG_BG', 'PCAP04_REG34_INT_TRIG_BG_DISABLE')
            dsp_trig_bg = reg_values.get('DSP_TRIG_BG', 'PCAP04_REG34_DSP_TRIG_BG_ENABLE')
            bg_perm = reg_values.get('BG_PERM', 'PCAP04_REG34_BG_PERM_PULSE_MODE')
            autostart = reg_values.get('AUTOSTART', 'PCAP04_REG34_AUTOSTART_DISABLE')
            
            # 使用常量映射器转换
            int_trig_bg = self.mapper.map_reg34_int_trig_bg(int_trig_bg)
            dsp_trig_bg = self.mapper.map_reg34_dsp_trig_bg(dsp_trig_bg)
            bg_perm = self.mapper.map_reg34_bg_perm(bg_perm)
            autostart = self.mapper.map_reg34_autostart(autostart)
            
            lines.append(f"    {{PCAP04_REG_ADDR_{reg_num},")
            lines.append(f"     PCAP04_REG34_VALUE(")
            lines.append(f"         {int_trig_bg},      // 读取结束触发带隙")
            lines.append(f"         {dsp_trig_bg},       // 带隙刷新由DSP位设置触发")
            lines.append(f"         {bg_perm},       // 激活带隙永久启用")
            lines.append(f"         {autostart}         // 在上电后触发CDC")
            lines.append(f"     )}},  // = 0x47 (AMS_INTERNAL自动设置为7)")
        
        elif macro_name == 'PCAP04_REG39_VALUE':
            pulse_sel1 = reg_values.get('PULSE_SEL1', 'PCAP04_REG39_PULSE_SEL1_RES7')
            pulse_sel0 = reg_values.get('PULSE_SEL0', 'PCAP04_REG39_PULSE_SEL0_RES1')
            
            # 使用常量映射器转换
            pulse_sel1 = self.mapper.map_reg39_pulse_sel(pulse_sel1, 1)
            pulse_sel0 = self.mapper.map_reg39_pulse_sel(pulse_sel0, 0)
            
            lines.append(f"    {{PCAP04_REG_ADDR_{reg_num},")
            lines.append(f"     PCAP04_REG39_VALUE(")
            lines.append(f"         {pulse_sel1},          // 为脉冲接口1选择源")
            lines.append(f"         {pulse_sel0}           // 为脉冲接口0选择源")
            lines.append(f"     )}},  // = 0x71")
        
        elif macro_name == 'PCAP04_REG42_VALUE':
            alarm1_select = reg_values.get('ALARM1_SELECT', 'PCAP04_REG42_ALARM1_SELECT_Z')
            alarm0_select = reg_values.get('ALARM0_SELECT', 'PCAP04_REG42_ALARM0_SELECT_Z')
            en_async_read = reg_values.get('EN_ASYNC_READ', 'PCAP04_REG42_EN_ASYNC_READ_DISABLE')
            r_median_en = reg_values.get('R_MEDIAN_EN', 'PCAP04_REG42_R_MEDIAN_EN_ENABLE')
            c_median_en = reg_values.get('C_MEDIAN_EN', 'PCAP04_REG42_C_MEDIAN_EN_ENABLE')
            
            # 使用常量映射器转换
            alarm1_select = self.mapper.map_reg42_alarm_select(alarm1_select, 1)
            alarm0_select = self.mapper.map_reg42_alarm_select(alarm0_select, 0)
            en_async_read = self.mapper.map_reg42_en_async_read(en_async_read)
            r_median_en = self.mapper.map_reg42_median_en(r_median_en, 'R')
            c_median_en = self.mapper.map_reg42_median_en(c_median_en, 'C')
            
            lines.append(f"    {{PCAP04_REG_ADDR_{reg_num},")
            lines.append(f"     PCAP04_REG42_VALUE(")
            lines.append(f"         {alarm1_select},           // 报警1选择")
            lines.append(f"         {alarm0_select},           // 报警0选择")
            lines.append(f"         {en_async_read},     // 禁用异步读取结果寄存器Res0到Res7中的值仅在读取先前值后才更新")
            lines.append(f"         {r_median_en},        // 启用R的中值滤波器")
            lines.append(f"         {c_median_en}         // 启用C的中值滤波器")
            lines.append(f"     )}},  // = 0x03 (HS_MODE_SEL自动设置为0)")
        
        else:
            # 其他寄存器，使用通用格式
            lines.append(f"    {{PCAP04_REG_ADDR_{reg_num}, {macro_name}(...)}},  // TODO: 需要实现")
        
        return lines


def main():
    # 测试代码生成
    generator = CodeGenerator()
    
    test_config = {
        'Reg0': {
            'I2C_A': '3',
            'OLF_FTUNE': '5',
            'OLF_CTUNE': 'PCAP04_REG0_OLF_CTUNE_200k'
        },
        'Reg1': {
            'OX_DIS': 'PCAP04_REG1_OX_DIS_ENABLE',
            'OX_DIV4': 'PCAP04_REG1_OX_DIV4_NO_DIV',
            'OX_RUN': 'PCAP04_REG1_OX_RUN_PERMANENT'
        }
    }
    
    code = generator.generate_c_code(test_config, {})
    print(code)


if __name__ == '__main__':
    main()

