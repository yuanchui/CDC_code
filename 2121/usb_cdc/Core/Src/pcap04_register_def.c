#include "pcap04_register.h"

/* 表格说明：
 * 本文件完整定义 PCAP04 每个寄存器及其位段信息，结合 pcap04_register.c
 * 可在调试输出或 USB 命令中给出友好的中文描述。请保持该文件与器件
 * 数据手册版本同步。
 */

// ==================== 寄存器0 (0x00): IIC地址与低频时钟配置 ====================
static const PCAP04_BitOption_t REG0_I2C_A_OPTIONS[] = {
    {0x00, "ADDR_0", "I²C地址补码0"},
    {0x01, "ADDR_1", "I²C地址补码1"},
    {0x02, "ADDR_2", "I²C地址补码2"},
    {0x03, "ADDR_3", "I²C地址补码3"}
};

static const PCAP04_BitOption_t REG0_OLF_FTUNE_OPTIONS[] = {
    {0x00, "MIN", "最小值0"},
    {0x07, "TYP", "典型值7"},
    {0x0F, "MAX", "最大值15"}
};

static const PCAP04_BitOption_t REG0_OLF_CTUNE_OPTIONS[] = {
    {0x00, "10KHZ", "10 kHz频率"},
    {0x01, "50KHZ", "50 kHz频率"},
    {0x02, "100KHZ", "100 kHz频率"},
    {0x03, "200KHZ", "200 kHz频率"}
};

/* 说明：
 * OLF_CTUNE 与 OLF_FTUNE 组合决定低频时钟，典型映射如下（单位：kHz）：
 *   OLF_CTUNE=0, OLF_FTUNE=7 -> 10 kHz
 *   OLF_CTUNE=1, OLF_FTUNE=7 -> 50 kHz
 *   OLF_CTUNE=2, OLF_FTUNE=4 -> 100 kHz
 *   OLF_CTUNE=3, OLF_FTUNE=5~7 -> 200 kHz（推荐）
 * 电容较大时需适当降低频率以匹配充放电时间。
 */

static const PCAP04_RegBit_t REG0_BITS[] = {
    {6, 7, "I2C_A", "I²C设备地址补码配置", 4, REG0_I2C_A_OPTIONS},
    {2, 5, "OLF_FTUNE", "低频时钟微调(0-15)", 3, REG0_OLF_FTUNE_OPTIONS},
    {0, 1, "OLF_CTUNE", "低频时钟粗调", 4, REG0_OLF_CTUNE_OPTIONS}
};

// ==================== 寄存器1 (0x01): 外部晶振时钟OX配置 ====================
static const PCAP04_BitOption_t REG1_OX_DIS_OPTIONS[] = {
    {0x00, "启用", "OX时钟启用"},
    {0x01, "禁用", "OX时钟禁用"}
};

static const PCAP04_BitOption_t REG1_OX_DIV4_OPTIONS[] = {
    {0x00, "2MHZ", "不分频,f_ox=2MHz"},
    {0x01, "0.5MHZ", "4分频,f_ox=0.5MHz"}
};

static const PCAP04_BitOption_t REG1_OX_RUN_OPTIONS[] = {
    {0x00, "关闭", "发生器关闭"},
    {0x01, "永久运行", "OX永久运行"},
    {0x02, "延迟31", "OX延迟=31/fOLF"},
    {0x03, "延迟2", "OX延迟=2/fOLF"},
    {0x06, "延迟1", "OX延迟=1/fOLF"}
};

static const PCAP04_RegBit_t REG1_BITS[] = {
    {7, 7, "OX_DIS", "禁用OX时钟", 2, REG1_OX_DIS_OPTIONS},
    {5, 5, "OX_DIV4", "OX时钟4分频", 2, REG1_OX_DIV4_OPTIONS},
    {0, 2, "OX_RUN", "OX发生器持续性或延迟", 5, REG1_OX_RUN_OPTIONS}
};

// ==================== 寄存器2 (0x02): 端口放电电阻配置 ====================
static const PCAP04_BitOption_t REG2_RDCHG_INT_SEL_OPTIONS[] = {
    {0x00, "180K", "180 kΩ放电电阻"},
    {0x01, "90K", "90 kΩ放电电阻"},
    {0x02, "30K", "30 kΩ放电电阻"},
    {0x03, "10K", "10 kΩ放电电阻"}
};

/* 说明：
 * 放电电阻越大，放电时间越长；常见组合：
 *   0x08 -> 所有通道 180kΩ（慢放电）
 *   0x38 -> 所有通道 30kΩ（标准配置）
 *   0x78 -> 所有通道 10kΩ（快速放电）
 * 使用外部放电电阻时需结合寄存器3的 AUX_PD_DIS 设置。
 */

static const PCAP04_BitOption_t REG2_ENABLE_OPTIONS[] = {
    {0x00, "禁用", "禁用"},
    {0x01, "启用", "启用"}
};

static const PCAP04_RegBit_t REG2_BITS[] = {
    {6, 7, "RDCHG_INT_SEL1", "PC4~PC5片上放电电阻", 4, REG2_RDCHG_INT_SEL_OPTIONS},
    {4, 5, "RDCHG_INT_SEL0", "PC0~PC3和PC6片上放电电阻", 4, REG2_RDCHG_INT_SEL_OPTIONS},
    {3, 3, "RDCHG_INT_EN", "内部放电电阻使能", 2, REG2_ENABLE_OPTIONS},
    {1, 1, "RDCHG_EXT_EN", "外部放电电阻使能", 2, REG2_ENABLE_OPTIONS}
};

// ==================== 寄存器3 (0x03): 端口充放电电阻配置 ====================
static const PCAP04_BitOption_t REG3_RCHG_SEL_OPTIONS[] = {
    {0x00, "180K", "180 kΩ充电电阻"},
    {0x01, "10K", "10 kΩ充电电阻(默认)"}
};

