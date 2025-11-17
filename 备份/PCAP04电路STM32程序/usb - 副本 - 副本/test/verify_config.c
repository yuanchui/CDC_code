/**
 * @file   verify_config.c
 * @brief  验证PCAP04配置数组构建是否正确
 * 
 * 编译方法:
 *   gcc -o verify_config.exe verify_config.c Core/Src/pcap04_reg.c -I Core/Inc -D__GNUC__
 * 
 * 运行方法:
 *   ./verify_config.exe
 */

#include "pcap04_reg.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main(void)
{
    uint8_t i;
    
    // 初始化配置数组
    PCAP04_InitConfig();
    
    // 打印配置数组
    PCAP04_PrintConfig();
    
    // // 验证配置数组是否已正确构建
    

    // // 检查关键寄存器
    // printf("\nKey Registers Check:\n");
    // printf("  Reg 0  (I2C/Clock): 0x%02X\n", PCAP04_Config[0]);
    // printf("  Reg 6  (Port Enable): 0x%02X\n", PCAP04_Config[6]);
    // printf("  Reg 47 (Run Bit): 0x%02X\n", PCAP04_Config[47]);
    
    // // 输出详细配置（JSON格式，便于解析）
    // // 注意：JSON 输出到 stderr，避免与其他输出混合
    // fprintf(stderr, "\n===JSON_START===\n");
    // fprintf(stderr, "{\n");
    // fprintf(stderr, "  \"registers\": [\n");
    // for (i = 0; i < 64; i++) {
    //     fprintf(stderr, "    {\n");
    //     fprintf(stderr, "      \"reg_num\": %d,\n", i);
    //     fprintf(stderr, "      \"hex_address\": \"0x%02X\",\n", i);
    //     fprintf(stderr, "      \"value\": \"0x%02X\",\n", PCAP04_Config[i]);
    //     fprintf(stderr, "      \"value_decimal\": %d,\n", PCAP04_Config[i]);
    //     fprintf(stderr, "      \"value_binary\": \"0b");
    //     // 输出二进制
    //     for (int bit = 7; bit >= 0; bit--) {
    //         fprintf(stderr, "%d", (PCAP04_Config[i] >> bit) & 1);
    //     }
    //     fprintf(stderr, "\"\n");
    //     fprintf(stderr, "    }%s\n", (i < 63) ? "," : "");
    // }
    // fprintf(stderr, "  ]\n");
    // fprintf(stderr, "}\n");
    // fprintf(stderr, "===JSON_END===\n");

    printf("\n========================================\n");
    printf("Verification Summary:\n");
    
    uint8_t non_zero_count = 0;
    for (i = 0; i < 64; i++) {
        if (PCAP04_Config[i] != 0x00) {
            non_zero_count++;
        }
    }
        system("pause");          // 让窗口等用户按键
    return 0;
}

