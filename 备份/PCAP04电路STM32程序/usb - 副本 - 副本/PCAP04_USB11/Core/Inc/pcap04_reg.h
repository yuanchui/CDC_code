#ifndef PCAP04_REGISTERS_H
#define PCAP04_REGISTERS_H

#include <stdint.h> // 包含stdint.h以使用uint8_t, uint16_t等标准整数类型

// --- PCAP04 I2C 从机地址 ---
// 注意: 实际的I2C从机地址通过寄存器0中的I2C_A位配置。
// 默认配置 (I2C_A = 0) 通常意味着设备响应0x28 (写) / 0x29 (读)
// 或0x50 (写) / 0x51 (读)，具体取决于芯片型号和I2C_A的解释。
// 此处以0x28作为示例基地址 (7位地址，用于8位读/写操作时需左移)。
#define PCAP04_I2C_BASE_ADDR        (0x28) // 示例7位I2C基地址

// --- PCAP04 寄存器地址定义 ---
// PCAP04共有64个寄存器，地址从0x00到0x3F
#define PCAP04_REG_ADDR_0           0x00
#define PCAP04_REG_ADDR_1           0x01
#define PCAP04_REG_ADDR_2           0x02
#define PCAP04_REG_ADDR_3           0x03
#define PCAP04_REG_ADDR_4           0x04
#define PCAP04_REG_ADDR_5           0x05
#define PCAP04_REG_ADDR_6           0x06
#define PCAP04_REG_ADDR_7           0x07
#define PCAP04_REG_ADDR_8           0x08
#define PCAP04_REG_ADDR_9           0x09
#define PCAP04_REG_ADDR_10          0x0A
#define PCAP04_REG_ADDR_11          0x0B
#define PCAP04_REG_ADDR_12          0x0C
#define PCAP04_REG_ADDR_13          0x0D
#define PCAP04_REG_ADDR_14          0x0E
#define PCAP04_REG_ADDR_15          0x0F
#define PCAP04_REG_ADDR_16          0x10
#define PCAP04_REG_ADDR_17          0x11
#define PCAP04_REG_ADDR_18          0x12
#define PCAP04_REG_ADDR_19          0x13
#define PCAP04_REG_ADDR_20          0x14
#define PCAP04_REG_ADDR_21          0x15
#define PCAP04_REG_ADDR_22          0x16
#define PCAP04_REG_ADDR_23          0x17
#define PCAP04_REG_ADDR_24          0x18
#define PCAP04_REG_ADDR_25          0x19
#define PCAP04_REG_ADDR_26          0x1A
#define PCAP04_REG_ADDR_27          0x1B
#define PCAP04_REG_ADDR_28          0x1C
#define PCAP04_REG_ADDR_29          0x1D
#define PCAP04_REG_ADDR_30          0x1E
#define PCAP04_REG_ADDR_31          0x1F
#define PCAP04_REG_ADDR_32          0x20
#define PCAP04_REG_ADDR_33          0x21
#define PCAP04_REG_ADDR_34          0x22
#define PCAP04_REG_ADDR_35          0x23
#define PCAP04_REG_ADDR_36          0x24 
#define PCAP04_REG_ADDR_37          0x25 
#define PCAP04_REG_ADDR_38          0x26
#define PCAP04_REG_ADDR_39          0x27
#define PCAP04_REG_ADDR_40          0x28
#define PCAP04_REG_ADDR_41          0x29
#define PCAP04_REG_ADDR_42          0x2A
#define PCAP04_REG_ADDR_43          0x2B // 保留
#define PCAP04_REG_ADDR_44          0x2C // 保留
#define PCAP04_REG_ADDR_45          0x2D // 保留
#define PCAP04_REG_ADDR_46          0x2E // 保留
#define PCAP04_REG_ADDR_47          0x2F
#define PCAP04_REG_ADDR_48          0x30
#define PCAP04_REG_ADDR_49          0x31
#define PCAP04_REG_ADDR_50          0x32
#define PCAP04_REG_ADDR_51          0x33 // ams内部寄存器
#define PCAP04_REG_ADDR_52          0x34 // ams内部寄存器
#define PCAP04_REG_ADDR_53          0x35 // ams内部寄存器
#define PCAP04_REG_ADDR_54          0x36
#define PCAP04_REG_ADDR_55          0x37 // ams内部寄存器
#define PCAP04_REG_ADDR_56          0x38 // ams内部寄存器
#define PCAP04_REG_ADDR_57          0x39 // ams内部寄存器
#define PCAP04_REG_ADDR_58          0x3A // ams内部寄存器
#define PCAP04_REG_ADDR_59          0x3B // ams内部寄存器
#define PCAP04_REG_ADDR_60          0x3C // ams内部寄存器
#define PCAP04_REG_ADDR_61          0x3D // ams内部寄存器
#define PCAP04_REG_ADDR_62          0x3E // 电荷泵配置 (禁止修改)
#define PCAP04_REG_ADDR_63          0x3F // 电荷泵配置 (禁止修改)


// ============================================================================
// 各寄存器的位域定义（按寄存器顺序排列）
// ============================================================================

// ----------------------------------------------------------------------------
// 寄存器 0 (0x00) - IIC地址与低频时钟配置
// ----------------------------------------------------------------------------
// 位[7:6] I2C_A: I²C设备地址补码配置（用于设置I²C从机地址）
#define PCAP04_REG0_I2C_A_SHIFT             6
#define PCAP04_REG0_I2C_A_MASK              (0x03 << PCAP04_REG0_I2C_A_SHIFT)
#define PCAP04_REG0_I2C_A_bit(val)          (((val) & 0x03) << PCAP04_REG0_I2C_A_SHIFT)

// 位[5:2] OLF_FTUNE: 低频时钟微调（范围: 0-15）
#define PCAP04_REG0_OLF_FTUNE_SHIFT         2
#define PCAP04_REG0_OLF_FTUNE_MASK          (0x0F << PCAP04_REG0_OLF_FTUNE_SHIFT)
#define PCAP04_REG0_OLF_FTUNE_bit(val)       (((val) & 0x0F) << PCAP04_REG0_OLF_FTUNE_SHIFT)
#define PCAP04_REG0_OLF_FTUNE_MIN           0   // 最小值
#define PCAP04_REG0_OLF_FTUNE_TYPICAL       7   // 典型值 (推荐)
#define PCAP04_REG0_OLF_FTUNE_MAX           15  // 最大值

// 位[1:0] OLF_CTUNE: 低频时钟粗调
#define PCAP04_REG0_OLF_CTUNE_SHIFT         0
#define PCAP04_REG0_OLF_CTUNE_MASK          (0x03 << PCAP04_REG0_OLF_CTUNE_SHIFT)
#define PCAP04_REG0_OLF_CTUNE_bit(val)      (((val) & 0x03) << PCAP04_REG0_OLF_CTUNE_SHIFT)
#define PCAP04_REG0_OLF_CTUNE_10k           0x00  // 10 kHz
#define PCAP04_REG0_OLF_CTUNE_50k           0x01  // 50 kHz
#define PCAP04_REG0_OLF_CTUNE_100k          0x02  // 100 kHz
#define PCAP04_REG0_OLF_CTUNE_200k          0x03  // 200 kHz (推荐)

// 寄存器0完整值构建宏
#define PCAP04_REG0_VALUE(i2c_a, olf_ftune, olf_ctune) \
    (PCAP04_REG0_I2C_A_bit(i2c_a) | PCAP04_REG0_OLF_FTUNE_bit(olf_ftune) | PCAP04_REG0_OLF_CTUNE_bit(olf_ctune))

// ----------------------------------------------------------------------------
// 寄存器 1 (0x01) - 外部晶振时钟OX配置
// ----------------------------------------------------------------------------
// 位[7] OX_DIS: 禁用OX时钟
#define PCAP04_REG1_OX_DIS_SHIFT            7
#define PCAP04_REG1_OX_DIS_MASK             (0x01 << PCAP04_REG1_OX_DIS_SHIFT)
#define PCAP04_REG1_OX_DIS_bit(val)         (((val) & 0x01) << PCAP04_REG1_OX_DIS_SHIFT)
#define PCAP04_REG1_OX_DIS_ENABLE           0  // 启用 (默认)
#define PCAP04_REG1_OX_DIS_DISABLE          1  // 禁用

// 位[5] OX_DIV4: OX时钟4分频
#define PCAP04_REG1_OX_DIV4_SHIFT           5
#define PCAP04_REG1_OX_DIV4_MASK            (0x01 << PCAP04_REG1_OX_DIV4_SHIFT)
#define PCAP04_REG1_OX_DIV4_bit(val)        (((val) & 0x01) << PCAP04_REG1_OX_DIV4_SHIFT)
#define PCAP04_REG1_OX_DIV4_NO_DIV          0  // 不分频，f_ox = 2MHz
#define PCAP04_REG1_OX_DIV4_DIV4            1  // 4分频，f_ox = 0.5MHz

// 位[4] OX_AUTOSTOP_DIS: ams内部位 (默认: 0)
#define PCAP04_REG1_OX_AUTOSTOP_DIS_SHIFT   4
#define PCAP04_REG1_OX_AUTOSTOP_DIS_MASK    (0x01 << PCAP04_REG1_OX_AUTOSTOP_DIS_SHIFT)
#define PCAP04_REG1_OX_AUTOSTOP_DIS_bit(val) (((val) & 0x01) << PCAP04_REG1_OX_AUTOSTOP_DIS_SHIFT)

// 位[3] OX_STOP: ams内部位 (默认: 0)
#define PCAP04_REG1_OX_STOP_SHIFT           3
#define PCAP04_REG1_OX_STOP_MASK            (0x01 << PCAP04_REG1_OX_STOP_SHIFT)
#define PCAP04_REG1_OX_STOP_bit(val)        (((val) & 0x01) << PCAP04_REG1_OX_STOP_SHIFT)

// 位[2:0] OX_RUN: 控制OX发生器的持续性或延迟
#define PCAP04_REG1_OX_RUN_SHIFT            0
#define PCAP04_REG1_OX_RUN_MASK             (0x07 << PCAP04_REG1_OX_RUN_SHIFT)
#define PCAP04_REG1_OX_RUN_bit(val)         (((val) & 0x07) << PCAP04_REG1_OX_RUN_SHIFT)
#define PCAP04_REG1_OX_RUN_OFF               0x00  // 发生器关闭
#define PCAP04_REG1_OX_RUN_PERMANENT        0x01  // OX永久运行
#define PCAP04_REG1_OX_RUN_DELAY31_fOLF     0x02  // OX延迟 = 31 / fOLF
#define PCAP04_REG1_OX_RUN_DELAY2_fOLF      0x03  // OX延迟 = 2 / fOLF
#define PCAP04_REG1_OX_RUN_DELAY1_fOLF      0x06  // OX延迟 = 1 / fOLF

// 寄存器1完整值构建宏（ams内部位OX_AUTOSTOP_DIS和OX_STOP使用默认值0，无需输入）
#define PCAP04_REG1_VALUE(ox_dis, ox_div4, ox_run) \
    (PCAP04_REG1_OX_DIS_bit(ox_dis) | PCAP04_REG1_OX_DIV4_bit(ox_div4) | \
     PCAP04_REG1_OX_AUTOSTOP_DIS_bit(0) | PCAP04_REG1_OX_STOP_bit(0) | \
     PCAP04_REG1_OX_RUN_bit(ox_run))

// ----------------------------------------------------------------------------
// 寄存器 2 (0x02) - 端口放电电阻配置
// ----------------------------------------------------------------------------
// 位[7:6] RDCHG_INT_SEL1: PC4~PC5的片上放电电阻选择
#define PCAP04_REG2_RDCHG_INT_SEL1_SHIFT    6
#define PCAP04_REG2_RDCHG_INT_SEL1_MASK     (0x03 << PCAP04_REG2_RDCHG_INT_SEL1_SHIFT)
#define PCAP04_REG2_RDCHG_INT_SEL1_bit(val) (((val) & 0x03) << PCAP04_REG2_RDCHG_INT_SEL1_SHIFT)
#define PCAP04_REG2_RDCHG_INT_SEL1_180K     0x00  // 180 kΩ
#define PCAP04_REG2_RDCHG_INT_SEL1_90K      0x01  // 90 kΩ
#define PCAP04_REG2_RDCHG_INT_SEL1_30K      0x02  // 30 kΩ (默认，推荐)
#define PCAP04_REG2_RDCHG_INT_SEL1_10K      0x03  // 10 kΩ

// 位[5:4] RDCHG_INT_SEL0: PC0~PC3和内部端口PC6的片上放电电阻选择
#define PCAP04_REG2_RDCHG_INT_SEL0_SHIFT    4
#define PCAP04_REG2_RDCHG_INT_SEL0_MASK     (0x03 << PCAP04_REG2_RDCHG_INT_SEL0_SHIFT)
#define PCAP04_REG2_RDCHG_INT_SEL0_bit(val) (((val) & 0x03) << PCAP04_REG2_RDCHG_INT_SEL0_SHIFT)
#define PCAP04_REG2_RDCHG_INT_SEL0_180K     0x00  // 180 kΩ
#define PCAP04_REG2_RDCHG_INT_SEL0_90K      0x01  // 90 kΩ
#define PCAP04_REG2_RDCHG_INT_SEL0_30K      0x02  // 30 kΩ (默认，推荐)
#define PCAP04_REG2_RDCHG_INT_SEL0_10K      0x03  // 10 kΩ

// 位[3] RDCHG_INT_EN: 内部放电电阻使能
#define PCAP04_REG2_RDCHG_INT_EN_SHIFT      3
#define PCAP04_REG2_RDCHG_INT_EN_MASK       (0x01 << PCAP04_REG2_RDCHG_INT_EN_SHIFT)
#define PCAP04_REG2_RDCHG_INT_EN_bit(val)   (((val) & 0x01) << PCAP04_REG2_RDCHG_INT_EN_SHIFT)
#define PCAP04_REG2_RDCHG_INT_EN_DISABLE    0  // 关闭
#define PCAP04_REG2_RDCHG_INT_EN_ENABLE     1  // 内部开启 (默认)

// 位[1] RDCHG_EXT_EN: 外部放电电阻使能
#define PCAP04_REG2_RDCHG_EXT_EN_SHIFT      1
#define PCAP04_REG2_RDCHG_EXT_EN_MASK       (0x01 << PCAP04_REG2_RDCHG_EXT_EN_SHIFT)
#define PCAP04_REG2_RDCHG_EXT_EN_bit(val)   (((val) & 0x01) << PCAP04_REG2_RDCHG_EXT_EN_SHIFT)
#define PCAP04_REG2_RDCHG_EXT_EN_DISABLE    0  // 关闭 (默认)
#define PCAP04_REG2_RDCHG_EXT_EN_ENABLE     1  // 外部开启

// 寄存器2完整值构建宏
#define PCAP04_REG2_VALUE(sel1, sel0, int_en, ext_en) \
    (PCAP04_REG2_RDCHG_INT_SEL1_bit(sel1) | PCAP04_REG2_RDCHG_INT_SEL0_bit(sel0) | \
     PCAP04_REG2_RDCHG_INT_EN_bit(int_en) | PCAP04_REG2_RDCHG_EXT_EN_bit(ext_en))

