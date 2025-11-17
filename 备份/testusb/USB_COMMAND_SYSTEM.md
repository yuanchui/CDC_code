# USB Command/Control Protocol System Documentation

## 系统架构

本系统为STM32F103 + USB2.0 + CD74HC4067 + PCAP04电容传感矩阵系统设计了一套完整的USB命令/控制协议体系。

### 模块结构

1. **usb_common** - USB命令解析和执行系统
   - 统一命令解析框架
   - 支持 && 组合命令
   - 支持命令队列执行
   - 系统状态管理

2. **usb_template** - 数据输出模板系统
   - 多种输出格式（TABLE, SIMPLE）
   - 多种数据模式（RAW, QUANT）
   - 可定制分隔符和样式
   - 表头控制

3. **pcap04_reg** - PCAP04寄存器控制系统
   - 基于PCAP04数据手册的寄存器访问
   - 寄存器读写操作
   - 配置管理
   - 状态监控

4. **tim** - 定时器中断支持
   - TIM1配置为5ms中断
   - 扫描速率控制
   - 定时触发扫描

## 命令集

### 1. 通用USB控制与扫描指令集

| 指令 | 说明 | 示例 |
|------|------|------|
| SET_RATE:<ms> | 设置扫描速率（ms） | SET_RATE:50 |
| FAST_MODE | 连续扫描（最快速率） | FAST_MODE |
| NORMAL_MODE | 按设定速率循环扫描 | NORMAL_MODE |
| SINGLE_SCAN | 执行一次扫描 | SINGLE_SCAN |
| START | 启动数据传输 | START |
| STOP | 停止扫描与数据传输 | STOP |
| STATUS | 查询当前状态 | STATUS |
| HELP 或 ? | 显示所有命令帮助 | HELP |
| SET_ROW:<n> | 设置当前扫描行通道（0~15） | SET_ROW:5 |
| SET_COL:<n> | 设置当前扫描列通道（0~15） | SET_COL:7 |
| GET_ROW | 查询当前行号 | GET_ROW |
| GET_COL | 查询当前列号 | GET_COL |
| SCAN_POINT:<r>:<c> | 扫描指定点 | SCAN_POINT:3:7 |
| MATRIX_INFO | 查询矩阵信息 | MATRIX_INFO |

### 2. PCAP04寄存器配置指令集

| 指令 | 说明 | 示例 |
|------|------|------|
| PCAP04_STATUS | 查询PCAP04状态 | PCAP04_STATUS |
| PCAP04_TEST | 测试PCAP04 SPI通信 | PCAP04_TEST |
| PCAP04_READ:<reg> | 读取指定寄存器（0~63） | PCAP04_READ:12 |
| PCAP04_WRITE:<reg>:<val> | 写入寄存器（0-63, 值0x00-0xFF） | PCAP04_WRITE:4:0x10 |
| PCAP04_DUMP | 导出全部64个寄存器内容 | PCAP04_DUMP |
| PCAP04_LOAD_DEFAULT | 载入默认配置数组 | PCAP04_LOAD_DEFAULT |
| SET_CDIFF:<0/1> | 设置差分模式 | SET_CDIFF:1 |
| SET_INTREF:<0/1> | 启用内部参考电容 | SET_INTREF:1 |
| SET_EXTREF:<0/1> | 启用外部参考电容 | SET_EXTREF:0 |

### 3. 数据输出模板与格式控制指令集

| 指令 | 说明 | 示例 |
|------|------|------|
| SET_MODE:<raw/quant> | 设置输出值类型 | SET_MODE:quant |
| SET_FORMAT:<table/simple> | 设置输出布局 | SET_FORMAT:table |
| SET_TABLE_DELIM:<char> | 设置表格分隔符（如,、\t） | SET_TABLE_DELIM:; |
| SET_HEX:<0/1> | 输出十六进制（1）或浮点（0） | SET_HEX:1 |
| SET_PRECISION:<n> | 设置浮点小数位数 | SET_PRECISION:3 |
| SET_HEADER:<0/1> | 控制是否输出表头 | SET_HEADER:1 |
| SET_MATRIX_SIZE:<row>:<col> | 设置矩阵行列数（最大16×16） | SET_MATRIX_SIZE:8:8 |

### 4. 复合与队列执行

支持 && 同行执行多条命令（按顺序执行）：
```
SET_MODE:quant && SET_FORMAT:table && START
```

或命令队列批处理（顺序执行，带延迟控制）：
```
QUEUE_START
SET_RATE:50
SET_MODE:quant
SET_FORMAT:table
START
WAIT:5000
STOP
QUEUE_END
```

## 状态返回规范

- 成功：`OK:<CMD>`
- 错误：`ERR:<CODE>:<INFO>`
- 状态：`STAT:<mode>:<rate>:<format>:<rows>x<cols>`

示例：
```
> SET_MODE:quant
< OK:SET_MODE
> STATUS
< STAT:NORMAL:50ms:16x16
```

## 输出模板格式

### 表格格式（TABLE）

RAW 模式：
```
START
X00,X01,X02,...,X15
Y00,12345,23456,...,98765
...
END
```

QUANT 模式：
```
START
X00,X01,X02,...,X15
Y00,12,23,...,98
...
END
```

### 简洁格式（SIMPLE）

RAW 模式：
```
START
X00Y00:12345
X00Y01:23456
...
END
```

QUANT 模式：
```
START
X00Y00:12
X00Y01:23
...
END
```

## 初始化

在main.c中需要初始化所有模块：

```c
// 初始化USB命令系统
USB_Common_Init();

// 初始化模板系统
Template_Init();

// 初始化PCAP04寄存器系统
PCAP04_Reg_Init();

// 启动定时器（可选）
TIM1_Start();
```

## 定时器配置

TIM1配置为5ms中断，用于控制扫描速率：
- Prescaler = 720-1 (72MHz / 720 = 100kHz)
- Period = 500-1 (100kHz / 500 = 200Hz = 5ms)

扫描速率通过计数阈值控制：
- 50ms扫描速率 = 10次中断计数
- 100ms扫描速率 = 20次中断计数

## 文件结构

```
usb/Core/
├── Inc/
│   ├── usb_common.h      # USB命令系统头文件
│   ├── usb_template.h    # 模板系统头文件
│   └── pcap04_reg.h      # PCAP04寄存器控制头文件
└── Src/
    ├── usb_common.c      # USB命令系统实现
    ├── usb_template.c    # 模板系统实现
    └── pcap04_reg.c      # PCAP04寄存器控制实现

usb/Core/Src/
├── tim.c                 # 定时器配置（已修改）
└── stm32f1xx_it.c       # 中断处理（已添加TIM1中断）

usb/USB_DEVICE/App/
└── usbd_cdc_if.c        # CDC接口（已集成命令处理）
```

## 注意事项

1. 确保在main.c中包含所有必要的头文件
2. TIM1中断优先级需要合理配置
3. USB CDC缓冲区大小需要足够（APP_RX_DATA_SIZE >= 256）
4. PCAP04 SPI通信需要正确的CS控制
5. 命令解析支持大小写不敏感

## 扩展性

系统设计具有良好的扩展性：
- 新命令可以在`CMD_ExecuteSingle`函数中添加
- 新的输出格式可以在模板系统中添加
- 新的寄存器操作可以在pcap04_reg模块中添加

