# CDC_code

# STM32F103 PCap04 16x16 电容矩阵扫描系统

本项目实现了基于 STM32F103 的 PCap04 电容传感器驱动，通过 CD74HC4067SM96 多路复用器实现 16x16 电容矩阵扫描，并通过 USB 2.0 CDC 接口输出电容值数据。

## 项目概述

- **MCU**: STM32F103C8T6
- **传感器**: PCap04 电容-数字转换器
- **通信方式**: SPI (单SPI模式)
- **矩阵规模**: 16x16 (256个电容测量点)
- **多路复用器**: CD74HC4067SM96 (2个：行列各1个)
- **输出接口**: USB CDC (虚拟串口)

## 硬件配置

### 引脚定义

#### PCap04 SPI 通信引脚
| PCap04 功能 | STM32F103 引脚 | 说明 |
|------------|---------------|------|
| MOSI       | PB15          | SPI2 MOSI |
| MISO       | PB14          | SPI2 MISO |
| SCK        | PB13          | SPI2 SCK |
| SSN        | PB12          | SPI2 NSS (软件控制，默认拉低使能) |
| IIC_EN     | PA8           | 接口选择：0=SPI, 1=I2C |

#### CD74HC4067SM96 多路复用器引脚

**列选 16选1 (Column Multiplexer)**
| 功能 | STM32F103 引脚 | 说明 |
|------|---------------|------|
| SY0  | PA3           | 通道选择位0 |
| SY1  | PA4           | 通道选择位1 |
| SY2  | PA6           | 通道选择位2 |
| SY3  | PA7           | 通道选择位3 |
| ENX  | PA5           | 使能信号（低电平有效） |

**行选 16选1 (Row Multiplexer)**
| 功能 | STM32F103 引脚 | 说明 |
|------|---------------|------|
| SX0  | PB1           | 通道选择位0 |
| SX1  | PB0           | 通道选择位1 |
| SX2  | PB10          | 通道选择位2 |
| SX3  | PB2           | 通道选择位3 |
| ENY  | PB11          | 使能信号（低电平有效） |

#### 其他引脚
| 功能 | STM32F103 引脚 | 说明 |
|------|---------------|------|
| USB_DM | PA11        | USB D- |
| USB_DP | PA12        | USB D+ |
| USB_EN | PA15        | USB使能 |

### SPI 配置

- **模式**: SPI Mode 1 (CPOL=0, CPHA=1)
- **数据格式**: 8位, MSB 优先
- **波特率**: SPI_BAUDRATEPRESCALER_2 (36MHz @ 72MHz APB1)
- **NSS 控制**: 软件控制（SPI_NSS_SOFT）
- **SSN 引脚**: 默认拉低使能，系统运行期间保持低电平，不需要拉高操作

## 功能说明

### 1. PCap04 初始化
- Power-On Reset (POR)
- 固件加载（548字节 Standard Firmware）
- 配置寄存器写入（52字节 Standard Configuration）
- 传感器初始化完成

### 2. 矩阵扫描
- 通过行列多路复用器依次选择 16x16 = 256 个测量点
- 对每个点触发 PCap04 测量（CDC_START opcode: 0x8C）
- 读取电容值（RD_RESULT opcode: 0x40）
- 存储到矩阵数据结构中
- 支持流式扫描：扫描一个点立即发送一个点

### 3. USB 输出
- 通过 USB CDC 虚拟串口输出 16x16 电容矩阵数据
- 格式化的文本输出，包含行列标签和数值
- 传输由会话控制：收到 `START` 才开始传输数据，`STOP` 立即结束并输出 `END`
- 普通模式基于系统节拍非阻塞调度（HAL_GetTick），间隔由 `SET_RATE:<ms>` 控制
- **默认输出格式**：表格格式（TABLE），包含列标题和逗号分隔的数据行

## 代码结构