// ----------------------------------------------------------------------------
// 寄存器 3 (0x03) - 端口充放电电阻配置
// ----------------------------------------------------------------------------
// 位[6] AUX_PD_DIS: 禁用PCAUX下拉电阻
#define PCAP04_REG3_AUX_PD_DIS_SHIFT        6
#define PCAP04_REG3_AUX_PD_DIS_MASK         (0x01 << PCAP04_REG3_AUX_PD_DIS_SHIFT)
#define PCAP04_REG3_AUX_PD_DIS_bit(val)     (((val) & 0x01) << PCAP04_REG3_AUX_PD_DIS_SHIFT)
#define PCAP04_REG3_AUX_PD_DIS_ACTIVE       0  // 下拉激活 (默认)
#define PCAP04_REG3_AUX_PD_DIS_DISABLE      1  // 下拉禁用

// 位[5] AUX_CINT: 仅在内部参考转换期间激活辅助端口PCAUX
#define PCAP04_REG3_AUX_CINT_SHIFT          5
#define PCAP04_REG3_AUX_CINT_MASK           (0x01 << PCAP04_REG3_AUX_CINT_SHIFT)
#define PCAP04_REG3_AUX_CINT_bit(val)       (((val) & 0x01) << PCAP04_REG3_AUX_CINT_SHIFT)
#define PCAP04_REG3_AUX_CINT_NORMAL         0  // 正常 (默认)
#define PCAP04_REG3_AUX_CINT_ACTIVE         1  // 在内部c参考转换期间激活辅助端口

// 位[4:3] RDCHG_OPEN: ams内部位 (推荐值: 2)
#define PCAP04_REG3_RDCHG_OPEN_SHIFT        3
#define PCAP04_REG3_RDCHG_OPEN_MASK         (0x03 << PCAP04_REG3_RDCHG_OPEN_SHIFT)
#define PCAP04_REG3_RDCHG_OPEN_bit(val)     (((val) & 0x03) << PCAP04_REG3_RDCHG_OPEN_SHIFT)

// 位[2] RDCHG_PERM_EN: 保持芯片内部放电电阻永久连接
#define PCAP04_REG3_RDCHG_PERM_EN_SHIFT     2
#define PCAP04_REG3_RDCHG_PERM_EN_MASK      (0x01 << PCAP04_REG3_RDCHG_PERM_EN_SHIFT)
#define PCAP04_REG3_RDCHG_PERM_EN_bit(val)  (((val) & 0x01) << PCAP04_REG3_RDCHG_PERM_EN_SHIFT)
#define PCAP04_REG3_RDCHG_PERM_EN_DISABLE   0  // 关闭 (默认)
#define PCAP04_REG3_RDCHG_PERM_EN_ENABLE    1  // 开启

// 位[1] RDCHG_EXT_PERM: 永久激活辅助端口PCAUX
#define PCAP04_REG3_RDCHG_EXT_PERM_SHIFT    1
#define PCAP04_REG3_RDCHG_EXT_PERM_MASK     (0x01 << PCAP04_REG3_RDCHG_EXT_PERM_SHIFT)
#define PCAP04_REG3_RDCHG_EXT_PERM_bit(val) (((val) & 0x01) << PCAP04_REG3_RDCHG_EXT_PERM_SHIFT)
#define PCAP04_REG3_RDCHG_EXT_PERM_DISABLE  0  // 关闭 (默认)
#define PCAP04_REG3_RDCHG_EXT_PERM_ENABLE   1  // 开启

// 位[0] RCHG_SEL: 选择充电电阻
#define PCAP04_REG3_RCHG_SEL_SHIFT          0
#define PCAP04_REG3_RCHG_SEL_MASK           (0x01 << PCAP04_REG3_RCHG_SEL_SHIFT)
#define PCAP04_REG3_RCHG_SEL_bit(val)       (((val) & 0x01) << PCAP04_REG3_RCHG_SEL_SHIFT)
#define PCAP04_REG3_RCHG_SEL_180K           0x00  // 180 kΩ
#define PCAP04_REG3_RCHG_SEL_10K            0x01  // 10 kΩ (默认)

// 寄存器3完整值构建宏（ams内部位RDCHG_OPEN使用默认值0，无需输入）
#define PCAP04_REG3_VALUE(aux_pd_dis, aux_cint, rdchg_perm_en, rdchg_ext_perm, rchg_sel) \
    (PCAP04_REG3_AUX_PD_DIS_bit(aux_pd_dis) | PCAP04_REG3_AUX_CINT_bit(aux_cint) | \
     PCAP04_REG3_RDCHG_OPEN_bit(2) | PCAP04_REG3_RDCHG_PERM_EN_bit(rdchg_perm_en) | \
     PCAP04_REG3_RDCHG_EXT_PERM_bit(rdchg_ext_perm) | PCAP04_REG3_RCHG_SEL_bit(rchg_sel))

// ----------------------------------------------------------------------------
// 寄存器 4 (0x04) - 端口电容连接方式
// ----------------------------------------------------------------------------
// 位[7] C_REF_INT: 使用片上参考电容（bit7用于选择使用外部参考电容或内部参考电容，内部参考电容值为1~31pF，如果我们需要测量的电容大于500pF时，务必使用外部参考电容）
#define PCAP04_REG4_C_REF_INT_SHIFT         7
#define PCAP04_REG4_C_REF_INT_MASK          (0x01 << PCAP04_REG4_C_REF_INT_SHIFT)
#define PCAP04_REG4_C_REF_INT_bit(val)      (((val) & 0x01) << PCAP04_REG4_C_REF_INT_SHIFT)
#define PCAP04_REG4_C_REF_INT_EXTERNAL      0  // 外部参考 (默认)
#define PCAP04_REG4_C_REF_INT_INTERNAL      1  // 内部参考

// 位[5] C_COMP_EXT: 激活外部寄生电容补偿机制（bit5为外部寄生电容补偿位，建议开启，这样芯片内部将会通过内部电路独立放电来测试寄生电容对测量值进行修正）
#define PCAP04_REG4_C_COMP_EXT_SHIFT        5
#define PCAP04_REG4_C_COMP_EXT_MASK         (0x01 << PCAP04_REG4_C_COMP_EXT_SHIFT)
#define PCAP04_REG4_C_COMP_EXT_bit(val)     (((val) & 0x01) << PCAP04_REG4_C_COMP_EXT_SHIFT)
#define PCAP04_REG4_C_COMP_EXT_IDLE         0  // 空闲 (默认)
#define PCAP04_REG4_C_COMP_EXT_ACTIVE        1  // 激活

// 位[4] C_COMP_INT: 激活片上寄生电容补偿机制和增益补偿（bit4为内部寄生电容补偿位）
#define PCAP04_REG4_C_COMP_INT_SHIFT        4
#define PCAP04_REG4_C_COMP_INT_MASK         (0x01 << PCAP04_REG4_C_COMP_INT_SHIFT)
#define PCAP04_REG4_C_COMP_INT_bit(val)     (((val) & 0x01) << PCAP04_REG4_C_COMP_INT_SHIFT)
#define PCAP04_REG4_C_COMP_INT_IDLE         0  // 空闲 (默认)
#define PCAP04_REG4_C_COMP_INT_ACTIVE        1  // 激活

// 位[1] C_DIFFERENTIAL: 选择单端或差分传感器（bit1和bit0用于是独立测量还是差分测量）
#define PCAP04_REG4_C_DIFFERENTIAL_SHIFT    1
#define PCAP04_REG4_C_DIFFERENTIAL_MASK     (0x01 << PCAP04_REG4_C_DIFFERENTIAL_SHIFT)
#define PCAP04_REG4_C_DIFFERENTIAL_bit(val) (((val) & 0x01) << PCAP04_REG4_C_DIFFERENTIAL_SHIFT)
#define PCAP04_REG4_C_DIFFERENTIAL_SINGLE_ENDED  0  // 普通 (单端) (默认)
#define PCAP04_REG4_C_DIFFERENTIAL_DIFF         1  // 差分

// 位[0] C_FLOATING: 选择接地或浮动传感器
#define PCAP04_REG4_C_FLOATING_SHIFT        0
#define PCAP04_REG4_C_FLOATING_MASK         (0x01 << PCAP04_REG4_C_FLOATING_SHIFT)
#define PCAP04_REG4_C_FLOATING_bit(val)     (((val) & 0x01) << PCAP04_REG4_C_FLOATING_SHIFT)
#define PCAP04_REG4_C_FLOATING_GROUNDED      0  // 接地 (默认)
#define PCAP04_REG4_C_FLOATING_FLOATING      1  // 浮动

// 寄存器4完整值构建宏
#define PCAP04_REG4_VALUE(c_ref_int, c_comp_ext, c_comp_int, c_differential, c_floating) \
    (PCAP04_REG4_C_REF_INT_bit(c_ref_int) | PCAP04_REG4_C_COMP_EXT_bit(c_comp_ext) | \
     PCAP04_REG4_C_COMP_INT_bit(c_comp_int) | PCAP04_REG4_C_DIFFERENTIAL_bit(c_differential) | \
     PCAP04_REG4_C_FLOATING_bit(c_floating))

// ----------------------------------------------------------------------------
// 寄存器 5 (0x05) - CDC时钟与端口模式配置
// ----------------------------------------------------------------------------
// 位[7] CY_PRE_MR1_SHORT: 减少内部时钟路径之间的延迟
#define PCAP04_REG5_CY_PRE_MR1_SHORT_SHIFT  7
#define PCAP04_REG5_CY_PRE_MR1_SHORT_MASK   (0x01 << PCAP04_REG5_CY_PRE_MR1_SHORT_SHIFT)
#define PCAP04_REG5_CY_PRE_MR1_SHORT_bit(val) (((val) & 0x01) << PCAP04_REG5_CY_PRE_MR1_SHORT_SHIFT)
#define PCAP04_REG5_CY_PRE_MR1_SHORT_NORMAL  0  // 正常 (推荐)
#define PCAP04_REG5_CY_PRE_MR1_SHORT_REDUCE  1  // 减少

// 位[5] C_PORT_PAT: 端口测量顺序交替（如果激活C_PORT_PAT，则C_AVRG + C_FAKE应为偶数）
#define PCAP04_REG5_C_PORT_PAT_SHIFT        5
#define PCAP04_REG5_C_PORT_PAT_MASK         (0x01 << PCAP04_REG5_C_PORT_PAT_SHIFT)
#define PCAP04_REG5_C_PORT_PAT_bit(val)     (((val) & 0x01) << PCAP04_REG5_C_PORT_PAT_SHIFT)
#define PCAP04_REG5_C_PORT_PAT_NORMAL        0  // 正常
#define PCAP04_REG5_C_PORT_PAT_ALTERNATE     1  // 端口顺序交替（每次序列后反转测量端口的顺序）

// 位[3] CY_HFCLK_SEL: CDC时钟源选择
#define PCAP04_REG5_CY_HFCLK_SEL_SHIFT      3
#define PCAP04_REG5_CY_HFCLK_SEL_MASK       (0x01 << PCAP04_REG5_CY_HFCLK_SEL_SHIFT)
#define PCAP04_REG5_CY_HFCLK_SEL_bit(val)   (((val) & 0x01) << PCAP04_REG5_CY_HFCLK_SEL_SHIFT)
#define PCAP04_REG5_CY_HFCLK_SEL_OLF        0  // OLF (低频时钟) (默认)
#define PCAP04_REG5_CY_HFCLK_SEL_OHF        1  // OHF (高频时钟)

// 位[2] CY_DIV4_DIS: 四倍时钟周期（仅在与CY_HFCLK_SEL == 1组合时）
#define PCAP04_REG5_CY_DIV4_DIS_SHIFT       2
#define PCAP04_REG5_CY_DIV4_DIS_MASK        (0x01 << PCAP04_REG5_CY_DIV4_DIS_SHIFT)
#define PCAP04_REG5_CY_DIV4_DIS_bit(val)    (((val) & 0x01) << PCAP04_REG5_CY_DIV4_DIS_SHIFT)
#define PCAP04_REG5_CY_DIV4_DIS_DISABLE      0  // 关闭
#define PCAP04_REG5_CY_DIV4_DIS_ENABLE       1  // 开启

// 位[1] CY_PRE_LONG: 在内部时钟路径之间添加安全延迟
#define PCAP04_REG5_CY_PRE_LONG_SHIFT       1
#define PCAP04_REG5_CY_PRE_LONG_MASK        (0x01 << PCAP04_REG5_CY_PRE_LONG_SHIFT)
#define PCAP04_REG5_CY_PRE_LONG_bit(val)    (((val) & 0x01) << PCAP04_REG5_CY_PRE_LONG_SHIFT)
#define PCAP04_REG5_CY_PRE_LONG_DISABLE      0  // 关闭 (推荐)
#define PCAP04_REG5_CY_PRE_LONG_ENABLE       1  // 开启

// 位[0] C_DC_BALANCE: 仅用于差分浮动模式
#define PCAP04_REG5_C_DC_BALANCE_SHIFT      0
#define PCAP04_REG5_C_DC_BALANCE_MASK       (0x01 << PCAP04_REG5_C_DC_BALANCE_SHIFT)
#define PCAP04_REG5_C_DC_BALANCE_bit(val)   (((val) & 0x01) << PCAP04_REG5_C_DC_BALANCE_SHIFT)
#define PCAP04_REG5_C_DC_BALANCE_DISABLE     0  // 关闭 ("single HiZ")
#define PCAP04_REG5_C_DC_BALANCE_ENABLE      1  // 直流自由 ("both HiZ")

// 寄存器5完整值构建宏
#define PCAP04_REG5_VALUE(cy_pre_mr1_short, c_port_pat, cy_hfclk_sel, cy_div4_dis, cy_pre_long, c_dc_balance) \
    (PCAP04_REG5_CY_PRE_MR1_SHORT_bit(cy_pre_mr1_short) | PCAP04_REG5_C_PORT_PAT_bit(c_port_pat) | \
     PCAP04_REG5_CY_HFCLK_SEL_bit(cy_hfclk_sel) | PCAP04_REG5_CY_DIV4_DIS_bit(cy_div4_dis) | \
     PCAP04_REG5_CY_PRE_LONG_bit(cy_pre_long) | PCAP04_REG5_C_DC_BALANCE_bit(c_dc_balance))

