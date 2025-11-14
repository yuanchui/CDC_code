#include "pcap04_register.h"
#include <stddef.h>
/* 
*  PCAP04  pcap04_register.c
*  USB 
* 
*/
// ==================== 0 (0x00): IIC ====================

static const PCAP04_BitOption_t REG0_I2C_A_OPTIONS[] = {
   {0x00, "ADDR_0", "I2C address complement 0"},
   {0x01, "ADDR_1", "I2C address complement 1"},
   {0x02, "ADDR_2", "I2C address complement 2"},
   {0x03, "ADDR_3", "I2C address complement 3"}
};
static const PCAP04_BitOption_t REG0_OLF_FTUNE_OPTIONS[] = {
   {0x00, "MIN", "Min value 0"},
   {0x07, "TYP", "Typical value 7"},
   {0x0F, "MAX", "Max value 15"}
};
static const PCAP04_BitOption_t REG0_OLF_CTUNE_OPTIONS[] = {
   {0x00, "10KHZ", "10 kHz frequency"},
   {0x01, "50KHZ", "50 kHz frequency"},
   {0x02, "100KHZ", "100 kHz frequency"},
   {0x03, "200KHZ", "200 kHz frequency"}
};
/* 
* OLF_CTUNE  OLF_FTUNE kHz
*   OLF_CTUNE=0, OLF_FTUNE=7 -> 10 kHz
*   OLF_CTUNE=1, OLF_FTUNE=7 -> 50 kHz
*   OLF_CTUNE=2, OLF_FTUNE=4 -> 100 kHz
*   OLF_CTUNE=3, OLF_FTUNE=5~7 -> 200 kHz
* 
*/
static const PCAP04_RegBit_t REG0_BITS[] = {
   {6, 7, "I2C_A", "I2C address complement config", 4, REG0_I2C_A_OPTIONS},
   {2, 5, "OLF_FTUNE", "Low freq clock fine tune (0-15)", 3, REG0_OLF_FTUNE_OPTIONS},
   {0, 1, "OLF_CTUNE", "Low freq clock coarse tune", 4, REG0_OLF_CTUNE_OPTIONS}
};
// ==================== 1 (0x01): OX ====================

static const PCAP04_BitOption_t REG1_OX_DIS_OPTIONS[] = {
   {0x00, "ENABLE", "OX clock enable"},
   {0x01, "DISABLE", "OX clock disable"}
};
static const PCAP04_BitOption_t REG1_OX_DIV4_OPTIONS[] = {
   {0x00, "2MHZ", "No divide, f_ox=2MHz"},
   {0x01, "0.5MHZ", "Divide by 4, f_ox=0.5MHz"}
};
static const PCAP04_BitOption_t REG1_OX_RUN_OPTIONS[] = {
   {0x00, "OFF", "Generator off"},
   {0x01, "PERMANENT", "OX permanent run"},
   {0x02, "DELAY31", "OX delay=31/fOLF"},
   {0x03, "DELAY2", "OX delay=2/fOLF"},
   {0x06, "DELAY1", "OX delay=1/fOLF"}
};
static const PCAP04_RegBit_t REG1_BITS[] = {
   {7, 7, "OX_DIS", "Disable OX clock", 2, REG1_OX_DIS_OPTIONS},
   {5, 5, "OX_DIV4", "OX clock divide by 4", 2, REG1_OX_DIV4_OPTIONS},
   {0, 2, "OX_RUN", "OX generator continuity or delay", 5, REG1_OX_RUN_OPTIONS}
};
// ==================== 2 (0x02):  ====================

static const PCAP04_BitOption_t REG2_RDCHG_INT_SEL_OPTIONS[] = {
   {0x00, "180K", "180 k"},
   {0x01, "90K", "90 k"},
   {0x02, "30K", "30 k"},
   {0x03, "10K", "10 k"}
};

static const PCAP04_BitOption_t REG2_ENABLE_OPTIONS[] = {
   {0x00, "DISABLE", "Disable"},
   {0x01, "ENABLE", "Enable"}
};
static const PCAP04_RegBit_t REG2_BITS[] = {
   {6, 7, "RDCHG_INT_SEL1", "PC4~PC5", 4, REG2_RDCHG_INT_SEL_OPTIONS},
   {4, 5, "RDCHG_INT_SEL0", "PC0~PC3PC6", 4, REG2_RDCHG_INT_SEL_OPTIONS},
   {3, 3, "RDCHG_INT_EN", "", 2, REG2_ENABLE_OPTIONS},
   {1, 1, "RDCHG_EXT_EN", "", 2, REG2_ENABLE_OPTIONS}
};
// ==================== 3 (0x03):  ====================