static const PCAP04_RegBit_t REG3_BITS[] = {
    {6, 6, "AUX_PD_DIS", "禁用PCAUX下拉电阻", 2, REG2_ENABLE_OPTIONS},
    {5, 5, "AUX_CINT", "内部参考转换期间激活PCAUX", 2, REG2_ENABLE_OPTIONS},
    {2, 2, "RDCHG_PERM_EN", "永久连接内部放电电阻", 2, REG2_ENABLE_OPTIONS},
    {1, 1, "RDCHG_EXT_PERM", "永久激活辅助端口PCAUX", 2, REG2_ENABLE_OPTIONS},
    {0, 0, "RCHG_SEL", "选择充电电阻", 2, REG3_RCHG_SEL_OPTIONS}
};

/* 说明：
 * RCHG_SEL=0 代表 180kΩ，适合慢充电、高精度；=1 代表 10kΩ，适合快速充电。
 * 启用外部放电（位1）时需同时设定 AUX_PD_DIS=1 以避免冲突。
 */

// ==================== 寄存器4 (0x04): 端口电容连接方式 ====================
static const PCAP04_BitOption_t REG4_MODE_OPTIONS[] = {
    {0x00, "外部", "外部参考"},
    {0x01, "内部", "内部参考"}
};

static const PCAP04_BitOption_t REG4_SENSOR_OPTIONS[] = {
    {0x00, "单端", "单端传感器"},
    {0x01, "差分", "差分传感器"}
};

static const PCAP04_BitOption_t REG4_FLOAT_OPTIONS[] = {
    {0x00, "接地", "接地传感器"},
    {0x01, "浮动", "浮动传感器"}
};

static const PCAP04_RegBit_t REG4_BITS[] = {
    {7, 7, "C_REF_INT", "使用片上参考电容", 2, REG4_MODE_OPTIONS},
    {5, 5, "C_COMP_EXT", "外部寄生电容补偿", 2, REG2_ENABLE_OPTIONS},
    {4, 4, "C_COMP_INT", "片上寄生电容补偿", 2, REG2_ENABLE_OPTIONS},
    {1, 1, "C_DIFFERENTIAL", "单端或差分传感器", 2, REG4_SENSOR_OPTIONS},
    {0, 0, "C_FLOATING", "接地或浮动传感器", 2, REG4_FLOAT_OPTIONS}
};

/* 常用模式：
 *   0x00 外部参考+单端接地（标准）
 *   0x01 外部参考+单端浮动
 *   0x02 外部参考+差分接地
 *   0x10 外部参考+单端接地（内部补偿关闭，当前配置）
 *   0x90 内部参考+单端接地并开启内部补偿
 */

// ==================== 寄存器5 (0x05): CDC时钟与端口模式配置 ====================
static const PCAP04_BitOption_t REG5_CLK_SEL_OPTIONS[] = {
    {0x00, "OLF", "低频时钟OLF"},
    {0x01, "OHF", "高频时钟OHF"}
};

static const PCAP04_RegBit_t REG5_BITS[] = {
    {7, 7, "CY_PRE_MR1_SHORT", "减少内部时钟路径延迟", 2, REG2_ENABLE_OPTIONS},
    {5, 5, "C_PORT_PAT", "端口测量顺序交替", 2, REG2_ENABLE_OPTIONS},
    {3, 3, "CY_HFCLK_SEL", "CDC时钟源选择", 2, REG5_CLK_SEL_OPTIONS},
    {2, 2, "CY_DIV4_DIS", "四倍时钟周期", 2, REG2_ENABLE_OPTIONS},
    {1, 1, "CY_PRE_LONG", "添加安全延迟", 2, REG2_ENABLE_OPTIONS},
    {0, 0, "C_DC_BALANCE", "差分浮动直流自由", 2, REG2_ENABLE_OPTIONS}
};

// ==================== 寄存器6 (0x06): 测量端口使能配置 ====================
static const PCAP04_BitOption_t REG6_PORT_OPTIONS[] = {
    {0x00, "禁用", "端口禁用"},
    {0x01, "启用", "端口启用"}
};

static const PCAP04_RegBit_t REG6_BITS[] = {
    {5, 5, "PC5_EN", "PC5端口使能", 2, REG6_PORT_OPTIONS},
    {4, 4, "PC4_EN", "PC4端口使能", 2, REG6_PORT_OPTIONS},
    {3, 3, "PC3_EN", "PC3端口使能", 2, REG6_PORT_OPTIONS},
    {2, 2, "PC2_EN", "PC2端口使能", 2, REG6_PORT_OPTIONS},
    {1, 1, "PC1_EN", "PC1端口使能", 2, REG6_PORT_OPTIONS},
    {0, 0, "PC0_EN", "PC0端口使能", 2, REG6_PORT_OPTIONS}
};

/* 组合示例：
 *   0x01 -> 仅 PC0
 *   0x03 -> PC0~PC1 双通道
 *   0x0F -> PC0~PC3 四通道
 *   0x3F -> PC0~PC5 全通道（当前配置）
 */

// ==================== 寄存器7-8 (0x07-0x08): 采样平均配置 (13位) ====================
static const PCAP04_BitOption_t REG7_AVRG_OPTIONS[] = {
    {0x00, "1", "1次采样"},
    {0x20, "32", "32次采样(默认)"},
    {0x100, "256", "256次采样"}
};

static const PCAP04_RegBit_t REG7_BITS[] = {
    {0, 7, "C_AVRG_L", "采样平均数低8位(0-8191)", 3, REG7_AVRG_OPTIONS}
};