// ----------------------------------------------------------------------------
// 寄存器 6 (0x06) - 测量端口使能配置
// ----------------------------------------------------------------------------
// 位[5:0] C_PORT_EN: 位使能CDC端口（PC0到PC5）（寄存器6为电容采集通道开关配置使用，PCAP04最大支持六路电容采集，如果需要开启相应的通道，只需要将对应位置1即可，例如0x0F则表示通道0~3开启）
#define PCAP04_REG6_C_PORT_EN_SHIFT         0
#define PCAP04_REG6_C_PORT_EN_MASK          (0x3F << PCAP04_REG6_C_PORT_EN_SHIFT)
#define PCAP04_REG6_C_PORT_EN_bit(val)      (((val) & 0x3F) << PCAP04_REG6_C_PORT_EN_SHIFT)
// 单独端口使能位
#define PCAP04_REG6_C_PORT_EN_PC0           (1 << 0) // 启用PC0
#define PCAP04_REG6_C_PORT_EN_PC1           (1 << 1) // 启用PC1
#define PCAP04_REG6_C_PORT_EN_PC2           (1 << 2) // 启用PC2
#define PCAP04_REG6_C_PORT_EN_PC3           (1 << 3) // 启用PC3
#define PCAP04_REG6_C_PORT_EN_PC4           (1 << 4) // 启用PC4
#define PCAP04_REG6_C_PORT_EN_PC5           (1 << 5) // 启用PC5
#define PCAP04_REG6_C_PORT_EN_ALL            (PCAP04_REG6_C_PORT_EN_PC0 | PCAP04_REG6_C_PORT_EN_PC1 | \
                                             PCAP04_REG6_C_PORT_EN_PC2 | PCAP04_REG6_C_PORT_EN_PC3 | \
                                             PCAP04_REG6_C_PORT_EN_PC4 | PCAP04_REG6_C_PORT_EN_PC5) // 0x3F (全部启用)

// 寄存器6完整值构建宏
#define PCAP04_REG6_VALUE(port_en)          PCAP04_REG6_C_PORT_EN_bit(port_en)

// ----------------------------------------------------------------------------
// 寄存器 7-8 (0x07, 0x08) - 采样平均配置 (C_AVRG, 13位)
// ----------------------------------------------------------------------------
// C_AVRG: 设置CDC测量的平均采样数（范围: 0-8191，寄存器7-8用于配置计算平均值时的采样次数，最大可以到8191次，也就是电容值是采样8191次之后的平均值，这个值越大，采样值越平稳，但对应的采样时间也会越长，这个根据实际需要配置）
#define PCAP04_C_AVRG_MAX           8191
// 寄存器7: C_AVRG的低8位
#define PCAP04_REG7_C_AVRG_LOW_bit(val)     ((uint8_t)((val) & 0xFF))
// 寄存器8: C_AVRG的高5位（位[4:0]）
#define PCAP04_REG8_C_AVRG_HIGH_bit(val)    ((uint8_t)(((val) >> 8) & 0x1F))

// ----------------------------------------------------------------------------
// 寄存器 9-11 (0x09, 0x0A, 0x0B) - 转换时间配置 (CONV_TIME, 23位)
// ----------------------------------------------------------------------------
// CONV_TIME: 转换触发周期或序列周期（范围: 0-0x7FFFFF，公式: Tconv/seq = 2 × CONV_TIME / fOLF，例如配置0x0007D0 = 2000，如果OLF = 200 kHz，则 Tconv = 2 × 2000 / 200000 = 20 ms）
#define PCAP04_CONV_TIME_MAX        0x7FFFFF
// 寄存器9: CONV_TIME的低8位
#define PCAP04_REG9_CONV_TIME_LOW_bit(val)  ((uint8_t)((val) & 0xFF))
// 寄存器10: CONV_TIME的中间8位
#define PCAP04_REG10_CONV_TIME_MID_bit(val)  ((uint8_t)(((val) >> 8) & 0xFF))
// 寄存器11: CONV_TIME的高7位（位[6:0]）
#define PCAP04_REG11_CONV_TIME_HIGH_bit(val) ((uint8_t)(((val) >> 16) & 0x7F))

// ----------------------------------------------------------------------------
// 寄存器 12-13 (0x0C, 0x0D) - 放电时间配置 (DISCHARGE_TIME, 10位)
// ----------------------------------------------------------------------------
// DISCHARGE_TIME: 设置CDC放电时间（范围: 0-1023，OLF模式: Tdischarge = (DISCHARGE_TIME + 1) × Tcycleclock，OHF模式: Tdischarge = DISCHARGE_TIME × Tcycleclock，1023表示关闭）
#define PCAP04_DISCHARGE_TIME_MAX           1023
// 寄存器12: DISCHARGE_TIME的低8位（直接输入十进制值，宏自动处理移位）
#define PCAP04_REG12_DISCHARGE_TIME_LOW_bit(val) ((uint8_t)((val) & 0xFF))
// 寄存器13: DISCHARGE_TIME的高2位（位[1:0]），同时包含其他位域
#define PCAP04_REG13_DISCHARGE_TIME_HIGH_bit(val) ((uint8_t)(((val) >> 8) & 0x03))
// 寄存器13中DISCHARGE_TIME的掩码（用于合并其他位域）
#define PCAP04_DISCHARGE_TIME_MASK_REG13    (0x03 << 0)  // 位[1:0]

// 寄存器13的其他位域（与DISCHARGE_TIME共享）
// 位[7:6] C_STARTONPIN: 选择允许触发CDC启动的GPIO端口
#define PCAP04_REG13_C_STARTONPIN_SHIFT     6
#define PCAP04_REG13_C_STARTONPIN_MASK      (0x03 << PCAP04_REG13_C_STARTONPIN_SHIFT)
#define PCAP04_REG13_C_STARTONPIN_bit(val)  (((val) & 0x03) << PCAP04_REG13_C_STARTONPIN_SHIFT)
#define PCAP04_REG13_C_STARTONPIN_PG0       0x00  // PG0
#define PCAP04_REG13_C_STARTONPIN_PG1       0x01  // PG1
#define PCAP04_REG13_C_STARTONPIN_PG2       0x02  // PG2
#define PCAP04_REG13_C_STARTONPIN_PG3       0x03  // PG3

// 位[4:2] C_TRIG_SEL: CDC触发模式选择（PCAP04每次测量包括CDC测量、RDC测量和DSP转换三个过程，一次测量并转换完成后，系统会产生一个下降沿信号INTN，这时表示转换完成并可以读取数据了，那么下一次何时转换取决于C_TRIG_SEL的配置：连续模式=一次转换结束后马上进行下一次转换；读取触发=转换完成后等待用户读取结果寄存器后进行下次转换；时间触发=每到设定转换时间系统就会做一次转换；操作码或外部触发=系统接收到转换指令后才开始转换）
#define PCAP04_REG13_C_TRIG_SEL_SHIFT       2
#define PCAP04_REG13_C_TRIG_SEL_MASK        (0x07 << PCAP04_REG13_C_TRIG_SEL_SHIFT)
#define PCAP04_REG13_C_TRIG_SEL_bit(val)    (((val) & 0x07) << PCAP04_REG13_C_TRIG_SEL_SHIFT)
#define PCAP04_REG13_C_TRIG_SEL_CONTINUOUS   0x00  // 连续模式（不推荐使用）
#define PCAP04_REG13_C_TRIG_SEL_READ_TRIGGER 0x01  // 读取触发
#define PCAP04_REG13_C_TRIG_SEL_TIMER_TRIGGER 0x02  // 定时器触发 (推荐)
#define PCAP04_REG13_C_TRIG_SEL_TIMER_STRETCH 0x03  // 定时器触发 (拉伸模式)
#define PCAP04_REG13_C_TRIG_SEL_UNDEFINED    0x04  // 未定义
#define PCAP04_REG13_C_TRIG_SEL_PIN_TRIGGER  0x05  // 引脚触发 (硬件触发)
#define PCAP04_REG13_C_TRIG_SEL_OPCODE_TRIGGER 0x06  // 操作码触发
#define PCAP04_REG13_C_TRIG_SEL_CONTINUOUS_EXT 0x07  // continuous_exp (不推荐)

// 寄存器13完整值构建宏（包含DISCHARGE_TIME高2位、C_STARTONPIN和C_TRIG_SEL）
// 注意：discharge_time_high可以是完整的DISCHARGE_TIME值（0-1023），宏会自动提取高2位
#define PCAP04_REG13_VALUE(discharge_time_high, startonpin, trig_sel) \
    (PCAP04_REG13_DISCHARGE_TIME_HIGH_bit(discharge_time_high) | \
     PCAP04_REG13_C_STARTONPIN_bit(startonpin) | PCAP04_REG13_C_TRIG_SEL_bit(trig_sel))

// ----------------------------------------------------------------------------
// 寄存器 14-15 (0x0E, 0x0F) - 预充电时间配置 (PRECHARGE_TIME, 10位)
// ----------------------------------------------------------------------------
// PRECHARGE_TIME: 设置CDC预充电时间（范围: 0-1023，寄存器14配置电容的预充电时间，其实我们可以选择是否需要预充电，按官方手册的说法，预充就是先快速将电容充电到某一个电压水平，然后再改变充电速度，缓慢充电到峰值水平，这样可以减小机械应力，同时还可以用来检测电容是否存在短路，1023表示关闭）
#define PCAP04_PRECHARGE_TIME_MAX           1023
// 寄存器14: PRECHARGE_TIME的低8位（直接输入十进制值，宏自动处理移位）
#define PCAP04_REG14_PRECHARGE_TIME_LOW_bit(val) ((uint8_t)((val) & 0xFF))
// 寄存器15: PRECHARGE_TIME的高2位（位[1:0]），同时包含C_FAKE位域
#define PCAP04_REG15_PRECHARGE_TIME_HIGH_bit(val) ((uint8_t)(((val) >> 8) & 0x03))
// 寄存器15中PRECHARGE_TIME的掩码（用于合并其他位域）
#define PCAP04_PRECHARGE_TIME_MASK_REG15    (0x03 << 0)  // 位[1:0]

// 寄存器15的其他位域（与PRECHARGE_TIME共享）
// 位[5:2] C_FAKE: CDC的"假"或"预热"测量数量（寄存器15用于配置伪测量次数，也就是再正式测量之前先做几次伪测量，但这几次的测量值并不会被用于真实测量值，这个根据需要配置，一般配置2-3个就可以了，不配置影响也不大，可以将寄存器7-8中的采样次数稍微增大一些，也可以减小数据的波动或测量未稳定时的干扰）
#define PCAP04_REG15_C_FAKE_SHIFT           2
#define PCAP04_REG15_C_FAKE_MASK            (0x0F << PCAP04_REG15_C_FAKE_SHIFT)
#define PCAP04_REG15_C_FAKE_bit(val)        (((val) & 0x0F) << PCAP04_REG15_C_FAKE_SHIFT)
// C_FAKE范围: 0-15（0=无假测量，1=1次假测量，...，15=15次假测量）

// 寄存器15完整值构建宏（包含PRECHARGE_TIME高2位和C_FAKE）
// 注意：precharge_time_high可以是完整的PRECHARGE_TIME值（0-1023），宏会自动提取高2位
#define PCAP04_REG15_VALUE(precharge_time_high, c_fake) \
    (PCAP04_REG15_PRECHARGE_TIME_HIGH_bit(precharge_time_high) | PCAP04_REG15_C_FAKE_bit(c_fake))

// ----------------------------------------------------------------------------
// 寄存器 16-17 (0x10, 0x11) - 满充电时间配置 (FULLCHARGE_TIME, 10位)
// ----------------------------------------------------------------------------
// FULLCHARGE_TIME: 设置CDC满充电时间（范围: 0-1023，寄存器16用于配置完全充电时的周期数，也就是充满电后持续的时间，这个根据需要设置即可，OLF模式: Tfullcharge = (FULLCHARGE_TIME + 1) × Tcycleclock，OHF模式: Tfullcharge = (FULLCHARGE_TIME + 2) × Tcycleclock）
#define PCAP04_FULLCHARGE_TIME_MAX          1023
// 寄存器16: FULLCHARGE_TIME的低8位（直接输入十进制值，宏自动处理移位）
#define PCAP04_REG16_FULLCHARGE_TIME_LOW_bit(val) ((uint8_t)((val) & 0xFF))
// 寄存器17: FULLCHARGE_TIME的高2位（位[1:0]），同时包含C_REF_SEL位域
#define PCAP04_REG17_FULLCHARGE_TIME_HIGH_bit(val) ((uint8_t)(((val) >> 8) & 0x03))
// 寄存器17中FULLCHARGE_TIME的掩码（用于合并其他位域）
#define PCAP04_FULLCHARGE_TIME_MASK_REG17    (0x03 << 0)  // 位[1:0]

// 寄存器17的其他位域（与FULLCHARGE_TIME共享）
// 位[6:2] C_REF_SEL: 设置片上参考电容（范围: 0-31，0=最小值，1=约1 pF，...，31=最大值约31 pF，注意: 步长从0.3 pF到1.5 pF不等）
#define PCAP04_REG17_C_REF_SEL_SHIFT        2
#define PCAP04_REG17_C_REF_SEL_MASK         (0x1F << PCAP04_REG17_C_REF_SEL_SHIFT)
#define PCAP04_REG17_C_REF_SEL_bit(val)     (((val) & 0x1F) << PCAP04_REG17_C_REF_SEL_SHIFT)
#define PCAP04_REG17_C_REF_SEL_MIN           0   // 最小值
#define PCAP04_REG17_C_REF_SEL_MAX           31  // 最大值 (约31 pF)

// 寄存器17完整值构建宏（包含FULLCHARGE_TIME高2位和C_REF_SEL）
// 注意：fullcharge_time_high可以是完整的FULLCHARGE_TIME值（0-1023），宏会自动提取高2位
#define PCAP04_REG17_VALUE(fullcharge_time_high, c_ref_sel) \
    (PCAP04_REG17_FULLCHARGE_TIME_HIGH_bit(fullcharge_time_high) | PCAP04_REG17_C_REF_SEL_bit(c_ref_sel))

// ----------------------------------------------------------------------------
// 寄存器 18 (0x12) - 保护驱动配置
// ----------------------------------------------------------------------------
// 位[7] C_G_OP_RUN: 保护运算放大器模式
#define PCAP04_REG18_C_G_OP_RUN_SHIFT       7
#define PCAP04_REG18_C_G_OP_RUN_MASK       (0x01 << PCAP04_REG18_C_G_OP_RUN_SHIFT)
#define PCAP04_REG18_C_G_OP_RUN_bit(val)   (((val) & 0x01) << PCAP04_REG18_C_G_OP_RUN_SHIFT)
#define PCAP04_REG18_C_G_OP_RUN_PERMANENT  0  // 永久 (默认)
#define PCAP04_REG18_C_G_OP_RUN_PULSED     1  // 脉冲（在转换之间将OP设置为睡眠模式）

// 位[6] C_G_OP_EXT: 保护外部运算放大器
#define PCAP04_REG18_C_G_OP_EXT_SHIFT      6
#define PCAP04_REG18_C_G_OP_EXT_MASK      (0x01 << PCAP04_REG18_C_G_OP_EXT_SHIFT)
#define PCAP04_REG18_C_G_OP_EXT_bit(val)  (((val) & 0x01) << PCAP04_REG18_C_G_OP_EXT_SHIFT)
#define PCAP04_REG18_C_G_OP_EXT_INTERNAL   0  // 内部OP (默认)
#define PCAP04_REG18_C_G_OP_EXT_EXTERNAL   1  // 外部OP，PG3作为C_G_MUX_SEL