```
stm32f103_usb_pcap04/
├── Core/
│   ├── Inc/
│   │   ├── pcap04_spi.h          # PCap04 SPI通信接口
│   │   ├── mux_control.h         # 多路复用器控制
│   │   ├── matrix_scan.h          # 矩阵扫描功能
│   │   ├── usb_command.h          # USB命令处理
│   │   └── main.h                # 主程序头文件（包含通信方式选择）
│   └── Src/
│       ├── pcap04_spi.c          # PCap04 SPI通信实现
│       ├── mux_control.c         # 多路复用器控制实现
│       ├── matrix_scan.c         # 矩阵扫描实现
│       ├── usb_command.c         # USB命令处理实现
│       ├── main.c                 # 主程序（初始化和主循环）
│       ├── spi.c                 # SPI2 初始化
│       └── gpio.c                # GPIO 初始化
├── USB_DEVICE/
│   └── App/
│       └── usbd_cdc_if.c         # USB CDC 接口（集成命令处理）
└── README.md                      # 本文件
```

## 主要函数说明

### PCap04 SPI 接口 (`pcap04_spi.c/h`)

- `PCap04_Set_IIC_EN(uint8_t enable)`: 设置 IIC_EN 引脚（0=SPI模式）
- `Set_SSN(uint8_t level)`: 设置SSN引脚（LOW/HIGH，默认保持LOW）
- `Write_Opcode(uint8_t opcode)`: 写入单字节操作码
- `Write_Dword(uint8_t opcode, uint8_t address, uint32_t dword)`: 写入双字数据
- `Read_Dword(uint8_t rd_opcode, uint8_t address)`: 读取双字数据
- `PCap04_Memory_Access()`: 写入内存（固件）
- `PCap04_Config_Access()`: 写入配置寄存器
- `PCap04_Read_Result()`: 读取结果寄存器
- `PCap04_Get_Status()`: 获取PCap04状态信息（通信状态、初始化状态、寄存器值）
- `PCap04_Test_Communication()`: 测试PCap04 SPI通信（发送TEST_READ操作码）

### 多路复用器控制 (`mux_control.c/h`)

- `MUX_Init()`: 初始化多路复用器
- `MUX_Select_Column(uint8_t channel)`: 选择列（0-15）
- `MUX_Select_Row(uint8_t channel)`: 选择行（0-15）
- `MUX_Disable_Column()`: 禁用列选
- `MUX_Disable_Row()`: 禁用行选

### 矩阵扫描 (`matrix_scan.c/h`)

- `Matrix_Scan_Init()`: 初始化矩阵扫描
- `Matrix_Scan_Point(uint8_t row, uint8_t col)`: 扫描单个点
- `Matrix_Scan_All(MatrixData_t *matrix)`: 扫描整个 16x16 矩阵
- `Matrix_Output_USB(MatrixData_t *matrix)`: 通过 USB 输出矩阵数据（完整扫描后输出）
- `Matrix_Scan_And_Stream(MatrixData_t *matrix)`: 流式扫描并传输（扫描一个点立即发送一个点，自动包含START/END包围）
- `Quantize_Value()`: 量化函数，将原始值映射到指定范围

## 使用方法

### 1. 编译和烧录
1. 使用 STM32CubeIDE 或 Keil MDK-ARM 打开项目
2. 编译项目
3. 烧录到 STM32F103C8T6 开发板

### 2. 连接硬件
1. 连接 PCap04 到 SPI2 接口（PB12-PB15）
2. 连接 CD74HC4067SM96 多路复用器到相应 GPIO
3. 连接 USB 到电脑（PA11/PA12）

### 3. 运行和查看数据
1. 上电后，系统自动初始化 PCap04
2. 通过 USB CDC 虚拟串口查看输出数据
3. 使用串口工具（如 PuTTY、SecureCRT、串口助手等）打开 COM 口
4. 波特率：115200（或其他配置值）

### 4. USB 命令控制

系统支持通过 USB 发送命令来控制扫描速率和工作模式：

#### 可用命令

