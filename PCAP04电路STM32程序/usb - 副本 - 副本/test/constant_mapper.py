#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
常量映射器
将 GUI 配置值转换为 pcap04_reg.h 中定义的宏常量
"""

class ConstantMapper:
    """将配置值映射到 pcap04_reg.h 中的宏常量"""
    
    @staticmethod
    def map_reg0_olf_ctune(value):
        """映射 OLF_CTUNE 值到宏常量"""
        if isinstance(value, str):
            # 如果是字符串，尝试解析
            if value.startswith('PCAP04_'):
                return value
            # 尝试从描述中提取
            if '200' in value or '200k' in value.lower():
                return 'PCAP04_REG0_OLF_CTUNE_200k'
            elif '100' in value or '100k' in value.lower():
                return 'PCAP04_REG0_OLF_CTUNE_100k'
            elif '50' in value or '50k' in value.lower():
                return 'PCAP04_REG0_OLF_CTUNE_50k'
            elif '10' in value or '10k' in value.lower():
                return 'PCAP04_REG0_OLF_CTUNE_10k'
            # 尝试转换为数字
            try:
                value = int(value)
            except (ValueError, TypeError):
                return 'PCAP04_REG0_OLF_CTUNE_200k'
        
        # 数字映射
        ctune_map = {
            0: 'PCAP04_REG0_OLF_CTUNE_10k',
            1: 'PCAP04_REG0_OLF_CTUNE_50k',
            2: 'PCAP04_REG0_OLF_CTUNE_100k',
            3: 'PCAP04_REG0_OLF_CTUNE_200k'
        }
        return ctune_map.get(int(value), 'PCAP04_REG0_OLF_CTUNE_200k')
    
    @staticmethod
    def map_reg1_ox_dis(value):
        """映射 OX_DIS 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'disable' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG1_OX_DIS_DISABLE'
        return 'PCAP04_REG1_OX_DIS_ENABLE'
    
    @staticmethod
    def map_reg1_ox_div4(value):
        """映射 OX_DIV4 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'div4' in value_str or '4分频' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG1_OX_DIV4_DIV4'
        return 'PCAP04_REG1_OX_DIV4_NO_DIV'
    
    @staticmethod
    def map_reg1_ox_run(value):
        """映射 OX_RUN 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'permanent' in value_str or '永久' in value_str:
            return 'PCAP04_REG1_OX_RUN_PERMANENT'
        elif 'off' in value_str or '关闭' in value_str:
            return 'PCAP04_REG1_OX_RUN_OFF'
        elif 'delay31' in value_str or '31' in value_str:
            return 'PCAP04_REG1_OX_RUN_DELAY31_fOLF'
        elif 'delay2' in value_str or value == '3' or value == 3:
            return 'PCAP04_REG1_OX_RUN_DELAY2_fOLF'
        elif 'delay1' in value_str or value == '6' or value == 6:
            return 'PCAP04_REG1_OX_RUN_DELAY1_fOLF'
        return 'PCAP04_REG1_OX_RUN_PERMANENT'
    
    @staticmethod
    def map_reg2_sel(value, sel_type='SEL1'):
        """映射 RDCHG_INT_SEL 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        prefix = f'PCAP04_REG2_RDCHG_INT_{sel_type}_'
        if '180' in value_str:
            return f'{prefix}180K'
        elif '90' in value_str:
            return f'{prefix}90K'
        elif '30' in value_str or '默认' in value_str:
            return f'{prefix}30K'
        elif '10' in value_str:
            return f'{prefix}10K'
        # 数字映射
        try:
            val = int(value)
            sel_map = {0: f'{prefix}180K', 1: f'{prefix}90K', 2: f'{prefix}30K', 3: f'{prefix}10K'}
            return sel_map.get(val, f'{prefix}30K')
        except (ValueError, TypeError):
            return f'{prefix}30K'
    
    @staticmethod
    def map_reg2_en(value, en_type='INT_EN'):
        """映射 RDCHG_EN 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        prefix = f'PCAP04_REG2_{en_type}_'
        if 'disable' in value_str or '关闭' in value_str or value == '0' or value == 0:
            return f'{prefix}DISABLE'
        return f'{prefix}ENABLE'
    
    @staticmethod
    def map_reg3_aux_pd_dis(value):
        """映射 AUX_PD_DIS 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'disable' in value_str or '禁用' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG3_AUX_PD_DIS_DISABLE'
        return 'PCAP04_REG3_AUX_PD_DIS_ACTIVE'
    
    @staticmethod
    def map_reg3_rchg_sel(value):
        """映射 RCHG_SEL 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if '180' in value_str or value == '0' or value == 0:
            return 'PCAP04_REG3_RCHG_SEL_180K'
        return 'PCAP04_REG3_RCHG_SEL_10K'
    
    @staticmethod
    def map_reg4_c_ref_int(value):
        """映射 C_REF_INT 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'internal' in value_str or '内部' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG4_C_REF_INT_INTERNAL'
        return 'PCAP04_REG4_C_REF_INT_EXTERNAL'
    
    @staticmethod
    def map_reg4_comp(value, comp_type='EXT'):
        """映射 C_COMP 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        prefix = f'PCAP04_REG4_C_COMP_{comp_type}_'
        if 'active' in value_str or '激活' in value_str or value == '1' or value == 1:
            return f'{prefix}ACTIVE'
        return f'{prefix}IDLE'
    
    @staticmethod
    def map_reg4_differential(value):
        """映射 C_DIFFERENTIAL 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'diff' in value_str or '差分' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG4_C_DIFFERENTIAL_DIFF'
        return 'PCAP04_REG4_C_DIFFERENTIAL_SINGLE_ENDED'
    
    @staticmethod
    def map_reg4_floating(value):
        """映射 C_FLOATING 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'floating' in value_str or '浮动' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG4_C_FLOATING_FLOATING'
        return 'PCAP04_REG4_C_FLOATING_GROUNDED'
    
    @staticmethod
    def map_reg5_cy_pre_mr1_short(value):
        """映射 CY_PRE_MR1_SHORT 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'reduce' in value_str or '减少' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG5_CY_PRE_MR1_SHORT_REDUCE'
        return 'PCAP04_REG5_CY_PRE_MR1_SHORT_NORMAL'
    
    @staticmethod
    def map_reg5_c_port_pat(value):
        """映射 C_PORT_PAT 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'alternate' in value_str or '交替' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG5_C_PORT_PAT_ALTERNATE'
        return 'PCAP04_REG5_C_PORT_PAT_NORMAL'
    
    @staticmethod
    def map_reg5_cy_hfclk_sel(value):
        """映射 CY_HFCLK_SEL 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'ohf' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG5_CY_HFCLK_SEL_OHF'
        return 'PCAP04_REG5_CY_HFCLK_SEL_OLF'
    
    @staticmethod
    def map_reg5_cy_div4_dis(value):
        """映射 CY_DIV4_DIS 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'enable' in value_str or '开启' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG5_CY_DIV4_DIS_ENABLE'
        return 'PCAP04_REG5_CY_DIV4_DIS_DISABLE'
    
    @staticmethod
    def map_reg5_cy_pre_long(value):
        """映射 CY_PRE_LONG 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'enable' in value_str or '开启' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG5_CY_PRE_LONG_ENABLE'
        return 'PCAP04_REG5_CY_PRE_LONG_DISABLE'
    
    @staticmethod
    def map_reg5_c_dc_balance(value):
        """映射 C_DC_BALANCE 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'enable' in value_str or '开启' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG5_C_DC_BALANCE_ENABLE'
        return 'PCAP04_REG5_C_DC_BALANCE_DISABLE'
    
    @staticmethod
    def map_reg6_port_en(value):
        """映射 C_PORT_EN 值（位掩码）"""
        if isinstance(value, str):
            if value.startswith('0x') or value.startswith('0X'):
                return value
            if value.startswith('PCAP04_'):
                return value
        # 如果是数字，直接返回
        return str(value)
    
    @staticmethod
    def map_reg13_startonpin(value):
        """映射 C_STARTONPIN 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'pg1' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG13_C_STARTONPIN_PG1'
        elif 'pg2' in value_str or value == '2' or value == 2:
            return 'PCAP04_REG13_C_STARTONPIN_PG2'
        elif 'pg3' in value_str or value == '3' or value == 3:
            return 'PCAP04_REG13_C_STARTONPIN_PG3'
        return 'PCAP04_REG13_C_STARTONPIN_PG0'
    
    @staticmethod
    def map_reg13_trig_sel(value):
        """映射 C_TRIG_SEL 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'continuous' in value_str or '连续' in value_str:
            return 'PCAP04_REG13_C_TRIG_SEL_CONTINUOUS'
        elif 'read' in value_str or '读取' in value_str:
            return 'PCAP04_REG13_C_TRIG_SEL_READ_TRIGGER'
        elif 'timer' in value_str or '定时器' in value_str or '时间' in value_str:
            if 'stretch' in value_str or '拉伸' in value_str:
                return 'PCAP04_REG13_C_TRIG_SEL_TIMER_STRETCH'
            return 'PCAP04_REG13_C_TRIG_SEL_TIMER_TRIGGER'
        elif 'pin' in value_str or '引脚' in value_str:
            return 'PCAP04_REG13_C_TRIG_SEL_PIN_TRIGGER'
        elif 'opcode' in value_str or '操作码' in value_str:
            return 'PCAP04_REG13_C_TRIG_SEL_OPCODE_TRIGGER'
        return 'PCAP04_REG13_C_TRIG_SEL_TIMER_TRIGGER'
    
    @staticmethod
    def map_reg22_r_trig_sel(value):
        """映射 R_TRIG_SEL 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'disable' in value_str or '关闭' in value_str:
            return 'PCAP04_REG22_R_TRIG_SEL_DISABLE'
        elif 'timer' in value_str or '定时器' in value_str:
            return 'PCAP04_REG22_R_TRIG_SEL_TIMER_TRIGGER'
        elif 'pin' in value_str or '引脚' in value_str:
            return 'PCAP04_REG22_R_TRIG_SEL_PIN_TRIGGER'
        elif 'async' in value_str or '异步' in value_str:
            return 'PCAP04_REG22_R_TRIG_SEL_CDC_ASYNC'
        elif 'sync' in value_str or '同步' in value_str:
            return 'PCAP04_REG22_R_TRIG_SEL_CDC_SYNC'
        return 'PCAP04_REG22_R_TRIG_SEL_CDC_ASYNC'
    
    @staticmethod
    def map_reg22_r_avrg(value):
        """映射 R_AVRG 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if '4' in value_str:
            return 'PCAP04_REG22_R_AVRG_4_AVG'
        elif '8' in value_str:
            return 'PCAP04_REG22_R_AVRG_8_AVG'
        elif '16' in value_str:
            return 'PCAP04_REG22_R_AVRG_16_AVG'
        return 'PCAP04_REG22_R_AVRG_NO_AVG'
    
    @staticmethod
    def map_reg23_r_port_en(value):
        """映射 R_PORT_EN 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'ptoref' in value_str and 'pt1' in value_str:
            return 'PCAP04_REG23_R_PORT_EN_PT1_PTOREF'
        elif 'pt1' in value_str:
            return 'PCAP04_REG23_R_PORT_EN_PT1'
        elif 'ptoref' in value_str:
            return 'PCAP04_REG23_R_PORT_EN_PTOREF'
        return 'PCAP04_REG23_R_PORT_EN_DISABLE'
    
    @staticmethod
    def map_reg23_r_port_en_imes(value):
        """映射 R_PORT_EN_IMES 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'enable' in value_str or '激活' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG23_R_PORT_EN_IMES_ACTIVE'
        return 'PCAP04_REG23_R_PORT_EN_IMES_DISABLE'
    
    @staticmethod
    def map_reg23_r_port_en_iref(value):
        """映射 R_PORT_EN_IREF 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'enable' in value_str or '激活' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG23_R_PORT_EN_IREF_ACTIVE'
        return 'PCAP04_REG23_R_PORT_EN_IREF_DISABLE'
    
    @staticmethod
    def map_reg23_r_fake(value):
        """映射 R_FAKE 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if '8' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG23_R_FAKE_8_CYCLES'
        return 'PCAP04_REG23_R_FAKE_2_CYCLES'
    
    @staticmethod
    def map_reg23_r_startonpin(value):
        """映射 R_STARTONPIN 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'pg1' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG23_R_STARTONPIN_PG1'
        elif 'pg2' in value_str or value == '2' or value == 2:
            return 'PCAP04_REG23_R_STARTONPIN_PG2'
        elif 'pg3' in value_str or value == '3' or value == 3:
            return 'PCAP04_REG23_R_STARTONPIN_PG3'
        return 'PCAP04_REG23_R_STARTONPIN_PG0'
    
    @staticmethod
    def map_reg27_dsp_moflo_en(value):
        """映射 DSP_MOFLO_EN 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'enable' in value_str or '开启' in value_str or value == '3' or value == 3:
            return 'PCAP04_REG27_DSP_MOFLO_EN_ENABLE'
        return 'PCAP04_REG27_DSP_MOFLO_EN_DISABLE'
    
    @staticmethod
    def map_reg27_dsp_speed(value):
        """映射 DSP_SPEED 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'slowest' in value_str or '最慢' in value_str or value == '3' or value == 3:
            return 'PCAP04_REG27_DSP_SPEED_SLOWEST'
        elif 'slow' in value_str or '慢' in value_str or value == '2' or value == 2:
            return 'PCAP04_REG27_DSP_SPEED_SLOW'
        elif 'fast' in value_str or '快' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG27_DSP_SPEED_FAST'
        return 'PCAP04_REG27_DSP_SPEED_FASTEST'
    
    @staticmethod
    def map_reg27_pg_swap(value, pg_type='PG1xPG3'):
        """映射 PG 交换值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'swap' in value_str or '切换' in value_str or value == '1' or value == 1:
            return f'PCAP04_REG27_{pg_type}_SWAP'
        return f'PCAP04_REG27_{pg_type}_NORMAL'
    
    @staticmethod
    def map_reg28_wd_dis(value):
        """映射 WD_DIS 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'disable' in value_str or '禁用' in value_str or value == '0x5A' or value == 0x5A:
            return 'PCAP04_REG28_WD_DIS_DISABLE'
        return 'PCAP04_REG28_WD_DIS_ENABLE'
    
    @staticmethod
    def map_reg30_pg_intn_en(value, pg_num=5):
        """映射 PG_INTN_EN 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'route' in value_str or '路由' in value_str or value == '1' or value == 1:
            return f'PCAP04_REG30_PG{pg_num}_INTN_EN_ROUTE'
        return f'PCAP04_REG30_PG{pg_num}_INTN_EN_NORMAL'
    
    @staticmethod
    def map_reg30_dsp_start_en(value):
        """映射 DSP_START_EN 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'cdc' in value_str and 'end' in value_str:
            return 'PCAP04_REG30_DSP_START_EN_CDC_END'
        elif 'rdc' in value_str and 'end' in value_str or 'rdc' in value_str:
            return 'PCAP04_REG30_DSP_START_EN_RDC_END'
        elif 'timer' in value_str or '定时器' in value_str:
            return 'PCAP04_REG30_DSP_START_EN_TIMER'
        return 'PCAP04_REG30_DSP_START_EN_RDC_END'
    
    @staticmethod
    def map_reg31_pio_res(value):
        """映射 PIO_RES 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if '16' in value_str or value == '3' or value == 3:
            return 'PCAP04_REG31_PIO_RES_16BIT'
        elif '14' in value_str or value == '2' or value == 2:
            return 'PCAP04_REG31_PIO_RES_14BIT'
        elif '12' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG31_PIO_RES_12BIT'
        return 'PCAP04_REG31_PIO_RES_10BIT'
    
    @staticmethod
    def map_reg31_pio_pdm_sel(value):
        """映射 PIO_PDM_SEL 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'pdm' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG31_PIO_PDM_SEL_PDM'
        return 'PCAP04_REG31_PIO_PDM_SEL_PWM'
    
    @staticmethod
    def map_reg31_pio_clk_sel(value):
        """映射 PIO_CLK_SEL 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'off' in value_str or '关闭' in value_str or value == '0' or value == 0:
            return 'PCAP04_REG31_PIO_CLK_SEL_OFF'
        elif 'olf' in value_str:
            if '/2' in value_str or 'div2' in value_str or value == '2' or value == 2:
                return 'PCAP04_REG31_PIO_CLK_SEL_OLF_DIV2'
            elif '/4' in value_str or 'div4' in value_str or value == '3' or value == 3:
                return 'PCAP04_REG31_PIO_CLK_SEL_OLF_DIV4'
            return 'PCAP04_REG31_PIO_CLK_SEL_OLF_DIV1'
        elif 'ox' in value_str:
            if '/2' in value_str or 'div2' in value_str or value == '5' or value == 5:
                return 'PCAP04_REG31_PIO_CLK_SEL_OX_DIV2'
            elif '/4' in value_str or 'div4' in value_str or value == '6' or value == 6:
                return 'PCAP04_REG31_PIO_CLK_SEL_OX_DIV4'
            return 'PCAP04_REG31_PIO_CLK_SEL_OX_DIV1'
        return 'PCAP04_REG31_PIO_CLK_SEL_OFF'
    
    @staticmethod
    def map_reg32_pi1_res(value):
        """映射 PI1_RES 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if '16' in value_str or value == '3' or value == 3:
            return 'PCAP04_REG32_PI1_RES_16BIT'
        elif '14' in value_str or value == '2' or value == 2:
            return 'PCAP04_REG32_PI1_RES_14BIT'
        elif '12' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG32_PI1_RES_12BIT'
        return 'PCAP04_REG32_PI1_RES_10BIT'
    
    @staticmethod
    def map_reg32_pi1_pdm_sel(value):
        """映射 PI1_PDM_SEL 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'pdm' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG32_PI1_PDM_SEL_PDM'
        return 'PCAP04_REG32_PI1_PDM_SEL_PWM'
    
    @staticmethod
    def map_reg32_pi1_clk_sel(value):
        """映射 PI1_CLK_SEL 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'off' in value_str or '关闭' in value_str or value == '0' or value == 0:
            return 'PCAP04_REG32_PI1_CLK_SEL_OFF'
        elif 'olf' in value_str:
            if '/2' in value_str or 'div2' in value_str or value == '2' or value == 2:
                return 'PCAP04_REG32_PI1_CLK_SEL_OLF_DIV2'
            elif '/4' in value_str or 'div4' in value_str or value == '3' or value == 3:
                return 'PCAP04_REG32_PI1_CLK_SEL_OLF_DIV4'
            return 'PCAP04_REG32_PI1_CLK_SEL_OLF_DIV1'
        elif 'ox' in value_str:
            if '/2' in value_str or 'div2' in value_str or value == '5' or value == 5:
                return 'PCAP04_REG32_PI1_CLK_SEL_OX_DIV2'
            elif '/4' in value_str or 'div4' in value_str or value == '6' or value == 6:
                return 'PCAP04_REG32_PI1_CLK_SEL_OX_DIV4'
            return 'PCAP04_REG32_PI1_CLK_SEL_OX_DIV1'
        return 'PCAP04_REG32_PI1_CLK_SEL_OFF'
    
    @staticmethod
    def map_reg34_int_trig_bg(value):
        """映射 INT_TRIG_BG 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'enable' in value_str or '开启' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG34_INT_TRIG_BG_ENABLE'
        return 'PCAP04_REG34_INT_TRIG_BG_DISABLE'
    
    @staticmethod
    def map_reg34_dsp_trig_bg(value):
        """映射 DSP_TRIG_BG 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'enable' in value_str or '开启' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG34_DSP_TRIG_BG_ENABLE'
        return 'PCAP04_REG34_DSP_TRIG_BG_DISABLE'
    
    @staticmethod
    def map_reg34_bg_perm(value):
        """映射 BG_PERM 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'permanent' in value_str or '永久' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG34_BG_PERM_PERMANENT'
        return 'PCAP04_REG34_BG_PERM_PULSE_MODE'
    
    @staticmethod
    def map_reg34_autostart(value):
        """映射 AUTOSTART 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'enable' in value_str or '开启' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG34_AUTOSTART_ENABLE'
        return 'PCAP04_REG34_AUTOSTART_DISABLE'
    
    @staticmethod
    def map_reg35_gain_corr(value):
        """映射 CDC_GAIN_CORR 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if '1.25' in value_str or '1_25' in value_str or value == '0x40' or value == 0x40 or value == '64' or value == 64:
            return 'PCAP04_REG35_CDC_GAIN_CORR_1_25'
        elif '1.5' in value_str or '1_5' in value_str or value == '0x80' or value == 0x80 or value == '128' or value == 128:
            return 'PCAP04_REG35_CDC_GAIN_CORR_1_50'
        elif '1.125' in value_str or '1_125' in value_str or value == '0x20' or value == 0x20 or value == '32' or value == 32:
            return 'PCAP04_REG35_CDC_GAIN_CORR_1_125'
        elif '1.375' in value_str or '1_375' in value_str or value == '0x60' or value == 0x60 or value == '96' or value == 96:
            return 'PCAP04_REG35_CDC_GAIN_CORR_1_375'
        elif '1.996' in value_str or '1_996' in value_str or value == '0xFF' or value == 0xFF or value == '255' or value == 255:
            return 'PCAP04_REG35_CDC_GAIN_CORR_1_996'
        return 'PCAP04_REG35_CDC_GAIN_CORR_1_25'
    
    @staticmethod
    def map_reg39_pulse_sel(value, sel_num=0):
        """映射 PULSE_SEL 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        prefix = f'PCAP04_REG39_PULSE_SEL{sel_num}_'
        if 'res7' in value_str or 'res_7' in value_str or value == '7' or value == 7:
            return f'{prefix}RES7'
        elif 'res6' in value_str or 'res_6' in value_str or value == '6' or value == 6:
            return f'{prefix}RES6'
        elif 'res5' in value_str or 'res_5' in value_str or value == '5' or value == 5:
            return f'{prefix}RES5'
        elif 'res4' in value_str or 'res_4' in value_str or value == '4' or value == 4:
            return f'{prefix}RES4'
        elif 'res3' in value_str or 'res_3' in value_str or value == '3' or value == 3:
            return f'{prefix}RES3'
        elif 'res2' in value_str or 'res_2' in value_str or value == '2' or value == 2:
            return f'{prefix}RES2'
        elif 'res1' in value_str or 'res_1' in value_str or value == '1' or value == 1:
            return f'{prefix}RES1'
        return f'{prefix}RES0'
    
    @staticmethod
    def map_reg42_alarm_select(value, alarm_num=0):
        """映射 ALARM_SELECT 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        prefix = f'PCAP04_REG42_ALARM{alarm_num}_SELECT_'
        if 'theta' in value_str or value == '1' or value == 1:
            return f'{prefix}Theta'
        return f'{prefix}Z'
    
    @staticmethod
    def map_reg42_en_async_read(value):
        """映射 EN_ASYNC_READ 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'enable' in value_str or '激活' in value_str or 'active' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG42_EN_ASYNC_READ_ACTIVE'
        return 'PCAP04_REG42_EN_ASYNC_READ_DISABLE'
    
    @staticmethod
    def map_reg42_median_en(value, median_type='C'):
        """映射 MEDIAN_EN 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        prefix = f'PCAP04_REG42_{median_type}_MEDIAN_EN_'
        if 'enable' in value_str or '开启' in value_str or value == '1' or value == 1:
            return f'{prefix}ENABLE'
        return f'{prefix}DISABLE'
    
    @staticmethod
    def map_reg47_runbit(value):
        """映射 RUNBIT 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'off' in value_str or '关闭' in value_str or value == '0' or value == 0:
            return 'PCAP04_REG47_RUNBIT_OFF'
        return 'PCAP04_REG47_RUNBIT_ON'
    
    @staticmethod
    def map_reg18_c_g_op_run(value):
        """映射 C_G_OP_RUN 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'pulse' in value_str or '脉冲' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG18_C_G_OP_RUN_PULSED'
        return 'PCAP04_REG18_C_G_OP_RUN_PERMANENT'
    
    @staticmethod
    def map_reg18_c_g_op_ext(value):
        """映射 C_G_OP_EXT 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'external' in value_str or '外部' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG18_C_G_OP_EXT_EXTERNAL'
        return 'PCAP04_REG18_C_G_OP_EXT_INTERNAL'
    
    @staticmethod
    def map_reg18_c_g_en(value):
        """映射 C_G_EN 值（位掩码）"""
        if isinstance(value, str):
            if value.startswith('0x') or value.startswith('0X'):
                return value
            if value.startswith('PCAP04_'):
                return value
        # 如果是数字，直接返回
        return str(value)
    
    @staticmethod
    def map_reg19_c_g_op_vu(value):
        """映射 C_G_OP_VU 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if '1.03' in value_str or '1_03' in value_str or value == '3' or value == 3:
            return 'PCAP04_REG19_C_G_OP_VU_1_03'
        elif '1.02' in value_str or '1_02' in value_str or value == '2' or value == 2:
            return 'PCAP04_REG19_C_G_OP_VU_1_02'
        elif '1.01' in value_str or '1_01' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG19_C_G_OP_VU_1_01'
        return 'PCAP04_REG19_C_G_OP_VU_1_00'
    
    @staticmethod
    def map_reg19_c_g_op_attn(value):
        """映射 C_G_OP_ATTN 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if '2.0' in value_str or '2_0' in value_str or value == '3' or value == 3:
            return 'PCAP04_REG19_C_G_OP_ATTN_2_0_AF'
        elif '1.5' in value_str or '1_5' in value_str or value == '2' or value == 2:
            return 'PCAP04_REG19_C_G_OP_ATTN_1_5_AF'
        elif '1.0' in value_str or '1_0' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG19_C_G_OP_ATTN_1_0_AF'
        return 'PCAP04_REG19_C_G_OP_ATTN_0_5_AF'
    
    @staticmethod
    def map_reg20_r_cy(value):
        """映射 R_CY 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'long' in value_str or '长' in value_str or value == '1' or value == 1:
            return 'PCAP04_REG20_R_CY_LONG'
        return 'PCAP04_REG20_R_CY_SHORT'
    
    @staticmethod
    def map_reg33_pg_dir_in(value):
        """映射 PG_DIR_IN 值（位掩码）"""
        if isinstance(value, str):
            if value.startswith('0x') or value.startswith('0X'):
                return value
            if value.startswith('PCAP04_'):
                return value
        return str(value)
    
    @staticmethod
    def map_reg33_pg_pu(value):
        """映射 PG_PU 值（位掩码）"""
        if isinstance(value, str):
            if value.startswith('0x') or value.startswith('0X'):
                return value
            if value.startswith('PCAP04_'):
                return value
        return str(value)
    
    @staticmethod
    def map_reg31_toggle_en(value, toggle_type='PIO'):
        """映射 TOGGLE_EN 值"""
        if isinstance(value, str) and value.startswith('PCAP04_'):
            return value
        value_str = str(value).lower()
        if 'toggle' in value_str or '切换' in value_str or value == '1' or value == 1:
            return f'PCAP04_REG31_{toggle_type}_TOGGLE_EN_TOGGLE'
        return f'PCAP04_REG31_{toggle_type}_TOGGLE_EN_NORMAL'