static const PCAP04_BitOption_t REG3_RCHG_SEL_OPTIONS[] = {
   {0x00, "180K", "180 k"},
   {0x01, "10K", "10 k()"}
};
static const PCAP04_RegBit_t REG3_BITS[] = {
   {6, 6, "AUX_PD_DIS", "PCAUX", 2, REG2_ENABLE_OPTIONS},
   {5, 5, "AUX_CINT", "PCAUX", 2, REG2_ENABLE_OPTIONS},
   {2, 2, "RDCHG_PERM_EN", "", 2, REG2_ENABLE_OPTIONS},
   {1, 1, "RDCHG_EXT_PERM", "PCAUX", 2, REG2_ENABLE_OPTIONS},
   {0, 0, "RCHG_SEL", "", 2, REG3_RCHG_SEL_OPTIONS}
};

// ==================== 4 (0x04):  ====================

static const PCAP04_BitOption_t REG4_MODE_OPTIONS[] = {
   {0x00, "EXTERNAL", "External reference"},
   {0x01, "INTERNAL", "Internal reference"}
};
static const PCAP04_BitOption_t REG4_SENSOR_OPTIONS[] = {
   {0x00, "SINGLE", "Single-ended sensor"},
   {0x01, "DIFF", "Differential sensor"}
};
static const PCAP04_BitOption_t REG4_FLOAT_OPTIONS[] = {
   {0x00, "GROUNDED", "Grounded sensor"},
   {0x01, "FLOATING", "Floating sensor"}
};
static const PCAP04_RegBit_t REG4_BITS[] = {
   {7, 7, "C_REF_INT", "Use on-chip reference cap", 2, REG4_MODE_OPTIONS},
   {5, 5, "C_COMP_EXT", "External parasitic cap compensation", 2, REG2_ENABLE_OPTIONS},
   {4, 4, "C_COMP_INT", "On-chip parasitic cap compensation", 2, REG2_ENABLE_OPTIONS},
   {1, 1, "C_DIFFERENTIAL", "Single or diff sensor", 2, REG4_SENSOR_OPTIONS},
   {0, 0, "C_FLOATING", "Grounded or floating sensor", 2, REG4_FLOAT_OPTIONS}
};

// ==================== 5 (0x05): CDC ====================

static const PCAP04_BitOption_t REG5_CLK_SEL_OPTIONS[] = {
   {0x00, "OLF", "Low freq clock OLF"},
   {0x01, "OHF", "High freq clock OHF"}
};
static const PCAP04_RegBit_t REG5_BITS[] = {
   {7, 7, "CY_PRE_MR1_SHORT", "Reduce internal clock path delay", 2, REG2_ENABLE_OPTIONS},
   {5, 5, "C_PORT_PAT", "Port measurement order alternate", 2, REG2_ENABLE_OPTIONS},
   {3, 3, "CY_HFCLK_SEL", "CDC clock source select", 2, REG5_CLK_SEL_OPTIONS},
   {2, 2, "CY_DIV4_DIS", "Four times clock period", 2, REG2_ENABLE_OPTIONS},
   {1, 1, "CY_PRE_LONG", "Add safety delay", 2, REG2_ENABLE_OPTIONS},
   {0, 0, "C_DC_BALANCE", "Diff floating DC free", 2, REG2_ENABLE_OPTIONS}
};
// ==================== 6 (0x06):  ====================

static const PCAP04_BitOption_t REG6_PORT_OPTIONS[] = {
   {0x00, "DISABLE", "Port disable"},
   {0x01, "ENABLE", "Port enable"}
};
static const PCAP04_RegBit_t REG6_BITS[] = {
   {5, 5, "PC5_EN", "PC5 port enable", 2, REG6_PORT_OPTIONS},
   {4, 4, "PC4_EN", "PC4 port enable", 2, REG6_PORT_OPTIONS},
   {3, 3, "PC3_EN", "PC3 port enable", 2, REG6_PORT_OPTIONS},
   {2, 2, "PC2_EN", "PC2 port enable", 2, REG6_PORT_OPTIONS},
   {1, 1, "PC1_EN", "PC1 port enable", 2, REG6_PORT_OPTIONS},
   {0, 0, "PC0_EN", "PC0 port enable", 2, REG6_PORT_OPTIONS}
};
/* 
*   0x01 ->  PC0
*   0x03 -> PC0~PC1 
*   0x0F -> PC0~PC3 
*   0x3F -> PC0~PC5 
*/
// ==================== 7-8 (0x07-0x08):  (13) ====================

static const PCAP04_BitOption_t REG7_AVRG_OPTIONS[] = {
   {0x00, "1", "1 sample"},
   {0x20, "32", "32 samples (default)"},
   {0x100, "256", "256 samples"}
};
static const PCAP04_RegBit_t REG7_BITS[] = {
   {0, 7, "C_AVRG_L", "Average count low 8 bits (0-8191)", 3, REG7_AVRG_OPTIONS}
};
/* 
*   0x0001 -> 1 
*   0x0010 -> 16 
*   0x0020 -> 32 
*   0x0100 -> 256 
*/
static const PCAP04_RegBit_t REG8_BITS[] = {
   {0, 4, "C_AVRG_H", "5(0-8191)", 1, REG7_AVRG_OPTIONS}
};
// ==================== 9-11 (0x09-0x0B):  (23) ====================