| 命令 | 说明 | 示例 |
|------|------|------|
| `SET_RATE:<ms>` | 设置扫描速率（毫秒） | `SET_RATE:50` 设置50ms扫描间隔 |
| `FAST_MODE` | 快速模式（连续扫描，无延时） | `FAST_MODE` 启用最快扫描 |
| `NORMAL_MODE` | 普通模式（按设置速率循环） | `NORMAL_MODE` 恢复正常扫描 |
| `SINGLE_SCAN` | 单次扫描 | `SINGLE_SCAN` 只扫描一次 |
| `START` | 会话开始（允许传输） | `START` 发送START标志并开始传输 |
| `STOP` | 停止扫描/会话结束 | `STOP` 发送END和STOP标志并立即停止 |
| `STATUS` | 查询状态 | `STATUS` 显示当前模式和速率 |
| `HELP` 或 `?` | 显示帮助信息 | `HELP` 显示所有命令 |
| `SET_ROW:<n>` | 设置行通道 | `SET_ROW:5` 设置行通道为5 (0-15) |
| `SET_COL:<n>` | 设置列通道 | `SET_COL:7` 设置列通道为7 (0-15) |
| `GET_ROW` | 查询当前行通道 | `GET_ROW` 显示当前行通道号 |
| `GET_COL` | 查询当前列通道 | `GET_COL` 显示当前列通道号 |
| `SCAN_POINT:<r>:<c>` | 扫描指定点 | `SCAN_POINT:3:7` 扫描行3列7的点 |
| `MATRIX_INFO` | 查询矩阵信息 | `MATRIX_INFO` 显示矩阵详细信息 |
| `SET_MODE:<raw\|quant>` | 设置输出模式 | `SET_MODE:quant` 启用量化模式 |
| `SET_RANGE:<min>:<max>` | 设置量化范围 | `SET_RANGE:1000:50000` 设置范围1000-50000 |
| `SET_LEVEL:<255\|1023>` | 设置量化档位 | `SET_LEVEL:1023` 设置为0-1023档位 |
| `SET_FORMAT:<simple\|table>` | 设置输出格式 | `SET_FORMAT:table` 设置为表格格式（默认） |
| `PCAP04_STATUS` | 查询PCap04状态 | `PCAP04_STATUS` 显示传感器状态信息 |
| `PCAP04_TEST` | 测试PCap04通信 | `PCAP04_TEST` 测试SPI通信是否正常 |

#### 工作模式说明

1. **普通模式（NORMAL_MODE）**
   - 按照 `SET_RATE` 设置的速率循环扫描
   - 默认速率：100ms
   - 例如：`SET_RATE:200` 设置200ms间隔

2. **快速模式（FAST_MODE）**
   - 收到就发送，一直循环
   - 使用非阻塞调度（基于系统节拍），避免阻塞延时

3. **单次模式（SINGLE_SCAN）**
   - 只扫描一次，完成后自动停止
   - 适合测试或单次数据采集

4. **停止模式（STOP）**
   - 停止扫描并关闭会话，立即输出 `END` 标志
   - 等待新命令或新的 `START`
   - 发送 `START` 后会自动恢复之前的工作模式

#### 输出模式说明

1. **原始值模式（RAW）** - 默认模式
   - 输出原始电容值（未处理）
   - 直接显示从 PCap04 读取的数值

2. **量化模式（QUANT）**
   - 将原始值量化到设定的最小值和最大值之间
   - 支持两个档位：
     - **0-255 档位**：输出值范围 0-255（8位）
     - **0-1023 档位**：输出值范围 0-1023（10位）
   - 量化公式：`quantized = (raw_value - min) * level / (max - min)`
   - 小于最小值的输入 → 输出 0
   - 大于最大值的输入 → 输出 level（255 或 1023）

#### 输出格式说明

1. **表格格式（TABLE）** - 默认格式
   - 包含列标题行：`X00,X01,X02,...,X15`
   - 每行数据：`Y00,值,值,值,...,值`（逗号分隔）
   - 适合数据分析和处理

2. **简洁格式（SIMPLE）**
   - 每行一个点：`X00Y00:值`
   - 格式：`X<列>Y<行>:<值>`
   - 适合逐点查看和调试