// 位[5:0] C_G_EN: 保护使能，每位对应一个端口
#define PCAP04_REG18_C_G_EN_SHIFT          0
#define PCAP04_REG18_C_G_EN_MASK           (0x3F << PCAP04_REG18_C_G_EN_SHIFT)
#define PCAP04_REG18_C_G_EN_bit(val)       (((val) & 0x3F) << PCAP04_REG18_C_G_EN_SHIFT)
// C_G_EN端口使能位掩码
#define PCAP04_REG18_C_G_EN_PC0            (1 << 0)  // 激活端口PC0
#define PCAP04_REG18_C_G_EN_PC1            (1 << 1)  // 激活端口PC1
#define PCAP04_REG18_C_G_EN_PC2            (1 << 2)  // 激活端口PC2
#define PCAP04_REG18_C_G_EN_PC3            (1 << 3)  // 激活端口PC3
#define PCAP04_REG18_C_G_EN_PC4            (1 << 4)  // 激活端口PC4
#define PCAP04_REG18_C_G_EN_PC5            (1 << 5)  // 激活端口PC5
#define PCAP04_REG18_C_G_EN_NONE           0x00      // 保护功能未启用 (默认)

// 寄存器18完整值构建宏
#define PCAP04_REG18_VALUE(c_g_op_run, c_g_op_ext, c_g_en) \
    (PCAP04_REG18_C_G_OP_RUN_bit(c_g_op_run) | PCAP04_REG18_C_G_OP_EXT_bit(c_g_op_ext) | \
     PCAP04_REG18_C_G_EN_bit(c_g_en))

// ----------------------------------------------------------------------------
// 寄存器 19 (0x13) - 保护运算放大器配置
// ----------------------------------------------------------------------------
// 位[7:6] C_G_OP_VU: 保护运算放大器增益（从感测端口到保护）
#define PCAP04_REG19_C_G_OP_VU_SHIFT        6
#define PCAP04_REG19_C_G_OP_VU_MASK         (0x03 << PCAP04_REG19_C_G_OP_VU_SHIFT)
#define PCAP04_REG19_C_G_OP_VU_bit(val)     (((val) & 0x03) << PCAP04_REG19_C_G_OP_VU_SHIFT)
#define PCAP04_REG19_C_G_OP_VU_1_00         0  // × 1.00 (默认)
#define PCAP04_REG19_C_G_OP_VU_1_01         1  // × 1.01
#define PCAP04_REG19_C_G_OP_VU_1_02         2  // × 1.02
#define PCAP04_REG19_C_G_OP_VU_1_03         3  // × 1.03

// 位[5:4] C_G_OP_ATTN: 保护运算放大器衰减
#define PCAP04_REG19_C_G_OP_ATTN_SHIFT      4
#define PCAP04_REG19_C_G_OP_ATTN_MASK       (0x03 << PCAP04_REG19_C_G_OP_ATTN_SHIFT)
#define PCAP04_REG19_C_G_OP_ATTN_bit(val)   (((val) & 0x03) << PCAP04_REG19_C_G_OP_ATTN_SHIFT)
#define PCAP04_REG19_C_G_OP_ATTN_0_5_AF     0  // 0.5 aF (默认)
#define PCAP04_REG19_C_G_OP_ATTN_1_0_AF     1  // 1.0 aF
#define PCAP04_REG19_C_G_OP_ATTN_1_5_AF     2  // 1.5 aF
#define PCAP04_REG19_C_G_OP_ATTN_2_0_AF     3  // 2.0 aF

// 位[3:0] C_G_TIME: 保护端口切换时间（在预充电期间将保护端口从"直接连接"切换到OP的时间）
#define PCAP04_REG19_C_G_TIME_SHIFT         0
#define PCAP04_REG19_C_G_TIME_MASK          (0x0F << PCAP04_REG19_C_G_TIME_SHIFT)
#define PCAP04_REG19_C_G_TIME_bit(val)      (((val) & 0x0F) << PCAP04_REG19_C_G_TIME_SHIFT)
// C_G_TIME范围: 0-15（t = C_G_TIME × 500 ns，默认值为0）

// 寄存器19完整值构建宏
#define PCAP04_REG19_VALUE(c_g_op_vu, c_g_op_attn, c_g_time) \
    (PCAP04_REG19_C_G_OP_VU_bit(c_g_op_vu) | PCAP04_REG19_C_G_OP_ATTN_bit(c_g_op_attn) | \
     PCAP04_REG19_C_G_TIME_bit(c_g_time))

// --- RDC配置相关 (寄存器 20-39) ---

// ----------------------------------------------------------------------------
// 寄存器 20 (0x14) - RDC时间配置
// ----------------------------------------------------------------------------
// 位[7] R_CY: RDC测定的循环时间（取决于OLF频率）
// OLF频率 | R_CY=0 | R_CY=1
// 10 kHz  | 100 μs | 200 μs
// 50 kHz  | 20 μs  | 40 μs
// 100 kHz | 10 μs  | 20 μs
// 200 kHz | 20 μs  | 40 μs
#define PCAP04_REG20_R_CY_SHIFT             7
#define PCAP04_REG20_R_CY_MASK              (0x01 << PCAP04_REG20_R_CY_SHIFT)
#define PCAP04_REG20_R_CY_bit(val)          (((val) & 0x01) << PCAP04_REG20_R_CY_SHIFT)
#define PCAP04_REG20_R_CY_SHORT             0  // 较短循环时间 (默认)
#define PCAP04_REG20_R_CY_LONG             1  // 较长循环时间

// 位[2:0] C_G_OP_TR: 保护运算放大器电流调整
#define PCAP04_REG20_C_G_OP_TR_SHIFT        0
#define PCAP04_REG20_C_G_OP_TR_MASK         (0x07 << PCAP04_REG20_C_G_OP_TR_SHIFT)
#define PCAP04_REG20_C_G_OP_TR_bit(val)     (((val) & 0x07) << PCAP04_REG20_C_G_OP_TR_SHIFT)
// C_G_OP_TR范围: 0-7（推荐值，默认值为0）

// 寄存器20完整值构建宏
#define PCAP04_REG20_VALUE(r_cy, c_g_op_tr) \
    (PCAP04_REG20_R_CY_bit(r_cy) | PCAP04_REG20_C_G_OP_TR_bit(c_g_op_tr))

// ----------------------------------------------------------------------------
// 寄存器 21-22 (0x15, 0x16) - RDC预分频配置 (R_TRIG_PREDIV, 10位)
// ----------------------------------------------------------------------------
// R_TRIG_PREDIV: 预分频器（范围: 0-1023，允许温度测量比电容测量慢，这是CDC与RDC测量速率之间的因子，如果OLF用作触发源，也用作OLF时钟分频器，0/1=每次信号触发，2=每2次信号触发，...，1023=最大因子）
#define PCAP04_R_TRIG_PREDIV_MAX            1023
// 寄存器21: R_TRIG_PREDIV的低8位（直接输入十进制值，宏自动处理移位）
#define PCAP04_REG21_R_TRIG_PREDIV_LOW_bit(val) ((uint8_t)((val) & 0xFF))
// 寄存器22: R_TRIG_PREDIV的高2位（位[1:0]），同时包含R_TRIG_SEL和R_AVRG位域
#define PCAP04_REG22_R_TRIG_PREDIV_HIGH_bit(val) ((uint8_t)(((val) >> 8) & 0x03))
// 寄存器22中R_TRIG_PREDIV的掩码（用于合并其他位域）
#define PCAP04_R_TRIG_PREDIV_MASK_REG22      (0x03 << 0)  // 位[1:0]

// 寄存器22的其他位域（与R_TRIG_PREDIV共享）
// 位[6:4] R_TRIG_SEL: RDC触发源选择
#define PCAP04_REG22_R_TRIG_SEL_SHIFT       4
#define PCAP04_REG22_R_TRIG_SEL_MASK        (0x07 << PCAP04_REG22_R_TRIG_SEL_SHIFT)
#define PCAP04_REG22_R_TRIG_SEL_bit(val)    (((val) & 0x07) << PCAP04_REG22_R_TRIG_SEL_SHIFT)
#define PCAP04_REG22_R_TRIG_SEL_DISABLE      0x00  // 关闭
#define PCAP04_REG22_R_TRIG_SEL_TIMER_TRIGGER 0x01  // 定时器触发
#define PCAP04_REG22_R_TRIG_SEL_PIN_TRIGGER  0x03  // 引脚触发
#define PCAP04_REG22_R_TRIG_SEL_CDC_ASYNC    0x05  // CDC异步 (推荐)
#define PCAP04_REG22_R_TRIG_SEL_CDC_SYNC     0x06  // CDC同步

// 位[3:2] R_AVRG: RDC测定的平均值配置
#define PCAP04_REG22_R_AVRG_SHIFT           2
#define PCAP04_REG22_R_AVRG_MASK            (0x03 << PCAP04_REG22_R_AVRG_SHIFT)
#define PCAP04_REG22_R_AVRG_bit(val)        (((val) & 0x03) << PCAP04_REG22_R_AVRG_SHIFT)
#define PCAP04_REG22_R_AVRG_NO_AVG           0x00  // 不平均
#define PCAP04_REG22_R_AVRG_4_AVG           0x01  // 4次平均
#define PCAP04_REG22_R_AVRG_8_AVG           0x02  // 8次平均
#define PCAP04_REG22_R_AVRG_16_AVG          0x03  // 16次平均

// 寄存器22完整值构建宏（包含R_TRIG_PREDIV高2位、R_AVRG和R_TRIG_SEL）
// 注意：r_trig_prediv_high可以是完整的R_TRIG_PREDIV值（0-1023），宏会自动提取高2位
#define PCAP04_REG22_VALUE(r_trig_prediv_high, r_avrg, r_trig_sel) \
    (PCAP04_REG22_R_TRIG_PREDIV_HIGH_bit(r_trig_prediv_high) | \
     PCAP04_REG22_R_AVRG_bit(r_avrg) | PCAP04_REG22_R_TRIG_SEL_bit(r_trig_sel))

// ----------------------------------------------------------------------------
// 寄存器 23 (0x17) - RDC端口与参考配置
// ----------------------------------------------------------------------------
// 位[7:6] R_PORT_EN: RDC部分的端口激活（非位操作，特殊编码）
#define PCAP04_REG23_R_PORT_EN_SHIFT        6
#define PCAP04_REG23_R_PORT_EN_MASK         (0x03 << PCAP04_REG23_R_PORT_EN_SHIFT)
#define PCAP04_REG23_R_PORT_EN_bit(val)     (((val) & 0x03) << PCAP04_REG23_R_PORT_EN_SHIFT)
#define PCAP04_REG23_R_PORT_EN_DISABLE       0x00  // 禁用
#define PCAP04_REG23_R_PORT_EN_PTOREF       0x01  // 激活端口PTOREF
#define PCAP04_REG23_R_PORT_EN_PT1          0x02  // 激活端口PT1
#define PCAP04_REG23_R_PORT_EN_PT1_PTOREF   0x03  // 激活端口PT1和PTOREF

// 位[5] R_PORT_EN_IMES: 内部铝温度传感器的端口激活
#define PCAP04_REG23_R_PORT_EN_IMES_SHIFT   5
#define PCAP04_REG23_R_PORT_EN_IMES_MASK    (0x01 << PCAP04_REG23_R_PORT_EN_IMES_SHIFT)
#define PCAP04_REG23_R_PORT_EN_IMES_bit(val) (((val) & 0x01) << PCAP04_REG23_R_PORT_EN_IMES_SHIFT)
#define PCAP04_REG23_R_PORT_EN_IMES_DISABLE  0  // 禁用
#define PCAP04_REG23_R_PORT_EN_IMES_ACTIVE  1  // 激活

// 位[4] R_PORT_EN_IREF: 内部参考电阻的端口激活
#define PCAP04_REG23_R_PORT_EN_IREF_SHIFT   4
#define PCAP04_REG23_R_PORT_EN_IREF_MASK    (0x01 << PCAP04_REG23_R_PORT_EN_IREF_SHIFT)
#define PCAP04_REG23_R_PORT_EN_IREF_bit(val) (((val) & 0x01) << PCAP04_REG23_R_PORT_EN_IREF_SHIFT)
#define PCAP04_REG23_R_PORT_EN_IREF_DISABLE  0  // 禁用
#define PCAP04_REG23_R_PORT_EN_IREF_ACTIVE   1  // 激活

// 位[2] R_FAKE: RDC的"假"或"预热"测量数量
#define PCAP04_REG23_R_FAKE_SHIFT           2
#define PCAP04_REG23_R_FAKE_MASK            (0x01 << PCAP04_REG23_R_FAKE_SHIFT)
#define PCAP04_REG23_R_FAKE_bit(val)        (((val) & 0x01) << PCAP04_REG23_R_FAKE_SHIFT)
#define PCAP04_REG23_R_FAKE_2_CYCLES         0  // 每次平均有2次假测量周期
#define PCAP04_REG23_R_FAKE_8_CYCLES         1  // 每次平均有8次假测量周期

// 位[1:0] R_STARTONPIN: 选择允许触发RDC启动的GPIO端口
#define PCAP04_REG23_R_STARTONPIN_SHIFT     0
#define PCAP04_REG23_R_STARTONPIN_MASK      (0x03 << PCAP04_REG23_R_STARTONPIN_SHIFT)
#define PCAP04_REG23_R_STARTONPIN_bit(val)  (((val) & 0x03) << PCAP04_REG23_R_STARTONPIN_SHIFT)
#define PCAP04_REG23_R_STARTONPIN_PG0       0x00  // PG0
#define PCAP04_REG23_R_STARTONPIN_PG1       0x01  // PG1
#define PCAP04_REG23_R_STARTONPIN_PG2       0x02  // PG2
#define PCAP04_REG23_R_STARTONPIN_PG3       0x03  // PG3

// 寄存器23完整值构建宏
#define PCAP04_REG23_VALUE(r_port_en, r_port_en_imes, r_port_en_iref, r_fake, r_startonpin) \
    (PCAP04_REG23_R_PORT_EN_bit(r_port_en) | PCAP04_REG23_R_PORT_EN_IMES_bit(r_port_en_imes) | \
     PCAP04_REG23_R_PORT_EN_IREF_bit(r_port_en_iref) | PCAP04_REG23_R_FAKE_bit(r_fake) | \
     PCAP04_REG23_R_STARTONPIN_bit(r_startonpin))

// ----------------------------------------------------------------------------
// 寄存器 24 (0x18) - TDC配置 (ams内部固定配置 - 禁止修改)
// ----------------------------------------------------------------------------
// 位[5:4] TDC_CHAN_EN: 必须为3
#define PCAP04_REG24_TDC_CHAN_EN_SHIFT      4
#define PCAP04_REG24_TDC_CHAN_EN_MASK       (0x03 << PCAP04_REG24_TDC_CHAN_EN_SHIFT)
#define PCAP04_REG24_TDC_CHAN_EN_bit(val)   (((val) & 0x03) << PCAP04_REG24_TDC_CHAN_EN_SHIFT)
#define PCAP04_REG24_TDC_CHAN_EN_MUST_BE_3   3   // 必须为3