/* 推荐平均次数：
 *   0x0001 -> 1 次（最快）
 *   0x0010 -> 16 次（标准）
 *   0x0020 -> 32 次（当前配置）
 *   0x0100 -> 256 次（高精度）
 */

static const PCAP04_RegBit_t REG8_BITS[] = {
    {0, 4, "C_AVRG_H", "采样平均数高5位(0-8191)", 1, REG7_AVRG_OPTIONS}
};

// ==================== 寄存器9-11 (0x09-0x0B): 转换时间配置 (23位) ====================
static const PCAP04_BitOption_t REG9_CONV_OPTIONS[] = {
    {0x00, "0", "最小转换时间"},
    {0x07D0, "2000", "2000(20ms@200kHz,默认)"}
};

static const PCAP04_RegBit_t REG9_BITS[] = {
    {0, 7, "CONV_TIME_L", "转换周期低8位", 2, REG9_CONV_OPTIONS}
};

/* 转换周期示例（假设 OLF=200kHz）：
 *   0x0003E8 -> 10 ms
 *   0x0007D0 -> 20 ms（当前配置）
 *   0x001388 -> 50 ms
 *   0x002710 -> 100 ms
 */

static const PCAP04_RegBit_t REG10_BITS[] = {
    {0, 7, "CONV_TIME_M", "转换周期中8位", 1, REG9_CONV_OPTIONS}
};

static const PCAP04_RegBit_t REG11_BITS[] = {
    {0, 6, "CONV_TIME_H", "转换周期高7位", 1, REG9_CONV_OPTIONS}
};

// ==================== 寄存器12-13 (0x0C-0x0D): 放电时间与触发配置 ====================
static const PCAP04_RegBit_t REG12_BITS[] = {
    {0, 7, "DISCHARGE_TIME_L", "放电时间低8位(0-1023)", 1, REG9_CONV_OPTIONS}
};

static const PCAP04_BitOption_t REG13_TRIG_SEL_OPTIONS[] = {
    {0x00, "连续", "连续模式"},
    {0x01, "读取", "读取触发"},
    {0x02, "定时器", "定时器触发(推荐)"},
    {0x03, "拉伸", "定时器拉伸"},
    {0x05, "引脚", "引脚触发"},
    {0x06, "操作码", "操作码触发"}
};

static const PCAP04_BitOption_t REG13_PIN_OPTIONS[] = {
    {0x00, "PG0", "GPIO端口0"},
    {0x01, "PG1", "GPIO端口1"},
    {0x02, "PG2", "GPIO端口2"},
    {0x03, "PG3", "GPIO端口3"}
};

static const PCAP04_RegBit_t REG13_BITS[] = {
    {6, 7, "C_STARTONPIN", "CDC启动GPIO选择", 4, REG13_PIN_OPTIONS},
    {2, 4, "C_TRIG_SEL", "CDC触发模式", 6, REG13_TRIG_SEL_OPTIONS},
    {0, 1, "DISCHARGE_TIME_H", "放电时间高2位", 1, REG9_CONV_OPTIONS}
};

// ==================== 寄存器14-15 (0x0E-0x0F): 预充电与假测量配置 ====================
static const PCAP04_RegBit_t REG14_BITS[] = {
    {0, 7, "PRECHARGE_TIME_L", "预充电时间低8位(0-1023)", 1, REG9_CONV_OPTIONS}
};

static const PCAP04_BitOption_t REG15_FAKE_OPTIONS[] = {
    {0x00, "0", "无假测量"},
    {0x01, "1", "1次假测量"},
    {0x0F, "15", "15次假测量"}
};

static const PCAP04_RegBit_t REG15_BITS[] = {
    {2, 5, "C_FAKE", "假测量次数(0-15)", 3, REG15_FAKE_OPTIONS},
    {0, 1, "PRECHARGE_TIME_H", "预充电时间高2位", 1, REG9_CONV_OPTIONS}
};

// ==================== 寄存器16-17 (0x10-0x11): 满充电时间与参考电容 ====================
static const PCAP04_RegBit_t REG16_BITS[] = {
    {0, 7, "FULLCHARGE_TIME_L", "满充电时间低8位", 1, REG9_CONV_OPTIONS}
};

static const PCAP04_BitOption_t REG17_CREF_OPTIONS[] = {
    {0x00, "0pF", "最小值"},
    {0x04, "4pF", "约4-5pF(默认)"},
    {0x1F, "31pF", "最大值约31pF"}
};

static const PCAP04_RegBit_t REG17_BITS[] = {
    {2, 6, "C_REF_SEL", "片上参考电容(0-31pF)", 3, REG17_CREF_OPTIONS},
    {0, 1, "FULLCHARGE_TIME_H", "满充电时间高2位", 1, REG9_CONV_OPTIONS}
};

// ==================== 寄存器18 (0x12): 保护驱动配置 ====================
static const PCAP04_BitOption_t REG18_OP_MODE_OPTIONS[] = {
    {0x00, "永久", "运算放大器永久模式"},
    {0x01, "脉冲", "脉冲模式(睡眠)"}
};

static const PCAP04_BitOption_t REG18_OP_EXT_OPTIONS[] = {
    {0x00, "内部", "内部运算放大器"},
    {0x01, "外部", "外部运算放大器"}
};

static const PCAP04_RegBit_t REG18_BITS[] = {
    {7, 7, "C_G_OP_RUN", "保护运算放大器模式", 2, REG18_OP_MODE_OPTIONS},
    {6, 6, "C_G_OP_EXT", "激活外部运算放大器", 2, REG18_OP_EXT_OPTIONS},
    {5, 5, "PC5_G_EN", "PC5保护使能", 2, REG6_PORT_OPTIONS},
    {4, 4, "PC4_G_EN", "PC4保护使能", 2, REG6_PORT_OPTIONS},
    {3, 3, "PC3_G_EN", "PC3保护使能", 2, REG6_PORT_OPTIONS},
    {2, 2, "PC2_G_EN", "PC2保护使能", 2, REG6_PORT_OPTIONS},
    {1, 1, "PC1_G_EN", "PC1保护使能", 2, REG6_PORT_OPTIONS},
    {0, 0, "PC0_G_EN", "PC0保护使能", 2, REG6_PORT_OPTIONS}
};

