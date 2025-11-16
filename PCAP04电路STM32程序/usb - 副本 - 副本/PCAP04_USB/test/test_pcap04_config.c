/**
 * @file   test_pcap04_config.c
 * @brief  PC环境测试程序 - 打印PCAP04配置数组
 * 
 * 编译方法（使用GCC）:
 *   gcc -o test_pcap04_config test_pcap04_config.c Core/Src/pcap04_reg.c -I Core/Inc
 * 
 * 运行方法:
 *   ./test_pcap04_config
 * 
 * 或者在Windows上使用MinGW:
 *   gcc -o test_pcap04_config.exe test_pcap04_config.c Core/Src/pcap04_reg.c -I Core/Inc
 *   test_pcap04_config.exe
 */

#include "pcap04_reg.h"
#include <stdio.h>

int main(void)
{ 
    // 打印配置数组
    PCAP04_PrintConfig();
    
    printf("Test completed successfully!\n");
    
    return 0;
}