**重要说明**：选择模式（`SET_MODE:raw` 或 `SET_MODE:quant`）后，系统会：
1. 返回模式信息确认
2. 自动发送 `START` 标志
3. 启用数据传输（`g_stream_enabled = 1`）
4. 自动恢复之前的工作模式（如果之前是STOP模式）

#### 使用示例

```
# 方式1：直接选择模式（推荐，会自动发送START）
SET_MODE:raw         # 选择原始值模式，自动发送START并开始传输
NORMAL_MODE          # 设置工作模式
SET_RATE:50          # 设置扫描速率为50ms

# 方式2：先START再选择模式
START                # 开启会话
SET_MODE:raw         # 选择模式（会自动发送START）
NORMAL_MODE          # 设置工作模式
SET_RATE:50          # 设置扫描速率为50ms

# 切换到快速模式
FAST_MODE            # 快速连续扫描

# 停止扫描并结束会话
STOP                 # 设备会输出 END 和 STOP

# 再次开始（自动恢复之前的工作模式）
START                # 自动恢复之前保存的工作模式

# 查询状态
STATUS               # 显示当前状态信息
PCAP04_STATUS        # 显示PCap04传感器状态
PCAP04_TEST          # 测试PCap04通信

# 设置输出格式
SET_FORMAT:table     # 表格格式（默认）
SET_FORMAT:simple    # 简洁格式

# 查询和设置行列通道
GET_ROW              # 查询当前行通道
SET_ROW:5            # 设置行通道为5
GET_COL              # 查询当前列通道
SET_COL:7            # 设置列通道为7

# 扫描指定点（自动用START/END包围）
SCAN_POINT:3:7       # 扫描行3列7的点

# 查询矩阵信息
MATRIX_INFO          # 显示矩阵详细信息

# 设置量化模式和参数
SET_MODE:quant       # 设置为量化模式（自动发送START）
SET_RANGE:1000:50000 # 设置量化范围：最小值1000，最大值50000
SET_LEVEL:255        # 设置为0-255档位（8位）
SET_LEVEL:1023       # 设置为0-1023档位（10位）

# 显示帮助
HELP                 # 或使用 ?
```

### 5. 会话包围规则与输出数据格式示例

在默认情况下，只有在接收到 `START` 指令后才会开始传输数据；接收到 `STOP` 指令会立即停止并输出 `END` 标志。

**重要说明**：
- 每次扫描数据都会自动用 `START`/`END` 包围
- 选择模式（`SET_MODE:raw` 或 `SET_MODE:quant`）后，系统会返回模式信息并自动发送 `START`，然后开始数据传输
- 发送 `STOP` 后，再次发送 `START` 会自动恢复之前的工作模式

#### 输出数据格式示例

**表格格式（TABLE） - 原始值模式（RAW）：**
```
START
X00,X01,X02,X03,...,X15
Y00,12345,23456,34567,...,98765
Y01,12346,23457,34568,...,98766
...
Y15,12350,23461,34572,...,98770
END
```

**表格格式（TABLE） - 量化模式（QUANT）：**
```
START
X00,X01,X02,X03,...,X15
Y00,12,23,34,...,98
Y01,12,23,34,...,98
...
Y15,12,23,34,...,98
END
```

**简洁格式（SIMPLE） - 原始值模式（RAW）：**
```
START
X00Y00:12345
X01Y00:23456
X02Y00:34567
...
X15Y15:98770
END
```

**简洁格式（SIMPLE） - 量化模式（QUANT）：**
```
START
X00Y00:12
X01Y00:23
X02Y00:34
...
X15Y15:98
END
```

## PCap04 操作码说明

参考 PCap04 数据手册（Figure 101），主要操作码：

| 操作码 | 值    | 说明 |
|--------|-------|------|
| POR    | 0x88  | 上电复位 |
| INIT   | 0x8A  | 初始化 |
| CDC_START | 0x8C | 启动电容测量 |
| WR_MEM | 0xA0  | 写入内存（固件） |
| RD_MEM | 0x20  | 读取内存 |
| WR_CONFIG | 0xA3 | 写入配置 |
| RD_CONFIG | 0x23 | 读取配置 |
| RD_RESULT | 0x40 | 读取结果寄存器 |
| TEST_READ | 0x7E | 测试读取（用于通信测试，正常返回0x11） |