// ==================== 寄存器19 (0x13): 保护运算放大器配置 ====================
static const PCAP04_BitOption_t REG19_GAIN_OPTIONS[] = {
    {0x00, "×1.00", "增益1.00"},
    {0x01, "×1.01", "增益1.01"},
    {0x02, "×1.02", "增益1.02"},
    {0x03, "×1.03", "增益1.03"}
};

static const PCAP04_BitOption_t REG19_ATTN_OPTIONS[] = {
    {0x00, "0.5aF", "衰减0.5aF"},
    {0x01, "1.0aF", "衰减1.0aF"},
    {0x02, "1.5aF", "衰减1.5aF"},
    {0x03, "2.0aF", "衰减2.0aF"}
};

static const PCAP04_RegBit_t REG19_BITS[] = {
    {6, 7, "C_G_OP_VU", "保护运算放大器增益", 4, REG19_GAIN_OPTIONS},
    {4, 5, "C_G_OP_ATTN", "保护运算放大器衰减", 4, REG19_ATTN_OPTIONS},
    {0, 3, "C_G_TIME", "保护切换时间(×500ns)", 1, REG9_CONV_OPTIONS}
};

// ==================== 寄存器20 (0x14): RDC时间配置 ====================
static const PCAP04_BitOption_t REG20_CY_OPTIONS[] = {
    {0x00, "标准", "标准循环时间"},
    {0x01, "双倍", "双倍循环时间"}
};

static const PCAP04_RegBit_t REG20_BITS[] = {
    {7, 7, "R_CY", "RDC循环时间", 2, REG20_CY_OPTIONS},
    {0, 2, "C_G_OP_TR", "保护OP电流调整", 1, REG9_CONV_OPTIONS}
};

// ==================== 寄存器21-22 (0x15-0x16): RDC预分频与触发配置 ====================
static const PCAP04_RegBit_t REG21_BITS[] = {
    {0, 7, "R_TRIG_PREDIV_L", "RDC预分频低8位", 1, REG9_CONV_OPTIONS}
};

static const PCAP04_BitOption_t REG22_TRIG_OPTIONS[] = {
    {0x00, "关闭", "RDC关闭"},
    {0x01, "定时器", "定时器触发"},
    {0x03, "引脚", "引脚触发"},
    {0x05, "CDC异步", "CDC异步(推荐)"},
    {0x06, "CDC同步", "CDC同步"}
};

static const PCAP04_BitOption_t REG22_AVRG_OPTIONS[] = {
    {0x00, "不平均", "不平均"},
    {0x01, "4次", "4次平均"},
    {0x02, "8次", "8次平均"},
    {0x03, "16次", "16次平均"}
};

static const PCAP04_RegBit_t REG22_BITS[] = {
    {4, 6, "R_TRIG_SEL", "RDC触发源选择", 5, REG22_TRIG_OPTIONS},
    {2, 3, "R_AVRG", "RDC平均配置", 4, REG22_AVRG_OPTIONS},
    {0, 1, "R_TRIG_PREDIV_H", "RDC预分频高2位", 1, REG9_CONV_OPTIONS}
};

/* 常用组合：
 *   0x00 -> RDC 关闭
 *   0x10 -> 定时器触发
 *   0x50 -> CDC 异步触发（当前配置）
 *   0x54/0x58/0x5C -> CDC 异步 + 4/8/16 次平均
 */

// ==================== 寄存器23 (0x17): RDC端口与参考配置 ====================
static const PCAP04_BitOption_t REG23_FAKE_OPTIONS[] = {
    {0x00, "2次", "2次假测量"},
    {0x01, "8次", "8次假测量"}
};

static const PCAP04_RegBit_t REG23_BITS[] = {
    {7, 7, "PT1_EN", "激活端口PT1", 2, REG6_PORT_OPTIONS},
    {6, 6, "PTOREF_EN", "激活端口PTOREF", 2, REG6_PORT_OPTIONS},
    {5, 5, "R_PORT_EN_IMES", "内部铝温度传感器", 2, REG6_PORT_OPTIONS},
    {4, 4, "R_PORT_EN_IREF", "内部参考电阻", 2, REG6_PORT_OPTIONS},
    {2, 2, "R_FAKE", "RDC假测量数量", 2, REG23_FAKE_OPTIONS},
    {0, 1, "R_STARTONPIN", "RDC启动GPIO", 4, REG13_PIN_OPTIONS}
};

// ==================== 寄存器24-26 (0x18-0x1A): TDC配置(固定) ====================
static const PCAP04_RegBit_t REG24_BITS[] = {
    {4, 5, "TDC_CHAN_EN", "TDC通道使能(必须为3)", 1, REG9_CONV_OPTIONS},
    {3, 3, "TDC_ALUPERMOPEN", "TDC铝永久开(必须为0)", 1, REG9_CONV_OPTIONS},
    {2, 2, "TDC_NOISE_DIS", "TDC噪声禁用(必须为0)", 1, REG9_CONV_OPTIONS},
    {0, 1, "TDC_MUPU_SPEED", "TDC速度(必须为3)", 1, REG9_CONV_OPTIONS}
};

static const PCAP04_RegBit_t REG25_BITS[] = {
    {2, 7, "TDC_MUPU_NO", "TDC编号(必须为1)", 1, REG9_CONV_OPTIONS}
};