// 位[3] TDC_ALUPERMOPEN: 必须为0
#define PCAP04_REG24_TDC_ALUPERMOPEN_SHIFT  3
#define PCAP04_REG24_TDC_ALUPERMOPEN_MASK   (0x01 << PCAP04_REG24_TDC_ALUPERMOPEN_SHIFT)
#define PCAP04_REG24_TDC_ALUPERMOPEN_bit(val) (((val) & 0x01) << PCAP04_REG24_TDC_ALUPERMOPEN_SHIFT)
#define PCAP04_REG24_TDC_ALUPERMOPEN_MUST_BE_0  0   // 必须为0

// 位[2] TDC_NOISE_DIS: 必须为0
#define PCAP04_REG24_TDC_NOISE_DIS_SHIFT    2
#define PCAP04_REG24_TDC_NOISE_DIS_MASK     (0x01 << PCAP04_REG24_TDC_NOISE_DIS_SHIFT)
#define PCAP04_REG24_TDC_NOISE_DIS_bit(val) (((val) & 0x01) << PCAP04_REG24_TDC_NOISE_DIS_SHIFT)
#define PCAP04_REG24_TDC_NOISE_DIS_MUST_BE_0  0   // 必须为0

// 位[1:0] TDC_MUPU_SPEED: 必须为3
#define PCAP04_REG24_TDC_MUPU_SPEED_SHIFT   0
#define PCAP04_REG24_TDC_MUPU_SPEED_MASK    (0x03 << PCAP04_REG24_TDC_MUPU_SPEED_SHIFT)
#define PCAP04_REG24_TDC_MUPU_SPEED_bit(val) (((val) & 0x03) << PCAP04_REG24_TDC_MUPU_SPEED_SHIFT)
#define PCAP04_REG24_TDC_MUPU_SPEED_MUST_BE_3  3   // 必须为3

// 寄存器24完整值构建宏（推荐默认值: 0x73）
#define PCAP04_REG24_VALUE(tdc_chan_en, tdc_alupermopen, tdc_noise_dis, tdc_mupu_speed) \
    (PCAP04_REG24_TDC_CHAN_EN_bit(tdc_chan_en) | PCAP04_REG24_TDC_ALUPERMOPEN_bit(tdc_alupermopen) | \
     PCAP04_REG24_TDC_NOISE_DIS_bit(tdc_noise_dis) | PCAP04_REG24_TDC_MUPU_SPEED_bit(tdc_mupu_speed))
#define PCAP04_TDC_REG24_DEFAULT    PCAP04_REG24_VALUE(3, 0, 0, 3)  // 0x73

// ----------------------------------------------------------------------------
// 寄存器 25 (0x19) - TDC配置 (ams内部固定配置 - 禁止修改)
// ----------------------------------------------------------------------------
// 位[7:2] TDC_MUPU_NO: 必须为1
#define PCAP04_REG25_TDC_MUPU_NO_SHIFT      2
#define PCAP04_REG25_TDC_MUPU_NO_MASK      (0x3F << PCAP04_REG25_TDC_MUPU_NO_SHIFT)
#define PCAP04_REG25_TDC_MUPU_NO_bit(val)  (((val) & 0x3F) << PCAP04_REG25_TDC_MUPU_NO_SHIFT)
#define PCAP04_REG25_TDC_MUPU_NO_MUST_BE_1   1   // 必须为1

// 寄存器25完整值构建宏（推荐默认值: 0x04）
#define PCAP04_REG25_VALUE(tdc_mupu_no) \
    (PCAP04_REG25_TDC_MUPU_NO_bit(tdc_mupu_no))
#define PCAP04_TDC_REG25_DEFAULT    PCAP04_REG25_VALUE(1)  // 0x04

// ----------------------------------------------------------------------------
// 寄存器 26 (0x1A) - TDC配置 (ams内部固定配置 - 禁止修改)
// ----------------------------------------------------------------------------
// 位[7:2] TDC_QHA_SEL: 必须为20
#define PCAP04_REG26_TDC_QHA_SEL_SHIFT      2
#define PCAP04_REG26_TDC_QHA_SEL_MASK       (0x3F << PCAP04_REG26_TDC_QHA_SEL_SHIFT)
#define PCAP04_REG26_TDC_QHA_SEL_bit(val)   (((val) & 0x3F) << PCAP04_REG26_TDC_QHA_SEL_SHIFT)
#define PCAP04_REG26_TDC_QHA_SEL_MUST_BE_20  20  // 必须为20

// 位[1] TDC_NOISE_CY_DIS: 必须为1
#define PCAP04_REG26_TDC_NOISE_CY_DIS_SHIFT 1
#define PCAP04_REG26_TDC_NOISE_CY_DIS_MASK  (0x01 << PCAP04_REG26_TDC_NOISE_CY_DIS_SHIFT)
#define PCAP04_REG26_TDC_NOISE_CY_DIS_bit(val) (((val) & 0x01) << PCAP04_REG26_TDC_NOISE_CY_DIS_SHIFT)
#define PCAP04_REG26_TDC_NOISE_CY_DIS_MUST_BE_1  1   // 必须为1

// 寄存器26完整值构建宏（推荐默认值: 0x50）
#define PCAP04_REG26_VALUE(tdc_qha_sel, tdc_noise_cy_dis) \
    (PCAP04_REG26_TDC_QHA_SEL_bit(tdc_qha_sel) | PCAP04_REG26_TDC_NOISE_CY_DIS_bit(tdc_noise_cy_dis))
#define PCAP04_TDC_REG26_DEFAULT    PCAP04_REG26_VALUE(20, 1)  // 0x50

// ----------------------------------------------------------------------------
// 寄存器 27 (0x1B) - DSP配置
// ----------------------------------------------------------------------------
// 位[7:6] DSP_MOFLO_EN: 在GPIO脉冲线中启用单稳态（防抖滤波器）
#define PCAP04_REG27_DSP_MOFLO_EN_SHIFT     6
#define PCAP04_REG27_DSP_MOFLO_EN_MASK      (0x03 << PCAP04_REG27_DSP_MOFLO_EN_SHIFT)
#define PCAP04_REG27_DSP_MOFLO_EN_bit(val)  (((val) & 0x03) << PCAP04_REG27_DSP_MOFLO_EN_SHIFT)
#define PCAP04_REG27_DSP_MOFLO_EN_DISABLE    0x00  // 禁用
#define PCAP04_REG27_DSP_MOFLO_EN_ENABLE     0x03  // 启用（推荐值）

// 位[3:2] DSP_SPEED: DSP速度
#define PCAP04_REG27_DSP_SPEED_SHIFT        2
#define PCAP04_REG27_DSP_SPEED_MASK         (0x03 << PCAP04_REG27_DSP_SPEED_SHIFT)
#define PCAP04_REG27_DSP_SPEED_bit(val)     (((val) & 0x03) << PCAP04_REG27_DSP_SPEED_SHIFT)
#define PCAP04_REG27_DSP_SPEED_FASTEST       0x00  // 最快
#define PCAP04_REG27_DSP_SPEED_FAST          0x01  // 快
#define PCAP04_REG27_DSP_SPEED_SLOW          0x02  // 慢 (推荐)
#define PCAP04_REG27_DSP_SPEED_SLOWEST        0x03  // 最慢

// 位[1] PG1xPG3: 切换PG1/PG3到/从DSP的接线
#define PCAP04_REG27_PG1xPG3_SHIFT          1
#define PCAP04_REG27_PG1xPG3_MASK           (0x01 << PCAP04_REG27_PG1xPG3_SHIFT)
#define PCAP04_REG27_PG1xPG3_bit(val)       (((val) & 0x01) << PCAP04_REG27_PG1xPG3_SHIFT)
#define PCAP04_REG27_PG1xPG3_NORMAL          0  // 正常
#define PCAP04_REG27_PG1xPG3_SWAP            1  // 切换

// 位[0] PG0xPG2: 切换PG0/PG2到/从DSP的接线
#define PCAP04_REG27_PG0xPG2_SHIFT          0
#define PCAP04_REG27_PG0xPG2_MASK           (0x01 << PCAP04_REG27_PG0xPG2_SHIFT)
#define PCAP04_REG27_PG0xPG2_bit(val)       (((val) & 0x01) << PCAP04_REG27_PG0xPG2_SHIFT)
#define PCAP04_REG27_PG0xPG2_NORMAL          0  // 正常
#define PCAP04_REG27_PG0xPG2_SWAP            1  // 切换

// 寄存器27完整值构建宏
#define PCAP04_REG27_VALUE(dsp_moflo_en, dsp_speed, pg1xpg3, pg0xpg2) \
    (PCAP04_REG27_DSP_MOFLO_EN_bit(dsp_moflo_en) | PCAP04_REG27_DSP_SPEED_bit(dsp_speed) | \
     PCAP04_REG27_PG1xPG3_bit(pg1xpg3) | PCAP04_REG27_PG0xPG2_bit(pg0xpg2))

// ----------------------------------------------------------------------------
// 寄存器 28 (0x1C) - 看门狗配置
// ----------------------------------------------------------------------------
// 位[7:0] WD_DIS: 看门狗禁用（写入0x5A禁用看门狗，其他值启用看门狗）
#define PCAP04_REG28_WD_DIS_SHIFT           0
#define PCAP04_REG28_WD_DIS_MASK            (0xFF << PCAP04_REG28_WD_DIS_SHIFT)
#define PCAP04_REG28_WD_DIS_bit(val)       ((uint8_t)((val) & 0xFF))
#define PCAP04_REG28_WD_DIS_ENABLE          0x00  // 看门狗启用 (推荐，除0x5A外的任何值)
#define PCAP04_REG28_WD_DIS_DISABLE         0x5A  // 看门狗禁用

// ----------------------------------------------------------------------------
// 寄存器 29 (0x1D) - DSP触发配置
// ----------------------------------------------------------------------------
// 位[7:4] DSP_STARTONPIN: 启动DSP的引脚掩码（位方式PG0到PG3，每位对应一个GPIO）
#define PCAP04_REG29_DSP_STARTONPIN_SHIFT   4
#define PCAP04_REG29_DSP_STARTONPIN_MASK    (0x0F << PCAP04_REG29_DSP_STARTONPIN_SHIFT)
#define PCAP04_REG29_DSP_STARTONPIN_bit(val) (((val) & 0x0F) << PCAP04_REG29_DSP_STARTONPIN_SHIFT)
#define PCAP04_REG29_DSP_STARTONPIN_PG0     (1 << 0)  // PG0
#define PCAP04_REG29_DSP_STARTONPIN_PG1     (1 << 1)  // PG1
#define PCAP04_REG29_DSP_STARTONPIN_PG2     (1 << 2)  // PG2
#define PCAP04_REG29_DSP_STARTONPIN_PG3     (1 << 3)  // PG3

// 位[3:0] DSP_FF_IN: 触发器激活的引脚掩码（位方式DSP_IN_0到DSP_IN_3，每位对应一个DSP输入）
#define PCAP04_REG29_DSP_FF_IN_SHIFT        0
#define PCAP04_REG29_DSP_FF_IN_MASK         (0x0F << PCAP04_REG29_DSP_FF_IN_SHIFT)
#define PCAP04_REG29_DSP_FF_IN_bit(val)     (((val) & 0x0F) << PCAP04_REG29_DSP_FF_IN_SHIFT)
#define PCAP04_REG29_DSP_FF_IN_DSP_IN_0     (1 << 0)  // DSP_IN_0
#define PCAP04_REG29_DSP_FF_IN_DSP_IN_1     (1 << 1)  // DSP_IN_1
#define PCAP04_REG29_DSP_FF_IN_DSP_IN_2     (1 << 2)  // DSP_IN_2
#define PCAP04_REG29_DSP_FF_IN_DSP_IN_3     (1 << 3)  // DSP_IN_3

// 寄存器29完整值构建宏
#define PCAP04_REG29_VALUE(dsp_startonpin, dsp_ff_in) \
    (PCAP04_REG29_DSP_STARTONPIN_bit(dsp_startonpin) | PCAP04_REG29_DSP_FF_IN_bit(dsp_ff_in))

// ----------------------------------------------------------------------------
// 寄存器 30 (0x1E) - 中断配置
// ----------------------------------------------------------------------------
// 位[7] PG5_INTN_EN: 将INTN信号路由到PG5
#define PCAP04_REG30_PG5_INTN_EN_SHIFT      7
#define PCAP04_REG30_PG5_INTN_EN_MASK       (0x01 << PCAP04_REG30_PG5_INTN_EN_SHIFT)
#define PCAP04_REG30_PG5_INTN_EN_bit(val)   (((val) & 0x01) << PCAP04_REG30_PG5_INTN_EN_SHIFT)
#define PCAP04_REG30_PG5_INTN_EN_NORMAL      0  // PG5正常工作
#define PCAP04_REG30_PG5_INTN_EN_ROUTE       1  // PG5 <== INTN

// 位[6] PG4_INTN_EN: 将INTN信号路由到PG4
#define PCAP04_REG30_PG4_INTN_EN_SHIFT      6
#define PCAP04_REG30_PG4_INTN_EN_MASK       (0x01 << PCAP04_REG30_PG4_INTN_EN_SHIFT)
#define PCAP04_REG30_PG4_INTN_EN_bit(val)   (((val) & 0x01) << PCAP04_REG30_PG4_INTN_EN_SHIFT)
#define PCAP04_REG30_PG4_INTN_EN_NORMAL      0  // PG4正常工作
#define PCAP04_REG30_PG4_INTN_EN_ROUTE       1  // PG4 <== INTN

// 位[2:0] DSP_START_EN: DSP触发器启用（'bxxx1=由CDC结束触发，'bxx1x=由RDC结束触发（推荐），'bx1xx=由定时器触发）
#define PCAP04_REG30_DSP_START_EN_SHIFT     0
#define PCAP04_REG30_DSP_START_EN_MASK      (0x07 << PCAP04_REG30_DSP_START_EN_SHIFT)
#define PCAP04_REG30_DSP_START_EN_bit(val)  (((val) & 0x07) << PCAP04_REG30_DSP_START_EN_SHIFT)
#define PCAP04_REG30_DSP_START_EN_CDC_END    (1 << 0)  // 由CDC结束触发
#define PCAP04_REG30_DSP_START_EN_RDC_END    (1 << 1)  // 由RDC结束触发 (推荐)
#define PCAP04_REG30_DSP_START_EN_TIMER      (1 << 2)  // 由定时器触发

// 寄存器30完整值构建宏
#define PCAP04_REG30_VALUE(pg5_intn_en, pg4_intn_en, dsp_start_en) \
    (PCAP04_REG30_PG5_INTN_EN_bit(pg5_intn_en) | PCAP04_REG30_PG4_INTN_EN_bit(pg4_intn_en) | \
     PCAP04_REG30_DSP_START_EN_bit(dsp_start_en))