static const PCAP04_BitOption_t REG9_CONV_OPTIONS[] = {
   {0x00, "0", ""},
   {0x07D0, "2000", "2000(20ms@200kHz,)"}
};
static const PCAP04_RegBit_t REG9_BITS[] = {
   {0, 7, "CONV_TIME_L", "8", 2, REG9_CONV_OPTIONS}
};
/*  OLF=200kHz
*   0x0003E8 -> 10 ms
*   0x0007D0 -> 20 ms
*   0x001388 -> 50 ms
*   0x002710 -> 100 ms
*/
static const PCAP04_RegBit_t REG10_BITS[] = {
   {0, 7, "CONV_TIME_M", "8", 1, REG9_CONV_OPTIONS}
};
static const PCAP04_RegBit_t REG11_BITS[] = {
   {0, 6, "CONV_TIME_H", "7", 1, REG9_CONV_OPTIONS}
};
// ==================== 12-13 (0x0C-0x0D):  ====================

static const PCAP04_RegBit_t REG12_BITS[] = {
   {0, 7, "DISCHARGE_TIME_L", "8(0-1023)", 1, REG9_CONV_OPTIONS}
};
static const PCAP04_BitOption_t REG13_TRIG_SEL_OPTIONS[] = {
   {0x00, "", ""},
   {0x01, "", ""},
   {0x02, "", "()"},
   {0x03, "", ""},
   {0x05, "", ""},
   {0x06, "", ""}
};
static const PCAP04_BitOption_t REG13_PIN_OPTIONS[] = {
   {0x00, "PG0", "GPIO0"},
   {0x01, "PG1", "GPIO1"},
   {0x02, "PG2", "GPIO2"},
   {0x03, "PG3", "GPIO3"}
};
static const PCAP04_RegBit_t REG13_BITS[] = {
   {6, 7, "C_STARTONPIN", "CDCGPIO", 4, REG13_PIN_OPTIONS},
   {2, 4, "C_TRIG_SEL", "CDC", 6, REG13_TRIG_SEL_OPTIONS},
   {0, 1, "DISCHARGE_TIME_H", "2", 1, REG9_CONV_OPTIONS}
};
// ==================== 14-15 (0x0E-0x0F):  ====================

static const PCAP04_RegBit_t REG14_BITS[] = {
   {0, 7, "PRECHARGE_TIME_L", "8(0-1023)", 1, REG9_CONV_OPTIONS}
};
static const PCAP04_BitOption_t REG15_FAKE_OPTIONS[] = {
   {0x00, "0", ""},
   {0x01, "1", "1"},
   {0x0F, "15", "15"}
};
static const PCAP04_RegBit_t REG15_BITS[] = {
   {2, 5, "C_FAKE", "(0-15)", 3, REG15_FAKE_OPTIONS},
   {0, 1, "PRECHARGE_TIME_H", "2", 1, REG9_CONV_OPTIONS}
};
// ==================== 16-17 (0x10-0x11):  ====================

static const PCAP04_RegBit_t REG16_BITS[] = {
   {0, 7, "FULLCHARGE_TIME_L", "8", 1, REG9_CONV_OPTIONS}
};
static const PCAP04_BitOption_t REG17_CREF_OPTIONS[] = {
   {0x00, "0pF", ""},
   {0x04, "4pF", "4-5pF()"},
   {0x1F, "31pF", "31pF"}
};
static const PCAP04_RegBit_t REG17_BITS[] = {
   {2, 6, "C_REF_SEL", "(0-31pF)", 3, REG17_CREF_OPTIONS},
   {0, 1, "FULLCHARGE_TIME_H", "2", 1, REG9_CONV_OPTIONS}
};
// ==================== 18 (0x12):  ====================

static const PCAP04_BitOption_t REG18_OP_MODE_OPTIONS[] = {
   {0x00, "", ""},
   {0x01, "", "()"}
};
static const PCAP04_BitOption_t REG18_OP_EXT_OPTIONS[] = {
   {0x00, "", ""},
   {0x01, "", ""}
};
static const PCAP04_RegBit_t REG18_BITS[] = {
   {7, 7, "C_G_OP_RUN", "", 2, REG18_OP_MODE_OPTIONS},
   {6, 6, "C_G_OP_EXT", "", 2, REG18_OP_EXT_OPTIONS},
   {5, 5, "PC5_G_EN", "PC5", 2, REG6_PORT_OPTIONS},
   {4, 4, "PC4_G_EN", "PC4", 2, REG6_PORT_OPTIONS},
   {3, 3, "PC3_G_EN", "PC3", 2, REG6_PORT_OPTIONS},
   {2, 2, "PC2_G_EN", "PC2", 2, REG6_PORT_OPTIONS},
   {1, 1, "PC1_G_EN", "PC1", 2, REG6_PORT_OPTIONS},
   {0, 0, "PC0_G_EN", "PC0", 2, REG6_PORT_OPTIONS}
};
// ==================== 19 (0x13):  ====================