static const PCAP04_RegBit_t REG26_BITS[] = {
    {2, 7, "TDC_QHA_SEL", "TDC QHA选择(必须为20)", 1, REG9_CONV_OPTIONS},
    {1, 1, "TDC_NOISE_CY_DIS", "TDC噪声周期禁用(必须为0)", 1, REG9_CONV_OPTIONS}
};

// ==================== 寄存器27 (0x1B): DSP配置 ====================
static const PCAP04_BitOption_t REG27_MOFLO_OPTIONS[] = {
    {0x00, "关闭", "单稳态关闭"},
    {0x03, "开启", "单稳态开启"}
};

static const PCAP04_BitOption_t REG27_SPEED_OPTIONS[] = {
    {0x00, "最快", "DSP最快速度"},
    {0x01, "快", "DSP快速"},
    {0x02, "慢", "DSP慢速(推荐)"},
    {0x03, "最慢", "DSP最慢速度"}
};

static const PCAP04_RegBit_t REG27_BITS[] = {
    {6, 7, "DSP_MOFLO_EN", "GPIO防抖滤波器", 2, REG27_MOFLO_OPTIONS},
    {2, 3, "DSP_SPEED", "DSP速度", 4, REG27_SPEED_OPTIONS},
    {1, 1, "PG1xPG3", "PG1/PG3切换", 2, REG6_PORT_OPTIONS},
    {0, 0, "PG0xPG2", "PG0/PG2切换", 2, REG6_PORT_OPTIONS}
};

// ==================== 寄存器28 (0x1C): 看门狗配置 ====================
static const PCAP04_BitOption_t REG28_WD_OPTIONS[] = {
    {0x00, "启用", "看门狗启用(推荐)"},
    {0x5A, "禁用", "看门狗禁用"}
};

static const PCAP04_RegBit_t REG28_BITS[] = {
    {0, 7, "WD_DIS", "看门狗禁用(0x5A=禁用)", 2, REG28_WD_OPTIONS}
};

// ==================== 寄存器29 (0x1D): DSP触发配置 ====================
static const PCAP04_RegBit_t REG29_BITS[] = {
    {4, 7, "DSP_STARTONPIN", "DSP启动引脚掩码(PG0~3)", 1, REG9_CONV_OPTIONS},
    {0, 3, "DSP_FF_IN", "DSP触发器输入掩码", 1, REG9_CONV_OPTIONS}
};

// ==================== 寄存器30 (0x1E): 中断配置 ====================
static const PCAP04_RegBit_t REG30_BITS[] = {
    {7, 7, "PG5_INTN_EN", "INTN信号路由到PG5", 2, REG6_PORT_OPTIONS},
    {6, 6, "PG4_INTN_EN", "INTN信号路由到PG4", 2, REG6_PORT_OPTIONS},
    {2, 2, "DSP_TRIG_TIMER", "定时器触发DSP", 2, REG6_PORT_OPTIONS},
    {1, 1, "DSP_TRIG_RDC", "RDC结束触发DSP(推荐)", 2, REG6_PORT_OPTIONS},
    {0, 0, "DSP_TRIG_CDC", "CDC结束触发DSP", 2, REG6_PORT_OPTIONS}
};

// ==================== 寄存器31-32 (0x1F-0x20): 脉冲接口配置 ====================
static const PCAP04_BitOption_t REG31_RES_OPTIONS[] = {
    {0x00, "10位", "10位分辨率"},
    {0x01, "12位", "12位分辨率"},
    {0x02, "14位", "14位分辨率"},
    {0x03, "16位", "16位分辨率"}
};

static const PCAP04_BitOption_t REG31_MODE_OPTIONS[] = {
    {0x00, "PWM", "PWM模式"},
    {0x01, "PDM", "PDM模式"}
};

static const PCAP04_BitOption_t REG31_CLK_OPTIONS[] = {
    {0x00, "关闭", "时钟关闭"},
    {0x01, "OLF/1", "OLF不分频"},
    {0x02, "OLF/2", "OLF 2分频"},
    {0x03, "OLF/4", "OLF 4分频"},
    {0x04, "OX/1", "OX不分频"},
    {0x05, "OX/2", "OX 2分频"},
    {0x06, "OX/4", "OX 4分频"}
};

static const PCAP04_RegBit_t REG31_BITS[] = {
    {7, 7, "PI1_TOGGLE_EN", "PI1切换触发器", 2, REG6_PORT_OPTIONS},
    {6, 6, "PIO_TOGGLE_EN", "PI0切换触发器", 2, REG6_PORT_OPTIONS},
    {4, 5, "PIO_RES", "脉冲接口0分辨率", 4, REG31_RES_OPTIONS},
    {3, 3, "PIO_PDM_SEL", "PI0 PWM/PDM选择", 2, REG31_MODE_OPTIONS},
    {0, 2, "PIO_CLK_SEL", "脉冲接口0时钟", 7, REG31_CLK_OPTIONS}
};

static const PCAP04_RegBit_t REG32_BITS[] = {
    {4, 5, "PI1_RES", "脉冲接口1分辨率", 4, REG31_RES_OPTIONS},
    {3, 3, "PI1_PDM_SEL", "PI1 PWM/PDM选择", 2, REG31_MODE_OPTIONS},
    {0, 2, "PI1_CLK_SEL", "脉冲接口1时钟", 7, REG31_CLK_OPTIONS}
};

// ==================== 寄存器33 (0x21): GPIO配置 ====================
static const PCAP04_BitOption_t REG33_DIR_OPTIONS[] = {
    {0x00, "输出", "GPIO输出模式"},
    {0x01, "输入", "GPIO输入模式"}
};