// ----------------------------------------------------------------------------
// 寄存器 31 (0x1F) - 脉冲接口0配置
// ----------------------------------------------------------------------------
// 位[7] PI1_TOGGLE_EN: 在脉冲接口1输出激活切换触发器
#define PCAP04_REG31_PI1_TOGGLE_EN_SHIFT    7
#define PCAP04_REG31_PI1_TOGGLE_EN_MASK     (0x01 << PCAP04_REG31_PI1_TOGGLE_EN_SHIFT)
#define PCAP04_REG31_PI1_TOGGLE_EN_bit(val) (((val) & 0x01) << PCAP04_REG31_PI1_TOGGLE_EN_SHIFT)
#define PCAP04_REG31_PI1_TOGGLE_EN_NORMAL    0  // 正常工作
#define PCAP04_REG31_PI1_TOGGLE_EN_TOGGLE    1  // 切换触发器激活（特别适用于PDM以创建1:1占空比）

// 位[6] PIO_TOGGLE_EN: 在脉冲接口0输出激活切换触发器
#define PCAP04_REG31_PIO_TOGGLE_EN_SHIFT    6
#define PCAP04_REG31_PIO_TOGGLE_EN_MASK     (0x01 << PCAP04_REG31_PIO_TOGGLE_EN_SHIFT)
#define PCAP04_REG31_PIO_TOGGLE_EN_bit(val) (((val) & 0x01) << PCAP04_REG31_PIO_TOGGLE_EN_SHIFT)
#define PCAP04_REG31_PIO_TOGGLE_EN_NORMAL    0  // 正常工作
#define PCAP04_REG31_PIO_TOGGLE_EN_TOGGLE    1  // 切换触发器激活（特别适用于PDM以创建1:1占空比）

// 位[5:4] PIO_RES: 脉冲编码接口0的分辨率
#define PCAP04_REG31_PIO_RES_SHIFT          4
#define PCAP04_REG31_PIO_RES_MASK           (0x03 << PCAP04_REG31_PIO_RES_SHIFT)
#define PCAP04_REG31_PIO_RES_bit(val)       (((val) & 0x03) << PCAP04_REG31_PIO_RES_SHIFT)
#define PCAP04_REG31_PIO_RES_10BIT          0x00  // 10位
#define PCAP04_REG31_PIO_RES_12BIT          0x01  // 12位
#define PCAP04_REG31_PIO_RES_14BIT          0x02  // 14位
#define PCAP04_REG31_PIO_RES_16BIT          0x03  // 16位

// 位[3] PIO_PDM_SEL: 脉冲接口0 PWM / PDM开关
#define PCAP04_REG31_PIO_PDM_SEL_SHIFT      3
#define PCAP04_REG31_PIO_PDM_SEL_MASK       (0x01 << PCAP04_REG31_PIO_PDM_SEL_SHIFT)
#define PCAP04_REG31_PIO_PDM_SEL_bit(val)   (((val) & 0x01) << PCAP04_REG31_PIO_PDM_SEL_SHIFT)
#define PCAP04_REG31_PIO_PDM_SEL_PWM        0  // PWM
#define PCAP04_REG31_PIO_PDM_SEL_PDM        1  // PDM

// 位[2:0] PIO_CLK_SEL: 脉冲接口0时钟选择
#define PCAP04_REG31_PIO_CLK_SEL_SHIFT      0
#define PCAP04_REG31_PIO_CLK_SEL_MASK       (0x07 << PCAP04_REG31_PIO_CLK_SEL_SHIFT)
#define PCAP04_REG31_PIO_CLK_SEL_bit(val)   (((val) & 0x07) << PCAP04_REG31_PIO_CLK_SEL_SHIFT)
#define PCAP04_REG31_PIO_CLK_SEL_OFF         0x00  // 关闭
#define PCAP04_REG31_PIO_CLK_SEL_OLF_DIV1   0x01  // OLF / 1
#define PCAP04_REG31_PIO_CLK_SEL_OLF_DIV2   0x02  // OLF / 2
#define PCAP04_REG31_PIO_CLK_SEL_OLF_DIV4   0x03  // OLF / 4
#define PCAP04_REG31_PIO_CLK_SEL_OX_DIV1    0x04  // OX / 1
#define PCAP04_REG31_PIO_CLK_SEL_OX_DIV2    0x05  // OX / 2
#define PCAP04_REG31_PIO_CLK_SEL_OX_DIV4    0x06  // OX / 4
#define PCAP04_REG31_PIO_CLK_SEL_UNDEFINED   0x07  // 未定义

// 寄存器31完整值构建宏
#define PCAP04_REG31_VALUE(pi1_toggle_en, pio_toggle_en, pio_res, pio_pdm_sel, pio_clk_sel) \
    (PCAP04_REG31_PI1_TOGGLE_EN_bit(pi1_toggle_en) | PCAP04_REG31_PIO_TOGGLE_EN_bit(pio_toggle_en) | \
     PCAP04_REG31_PIO_RES_bit(pio_res) | PCAP04_REG31_PIO_PDM_SEL_bit(pio_pdm_sel) | \
     PCAP04_REG31_PIO_CLK_SEL_bit(pio_clk_sel))

// ----------------------------------------------------------------------------
// 寄存器 32 (0x20) - 脉冲接口1配置
// ----------------------------------------------------------------------------
// 位[5:4] PI1_RES: 脉冲接口1的分辨率
#define PCAP04_REG32_PI1_RES_SHIFT          4
#define PCAP04_REG32_PI1_RES_MASK           (0x03 << PCAP04_REG32_PI1_RES_SHIFT)
#define PCAP04_REG32_PI1_RES_bit(val)       (((val) & 0x03) << PCAP04_REG32_PI1_RES_SHIFT)
#define PCAP04_REG32_PI1_RES_10BIT          0x00  // 10位
#define PCAP04_REG32_PI1_RES_12BIT          0x01  // 12位
#define PCAP04_REG32_PI1_RES_14BIT          0x02  // 14位
#define PCAP04_REG32_PI1_RES_16BIT          0x03  // 16位

// 位[3] PI1_PDM_SEL: 脉冲接口1 PWM / PDM开关
#define PCAP04_REG32_PI1_PDM_SEL_SHIFT      3
#define PCAP04_REG32_PI1_PDM_SEL_MASK       (0x01 << PCAP04_REG32_PI1_PDM_SEL_SHIFT)
#define PCAP04_REG32_PI1_PDM_SEL_bit(val)   (((val) & 0x01) << PCAP04_REG32_PI1_PDM_SEL_SHIFT)
#define PCAP04_REG32_PI1_PDM_SEL_PWM        0  // PWM
#define PCAP04_REG32_PI1_PDM_SEL_PDM        1  // PDM

// 位[2:0] PI1_CLK_SEL: 脉冲接口1时钟选择
#define PCAP04_REG32_PI1_CLK_SEL_SHIFT      0
#define PCAP04_REG32_PI1_CLK_SEL_MASK       (0x07 << PCAP04_REG32_PI1_CLK_SEL_SHIFT)
#define PCAP04_REG32_PI1_CLK_SEL_bit(val)   (((val) & 0x07) << PCAP04_REG32_PI1_CLK_SEL_SHIFT)
#define PCAP04_REG32_PI1_CLK_SEL_OFF         0x00  // 关闭
#define PCAP04_REG32_PI1_CLK_SEL_OLF_DIV1   0x01  // OLF / 1
#define PCAP04_REG32_PI1_CLK_SEL_OLF_DIV2   0x02  // OLF / 2
#define PCAP04_REG32_PI1_CLK_SEL_OLF_DIV4   0x03  // OLF / 4
#define PCAP04_REG32_PI1_CLK_SEL_OX_DIV1    0x04  // OX / 1
#define PCAP04_REG32_PI1_CLK_SEL_OX_DIV2    0x05  // OX / 2
#define PCAP04_REG32_PI1_CLK_SEL_OX_DIV4    0x06  // OX / 4
#define PCAP04_REG32_PI1_CLK_SEL_UNDEFINED   0x07  // 未定义

// 寄存器32完整值构建宏
#define PCAP04_REG32_VALUE(pi1_res, pi1_pdm_sel, pi1_clk_sel) \
    (PCAP04_REG32_PI1_RES_bit(pi1_res) | PCAP04_REG32_PI1_PDM_SEL_bit(pi1_pdm_sel) | \
     PCAP04_REG32_PI1_CLK_SEL_bit(pi1_clk_sel))

// ----------------------------------------------------------------------------
// 寄存器 33 (0x21) - GPIO配置
// ----------------------------------------------------------------------------
// 位[7:4] PG_DIR_IN: 通用端口方向切换（输入/输出）
//  位4: PG0方向 (0=输出, 1=输入)
//  位5: PG1方向 (0=输出, 1=输入)
//  位6: PG2方向 (0=输出, 1=输入)
//  位7: PG3方向 (0=输出, 1=输入)
#define PCAP04_REG33_PG_DIR_IN_SHIFT        4
#define PCAP04_REG33_PG_DIR_IN_MASK         (0x0F << PCAP04_REG33_PG_DIR_IN_SHIFT)
#define PCAP04_REG33_PG_DIR_IN_bit(val)     (((val) & 0x0F) << PCAP04_REG33_PG_DIR_IN_SHIFT)
// PG_DIR_IN端口方向位掩码 (0=输出, 1=输入)
#define PCAP04_REG33_PG0_DIR_OUTPUT        0  // PG0方向 = 输出 (默认)
#define PCAP04_REG33_PG0_DIR_INPUT         (1 << 0)  // PG0方向 = 输入 (位4)
#define PCAP04_REG33_PG1_DIR_OUTPUT        0  // PG1方向 = 输出 (默认)
#define PCAP04_REG33_PG1_DIR_INPUT         (1 << 1)  // PG1方向 = 输入 (位5)
#define PCAP04_REG33_PG2_DIR_OUTPUT        0  // PG2方向 = 输出 (默认)
#define PCAP04_REG33_PG2_DIR_INPUT         (1 << 2)  // PG2方向 = 输入 (位6)
#define PCAP04_REG33_PG3_DIR_OUTPUT        0  // PG3方向 = 输出 (默认)
#define PCAP04_REG33_PG3_DIR_INPUT         (1 << 3)  // PG3方向 = 输入 (位7)

// 位[3:0] PG_PU: 在通用端口激活保护上拉电阻
#define PCAP04_REG33_PG_PU_SHIFT            0
#define PCAP04_REG33_PG_PU_MASK             (0x0F << PCAP04_REG33_PG_PU_SHIFT)
#define PCAP04_REG33_PG_PU_bit(val)         (((val) & 0x0F) << PCAP04_REG33_PG_PU_SHIFT)
// PG_PU端口上拉位掩码 (0=禁用, 1=激活)
#define PCAP04_REG33_PG0_PU_DISABLE         0  // PG0上拉 = 禁用 (默认)
#define PCAP04_REG33_PG0_PU_ENABLE         (1 << 0)  // PG0上拉 = 激活
#define PCAP04_REG33_PG1_PU_DISABLE         0  // PG1上拉 = 禁用 (默认)
#define PCAP04_REG33_PG1_PU_ENABLE         (1 << 1)  // PG1上拉 = 激活
#define PCAP04_REG33_PG2_PU_DISABLE         0  // PG2上拉 = 禁用 (默认)
#define PCAP04_REG33_PG2_PU_ENABLE         (1 << 2)  // PG2上拉 = 激活
#define PCAP04_REG33_PG3_PU_DISABLE         0  // PG3上拉 = 禁用 (默认)
#define PCAP04_REG33_PG3_PU_ENABLE         (1 << 3)  // PG3上拉 = 激活

// 寄存器33完整值构建宏
// 注意：pg_dir_in和pg_pu可以是位掩码的组合（使用 | 操作符）
#define PCAP04_REG33_VALUE(pg_dir_in, pg_pu) \
    (PCAP04_REG33_PG_DIR_IN_bit(pg_dir_in) | PCAP04_REG33_PG_PU_bit(pg_pu))

// ----------------------------------------------------------------------------
// 寄存器 34 (0x22) - 带隙与自动启动配置
// ----------------------------------------------------------------------------
// 位[7] INT_TRIG_BG: 读取结束触发带隙
#define PCAP04_REG34_INT_TRIG_BG_SHIFT      7
#define PCAP04_REG34_INT_TRIG_BG_MASK       (0x01 << PCAP04_REG34_INT_TRIG_BG_SHIFT)
#define PCAP04_REG34_INT_TRIG_BG_bit(val)   (((val) & 0x01) << PCAP04_REG34_INT_TRIG_BG_SHIFT)
#define PCAP04_REG34_INT_TRIG_BG_DISABLE     0  // 禁用
#define PCAP04_REG34_INT_TRIG_BG_ENABLE      1  // 启用

// 位[6] DSP_TRIG_BG: 带隙刷新由DSP位设置触发
#define PCAP04_REG34_DSP_TRIG_BG_SHIFT      6
#define PCAP04_REG34_DSP_TRIG_BG_MASK       (0x01 << PCAP04_REG34_DSP_TRIG_BG_SHIFT)
#define PCAP04_REG34_DSP_TRIG_BG_bit(val)   (((val) & 0x01) << PCAP04_REG34_DSP_TRIG_BG_SHIFT)
#define PCAP04_REG34_DSP_TRIG_BG_DISABLE     0  // 禁用
#define PCAP04_REG34_DSP_TRIG_BG_ENABLE      1  // 启用

// 位[5] BG_PERM: 激活带隙永久启用（当BG_PERM = 1时，电流消耗增加约20 μA）
#define PCAP04_REG34_BG_PERM_SHIFT          5
#define PCAP04_REG34_BG_PERM_MASK           (0x01 << PCAP04_REG34_BG_PERM_SHIFT)
#define PCAP04_REG34_BG_PERM_bit(val)       (((val) & 0x01) << PCAP04_REG34_BG_PERM_SHIFT)
#define PCAP04_REG34_BG_PERM_PULSE_MODE      0  // 带隙脉冲模式
#define PCAP04_REG34_BG_PERM_PERMANENT       1  // 带隙永久启用

// 位[4] AUTOSTART: 在上电后触发CDC（用于独立操作，在上电后触发CDC）
#define PCAP04_REG34_AUTOSTART_SHIFT        4
#define PCAP04_REG34_AUTOSTART_MASK         (0x01 << PCAP04_REG34_AUTOSTART_SHIFT)
#define PCAP04_REG34_AUTOSTART_bit(val)     (((val) & 0x01) << PCAP04_REG34_AUTOSTART_SHIFT)
#define PCAP04_REG34_AUTOSTART_DISABLE        0  // 禁用
#define PCAP04_REG34_AUTOSTART_ENABLE        1  // 启用

// 位[3:0] AMS_INTERNAL: ams内部位（必须为7）
#define PCAP04_REG34_AMS_INTERNAL_SHIFT     0
#define PCAP04_REG34_AMS_INTERNAL_MASK      (0x0F << PCAP04_REG34_AMS_INTERNAL_SHIFT)
#define PCAP04_REG34_AMS_INTERNAL_bit(val)  (((val) & 0x0F) << PCAP04_REG34_AMS_INTERNAL_SHIFT)
#define PCAP04_REG34_AMS_INTERNAL_MUST_BE_7  7   // 必须为7