static const PCAP04_BitOption_t REG19_GAIN_OPTIONS[] = {
   {0x00, "1.00", "1.00"},
   {0x01, "1.01", "1.01"},
   {0x02, "1.02", "1.02"},
   {0x03, "1.03", "1.03"}
};
static const PCAP04_BitOption_t REG19_ATTN_OPTIONS[] = {
   {0x00, "0.5aF", "0.5aF"},
   {0x01, "1.0aF", "1.0aF"},
   {0x02, "1.5aF", "1.5aF"},
   {0x03, "2.0aF", "2.0aF"}
};
static const PCAP04_RegBit_t REG19_BITS[] = {
   {6, 7, "C_G_OP_VU", "", 4, REG19_GAIN_OPTIONS},
   {4, 5, "C_G_OP_ATTN", "", 4, REG19_ATTN_OPTIONS},
   {0, 3, "C_G_TIME", "(500ns)", 1, REG9_CONV_OPTIONS}
};
// ==================== 20 (0x14): RDC ====================

static const PCAP04_BitOption_t REG20_CY_OPTIONS[] = {
   {0x00, "", ""},
   {0x01, "", ""}
};
static const PCAP04_RegBit_t REG20_BITS[] = {
   {7, 7, "R_CY", "RDC", 2, REG20_CY_OPTIONS},
   {0, 2, "C_G_OP_TR", "OP", 1, REG9_CONV_OPTIONS}
};
// ==================== 21-22 (0x15-0x16): RDC ====================

static const PCAP04_RegBit_t REG21_BITS[] = {
   {0, 7, "R_TRIG_PREDIV_L", "RDC8", 1, REG9_CONV_OPTIONS}
};
static const PCAP04_BitOption_t REG22_TRIG_OPTIONS[] = {
   {0x00, "", "RDC"},
   {0x01, "", ""},
   {0x03, "", ""},
   {0x05, "CDC", "CDC()"},
   {0x06, "CDC", "CDC"}
};
static const PCAP04_BitOption_t REG22_AVRG_OPTIONS[] = {
   {0x00, "", ""},
   {0x01, "4", "4"},
   {0x02, "8", "8"},
   {0x03, "16", "16"}
};
static const PCAP04_RegBit_t REG22_BITS[] = {
   {4, 6, "R_TRIG_SEL", "RDC", 5, REG22_TRIG_OPTIONS},
   {2, 3, "R_AVRG", "RDC", 4, REG22_AVRG_OPTIONS},
   {0, 1, "R_TRIG_PREDIV_H", "RDC2", 1, REG9_CONV_OPTIONS}
};
/* 
*   0x00 -> RDC 
*   0x10 -> 
*   0x50 -> CDC 
*   0x54/0x58/0x5C -> CDC  + 4/8/16 
*/
// ==================== 23 (0x17): RDC ====================

static const PCAP04_BitOption_t REG23_FAKE_OPTIONS[] = {
   {0x00, "2", "2"},
   {0x01, "8", "8"}
};
static const PCAP04_RegBit_t REG23_BITS[] = {
   {7, 7, "PT1_EN", "PT1", 2, REG6_PORT_OPTIONS},
   {6, 6, "PTOREF_EN", "PTOREF", 2, REG6_PORT_OPTIONS},
   {5, 5, "R_PORT_EN_IMES", "", 2, REG6_PORT_OPTIONS},
   {4, 4, "R_PORT_EN_IREF", "", 2, REG6_PORT_OPTIONS},
   {2, 2, "R_FAKE", "RDC", 2, REG23_FAKE_OPTIONS},
   {0, 1, "R_STARTONPIN", "RDCGPIO", 4, REG13_PIN_OPTIONS}
};
// ==================== 24-26 (0x18-0x1A): TDC() ====================

static const PCAP04_RegBit_t REG24_BITS[] = {
   {4, 5, "TDC_CHAN_EN", "TDC(3)", 1, REG9_CONV_OPTIONS},
   {3, 3, "TDC_ALUPERMOPEN", "TDC(0)", 1, REG9_CONV_OPTIONS},
   {2, 2, "TDC_NOISE_DIS", "TDC(0)", 1, REG9_CONV_OPTIONS},
   {0, 1, "TDC_MUPU_SPEED", "TDC(3)", 1, REG9_CONV_OPTIONS}
};
static const PCAP04_RegBit_t REG25_BITS[] = {
   {2, 7, "TDC_MUPU_NO", "TDC(1)", 1, REG9_CONV_OPTIONS}
};
static const PCAP04_RegBit_t REG26_BITS[] = {
   {2, 7, "TDC_QHA_SEL", "TDC QHA(20)", 1, REG9_CONV_OPTIONS},
   {1, 1, "TDC_NOISE_CY_DIS", "TDC(0)", 1, REG9_CONV_OPTIONS}
};
// ==================== 27 (0x1B): DSP ====================