static const PCAP04_RegBit_t REG33_BITS[] = {
    {7, 7, "PG3_DIR", "PG3方向", 2, REG33_DIR_OPTIONS},
    {6, 6, "PG2_DIR", "PG2方向", 2, REG33_DIR_OPTIONS},
    {5, 5, "PG1_DIR", "PG1方向", 2, REG33_DIR_OPTIONS},
    {4, 4, "PG0_DIR", "PG0方向", 2, REG33_DIR_OPTIONS},
    {3, 3, "PG3_PU", "PG3上拉", 2, REG6_PORT_OPTIONS},
    {2, 2, "PG2_PU", "PG2上拉", 2, REG6_PORT_OPTIONS},
    {1, 1, "PG1_PU", "PG1上拉", 2, REG6_PORT_OPTIONS},
    {0, 0, "PG0_PU", "PG0上拉", 2, REG6_PORT_OPTIONS}
};

// ==================== 寄存器34 (0x22): 带隙与自动启动配置 ====================
static const PCAP04_BitOption_t REG34_BG_MODE_OPTIONS[] = {
    {0x00, "脉冲", "带隙脉冲模式"},
    {0x01, "永久", "带隙永久启用(+20μA)"}
};

static const PCAP04_RegBit_t REG34_BITS[] = {
    {7, 7, "INT_TRIG_BG", "读取结束触发带隙", 2, REG6_PORT_OPTIONS},
    {6, 6, "DSP_TRIG_BG", "DSP触发带隙", 2, REG6_PORT_OPTIONS},
    {5, 5, "BG_PERM", "带隙永久启用", 2, REG34_BG_MODE_OPTIONS},
    {4, 4, "AUTOSTART", "上电后自动触发CDC", 2, REG6_PORT_OPTIONS}
};

// ==================== 寄存器35 (0x23): 增益缩放因子 ====================
static const PCAP04_BitOption_t REG35_GAIN_OPTIONS[] = {
    {0x00, "1.00", "增益1.00"},
    {0x40, "1.25", "增益1.25(推荐)"},
    {0x80, "1.50", "增益1.50"},
    {0xFF, "1.996", "增益1.996(最大)"}
};

static const PCAP04_RegBit_t REG35_BITS[] = {
    {0, 7, "CDC_GAIN_CORR", "增益校正因子(1+n/256)", 4, REG35_GAIN_OPTIONS}
};

// ==================== 寄存器38 (0x26): 带隙时间配置 ====================
static const PCAP04_RegBit_t REG38_BITS[] = {
    {0, 7, "BG_TIME", "带隙时间(0=推荐)", 1, REG9_CONV_OPTIONS}
};

// ==================== 寄存器39 (0x27): 脉冲选择配置 ====================
static const PCAP04_BitOption_t REG39_SEL_OPTIONS[] = {
    {0x00, "Res0", "C0/Cref"},
    {0x01, "Res1", "C1/Cref"},
    {0x02, "Res2", "C2/Cref"},
    {0x03, "Res3", "C3/Cref"},
    {0x04, "Res4", "C4/Cref"},
    {0x05, "Res5", "C5/Cref"},
    {0x06, "Res6", "PT1/Ref"},
    {0x07, "Res7", "Alu/Ref"}
};

static const PCAP04_RegBit_t REG39_BITS[] = {
    {4, 7, "PULSE_SEL1", "脉冲接口1源选择", 8, REG39_SEL_OPTIONS},
    {0, 3, "PULSE_SEL0", "脉冲接口0源选择", 8, REG39_SEL_OPTIONS}
};

// ==================== 寄存器40-41 (0x28-0x29): 线性化固件配置 ====================
static const PCAP04_RegBit_t REG40_BITS[] = {
    {0, 7, "C_SENSE_SEL", "电容比率选择(线性化)", 1, REG9_CONV_OPTIONS}
};

static const PCAP04_RegBit_t REG41_BITS[] = {
    {0, 7, "R_SENSE_SEL", "电阻比率选择(线性化)", 1, REG9_CONV_OPTIONS}
};

// ==================== 寄存器42 (0x2A): 固件配置与报警选择 ====================
static const PCAP04_BitOption_t REG42_ALARM_OPTIONS[] = {
    {0x00, "Z", "阻抗Z选择"},
    {0x01, "Theta", "相位Theta选择"}
};

static const PCAP04_RegBit_t REG42_BITS[] = {
    {6, 6, "ALARM1_SELECT", "报警1选择", 2, REG42_ALARM_OPTIONS},
    {4, 4, "ALARM0_SELECT", "报警0选择", 2, REG42_ALARM_OPTIONS},
    {3, 3, "EN_ASYNC_READ", "启用异步读取", 2, REG6_PORT_OPTIONS},
    {1, 1, "R_MEDIAN_EN", "R中值滤波器", 2, REG6_PORT_OPTIONS},
    {0, 0, "C_MEDIAN_EN", "C中值滤波器", 2, REG6_PORT_OPTIONS}
};

// ==================== 寄存器47 (0x2F): 运行位配置 ====================
static const PCAP04_BitOption_t REG47_RUN_OPTIONS[] = {
    {0x00, "停止", "系统空闲并受保护"},
    {0x01, "运行", "系统可以运行"}
};

static const PCAP04_RegBit_t REG47_BITS[] = {
    {0, 0, "RUNBIT", "前端和DSP开关", 2, REG47_RUN_OPTIONS}
};

// ==================== 寄存器48 (0x30): 内存锁配置 ====================
static const PCAP04_RegBit_t REG48_BITS[] = {
    {3, 3, "MEM_LOCK_960", "锁定960-1007及1022-1023", 2, REG6_PORT_OPTIONS},
    {2, 2, "MEM_LOCK_832", "锁定832-959", 2, REG6_PORT_OPTIONS},
    {1, 1, "MEM_LOCK_704", "锁定704-831", 2, REG6_PORT_OPTIONS},
    {0, 0, "MEM_LOCK_0", "锁定0-703", 2, REG6_PORT_OPTIONS}
};