// 寄存器34完整值构建宏
// 注意：AMS_INTERNAL[3:0] = 7 (必须为7，固定值，自动设置，无需用户输入)
#define PCAP04_REG34_VALUE(int_trig_bg, dsp_trig_bg, bg_perm, autostart) \
    (PCAP04_REG34_INT_TRIG_BG_bit(int_trig_bg) | PCAP04_REG34_DSP_TRIG_BG_bit(dsp_trig_bg) | \
     PCAP04_REG34_BG_PERM_bit(bg_perm) | PCAP04_REG34_AUTOSTART_bit(autostart) | \
     PCAP04_REG34_AMS_INTERNAL_bit(7))  // AMS_INTERNAL自动设置为7（固定值）

// ----------------------------------------------------------------------------
// 寄存器 35 (0x23) - 增益缩放因子 (CDC_GAIN_CORR)
// ----------------------------------------------------------------------------
// 位[7:0] CDC_GAIN_CORR: 增益校正因子（范围: 0-255，公式: 增益 = 1 + n/256）
#define PCAP04_REG35_CDC_GAIN_CORR_SHIFT    0
#define PCAP04_REG35_CDC_GAIN_CORR_MASK     (0xFF << PCAP04_REG35_CDC_GAIN_CORR_SHIFT)
#define PCAP04_REG35_CDC_GAIN_CORR_bit(val) ((uint8_t)((val) & 0xFF))
#define PCAP04_REG35_CDC_GAIN_CORR_1_00      0x00  // 增益 = 1.0
#define PCAP04_REG35_CDC_GAIN_CORR_1_125    0x20  // 增益 = 1.125 (n=32)
#define PCAP04_REG35_CDC_GAIN_CORR_1_25     0x40  // 增益 = 1.25 (n=64, 推荐)
#define PCAP04_REG35_CDC_GAIN_CORR_1_375    0x60  // 增益 = 1.375 (n=96)
#define PCAP04_REG35_CDC_GAIN_CORR_1_50     0x80  // 增益 = 1.5 (n=128)
#define PCAP04_REG35_CDC_GAIN_CORR_1_996    0xFF  // 增益 ≈ 1.996 (n=255)

// 寄存器 36 (0x24) - 未使用 (必须为0x00)
// 寄存器 37 (0x25) - 未使用 (必须为0x00)

// 寄存器 38 (0x26) - 带隙时间配置
#define PCAP04_REG38_BG_TIME_SHIFT          0  // 带隙时间 (固件定义) [7:0]
#define PCAP04_REG38_BG_TIME_MASK           (0xFF << PCAP04_REG38_BG_TIME_SHIFT)

// ----------------------------------------------------------------------------
// 寄存器 39 (0x27) - 脉冲选择配置
// ----------------------------------------------------------------------------
// 位[7:4] PULSE_SEL1: 为脉冲接口1选择源（固件定义）
#define PCAP04_REG39_PULSE_SEL1_SHIFT       4
#define PCAP04_REG39_PULSE_SEL1_MASK        (0x0F << PCAP04_REG39_PULSE_SEL1_SHIFT)
#define PCAP04_REG39_PULSE_SEL1_bit(val)    (((val) & 0x0F) << PCAP04_REG39_PULSE_SEL1_SHIFT)
#define PCAP04_REG39_PULSE_SEL1_RES0        0x00  // C0/Cref
#define PCAP04_REG39_PULSE_SEL1_RES1        0x01  // C1/Cref
#define PCAP04_REG39_PULSE_SEL1_RES2        0x02  // C2/Cref
#define PCAP04_REG39_PULSE_SEL1_RES3        0x03  // C3/Cref
#define PCAP04_REG39_PULSE_SEL1_RES4        0x04  // C4/Cref
#define PCAP04_REG39_PULSE_SEL1_RES5        0x05  // C5/Cref
#define PCAP04_REG39_PULSE_SEL1_RES6        0x06  // Res6 (PT1/Ref @PCap04_standard)
#define PCAP04_REG39_PULSE_SEL1_RES7        0x07  // Res7 (Alu/Ref @PCap04_standard)

// 位[3:0] PULSE_SEL0: 为脉冲接口0选择源（固件定义）
#define PCAP04_REG39_PULSE_SEL0_SHIFT       0
#define PCAP04_REG39_PULSE_SEL0_MASK        (0x0F << PCAP04_REG39_PULSE_SEL0_SHIFT)
#define PCAP04_REG39_PULSE_SEL0_bit(val)    (((val) & 0x0F) << PCAP04_REG39_PULSE_SEL0_SHIFT)
#define PCAP04_REG39_PULSE_SEL0_RES0        0x00  // C0/Cref
#define PCAP04_REG39_PULSE_SEL0_RES1        0x01  // C1/Cref
#define PCAP04_REG39_PULSE_SEL0_RES2        0x02  // C2/Cref
#define PCAP04_REG39_PULSE_SEL0_RES3        0x03  // C3/Cref
#define PCAP04_REG39_PULSE_SEL0_RES4        0x04  // C4/Cref
#define PCAP04_REG39_PULSE_SEL0_RES5        0x05  // C5/Cref
#define PCAP04_REG39_PULSE_SEL0_RES6        0x06  // Res6 (PT1/Ref @PCap04_standard)
#define PCAP04_REG39_PULSE_SEL0_RES7        0x07  // Res7 (Alu/Ref @PCap04_standard)

// 寄存器39完整值构建宏
#define PCAP04_REG39_VALUE(pulse_sel1, pulse_sel0) \
    (PCAP04_REG39_PULSE_SEL1_bit(pulse_sel1) | PCAP04_REG39_PULSE_SEL0_bit(pulse_sel0))

// --- 固件和DSP相关 (寄存器 40-63) ---

// 寄存器 40 (0x28) - 电容感测选择 (仅PCap04_linearize固件)
#define PCAP04_REG40_C_SENSE_SEL_SHIFT      0  // 电容比率选择 [7:0]
#define PCAP04_REG40_C_SENSE_SEL_MASK       (0xFF << PCAP04_REG40_C_SENSE_SEL_SHIFT)

// 寄存器 41 (0x29) - 电阻感测选择 (仅PCap04_linearize固件)
#define PCAP04_REG41_R_SENSE_SEL_SHIFT      0  // 电阻比率选择 [7:0]
#define PCAP04_REG41_R_SENSE_SEL_MASK       (0xFF << PCAP04_REG41_R_SENSE_SEL_SHIFT)

// ----------------------------------------------------------------------------
// 寄存器 42 (0x2A) - 固件配置与报警选择
// ----------------------------------------------------------------------------
// 位[6] ALARM1_SELECT: 报警1选择（仅PCap04_linearize固件，极性选择: 0 = Z, 1 = Theta，在PG1选择报警信号源（高电平有效））
#define PCAP04_REG42_ALARM1_SELECT_SHIFT    6
#define PCAP04_REG42_ALARM1_SELECT_MASK     (0x01 << PCAP04_REG42_ALARM1_SELECT_SHIFT)
#define PCAP04_REG42_ALARM1_SELECT_bit(val)  (((val) & 0x01) << PCAP04_REG42_ALARM1_SELECT_SHIFT)
#define PCAP04_REG42_ALARM1_SELECT_Z        0  // Z
#define PCAP04_REG42_ALARM1_SELECT_Theta    1  // Theta

// 位[4] ALARM0_SELECT: 报警0选择（仅PCap04_linearize固件，极性选择: 0 = Z, 1 = Theta，在PG0选择报警信号源（高电平有效））
#define PCAP04_REG42_ALARM0_SELECT_SHIFT    4
#define PCAP04_REG42_ALARM0_SELECT_MASK     (0x01 << PCAP04_REG42_ALARM0_SELECT_SHIFT)
#define PCAP04_REG42_ALARM0_SELECT_bit(val)  (((val) & 0x01) << PCAP04_REG42_ALARM0_SELECT_SHIFT)
#define PCAP04_REG42_ALARM0_SELECT_Z        0  // Z
#define PCAP04_REG42_ALARM0_SELECT_Theta    1  // Theta

// 位[3] EN_ASYNC_READ: 启用异步读取（结果寄存器Res0到Res7中的值仅在读取先前值后才更新）
#define PCAP04_REG42_EN_ASYNC_READ_SHIFT    3
#define PCAP04_REG42_EN_ASYNC_READ_MASK     (0x01 << PCAP04_REG42_EN_ASYNC_READ_SHIFT)
#define PCAP04_REG42_EN_ASYNC_READ_bit(val) (((val) & 0x01) << PCAP04_REG42_EN_ASYNC_READ_SHIFT)
#define PCAP04_REG42_EN_ASYNC_READ_DISABLE   0  // 禁用
#define PCAP04_REG42_EN_ASYNC_READ_ACTIVE    1  // 激活

// 位[2] HS_MODE_SEL: ams内部位（必须为0）
#define PCAP04_REG42_HS_MODE_SEL_SHIFT      2
#define PCAP04_REG42_HS_MODE_SEL_MASK       (0x01 << PCAP04_REG42_HS_MODE_SEL_SHIFT)
#define PCAP04_REG42_HS_MODE_SEL_bit(val)   (((val) & 0x01) << PCAP04_REG42_HS_MODE_SEL_SHIFT)
#define PCAP04_REG42_HS_MODE_SEL_MUST_BE_0   0   // 必须为0

// 位[1] R_MEDIAN_EN: 启用R的中值滤波器（仅PCap04_linearize固件，在线性化固件中启用ri的中值滤波器）
#define PCAP04_REG42_R_MEDIAN_EN_SHIFT      1
#define PCAP04_REG42_R_MEDIAN_EN_MASK       (0x01 << PCAP04_REG42_R_MEDIAN_EN_SHIFT)
#define PCAP04_REG42_R_MEDIAN_EN_bit(val)   (((val) & 0x01) << PCAP04_REG42_R_MEDIAN_EN_SHIFT)
#define PCAP04_REG42_R_MEDIAN_EN_DISABLE     0  // 禁用
#define PCAP04_REG42_R_MEDIAN_EN_ENABLE      1  // 启用

// 位[0] C_MEDIAN_EN: 启用C的中值滤波器（仅PCap04_linearize固件，在线性化固件中启用ci的中值滤波器）
#define PCAP04_REG42_C_MEDIAN_EN_SHIFT      0
#define PCAP04_REG42_C_MEDIAN_EN_MASK       (0x01 << PCAP04_REG42_C_MEDIAN_EN_SHIFT)
#define PCAP04_REG42_C_MEDIAN_EN_bit(val)   (((val) & 0x01) << PCAP04_REG42_C_MEDIAN_EN_SHIFT)
#define PCAP04_REG42_C_MEDIAN_EN_DISABLE     0  // 禁用
#define PCAP04_REG42_C_MEDIAN_EN_ENABLE      1  // 启用

// 寄存器42完整值构建宏
// 注意：HS_MODE_SEL[2] = 0 (必须为0，固定值，自动设置，无需用户输入)
#define PCAP04_REG42_VALUE(alarm1_select, alarm0_select, en_async_read, r_median_en, c_median_en) \
    (PCAP04_REG42_ALARM1_SELECT_bit(alarm1_select) | PCAP04_REG42_ALARM0_SELECT_bit(alarm0_select) | \
     PCAP04_REG42_EN_ASYNC_READ_bit(en_async_read) | PCAP04_REG42_HS_MODE_SEL_bit(0) | \
     PCAP04_REG42_R_MEDIAN_EN_bit(r_median_en) | PCAP04_REG42_C_MEDIAN_EN_bit(c_median_en))  // HS_MODE_SEL自动设置为0（固定值）

// ----------------------------------------------------------------------------
// 寄存器 43-46 (0x2B-0x2E) - 保留寄存器 (必须为0x00)
// ----------------------------------------------------------------------------
// 这些寄存器必须保持为0x00，禁止修改

// ----------------------------------------------------------------------------
// 寄存器 47 (0x2F) - 运行位配置
// ----------------------------------------------------------------------------
// 位[0] RUNBIT: 前端和DSP的开关
#define PCAP04_REG47_RUNBIT_SHIFT           0
#define PCAP04_REG47_RUNBIT_MASK            (0x01 << PCAP04_REG47_RUNBIT_SHIFT)
#define PCAP04_REG47_RUNBIT_bit(val)        (((val) & 0x01) << PCAP04_REG47_RUNBIT_SHIFT)
#define PCAP04_REG47_RUNBIT_OFF              0  // 关闭 = 芯片系统空闲并受保护
#define PCAP04_REG47_RUNBIT_ON               1  // 开启 = 保护被移除，系统可以运行

// ----------------------------------------------------------------------------
// 寄存器 48 (0x30) - 内存锁配置
// ----------------------------------------------------------------------------
// 位[3:0] MEM_LOCK: 数据安全功能（范围: 0-15，具体功能由固件定义）
#define PCAP04_REG48_MEM_LOCK_SHIFT         0
#define PCAP04_REG48_MEM_LOCK_MASK          (0x0F << PCAP04_REG48_MEM_LOCK_SHIFT)
#define PCAP04_REG48_MEM_LOCK_bit(val)      (((val) & 0x0F) << PCAP04_REG48_MEM_LOCK_SHIFT)

// ----------------------------------------------------------------------------
// 寄存器 49 (0x31) - 序列号低字节（只读）
// ----------------------------------------------------------------------------
// 位[7:0] SERIAL_NUMBER_L: 序列号低字节（只读寄存器）
#define PCAP04_REG49_SERIAL_NUMBER_L_SHIFT  0
#define PCAP04_REG49_SERIAL_NUMBER_L_MASK   (0xFF << PCAP04_REG49_SERIAL_NUMBER_L_SHIFT)
#define PCAP04_REG49_SERIAL_NUMBER_L_bit(val) ((uint8_t)((val) & 0xFF))

// ----------------------------------------------------------------------------
// 寄存器 50 (0x32) - 序列号高字节（只读）
// ----------------------------------------------------------------------------
// 位[7:0] SERIAL_NUMBER_H: 序列号高字节（只读寄存器）
#define PCAP04_REG50_SERIAL_NUMBER_H_SHIFT  0
#define PCAP04_REG50_SERIAL_NUMBER_H_MASK   (0xFF << PCAP04_REG50_SERIAL_NUMBER_H_SHIFT)
#define PCAP04_REG50_SERIAL_NUMBER_H_bit(val) ((uint8_t)((val) & 0xFF))

// ----------------------------------------------------------------------------
// 寄存器 51-53 (0x33-0x35) - ams内部寄存器 (必须为0x00)
// ----------------------------------------------------------------------------
// 这些寄存器必须保持为0x00，禁止修改

// ----------------------------------------------------------------------------
// 寄存器 54 (0x36) - 内存控制
// ----------------------------------------------------------------------------
// 位[7:0] MEM_CTRL: 内存控制命令
#define PCAP04_REG54_MEM_CTRL_SHIFT         0
#define PCAP04_REG54_MEM_CTRL_MASK          (0xFF << PCAP04_REG54_MEM_CTRL_SHIFT)
#define PCAP04_REG54_MEM_CTRL_bit(val)      ((uint8_t)((val) & 0xFF))
#define PCAP04_REG54_MEM_CTRL_STORE_NVRAM   0x2D  // NVRAM存储启用 (将配置存储到NVRAM)
#define PCAP04_REG54_MEM_CTRL_RECALL_NVRAM  0x59  // NVRAM召回启用 (从NVRAM重新加载配置)
#define PCAP04_REG54_MEM_CTRL_ERASE_NVRAM   0xB8  // NVRAM擦除 (擦除NVRAM)