static const PCAP04_BitOption_t REG27_MOFLO_OPTIONS[] = {
   {0x00, "", ""},
   {0x03, "", ""}
};
static const PCAP04_BitOption_t REG27_SPEED_OPTIONS[] = {
   {0x00, "", "DSP"},
   {0x01, "", "DSP"},
   {0x02, "", "DSP()"},
   {0x03, "", "DSP"}
};
static const PCAP04_RegBit_t REG27_BITS[] = {
   {6, 7, "DSP_MOFLO_EN", "GPIO", 2, REG27_MOFLO_OPTIONS},
   {2, 3, "DSP_SPEED", "DSP", 4, REG27_SPEED_OPTIONS},
   {1, 1, "PG1xPG3", "PG1/PG3", 2, REG6_PORT_OPTIONS},
   {0, 0, "PG0xPG2", "PG0/PG2", 2, REG6_PORT_OPTIONS}
};
// ==================== 28 (0x1C):  ====================

static const PCAP04_BitOption_t REG28_WD_OPTIONS[] = {
   {0x00, "", "()"},
   {0x5A, "", ""}
};
static const PCAP04_RegBit_t REG28_BITS[] = {
   {0, 7, "WD_DIS", "(0x5A=)", 2, REG28_WD_OPTIONS}
};
// ==================== 29 (0x1D): DSP ====================

static const PCAP04_RegBit_t REG29_BITS[] = {
   {4, 7, "DSP_STARTONPIN", "DSP(PG0~3)", 1, REG9_CONV_OPTIONS},
   {0, 3, "DSP_FF_IN", "DSP", 1, REG9_CONV_OPTIONS}
};
// ==================== 30 (0x1E):  ====================

static const PCAP04_RegBit_t REG30_BITS[] = {
   {7, 7, "PG5_INTN_EN", "INTNPG5", 2, REG6_PORT_OPTIONS},
   {6, 6, "PG4_INTN_EN", "INTNPG4", 2, REG6_PORT_OPTIONS},
   {2, 2, "DSP_TRIG_TIMER", "DSP", 2, REG6_PORT_OPTIONS},
   {1, 1, "DSP_TRIG_RDC", "RDCDSP()", 2, REG6_PORT_OPTIONS},
   {0, 0, "DSP_TRIG_CDC", "CDCDSP", 2, REG6_PORT_OPTIONS}
};
// ==================== 31-32 (0x1F-0x20):  ====================

static const PCAP04_BitOption_t REG31_RES_OPTIONS[] = {
   {0x00, "10", "10"},
   {0x01, "12", "12"},
   {0x02, "14", "14"},
   {0x03, "16", "16"}
};
static const PCAP04_BitOption_t REG31_MODE_OPTIONS[] = {
   {0x00, "PWM", "PWM"},
   {0x01, "PDM", "PDM"}
};
static const PCAP04_BitOption_t REG31_CLK_OPTIONS[] = {
   {0x00, "", ""},
   {0x01, "OLF/1", "OLF"},
   {0x02, "OLF/2", "OLF 2"},
   {0x03, "OLF/4", "OLF 4"},
   {0x04, "OX/1", "OX"},
   {0x05, "OX/2", "OX 2"},
   {0x06, "OX/4", "OX 4"}
};
static const PCAP04_RegBit_t REG31_BITS[] = {
   {7, 7, "PI1_TOGGLE_EN", "PI1", 2, REG6_PORT_OPTIONS},
   {6, 6, "PIO_TOGGLE_EN", "PI0", 2, REG6_PORT_OPTIONS},
   {4, 5, "PIO_RES", "0", 4, REG31_RES_OPTIONS},
   {3, 3, "PIO_PDM_SEL", "PI0 PWM/PDM", 2, REG31_MODE_OPTIONS},
   {0, 2, "PIO_CLK_SEL", "0", 7, REG31_CLK_OPTIONS}
};
static const PCAP04_RegBit_t REG32_BITS[] = {
   {4, 5, "PI1_RES", "1", 4, REG31_RES_OPTIONS},
   {3, 3, "PI1_PDM_SEL", "PI1 PWM/PDM", 2, REG31_MODE_OPTIONS},
   {0, 2, "PI1_CLK_SEL", "1", 7, REG31_CLK_OPTIONS}
};
// ==================== 33 (0x21): GPIO ====================