// ==================== 寄存器49-50 (0x31-0x32): 序列号 ====================
static const PCAP04_RegBit_t REG49_BITS[] = {
    {0, 7, "SERIAL_NUMBER_L", "序列号低字节", 1, REG9_CONV_OPTIONS}
};

static const PCAP04_RegBit_t REG50_BITS[] = {
    {0, 7, "SERIAL_NUMBER_H", "序列号高字节", 1, REG9_CONV_OPTIONS}
};

// ==================== 寄存器54 (0x36): 内存控制 ====================
static const PCAP04_BitOption_t REG54_CTRL_OPTIONS[] = {
    {0x00, "无操作", "无操作"},
    {0x2D, "存储", "存储到NVRAM"},
    {0x59, "召回", "从NVRAM召回"},
    {0xB8, "擦除", "擦除NVRAM"}
};

static const PCAP04_RegBit_t REG54_BITS[] = {
    {0, 7, "MEM_CTRL", "内存控制", 4, REG54_CTRL_OPTIONS}
};

// ==================== 寄存器62-63 (0x3E-0x3F): 电荷泵配置(设备特定,不可修改) ====================
static const PCAP04_RegBit_t REG62_BITS[] = {
    {0, 7, "CHARGE_PUMP_L", "电荷泵低字节(不可修改)", 1, REG9_CONV_OPTIONS}
};

static const PCAP04_RegBit_t REG63_BITS[] = {
    {0, 7, "CHARGE_PUMP_H", "电荷泵高字节(不可修改)", 1, REG9_CONV_OPTIONS}
};