// ----------------------------------------------------------------------------
// 寄存器 55-61 (0x37-0x3D) - ams内部寄存器 (必须为0x00)
// ----------------------------------------------------------------------------
// 这些寄存器必须保持为0x00，禁止修改

// ----------------------------------------------------------------------------
// 寄存器 62 (0x3E) - 电荷泵配置低字节 (重要: 设备特定设置，禁止更改)
// ----------------------------------------------------------------------------
// 位[7:0] CHARGE_PUMP_L: 电荷泵配置低字节（设备特定设置，禁止更改）
#define PCAP04_REG62_CHARGE_PUMP_L_SHIFT    0
#define PCAP04_REG62_CHARGE_PUMP_L_MASK     (0xFF << PCAP04_REG62_CHARGE_PUMP_L_SHIFT)
#define PCAP04_REG62_CHARGE_PUMP_L_bit(val) ((uint8_t)((val) & 0xFF))

// ----------------------------------------------------------------------------
// 寄存器 63 (0x3F) - 电荷泵配置高字节 (重要: 设备特定设置，禁止更改)
// ----------------------------------------------------------------------------
// 位[7:0] CHARGE_PUMP_H: 电荷泵配置高字节（设备特定设置，禁止更改）
#define PCAP04_REG63_CHARGE_PUMP_H_SHIFT    0
#define PCAP04_REG63_CHARGE_PUMP_H_MASK     (0xFF << PCAP04_REG63_CHARGE_PUMP_H_SHIFT)
#define PCAP04_REG63_CHARGE_PUMP_H_bit(val) ((uint8_t)((val) & 0xFF))


// --- 多字节寄存器访问宏 ---
// 这些宏用于将跨越多个寄存器的值进行组合和拆分。

// C_AVRG (寄存器 7-8, 13位)
// 从寄存器值获取C_AVRG (reg_val_8是高字节，reg_val_7是低字节)
#define PCAP04_GET_C_AVRG(reg_val_8, reg_val_7) \
    (((uint16_t)((reg_val_8) & 0x1F) << 8) | (reg_val_7))

// 设置C_AVRG的寄存器7 (低8位)
#define PCAP04_SET_C_AVRG_REG7(value) \
    ((uint8_t)((value) & 0xFF))

// 设置C_AVRG的寄存器8 (高5位)
#define PCAP04_SET_C_AVRG_REG8(value) \
    ((uint8_t)(((value) >> 8) & 0x1F))

// C_AVRG配置项生成宏（一次性设置寄存器7-8，只需输入一次值）
// 使用方式：在配置数组中展开为两个配置项
// 例如：PCAP04_C_AVRG_CONFIG(32) 会展开为两个配置项，分别设置寄存器7和8
#define PCAP04_C_AVRG_CONFIG(value) \
    {PCAP04_REG_ADDR_7, PCAP04_SET_C_AVRG_REG7(value)}, \
    {PCAP04_REG_ADDR_8, PCAP04_SET_C_AVRG_REG8(value)}

// CONV_TIME (寄存器 9-11, 23位)
// 从寄存器值获取CONV_TIME (reg_val_11是最高字节，reg_val_9是最低字节)
#define PCAP04_GET_CONV_TIME(reg_val_11, reg_val_10, reg_val_9) \
    (((uint32_t)((reg_val_11) & 0x7F) << 16) | ((uint32_t)(reg_val_10) << 8) | (reg_val_9))

// 设置CONV_TIME的寄存器9 (低8位)
#define PCAP04_SET_CONV_TIME_REG9(value) \
    ((uint8_t)((value) & 0xFF))

// 设置CONV_TIME的寄存器10 (中间8位)
#define PCAP04_SET_CONV_TIME_REG10(value) \
    ((uint8_t)(((value) >> 8) & 0xFF))

// 设置CONV_TIME的寄存器11 (高7位)
#define PCAP04_SET_CONV_TIME_REG11(value) \
    ((uint8_t)(((value) >> 16) & 0x7F))

// CONV_TIME配置项生成宏（一次性设置寄存器9-11，只需输入一次值）
// 使用方式：在配置数组中展开为三个配置项
// 例如：PCAP04_CONV_TIME_CONFIG(2000) 会展开为三个配置项，分别设置寄存器9、10、11
// 说明：CONV_TIME值会影响寄存器9[7:0]、寄存器10[7:0]、寄存器11[6:0]
#define PCAP04_CONV_TIME_CONFIG(value) \
    {PCAP04_REG_ADDR_9, PCAP04_SET_CONV_TIME_REG9(value)}, \
    {PCAP04_REG_ADDR_10, PCAP04_SET_CONV_TIME_REG10(value)}, \
    {PCAP04_REG_ADDR_11, PCAP04_SET_CONV_TIME_REG11(value)}

// DISCHARGE_TIME (寄存器 12-13, 10位)
// 从寄存器值获取DISCHARGE_TIME (reg_val_13是高2位，reg_val_12是低8位)
#define PCAP04_GET_DISCHARGE_TIME(reg_val_13, reg_val_12) \
    (((uint16_t)((reg_val_13) & 0x03) << 8) | (reg_val_12))

// 设置DISCHARGE_TIME的寄存器12 (低8位)
#define PCAP04_SET_DISCHARGE_TIME_REG12(value) \
    ((uint8_t)((value) & 0xFF))

// 设置DISCHARGE_TIME的寄存器13 (高2位，注意保留其他位域)
#define PCAP04_SET_DISCHARGE_TIME_REG13(current_reg13_val, value) \
    ((uint8_t)(((current_reg13_val) & ~PCAP04_DISCHARGE_TIME_MASK_REG13) | (((value) >> 8) & 0x03)))

// DISCHARGE_TIME配置项生成宏（一次性设置寄存器12，寄存器13需要与其他位域合并）
// 使用方式：在配置数组中展开为一个配置项（寄存器12）
// 说明：DISCHARGE_TIME值会影响寄存器12[7:0]（低8位）和寄存器13[1:0]（高2位）
// 注意：寄存器13还包含C_STARTONPIN[7:6]和C_TRIG_SEL[4:2]，需要使用PCAP04_REG13_VALUE合并
#define PCAP04_DISCHARGE_TIME_CONFIG(value) \
    {PCAP04_REG_ADDR_12, PCAP04_SET_DISCHARGE_TIME_REG12(value)}

// DISCHARGE_TIME和寄存器13组合配置宏（一次性设置寄存器12和13，只需输入一次DISCHARGE_TIME值）
// 使用方式：在配置数组中展开为两个配置项（寄存器12和13）
// 说明：DISCHARGE_TIME值会自动设置到寄存器12[7:0]和寄存器13[1:0]，同时设置C_STARTONPIN和C_TRIG_SEL
#define PCAP04_DISCHARGE_TIME_AND_REG13_CONFIG(discharge_time, startonpin, trig_sel) \
    {PCAP04_REG_ADDR_12, PCAP04_SET_DISCHARGE_TIME_REG12(discharge_time)}, \
    {PCAP04_REG_ADDR_13, PCAP04_REG13_VALUE(discharge_time, startonpin, trig_sel)}

// PRECHARGE_TIME (寄存器 14-15, 10位)
// 从寄存器值获取PRECHARGE_TIME (reg_val_15是高2位，reg_val_14是低8位)
#define PCAP04_GET_PRECHARGE_TIME(reg_val_15, reg_val_14) \
    (((uint16_t)((reg_val_15) & 0x03) << 8) | (reg_val_14))

// 设置PRECHARGE_TIME的寄存器14 (低8位)
#define PCAP04_SET_PRECHARGE_TIME_REG14(value) \
    ((uint8_t)((value) & 0xFF))

// 设置PRECHARGE_TIME的寄存器15 (高2位，注意保留其他位域)
#define PCAP04_SET_PRECHARGE_TIME_REG15(current_reg15_val, value) \
    ((uint8_t)(((current_reg15_val) & ~PCAP04_PRECHARGE_TIME_MASK_REG15) | (((value) >> 8) & 0x03)))

// PRECHARGE_TIME配置项生成宏（一次性设置寄存器14，寄存器15需要与其他位域合并）
// 使用方式：在配置数组中展开为一个配置项（寄存器14）
// 说明：PRECHARGE_TIME值会影响寄存器14[7:0]（低8位）和寄存器15[1:0]（高2位）
// 注意：寄存器15还包含C_FAKE[5:2]，需要使用PCAP04_REG15_VALUE合并
#define PCAP04_PRECHARGE_TIME_CONFIG(value) \
    {PCAP04_REG_ADDR_14, PCAP04_SET_PRECHARGE_TIME_REG14(value)}

// PRECHARGE_TIME和寄存器15组合配置宏（一次性设置寄存器14和15，只需输入一次PRECHARGE_TIME值）
// 使用方式：在配置数组中展开为两个配置项（寄存器14和15）
// 说明：PRECHARGE_TIME值会自动设置到寄存器14[7:0]和寄存器15[1:0]，同时设置C_FAKE
#define PCAP04_PRECHARGE_TIME_AND_REG15_CONFIG(precharge_time, c_fake) \
    {PCAP04_REG_ADDR_14, PCAP04_SET_PRECHARGE_TIME_REG14(precharge_time)}, \
    {PCAP04_REG_ADDR_15, PCAP04_REG15_VALUE(precharge_time, c_fake)}

// FULLCHARGE_TIME (寄存器 16-17, 10位)
// 从寄存器值获取FULLCHARGE_TIME (reg_val_17是高2位，reg_val_16是低8位)
#define PCAP04_GET_FULLCHARGE_TIME(reg_val_17, reg_val_16) \
    (((uint16_t)((reg_val_17) & 0x03) << 8) | (reg_val_16))

// 设置FULLCHARGE_TIME的寄存器16 (低8位)
#define PCAP04_SET_FULLCHARGE_TIME_REG16(value) \
    ((uint8_t)((value) & 0xFF))

// 设置FULLCHARGE_TIME的寄存器17 (高2位，注意保留其他位域)
#define PCAP04_SET_FULLCHARGE_TIME_REG17(current_reg17_val, value) \
    ((uint8_t)(((current_reg17_val) & ~PCAP04_FULLCHARGE_TIME_MASK_REG17) | (((value) >> 8) & 0x03)))

// FULLCHARGE_TIME配置项生成宏（一次性设置寄存器16，寄存器17需要与其他位域合并）
// 使用方式：在配置数组中展开为一个配置项（寄存器16）
// 说明：FULLCHARGE_TIME值会影响寄存器16[7:0]（低8位）和寄存器17[1:0]（高2位）
// 注意：寄存器17还包含C_REF_SEL[6:2]，需要使用PCAP04_REG17_VALUE合并
#define PCAP04_FULLCHARGE_TIME_CONFIG(value) \
    {PCAP04_REG_ADDR_16, PCAP04_SET_FULLCHARGE_TIME_REG16(value)}

// FULLCHARGE_TIME和寄存器17组合配置宏（一次性设置寄存器16和17，只需输入一次FULLCHARGE_TIME值）
// 使用方式：在配置数组中展开为两个配置项（寄存器16和17）
// 说明：FULLCHARGE_TIME值会自动设置到寄存器16[7:0]和寄存器17[1:0]，同时设置C_REF_SEL
#define PCAP04_FULLCHARGE_TIME_AND_REG17_CONFIG(fullcharge_time, c_ref_sel) \
    {PCAP04_REG_ADDR_16, PCAP04_SET_FULLCHARGE_TIME_REG16(fullcharge_time)}, \
    {PCAP04_REG_ADDR_17, PCAP04_REG17_VALUE(fullcharge_time, c_ref_sel)}

// R_TRIG_PREDIV (寄存器 21-22, 10位)
// 从寄存器值获取R_TRIG_PREDIV (reg_val_22是高2位，reg_val_21是低8位)
#define PCAP04_GET_R_TRIG_PREDIV(reg_val_22, reg_val_21) \
    (((uint16_t)((reg_val_22) & 0x03) << 8) | (reg_val_21))

// 设置R_TRIG_PREDIV的寄存器21 (低8位)
#define PCAP04_SET_R_TRIG_PREDIV_REG21(value) \
    ((uint8_t)((value) & 0xFF))

// 设置R_TRIG_PREDIV的寄存器22 (高2位，注意保留其他位域)
#define PCAP04_SET_R_TRIG_PREDIV_REG22(current_reg22_val, value) \
    ((uint8_t)(((current_reg22_val) & ~PCAP04_R_TRIG_PREDIV_MASK_REG22) | (((value) >> 8) & 0x03)))

// R_TRIG_PREDIV配置项生成宏（一次性设置寄存器21，寄存器22需要与其他位域合并）
// 使用方式：在配置数组中展开为一个配置项（寄存器21）
// 说明：R_TRIG_PREDIV值会影响寄存器21[7:0]（低8位）和寄存器22[1:0]（高2位）
// 注意：寄存器22还包含R_AVRG[3:2]和R_TRIG_SEL[6:4]，需要使用PCAP04_REG22_VALUE合并
#define PCAP04_R_TRIG_PREDIV_CONFIG(value) \
    {PCAP04_REG_ADDR_21, PCAP04_SET_R_TRIG_PREDIV_REG21(value)}

// R_TRIG_PREDIV和寄存器22组合配置宏（一次性设置寄存器21和22，只需输入一次R_TRIG_PREDIV值）
// 使用方式：在配置数组中展开为两个配置项（寄存器21和22）
// 说明：R_TRIG_PREDIV值会自动设置到寄存器21[7:0]和寄存器22[1:0]，同时设置R_AVRG和R_TRIG_SEL
#define PCAP04_R_TRIG_PREDIV_AND_REG22_CONFIG(r_trig_prediv, r_avrg, r_trig_sel) \
    {PCAP04_REG_ADDR_21, PCAP04_SET_R_TRIG_PREDIV_REG21(r_trig_prediv)}, \
    {PCAP04_REG_ADDR_22, PCAP04_REG22_VALUE(r_trig_prediv, r_avrg, r_trig_sel)}

/**
 * @brief  PCAP04配置数组（64字节）
 */
extern unsigned char PCAP04_Config[64];
const unsigned char* PCAP04_GetConfig(void);

/**
 * @brief  打印PCAP04配置数组内容（用于PC环境调试）
 *         输出所有64个寄存器的地址和值
 * 
 * @note 使用标准C库printf输出，可在PC环境编译运行
 * @note 调用示例：PCAP04_PrintConfig();
 */
void PCAP04_PrintConfig(void);

typedef struct {
    uint8_t reg_addr;  // 寄存器地址 (0-63)
    uint8_t value;     // 寄存器值
} PCAP04_RegConfigItem_t;
int32_t PCAP04_BuildConfigArray(const PCAP04_RegConfigItem_t *config_items,
    uint8_t item_num,
    uint8_t *config_array,
    const uint8_t *default_array);

void PCAP04_InitConfig(void);
void PCAP04_PrintConfig(void);   /* 如果 verify_config.c 也要用 */
#endif // PCAP04_REGISTERS_H
