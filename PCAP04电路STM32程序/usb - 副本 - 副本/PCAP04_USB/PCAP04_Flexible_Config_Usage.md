# PCAP04 灵活配置系统使用指南

## 功能概述

本系统支持以下功能：
1. **不按顺序设置寄存器** - 可以任意顺序配置寄存器
2. **使用功能位宏定义** - 使用寄存器宏定义构建配置值，不依赖具体位置
3. **直接使用十六进制值** - 也支持直接使用十六进制值
4. **单寄存器写入** - 支持直接写入单个寄存器
5. **批量写入** - 支持从配置数组批量写入寄存器

## API 函数

### 1. PCAP04_BuildConfigArray
将不按顺序的寄存器配置项转换为PCAP04_Config数组。

### 2. PCAP04_WriteSingleReg
直接通过SPI写入单个寄存器。

### 3. PCAP04_WriteConfigArray
从配置数组批量写入寄存器。

## 使用示例

### 示例1：不按顺序设置寄存器（使用功能位宏）

```c
#include "pcap04_reg.h"

void example1(void)
{
    // 不按顺序设置寄存器，使用功能位宏定义
    PCAP04_RegConfigItem_t my_config[] = {
        // 先设置寄存器6（端口使能）
        {PCAP04_REG_ADDR_6, 
         PCAP04_REG6_VALUE(
             PCAP04_REG6_C_PORT_EN_PC0 |
             PCAP04_REG6_C_PORT_EN_PC1 |
             PCAP04_REG6_C_PORT_EN_PC2 |
             PCAP04_REG6_C_PORT_EN_PC3
         )},
        
        // 再设置寄存器4（电容连接方式）
        {PCAP04_REG_ADDR_4,
         PCAP04_REG4_VALUE(
             PCAP04_REG4_C_REF_INT_EXTERNAL,
             0,
             PCAP04_REG4_C_COMP_INT_ACTIVE,
             PCAP04_REG4_C_DIFFERENTIAL_DIFF,
             PCAP04_REG4_C_FLOATING_FLOATING
         )},
        
        // 最后设置寄存器0（I2C地址与时钟配置）
        {PCAP04_REG_ADDR_0,
         PCAP04_REG0_VALUE(
             0,
             PCAP04_REG0_OLF_FTUNE_TYPICAL,
             PCAP04_REG0_OLF_CTUNE_50k
         )},
        
        // 设置寄存器47（运行位）
        {PCAP04_REG_ADDR_47,
         PCAP04_REG47_RUNBIT_bit(PCAP04_REG47_RUNBIT_ON)},
    };
    
    uint8_t output_config[64];
    
    // 将配置项转换为配置数组（基于默认值0x00）
    PCAP04_BuildConfigArray(my_config, 
                            sizeof(my_config)/sizeof(my_config[0]), 
                            output_config, 
                            NULL);
    
    // 写入所有寄存器
    PCAP04_WriteConfigArray(output_config, 0, 64);
}
```

### 示例2：基于现有配置修改部分寄存器

```c
void example2(void)
{
    // 基于现有的PCAP04_Config，只修改部分寄存器
    PCAP04_RegConfigItem_t changes[] = {
        // 启用所有端口（PC0-PC5）
        {PCAP04_REG_ADDR_6, 
         PCAP04_REG6_C_PORT_EN_ALL},
        
        // 修改触发模式为读取触发
        {PCAP04_REG_ADDR_13,
         PCAP04_REG13_VALUE(
             0,
             PCAP04_REG13_C_STARTONPIN_PG0,
             PCAP04_REG13_C_TRIG_SEL_READ_TRIGGER
         )},
    };
    
    uint8_t output_config[64];
    
    // 基于现有配置构建新配置
    PCAP04_BuildConfigArray(changes, 
                            sizeof(changes)/sizeof(changes[0]), 
                            output_config, 
                            PCAP04_Config);
    
    // 写入所有寄存器
    PCAP04_WriteConfigArray(output_config, 0, 64);
}
```

### 示例3：直接使用十六进制值