## 通信方式切换

### 一键切换 I2C/SPI

在 `main.h` 中通过修改 `USE_I2C_MODE` 宏定义即可切换通信方式：

```c
#define USE_I2C_MODE  0  /* 0=SPI模式, 1=I2C模式 */
```

- **USE_I2C_MODE = 0**: SPI 模式（默认）
  - IIC_EN 引脚自动设置为低电平
  - 使用 SPI2 接口通信

- **USE_I2C_MODE = 1**: I2C 模式
  - IIC_EN 引脚自动设置为高电平
  - 使用 I2C1 接口通信（需要实现I2C通信函数）

修改后重新编译即可，无需修改其他代码。

## 注意事项

1. **PB12 (SPI NSS/SSN)**: 
   - 已配置为 GPIO 输出模式，软件控制
   - **重要**：SSN引脚默认拉低使能，系统运行期间保持低电平，不需要拉高操作
   - 所有SPI操作期间SSN保持低电平，操作完成后不拉高
   - 确保在 `gpio.c` 中正确初始化

2. **IIC_EN 引脚**: 根据 `USE_I2C_MODE` 宏自动配置：
   - SPI模式：低电平（0）
   - I2C模式：高电平（1）

3. **USB命令**: 命令不区分大小写，支持简写（如 `FAST` 等同于 `FAST_MODE`）

4. **快速模式**: 快速模式会连续扫描，CPU占用率较高，建议仅在需要最高数据速率时使用

5. **多路复用器使能**: 
   - ENX/ENY 低电平使能，高电平禁用
   - 扫描完成后应禁用多路复用器，避免影响测量

6. **扫描时序**: 
   - 多路复用器切换后需要稳定时间（1ms）
   - PCap04 测量需要时间（10ms），可根据实际需求调整

7. **USB 连接**: 需要等待 USB 枚举完成（约1秒）后再开始数据输出。

8. **固件和配置**: 当前使用 Standard Firmware 和 Standard Configuration，如需更改，修改 `main.c` 中的数组数据。

9. **SSN引脚**: SSN（片选）引脚默认拉低使能，所有SPI操作期间保持低电平，操作完成后不拉高。

10. **默认输出格式**: 系统默认使用表格格式（TABLE），如需切换可使用 `SET_FORMAT:simple`。

11. **模式选择行为**: 使用 `SET_MODE:raw` 或 `SET_MODE:quant` 选择模式后，系统会：
    - 返回模式确认信息
    - 自动发送 `START` 标志
    - 启用数据传输（`g_stream_enabled = 1`）
    - 自动恢复之前的工作模式（如果之前是STOP模式）

12. **START/STOP命令**: 
    - `START` 命令会启用流式传输，如果当前是STOP模式，会自动恢复之前保存的工作模式
    - `STOP` 命令会保存当前工作模式，然后停止扫描并输出 `END` 和 `STOP`

13. **PCap04状态检测**: 
    - `PCAP04_STATUS` 命令会读取配置寄存器和结果寄存器来判断通信状态和初始化状态
    - `PCAP04_TEST` 命令会发送TEST_READ操作码（0x7E）测试通信，正常应返回0x11

## 移植来源

本代码从 `I2C-Interface-PCap04-main` 项目移植而来，主要修改：
- 从 STM32L476 移植到 STM32F103
- 从 I2C 通信改为 SPI 通信
- 添加了 16x16 矩阵扫描功能
- 添加了 USB CDC 输出功能
- 添加了 CD74HC4067SM96 多路复用器控制
- 添加了PCap04状态检测和通信测试功能
- 添加了输出格式选择（表格/简洁）
- 添加了量化模式支持

## 开发环境

- **STM32CubeMX**: 用于 GPIO、SPI、USB 配置
- **HAL 库**: STM32F1xx HAL Driver
- **编译器**: ARM GCC 或 Keil MDK-ARM

## 许可证

请参考原始项目许可证。

---

**最后更新**: 2025年1月