// ==================== 完整寄存器表定义 ====================
static const PCAP04_Register_t PCAP04_REGISTERS[] = {
    {0x00, "IIC_LF_CONFIG", "IIC地址与低频时钟配置", 3, REG0_BITS, 0x1D},//REG0: 0x1D: 0001 1101
    {0x01, "OX_CONFIG", "外部晶振时钟OX配置", 3, REG1_BITS, 0x00},//REG1: 0x00: 0000 0000
    {0x02, "DISCHARGE_RES", "端口放电电阻配置", 4, REG2_BITS, 0x58},//REG2: 0x58: 0101 1000
    {0x03, "CHARGE_CONFIG", "端口充放电电阻配置", 5, REG3_BITS, 0x10},//REG3: 0x10: 0001 0000     
    {0x04, "CAP_CONNECT", "端口电容连接方式", 5, REG4_BITS, 0x10},//REG4: 0x10: 0001 0000
    {0x05, "CDC_CLOCK", "CDC时钟与端口模式", 6, REG5_BITS, 0x00},//REG5: 0x00: 0000 0000
    {0x06, "PORT_ENABLE", "测量端口使能配置", 6, REG6_BITS, 0x3F},//REG6: 0x3F: 0011 1111
    {0x07, "C_AVRG_L", "采样平均低字节", 1, REG7_BITS, 0x20},//REG7: 0x20: 0010 0000
    {0x08, "C_AVRG_H", "采样平均高字节", 1, REG8_BITS, 0x00},//REG8: 0x00: 0000 0000
    {0x09, "CONV_TIME_L", "转换时间低字节", 1, REG9_BITS, 0xD0},//REG9: 0xD0: 1101 0000
    {0x0A, "CONV_TIME_M", "转换时间中字节", 1, REG10_BITS, 0x07},//REG10: 0x07: 0000 0111
    {0x0B, "CONV_TIME_H", "转换时间高字节", 1, REG11_BITS, 0x00},//REG11: 0x00: 0000 0000
    {0x0C, "DISCHARGE_L", "放电时间低字节", 1, REG12_BITS, 0x00},//REG12: 0x00: 0000 0000
    {0x0D, "TRIG_CONFIG", "触发与放电配置", 3, REG13_BITS, 0x08},//REG13: 0x08: 0000 1000
    {0x0E, "PRECHARGE_L", "预充电时间低字节", 1, REG14_BITS, 0xFF},//REG14: 0xFF: 1111 1111
    {0x0F, "PRECHARGE_CFG", "预充电与假测量", 2, REG15_BITS, 0x03},//REG15: 0x03: 0000 0011
    {0x10, "FULLCHARGE_L", "满充电时间低字节", 1, REG16_BITS, 0x00},//REG16: 0x00: 0000 0000
    {0x11, "CREF_CONFIG", "参考电容配置", 2, REG17_BITS, 0x24},//REG17: 0x24: 0010 0100
    {0x12, "GUARD_ENABLE", "保护驱动配置", 8, REG18_BITS, 0x00},//REG18: 0x00: 0000 0000
    {0x13, "GUARD_OP", "保护运算放大器", 3, REG19_BITS, 0x00},//REG19: 0x00: 0000 0000
    {0x14, "RDC_TIME", "RDC时间配置", 2, REG20_BITS, 0x00},//REG20: 0x00: 0000 0000
    {0x15, "R_PREDIV_L", "RDC预分频低字节", 1, REG21_BITS, 0x01},//REG21: 0x01: 0000 0001
    {0x16, "RDC_TRIG", "RDC触发配置", 3, REG22_BITS, 0x50},//REG22: 0x50: 0101 0000
    {0x17, "RDC_PORT", "RDC端口配置", 6, REG23_BITS, 0x30},//REG23: 0x30: 0011 0000
    {0x18, "TDC_CONFIG0", "TDC配置0(固定)", 4, REG24_BITS, 0x73},//REG24: 0x73: 0111 0011
    {0x19, "TDC_CONFIG1", "TDC配置1(固定)", 1, REG25_BITS, 0x04},//REG25: 0x04: 0000 0100
    {0x1A, "TDC_CONFIG2", "TDC配置2(固定)", 2, REG26_BITS, 0x50},//REG26: 0x50: 0101 0000
    {0x1B, "DSP_CONFIG", "DSP配置", 4, REG27_BITS, 0x08},//REG27: 0x08: 0000 1000
    {0x1C, "WATCHDOG", "看门狗配置", 1, REG28_BITS, 0x5A},//REG28: 0x5A: 0101 1010
    {0x1D, "DSP_TRIGGER", "DSP触发配置", 2, REG29_BITS, 0x00},//REG29: 0x00: 0000 0000
    {0x1E, "INT_CONFIG", "中断配置", 5, REG30_BITS, 0x82},//0x82: 1000 0010
    {0x1F, "PULSE_IF0", "脉冲接口0配置", 5, REG31_BITS, 0x08},//REG31: 0x08: 0000 1000
    {0x20, "PULSE_IF1", "脉冲接口1配置", 3, REG32_BITS, 0x08},//REG32: 0x08: 0000 1000
    {0x21, "GPIO_CONFIG", "GPIO配置", 8, REG33_BITS, 0x00},//REG33: 0x00: 0000 0000
    {0x22, "BANDGAP", "带隙与自动启动", 4, REG34_BITS, 0x47},//0x47: 0100 0111
    {0x23, "GAIN_CORR", "增益缩放因子", 1, REG35_BITS, 0x40},//REG35: 0x40: 0100 0000
    {0x24, "RESERVED36", "保留寄存器36", 0, NULL, 0x00},//REG36: 0x00: 0000 0000
    {0x25, "RESERVED37", "保留寄存器37", 0, NULL, 0x00},//REG37: 0x00: 0000 0000
    {0x26, "BG_TIME", "带隙时间配置", 1, REG38_BITS, 0x00},//REG38: 0x00: 0000 0000
    {0x27, "PULSE_SEL", "脉冲选择配置", 2, REG39_BITS, 0x71},//REG39: 0x71: 0111 0001
    {0x28, "C_SENSE_SEL", "电容感测选择", 1, REG40_BITS, 0x00},//REG40: 0x00: 0000 0000
    {0x29, "R_SENSE_SEL", "电阻感测选择", 1, REG41_BITS, 0x00},//REG41: 0x00: 0000 0000
    {0x2A, "FW_CONFIG", "固件配置", 5, REG42_BITS, 0x08},//REG42: 0x08: 0000 1000
    {0x2B, "RESERVED43", "保留寄存器43", 0, NULL, 0x00},//REG43: 0x00: 0000 0000
    {0x2C, "RESERVED44", "保留寄存器44", 0, NULL, 0x00},//REG44: 0x00: 0000 0000
    {0x2D, "RESERVED45", "保留寄存器45", 0, NULL, 0x00},//REG45: 0x00: 0000 0000
    {0x2E, "RESERVED46", "保留寄存器46", 0, NULL, 0x00},//REG46: 0x00: 0000 0000
    {0x2F, "RUNBIT", "运行位配置", 1, REG47_BITS, 0x01},//REG47: 0x01: 0000 0001
    {0x30, "MEM_LOCK", "内存锁配置", 4, REG48_BITS, 0x00},//REG48: 0x00: 0000 0000
    {0x31, "SERIAL_L", "序列号低字节", 1, REG49_BITS, 0x00},//REG49: 0x00: 0000 0000
    {0x32, "SERIAL_H", "序列号高字节", 1, REG50_BITS, 0x00},//REG50: 0x00: 0000 0000
    {0x33, "RESERVED51", "保留寄存器51", 0, NULL, 0x00},//REG51: 0x00: 0000 0000
    {0x34, "RESERVED52", "保留寄存器52", 0, NULL, 0x00},//REG52: 0x00: 0000 0000
    {0x35, "RESERVED53", "保留寄存器53", 0, NULL, 0x00},//REG53: 0x00: 0000 0000
    {0x36, "MEM_CTRL", "内存控制", 1, REG54_BITS, 0x00},//REG54: 0x00: 0000 0000
    {0x37, "RESERVED55", "保留寄存器55", 0, NULL, 0x00},//REG55: 0x00: 0000 0000
    {0x38, "RESERVED56", "保留寄存器56", 0, NULL, 0x00},//REG56: 0x00: 0000 0000
    {0x39, "RESERVED57", "保留寄存器57", 0, NULL, 0x00},//REG57: 0x00: 0000 0000
    {0x3A, "RESERVED58", "保留寄存器58", 0, NULL, 0x00},//REG58: 0x00: 0000 0000
    {0x3B, "RESERVED59", "保留寄存器59", 0, NULL, 0x00},//REG59: 0x00: 0000 0000
    {0x3C, "RESERVED60", "保留寄存器60", 0, NULL, 0x00},//REG60: 0x00: 0000 0000
    {0x3D, "RESERVED61", "保留寄存器61", 0, NULL, 0x00},//REG61: 0x00: 0000 0000
    {0x3E, "CHARGE_PUMP_L", "电荷泵低字节(禁止修改)", 1, REG62_BITS, 0x00},//REG62: 0x00: 0000 0000
    {0x3F, "CHARGE_PUMP_H", "电荷泵高字节(禁止修改)", 1, REG63_BITS, 0x00}//REG63: 0x00: 0000 0000
};

// 全局寄存器表
const PCAP04_RegisterTable_t PCAP04_REGISTER_TABLE = {
    sizeof(PCAP04_REGISTERS) / sizeof(PCAP04_Register_t),
    PCAP04_REGISTERS
};