```c
void example3(void)
{
    // 直接使用十六进制值配置寄存器
    PCAP04_RegConfigItem_t hex_config[] = {
        {PCAP04_REG_ADDR_6, 0x0F},   // 启用PC0-PC3
        {PCAP04_REG_ADDR_4, 0x13},   // 直接十六进制值
        {PCAP04_REG_ADDR_0, 0x1D},   // 直接十六进制值
        {PCAP04_REG_ADDR_47, 0x01},  // 运行位开启
    };
    
    uint8_t output_config[64];
    
    PCAP04_BuildConfigArray(hex_config, 
                            sizeof(hex_config)/sizeof(hex_config[0]), 
                            output_config, 
                            NULL);
    
    PCAP04_WriteConfigArray(output_config, 0, 64);
}
```

### 示例4：单寄存器写入

```c
void example4(void)
{
    // 方法1：直接写入十六进制值
    PCAP04_WriteSingleReg(PCAP04_REG_ADDR_6, 0x0F);
    
    // 方法2：使用功能位宏构建值
    uint8_t reg4_value = PCAP04_REG4_VALUE(
        PCAP04_REG4_C_REF_INT_EXTERNAL,
        0,
        PCAP04_REG4_C_COMP_INT_ACTIVE,
        PCAP04_REG4_C_DIFFERENTIAL_DIFF,
        PCAP04_REG4_C_FLOATING_FLOATING
    );
    PCAP04_WriteSingleReg(PCAP04_REG_ADDR_4, reg4_value);
    
    // 方法3：使用单个位宏
    PCAP04_WriteSingleReg(PCAP04_REG_ADDR_47, 
                          PCAP04_REG47_RUNBIT_bit(PCAP04_REG47_RUNBIT_ON));
}
```

### 示例5：混合使用功能位宏和十六进制

```c
void example5(void)
{
    // 混合使用功能位宏和直接十六进制值
    PCAP04_RegConfigItem_t mixed_config[] = {
        // 使用功能位宏
        {PCAP04_REG_ADDR_6,
         PCAP04_REG6_VALUE(
             PCAP04_REG6_C_PORT_EN_PC0 |
             PCAP04_REG6_C_PORT_EN_PC1
         )},
        
        // 直接使用十六进制
        {PCAP04_REG_ADDR_28, 0x5A},  // 看门狗禁用
        
        // 使用功能位宏
        {PCAP04_REG_ADDR_47,
         PCAP04_REG47_RUNBIT_bit(PCAP04_REG47_RUNBIT_ON)},
    };
    
    uint8_t output_config[64];
    
    // 基于默认配置
    PCAP04_BuildConfigArray(mixed_config, 
                            sizeof(mixed_config)/sizeof(mixed_config[0]), 
                            output_config, 
                            NULL);
    
    // 只写入前32个寄存器
    PCAP04_WriteConfigArray(output_config, 0, 32);
}
```

### 示例6：动态修改配置

```c
void example6(void)
{
    uint8_t dynamic_config[64];
    
    // 从默认配置开始
    PCAP04_RegConfigItem_t init_config[] = {
        {PCAP04_REG_ADDR_6, PCAP04_REG6_C_PORT_EN_ALL},
        {PCAP04_REG_ADDR_47, PCAP04_REG47_RUNBIT_bit(PCAP04_REG47_RUNBIT_ON)},
    };
    PCAP04_BuildConfigArray(init_config, 
                            sizeof(init_config)/sizeof(init_config[0]), 
                            dynamic_config, 
                            PCAP04_Config);
    
    // 写入初始配置
    PCAP04_WriteConfigArray(dynamic_config, 0, 64);
    
    // 运行时动态修改单个寄存器
    // 例如：根据传感器状态启用/禁用端口
    if (sensor_enabled) {
        PCAP04_WriteSingleReg(PCAP04_REG_ADDR_6, PCAP04_REG6_C_PORT_EN_ALL);
    } else {
        PCAP04_WriteSingleReg(PCAP04_REG_ADDR_6, 0x00);
    }
}
```

## 优势

1. **灵活性** - 不要求按寄存器顺序配置
2. **可读性** - 使用功能位宏，代码更易读
3. **兼容性** - 支持直接使用十六进制值
4. **便捷性** - 支持单寄存器写入，适合动态配置
5. **可维护性** - 可以基于现有配置进行修改，避免重复定义

## 注意事项

1. 如果多个配置项指向同一寄存器，后面的值会覆盖前面的值
2. 未设置的寄存器将使用base_config中的值（如果提供），否则为0x00
3. 单寄存器写入不会修改PCAP04_Config数组
4. 批量写入时，可以指定起始地址和数量，实现部分寄存器更新