static const PCAP04_BitOption_t REG33_DIR_OPTIONS[] = {
   {0x00, "", "GPIO"},
   {0x01, "", "GPIO"}
};
static const PCAP04_RegBit_t REG33_BITS[] = {
   {7, 7, "PG3_DIR", "PG3", 2, REG33_DIR_OPTIONS},
   {6, 6, "PG2_DIR", "PG2", 2, REG33_DIR_OPTIONS},
   {5, 5, "PG1_DIR", "PG1", 2, REG33_DIR_OPTIONS},
   {4, 4, "PG0_DIR", "PG0", 2, REG33_DIR_OPTIONS},
   {3, 3, "PG3_PU", "PG3", 2, REG6_PORT_OPTIONS},
   {2, 2, "PG2_PU", "PG2", 2, REG6_PORT_OPTIONS},
   {1, 1, "PG1_PU", "PG1", 2, REG6_PORT_OPTIONS},
   {0, 0, "PG0_PU", "PG0", 2, REG6_PORT_OPTIONS}
};
// ==================== 34 (0x22):  ====================

static const PCAP04_BitOption_t REG34_BG_MODE_OPTIONS[] = {
   {0x00, "", ""},
   {0x01, "", "(+20A)"}
};
static const PCAP04_RegBit_t REG34_BITS[] = {
   {7, 7, "INT_TRIG_BG", "", 2, REG6_PORT_OPTIONS},
   {6, 6, "DSP_TRIG_BG", "DSP", 2, REG6_PORT_OPTIONS},
   {5, 5, "BG_PERM", "", 2, REG34_BG_MODE_OPTIONS},
   {4, 4, "AUTOSTART", "CDC", 2, REG6_PORT_OPTIONS}
};
// ==================== 35 (0x23):  ====================

static const PCAP04_BitOption_t REG35_GAIN_OPTIONS[] = {
   {0x00, "1.00", "1.00"},
   {0x40, "1.25", "1.25()"},
   {0x80, "1.50", "1.50"},
   {0xFF, "1.996", "1.996()"}
};
static const PCAP04_RegBit_t REG35_BITS[] = {
   {0, 7, "CDC_GAIN_CORR", "(1+n/256)", 4, REG35_GAIN_OPTIONS}
};
// ==================== 38 (0x26):  ====================

static const PCAP04_RegBit_t REG38_BITS[] = {
   {0, 7, "BG_TIME", "(0=)", 1, REG9_CONV_OPTIONS}
};
// ==================== 39 (0x27):  ====================

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
   {4, 7, "PULSE_SEL1", "1", 8, REG39_SEL_OPTIONS},
   {0, 3, "PULSE_SEL0", "0", 8, REG39_SEL_OPTIONS}
};
// ==================== 40-41 (0x28-0x29):  ====================

static const PCAP04_RegBit_t REG40_BITS[] = {
   {0, 7, "C_SENSE_SEL", "()", 1, REG9_CONV_OPTIONS}
};
static const PCAP04_RegBit_t REG41_BITS[] = {
   {0, 7, "R_SENSE_SEL", "()", 1, REG9_CONV_OPTIONS}
};
// ==================== 42 (0x2A):  ====================

static const PCAP04_BitOption_t REG42_ALARM_OPTIONS[] = {
   {0x00, "Z", "Z"},
   {0x01, "Theta", "Theta"}
};
static const PCAP04_RegBit_t REG42_BITS[] = {
   {6, 6, "ALARM1_SELECT", "1", 2, REG42_ALARM_OPTIONS},
   {4, 4, "ALARM0_SELECT", "0", 2, REG42_ALARM_OPTIONS},
   {3, 3, "EN_ASYNC_READ", "", 2, REG6_PORT_OPTIONS},
   {1, 1, "R_MEDIAN_EN", "R", 2, REG6_PORT_OPTIONS},
   {0, 0, "C_MEDIAN_EN", "C", 2, REG6_PORT_OPTIONS}
};
// ==================== 47 (0x2F):  ====================

static const PCAP04_BitOption_t REG47_RUN_OPTIONS[] = {
   {0x00, "", ""},
   {0x01, "", ""}
};
static const PCAP04_RegBit_t REG47_BITS[] = {
   {0, 0, "RUNBIT", "DSP", 2, REG47_RUN_OPTIONS}
};
// ==================== 48 (0x30):  ====================

static const PCAP04_RegBit_t REG48_BITS[] = {
   {3, 3, "MEM_LOCK_960", "960-10071022-1023", 2, REG6_PORT_OPTIONS},
   {2, 2, "MEM_LOCK_832", "832-959", 2, REG6_PORT_OPTIONS},
   {1, 1, "MEM_LOCK_704", "704-831", 2, REG6_PORT_OPTIONS},
   {0, 0, "MEM_LOCK_0", "0-703", 2, REG6_PORT_OPTIONS}
};
// ==================== 49-50 (0x31-0x32):  ====================

