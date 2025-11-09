# USB 命令系统使用手册

## 目录
1. [命令格式](#命令格式)
2. [通用控制命令](#通用控制命令)
3. [扫描控制命令](#扫描控制命令)
4. [矩阵控制命令](#矩阵控制命令)
5. [PCAP04 命令](#pcap04-命令)
6. [模板输出命令](#模板输出命令)
7. [队列命令](#队列命令)
8. [命令组合](#命令组合)
9. [响应格式](#响应格式)

---

## 命令格式

### 基本格式
```
命令名[:参数1][:参数2][...]
```

### 命令特性
- **大小写不敏感**：命令名和参数不区分大小写
- **参数分隔符**：使用冒号 `:` 分隔参数
- **命令组合**：使用 `&&` 连接多个命令（按顺序执行）
- **最大长度**：单条命令最大 256 字符
- **参数数量**：最多 8 个参数

### 示例
```
START
SET_RATE:100
SET_ROW:5
SCAN_POINT:3:7
START && SET_RATE:50 && STATUS
```

---

## 通用控制命令

### 1. START
**功能**：启动系统扫描  
**格式**：`START`  
**参数**：无  
**响应**：`OK:START\r\n`  
**示例**：
```
START
```

### 2. STOP
**功能**：停止系统扫描  
**格式**：`STOP`  
**参数**：无  
**响应**：`OK:STOP\r\n`  
**示例**：
```
STOP
```

### 3. STATUS
**功能**：查询系统状态  
**格式**：`STATUS`  
**参数**：无  
**响应**：`STAT:模式:扫描速率ms:行数x列数\r\n`  
**示例**：
```
STATUS
```
**响应示例**：
```
STAT:NORMAL:50:16x16
```

### 4. HELP 或 ?
**功能**：显示帮助信息  
**格式**：`HELP` 或 `?`  
**参数**：无  
**响应**：显示所有可用命令的帮助信息  
**示例**：
```
HELP
?
```

### 5. SINGLE_SCAN
**功能**：执行单次扫描  
**格式**：`SINGLE_SCAN`  
**参数**：无  
**响应**：`OK:SINGLE_SCAN\r\n`  
**示例**：
```
SINGLE_SCAN
```

### 6. FAST_MODE
**功能**：设置为快速扫描模式  
**格式**：`FAST_MODE`  
**参数**：无  
**响应**：`OK:FAST_MODE\r\n`  
**示例**：
```
FAST_MODE
```

### 7. NORMAL_MODE
**功能**：设置为正常扫描模式  
**格式**：`NORMAL_MODE`  
**参数**：无  
**响应**：`OK:NORMAL_MODE\r\n`  
**示例**：
```
NORMAL_MODE
```

---

## 扫描控制命令

### 8. SET_RATE
**功能**：设置扫描速率（毫秒）  
**格式**：`SET_RATE:<ms>`  
**参数**：
- `<ms>`：扫描速率，范围 1-10000 毫秒

**响应**：`OK:SET_RATE\r\n` 或 `ERR:1:Invalid SET_RATE parameter\r\n`  
**示例**：
```
SET_RATE:100    # 设置扫描速率为 100ms
SET_RATE:50     # 设置扫描速率为 50ms
SET_RATE:1000   # 设置扫描速率为 1 秒
```

---

## 矩阵控制命令

### 9. SET_ROW
**功能**：设置当前扫描行  
**格式**：`SET_ROW:<n>`  
**参数**：
- `<n>`：行号，范围 0-15

**响应**：`OK:SET_ROW\r\n` 或 `ERR:2:Invalid SET_ROW parameter\r\n`  
**示例**：
```
SET_ROW:0
SET_ROW:5
SET_ROW:15
```

### 10. SET_COL
**功能**：设置当前扫描列  
**格式**：`SET_COL:<n>`  
**参数**：
- `<n>`：列号，范围 0-15

**响应**：`OK:SET_COL\r\n` 或 `ERR:3:Invalid SET_COL parameter\r\n`  
**示例**：
```
SET_COL:0
SET_COL:8
SET_COL:15
```

### 11. GET_ROW
**功能**：获取当前扫描行  
**格式**：`GET_ROW`  
**参数**：无  
**响应**：`ROW:<n>\r\n`  
**示例**：
```
GET_ROW
```
**响应示例**：
```
ROW:5
```

### 12. GET_COL
**功能**：获取当前扫描列  
**格式**：`GET_COL`  
**参数**：无  
**响应**：`COL:<n>\r\n`  
**示例**：
```
GET_COL
```
**响应示例**：
```
COL:8
```

### 13. SCAN_POINT
**功能**：扫描指定坐标点  
**格式**：`SCAN_POINT:<r>:<c>`  
**参数**：
- `<r>`：行号，范围 0-15
- `<c>`：列号，范围 0-15

**响应**：`OK:SCAN_POINT\r\n` 或 `ERR:4:Invalid SCAN_POINT parameters\r\n`  
**示例**：
```
SCAN_POINT:0:0
SCAN_POINT:5:10
SCAN_POINT:15:15
```

### 14. MATRIX_INFO
**功能**：显示矩阵信息  
**格式**：`MATRIX_INFO`  
**参数**：无  
**响应**：`MATRIX_INFO:行数x列数:ROW:当前行:COL:当前列\r\n`  
**示例**：
```
MATRIX_INFO
```
**响应示例**：
```
MATRIX_INFO:16x16:ROW:5:COL:8
```

---

## PCAP04 命令

### 15. PCAP04_STATUS
**功能**：显示 PCAP04 状态  
**格式**：`PCAP04_STATUS`  
**参数**：无  
**响应**：显示 PCAP04 寄存器状态和 `OK:PCAP04_STATUS\r\n`  
**示例**：
```
PCAP04_STATUS
```

### 16. PCAP04_TEST
**功能**：测试 PCAP04 通信  
**格式**：`PCAP04_TEST`  
**参数**：无  
**响应**：`PCAP04_TEST:OK\r\n` 或 `PCAP04_TEST:FAIL\r\n` 和 `OK:PCAP04_TEST\r\n`  
**示例**：
```
PCAP04_TEST
```

### 17. PCAP04_READ
**功能**：读取 PCAP04 寄存器  
**格式**：`PCAP04_READ:<reg>`  
**参数**：
- `<reg>`：寄存器地址（十进制或十六进制，如 0x10）

**响应**：`PCAP04_REG[0xXX]=0xXX\r\n` 和 `OK:PCAP04_READ\r\n`  
**示例**：
```
PCAP04_READ:0x10
PCAP04_READ:16
```
**响应示例**：
```
PCAP04_REG[0x10]=0x5A
OK:PCAP04_READ
```

### 18. PCAP04_WRITE
**功能**：写入 PCAP04 寄存器  
**格式**：`PCAP04_WRITE:<reg>:<val>`  
**参数**：
- `<reg>`：寄存器地址
- `<val>`：写入值

**响应**：`OK:PCAP04_WRITE\r\n` 或 `ERR:11:Invalid PCAP04_WRITE parameters\r\n`  
**示例**：
```
PCAP04_WRITE:0x10:0x5A
PCAP04_WRITE:16:90
```

### 19. PCAP04_DUMP
**功能**：转储所有 PCAP04 寄存器  
**格式**：`PCAP04_DUMP`  
**参数**：无  
**响应**：显示所有寄存器值和 `OK:PCAP04_DUMP\r\n`  
**示例**：
```
PCAP04_DUMP
```

### 20. PCAP04_LOAD_DEFAULT
**功能**：加载 PCAP04 默认配置  
**格式**：`PCAP04_LOAD_DEFAULT`  
**参数**：无  
**响应**：`OK:PCAP04_LOAD_DEFAULT\r\n`  
**示例**：
```
PCAP04_LOAD_DEFAULT
```

### 21. SET_CDIFF
**功能**：设置差分模式  
**格式**：`SET_CDIFF:<0/1>`  
**参数**：
- `<0/1>`：0=禁用，1=启用

**响应**：`OK:SET_CDIFF\r\n` 或 `ERR:12:Invalid SET_CDIFF parameter\r\n`  
**示例**：
```
SET_CDIFF:1
SET_CDIFF:0
```

### 22. SET_INTREF
**功能**：设置内部参考  
**格式**：`SET_INTREF:<0/1>`  
**参数**：
- `<0/1>`：0=禁用，1=启用

**响应**：`OK:SET_INTREF\r\n` 或 `ERR:13:Invalid SET_INTREF parameter\r\n`  
**示例**：
```
SET_INTREF:1
SET_INTREF:0
```

### 23. SET_EXTREF
**功能**：设置外部参考  
**格式**：`SET_EXTREF:<0/1>`  
**参数**：
- `<0/1>`：0=禁用，1=启用

**响应**：`OK:SET_EXTREF\r\n` 或 `ERR:14:Invalid SET_EXTREF parameter\r\n`  
**示例**：
```
SET_EXTREF:1
SET_EXTREF:0
```

---

## 模板输出命令

### 24. SET_MODE
**功能**：设置数据输出模式  
**格式**：`SET_MODE:<raw/quant>`  
**参数**：
- `raw`：原始数据模式（十六进制）
- `quant`：量化数据模式（浮点数）

**响应**：`OK:SET_MODE\r\n` 或 `ERR:17:Invalid SET_MODE parameter\r\n`  
**示例**：
```
SET_MODE:raw
SET_MODE:quant
SET_MODE:RAW
SET_MODE:QUANT
```

### 25. SET_FORMAT
**功能**：设置输出格式  
**格式**：`SET_FORMAT:<table/simple>`  
**参数**：
- `table`：表格格式（CSV 风格）
- `simple`：简单格式（X00Y00:value）

**响应**：`OK:SET_FORMAT\r\n` 或 `ERR:18:Invalid SET_FORMAT parameter\r\n`  
**示例**：
```
SET_FORMAT:table
SET_FORMAT:simple
SET_FORMAT:TABLE
SET_FORMAT:SIMPLE
```

### 26. SET_TABLE_DELIM
**功能**：设置表格分隔符  
**格式**：`SET_TABLE_DELIM:<char>`  
**参数**：
- `<char>`：分隔字符（如 `,`、`;`、`\t`）

**响应**：`OK:SET_TABLE_DELIM\r\n` 或 `ERR:19:Invalid SET_TABLE_DELIM parameter\r\n`  
**示例**：
```
SET_TABLE_DELIM:,
SET_TABLE_DELIM:;
SET_TABLE_DELIM:\t
```

### 27. SET_HEX
**功能**：设置是否使用十六进制输出  
**格式**：`SET_HEX:<0/1>`  
**参数**：
- `<0/1>`：0=十进制，1=十六进制

**响应**：`OK:SET_HEX\r\n` 或 `ERR:15:Invalid SET_HEX parameter\r\n`  
**示例**：
```
SET_HEX:1
SET_HEX:0
```

### 28. SET_PRECISION
**功能**：设置浮点数精度  
**格式**：`SET_PRECISION:<n>`  
**参数**：
- `<n>`：精度位数，范围 0-9

**响应**：`OK:SET_PRECISION\r\n` 或 `ERR:16:Invalid SET_PRECISION parameter\r\n`  
**示例**：
```
SET_PRECISION:0
SET_PRECISION:3
SET_PRECISION:9
```

### 29. SET_HEADER
**功能**：设置是否显示表头  
**格式**：`SET_HEADER:<0/1>`  
**参数**：
- `<0/1>`：0=不显示，1=显示

**响应**：`OK:SET_HEADER\r\n` 或 `ERR:20:Invalid SET_HEADER parameter\r\n`  
**示例**：
```
SET_HEADER:1
SET_HEADER:0
```

### 30. SET_MATRIX_SIZE
**功能**：设置矩阵大小  
**格式**：`SET_MATRIX_SIZE:<r>:<c>`  
**参数**：
- `<r>`：行数，范围 1-16
- `<c>`：列数，范围 1-16

**响应**：`OK:SET_MATRIX_SIZE\r\n` 或 `ERR:21:Invalid SET_MATRIX_SIZE parameters\r\n`  
**示例**：
```
SET_MATRIX_SIZE:16:16
SET_MATRIX_SIZE:8:8
SET_MATRIX_SIZE:4:4
```

---

## 队列命令

### 31. QUEUE_START
**功能**：开始命令队列  
**格式**：`QUEUE_START`  
**参数**：无  
**响应**：`OK:QUEUE_START\r\n`  
**说明**：后续命令将加入队列，直到执行 QUEUE_END  
**示例**：
```
QUEUE_START
```

### 32. QUEUE_END
**功能**：结束命令队列并执行  
**格式**：`QUEUE_END`  
**参数**：无  
**响应**：`OK:QUEUE_END\r\n`  
**说明**：执行队列中的所有命令  
**示例**：
```
QUEUE_END
```

### 33. WAIT
**功能**：在队列中等待指定时间  
**格式**：`WAIT:<ms>`  
**参数**：
- `<ms>`：等待时间（毫秒）

**响应**：无（在队列中执行）  
**说明**：只能在队列中使用  
**示例**：
```
WAIT:1000
WAIT:500
```

---

## 命令组合

### 使用 && 连接多个命令
可以使用 `&&` 将多个命令连接在一起，按顺序执行：

**格式**：
```
命令1 && 命令2 && 命令3
```

**示例**：
```
START && SET_RATE:100 && STATUS
SET_ROW:5 && SET_COL:10 && SCAN_POINT:5:10
SET_FORMAT:table && SET_MODE:raw && SET_HEX:1
```

### 完整初始化示例
```
PCAP04_TEST && PCAP04_LOAD_DEFAULT && SET_MATRIX_SIZE:16:16 && SET_FORMAT:table && SET_MODE:raw && SET_HEX:1 && STATUS
```

### 扫描序列示例
```
QUEUE_START
SCAN_POINT:0:0
WAIT:100
SCAN_POINT:0:1
WAIT:100
SCAN_POINT:0:2
WAIT:100
QUEUE_END
```

---

## 响应格式

### 成功响应
```
OK:<命令名>\r\n
```

### 错误响应
```
ERR:<错误码>:<错误信息>\r\n
```

### 数据响应
不同命令返回不同格式的数据，例如：
- `STAT:NORMAL:50:16x16\r\n`
- `ROW:5\r\n`
- `COL:8\r\n`
- `PCAP04_REG[0x10]=0x5A\r\n`

### 错误码列表
| 错误码 | 说明 |
|--------|------|
| 1 | Invalid SET_RATE parameter |
| 2 | Invalid SET_ROW parameter |
| 3 | Invalid SET_COL parameter |
| 4 | Invalid SCAN_POINT parameters |
| 5 | Invalid WAIT parameter |
| 10 | Invalid PCAP04_READ parameter |
| 11 | Invalid PCAP04_WRITE parameters |
| 12 | Invalid SET_CDIFF parameter |
| 13 | Invalid SET_INTREF parameter |
| 14 | Invalid SET_EXTREF parameter |
| 15 | Invalid SET_HEX parameter |
| 16 | Invalid SET_PRECISION parameter |
| 17 | Invalid SET_MODE parameter |
| 18 | Invalid SET_FORMAT parameter |
| 19 | Invalid SET_TABLE_DELIM parameter |
| 20 | Invalid SET_HEADER parameter |
| 21 | Invalid SET_MATRIX_SIZE parameters |
| 255 | Unknown command |

---

## 使用示例

### 示例 1：基本扫描流程
```
# 1. 查询系统状态
STATUS

# 2. 设置扫描参数
SET_RATE:100
SET_MATRIX_SIZE:16:16

# 3. 设置输出格式
SET_FORMAT:table
SET_MODE:raw
SET_HEX:1
SET_HEADER:1

# 4. 启动扫描
START

# 5. 停止扫描
STOP
```

### 示例 2：单点扫描
```
# 扫描指定点
SCAN_POINT:5:10

# 获取当前行列
GET_ROW
GET_COL

# 扫描多个点
SCAN_POINT:0:0 && SCAN_POINT:1:1 && SCAN_POINT:2:2
```

### 示例 3：PCAP04 配置
```
# 测试通信
PCAP04_TEST

# 加载默认配置
PCAP04_LOAD_DEFAULT

# 设置参考模式
SET_INTREF:1
SET_EXTREF:0
SET_CDIFF:1

# 查看状态
PCAP04_STATUS
```

### 示例 4：输出格式配置
```
# 表格格式，原始数据，十六进制
SET_FORMAT:table
SET_MODE:raw
SET_HEX:1
SET_HEADER:1
SET_TABLE_DELIM:,

# 简单格式，量化数据，浮点数
SET_FORMAT:simple
SET_MODE:quant
SET_PRECISION:3
```

### 示例 5：命令队列
```
# 创建扫描序列
QUEUE_START
SCAN_POINT:0:0
WAIT:100
SCAN_POINT:0:1
WAIT:100
SCAN_POINT:0:2
WAIT:100
SCAN_POINT:1:0
WAIT:100
SCAN_POINT:1:1
WAIT:100
QUEUE_END
```

### 示例 6：完整初始化序列
```
# 系统初始化
PCAP04_TEST
PCAP04_LOAD_DEFAULT
SET_MATRIX_SIZE:16:16
SET_FORMAT:table
SET_MODE:raw
SET_HEX:1
SET_HEADER:1
SET_TABLE_DELIM:,
SET_RATE:50
NORMAL_MODE
STATUS
```

---

## 注意事项

1. **命令大小写**：命令名和参数不区分大小写
2. **参数格式**：数字参数支持十进制和十六进制（0x前缀）
3. **命令长度**：单条命令最大 256 字符
4. **参数数量**：最多 8 个参数
5. **响应格式**：所有响应以 `\r\n` 结尾
6. **错误处理**：命令执行失败会返回错误码和错误信息
7. **队列使用**：WAIT 命令只能在队列中使用
8. **扫描速率**：范围 1-10000 毫秒
9. **矩阵大小**：行和列范围 0-15
10. **精度设置**：浮点数精度范围 0-9 位

---

## 快速参考

### 最常用命令
```
HELP              # 显示帮助
STATUS            # 查询状态
START             # 启动扫描
STOP              # 停止扫描
SET_RATE:100      # 设置扫描速率
SCAN_POINT:5:10   # 扫描指定点
```

### 配置命令
```
SET_FORMAT:table      # 表格格式
SET_MODE:raw          # 原始数据
SET_HEX:1             # 十六进制
SET_HEADER:1          # 显示表头
SET_MATRIX_SIZE:16:16 # 矩阵大小
```

### PCAP04 命令
```
PCAP04_TEST           # 测试通信
PCAP04_STATUS         # 查看状态
PCAP04_LOAD_DEFAULT   # 加载默认配置
PCAP04_READ:0x10      # 读取寄存器
PCAP04_WRITE:0x10:0x5A # 写入寄存器
```

---

**文档版本**：1.0  
**最后更新**：2025-01-XX