static const PCAP04_RegBit_t REG49_BITS[] = {
   {0, 7, "SERIAL_NUMBER_L", "", 1, REG9_CONV_OPTIONS}
};
static const PCAP04_RegBit_t REG50_BITS[] = {
   {0, 7, "SERIAL_NUMBER_H", "", 1, REG9_CONV_OPTIONS}
};
// ==================== 54 (0x36):  ====================

static const PCAP04_BitOption_t REG54_CTRL_OPTIONS[] = {
   {0x00, "", ""},
   {0x2D, "", "NVRAM"},
   {0x59, "", "NVRAM"},
   {0xB8, "", "NVRAM"}
};
static const PCAP04_RegBit_t REG54_BITS[] = {
   {0, 7, "MEM_CTRL", "", 4, REG54_CTRL_OPTIONS}
};
// ==================== 62-63 (0x3E-0x3F): (,) ====================

static const PCAP04_RegBit_t REG62_BITS[] = {
   {0, 7, "CHARGE_PUMP_L", "()", 1, REG9_CONV_OPTIONS}
};
static const PCAP04_RegBit_t REG63_BITS[] = {
   {0, 7, "CHARGE_PUMP_H", "()", 1, REG9_CONV_OPTIONS}
};
// ====================  ====================

static const PCAP04_Register_t PCAP04_REGISTERS[] = {
   {0x00, "IIC_LF_CONFIG", "IIC", 3, REG0_BITS, 0x1D},//REG0: 0x1D: 0001 1101
   {0x01, "OX_CONFIG", "OX", 3, REG1_BITS, 0x00},//REG1: 0x00: 0000 0000
   {0x02, "DISCHARGE_RES", "", 4, REG2_BITS, 0x58},//REG2: 0x58: 0101 1000
   {0x03, "CHARGE_CONFIG", "", 5, REG3_BITS, 0x10},//REG3: 0x10: 0001 0000     
   {0x04, "CAP_CONNECT", "", 5, REG4_BITS, 0x10},//REG4: 0x10: 0001 0000
   {0x05, "CDC_CLOCK", "CDC", 6, REG5_BITS, 0x00},//REG5: 0x00: 0000 0000
   {0x06, "PORT_ENABLE", "", 6, REG6_BITS, 0x3F},//REG6: 0x3F: 0011 1111
   {0x07, "C_AVRG_L", "", 1, REG7_BITS, 0x20},//REG7: 0x20: 0010 0000
   {0x08, "C_AVRG_H", "", 1, REG8_BITS, 0x00},//REG8: 0x00: 0000 0000
   {0x09, "CONV_TIME_L", "", 1, REG9_BITS, 0xD0},//REG9: 0xD0: 1101 0000
   {0x0A, "CONV_TIME_M", "", 1, REG10_BITS, 0x07},//REG10: 0x07: 0000 0111
   {0x0B, "CONV_TIME_H", "", 1, REG11_BITS, 0x00},//REG11: 0x00: 0000 0000
   {0x0C, "DISCHARGE_L", "", 1, REG12_BITS, 0x00},//REG12: 0x00: 0000 0000
   {0x0D, "TRIG_CONFIG", "", 3, REG13_BITS, 0x08},//REG13: 0x08: 0000 1000
   {0x0E, "PRECHARGE_L", "", 1, REG14_BITS, 0xFF},//REG14: 0xFF: 1111 1111
   {0x0F, "PRECHARGE_CFG", "", 2, REG15_BITS, 0x03},//REG15: 0x03: 0000 0011
   {0x10, "FULLCHARGE_L", "", 1, REG16_BITS, 0x00},//REG16: 0x00: 0000 0000
   {0x11, "CREF_CONFIG", "", 2, REG17_BITS, 0x24},//REG17: 0x24: 0010 0100
   {0x12, "GUARD_ENABLE", "", 8, REG18_BITS, 0x00},//REG18: 0x00: 0000 0000
   {0x13, "GUARD_OP", "", 3, REG19_BITS, 0x00},//REG19: 0x00: 0000 0000
   {0x14, "RDC_TIME", "RDC", 2, REG20_BITS, 0x00},//REG20: 0x00: 0000 0000
   {0x15, "R_PREDIV_L", "RDC", 1, REG21_BITS, 0x01},//REG21: 0x01: 0000 0001
   {0x16, "RDC_TRIG", "RDC", 3, REG22_BITS, 0x50},//REG22: 0x50: 0101 0000
   {0x17, "RDC_PORT", "RDC", 6, REG23_BITS, 0x30},//REG23: 0x30: 0011 0000
   {0x18, "TDC_CONFIG0", "TDC0()", 4, REG24_BITS, 0x73},//REG24: 0x73: 0111 0011
   {0x19, "TDC_CONFIG1", "TDC1()", 1, REG25_BITS, 0x04},//REG25: 0x04: 0000 0100
   {0x1A, "TDC_CONFIG2", "TDC2()", 2, REG26_BITS, 0x50},//REG26: 0x50: 0101 0000
   {0x1B, "DSP_CONFIG", "DSP", 4, REG27_BITS, 0x08},//REG27: 0x08: 0000 1000
   {0x1C, "WATCHDOG", "", 1, REG28_BITS, 0x5A},//REG28: 0x5A: 0101 1010
   {0x1D, "DSP_TRIGGER", "DSP", 2, REG29_BITS, 0x00},//REG29: 0x00: 0000 0000
   {0x1E, "INT_CONFIG", "", 5, REG30_BITS, 0x82},//0x82: 1000 0010
   {0x1F, "PULSE_IF0", "0", 5, REG31_BITS, 0x08},//REG31: 0x08: 0000 1000
   {0x20, "PULSE_IF1", "1", 3, REG32_BITS, 0x08},//REG32: 0x08: 0000 1000
   {0x21, "GPIO_CONFIG", "GPIO", 8, REG33_BITS, 0x00},//REG33: 0x00: 0000 0000
   {0x22, "BANDGAP", "", 4, REG34_BITS, 0x47},//0x47: 0100 0111
   {0x23, "GAIN_CORR", "", 1, REG35_BITS, 0x40},//REG35: 0x40: 0100 0000
   {0x24, "RESERVED36", "36", 0, 0, 0x00},//REG36: 0x00: 0000 0000
   {0x25, "RESERVED37", "37", 0, 0, 0x00},//REG37: 0x00: 0000 0000
   {0x26, "BG_TIME", "", 1, REG38_BITS, 0x00},//REG38: 0x00: 0000 0000
   {0x27, "PULSE_SEL", "", 2, REG39_BITS, 0x71},//REG39: 0x71: 0111 0001
   {0x28, "C_SENSE_SEL", "", 1, REG40_BITS, 0x00},//REG40: 0x00: 0000 0000
   {0x29, "R_SENSE_SEL", "", 1, REG41_BITS, 0x00},//REG41: 0x00: 0000 0000
   {0x2A, "FW_CONFIG", "", 5, REG42_BITS, 0x08},//REG42: 0x08: 0000 1000
   {0x2B, "RESERVED43", "43", 0, 0, 0x00},//REG43: 0x00: 0000 0000
   {0x2C, "RESERVED44", "44", 0, 0, 0x00},//REG44: 0x00: 0000 0000
   {0x2D, "RESERVED45", "45", 0, 0, 0x00},//REG45: 0x00: 0000 0000
   {0x2E, "RESERVED46", "46", 0, 0, 0x00},//REG46: 0x00: 0000 0000
   {0x2F, "RUNBIT", "", 1, REG47_BITS, 0x01},//REG47: 0x01: 0000 0001
   {0x30, "MEM_LOCK", "", 4, REG48_BITS, 0x00},//REG48: 0x00: 0000 0000
   {0x31, "SERIAL_L", "", 1, REG49_BITS, 0x00},//REG49: 0x00: 0000 0000
   {0x32, "SERIAL_H", "", 1, REG50_BITS, 0x00},//REG50: 0x00: 0000 0000
   {0x33, "RESERVED51", "51", 0, 0, 0x00},//REG51: 0x00: 0000 0000
   {0x34, "RESERVED52", "52", 0, 0, 0x00},//REG52: 0x00: 0000 0000
   {0x35, "RESERVED53", "53", 0, 0, 0x00},//REG53: 0x00: 0000 0000
   {0x36, "MEM_CTRL", "", 1, REG54_BITS, 0x00},//REG54: 0x00: 0000 0000
   {0x37, "RESERVED55", "55", 0, 0, 0x00},//REG55: 0x00: 0000 0000
   {0x38, "RESERVED56", "56", 0, 0, 0x00},//REG56: 0x00: 0000 0000
   {0x39, "RESERVED57", "57", 0, 0, 0x00},//REG57: 0x00: 0000 0000
   {0x3A, "RESERVED58", "58", 0, 0, 0x00},//REG58: 0x00: 0000 0000
   {0x3B, "RESERVED59", "59", 0, 0, 0x00},//REG59: 0x00: 0000 0000
   {0x3C, "RESERVED60", "60", 0, 0, 0x00},//REG60: 0x00: 0000 0000
   {0x3D, "RESERVED61", "61", 0, 0, 0x00},//REG61: 0x00: 0000 0000
   {0x3E, "CHARGE_PUMP_L", "()", 1, REG62_BITS, 0x00},//REG62: 0x00: 0000 0000
   {0x3F, "CHARGE_PUMP_H", "()", 1, REG63_BITS, 0x00}//REG63: 0x00: 0000 0000
};
// 

const PCAP04_RegisterTable_t PCAP04_REGISTER_TABLE = {
   sizeof(PCAP04_REGISTERS) / sizeof(PCAP04_Register_t),
   PCAP04_REGISTERS
};

