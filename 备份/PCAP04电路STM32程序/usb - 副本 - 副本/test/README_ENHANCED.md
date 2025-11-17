# PCAP04 配置工具增强功能说明

## 新增功能

### 1. C 代码配置解析
- `verify_config.c` 现在输出 JSON 格式的详细配置信息
- 包含每个寄存器的值（十六进制、十进制、二进制）
- 支持位域解析

### 2. GUI 详细说明显示
- 从 `1.txt` 解析寄存器详细说明
- 在 GUI 中显示每个寄存器的：
  - 寄存器名称和描述
  - 详细说明文本
  - 注意事项（黄色高亮显示）
  - 每个位域的详细说明
  - 当前配置值（从 verify_config.exe 解析）

## 使用步骤

### 第一步：生成配置文件

运行更新脚本（Windows）：
```bash
cd PCAP04_USB/test
update_configs.bat
```

或手动执行：

1. **解析寄存器详细说明**：
   ```bash
   python parse_register_docs.py
   ```
   这会从 `1.txt` 解析寄存器说明，生成 `register_docs.json`

2. **编译 verify_config.exe**：
   ```bash
   gcc -o verify_config.exe verify_config.c ../Core/Src/pcap04_reg.c -I ../Core/Inc -D__GNUC__
   ```

3. **运行并解析配置**：
   ```bash
   python parse_verify_output.py
   ```
   这会运行 `verify_config.exe`，解析输出，生成 `parsed_config_detailed.json`

### 第二步：启动 GUI

```bash
python config_gui.py
```

GUI 会自动加载：
- `ui_options.json` - 寄存器配置选项
- `register_docs.json` - 寄存器详细说明（从 1.txt）
- `parsed_config_detailed.json` - 当前配置值（从 verify_config.exe）

## GUI 功能说明

### 寄存器信息显示

选择寄存器后，右侧面板会显示：

1. **寄存器标题**
   - 寄存器编号和地址
   - 当前配置值（如果已解析）

2. **寄存器详细说明**
   - 寄存器名称
   - 详细描述文本
   - 注意事项（黄色背景高亮）

3. **位域配置**
   - 每个位域的：
     - 名称和位范围
     - 当前值（如果已解析）
     - 详细说明
     - 配置选项（下拉框或输入框）

### 当前值显示

如果运行了 `parse_verify_output.py`，GUI 会显示：
- 寄存器级别的当前值
- 位域级别的当前值
- 自动选择匹配的选项

## 文件说明

### 输入文件
- `1.txt` - 寄存器详细说明文档（Markdown 格式）
- `verify_config.c` - C 验证程序（已增强，输出 JSON）

### 生成的配置文件
- `register_docs.json` - 从 1.txt 解析的寄存器详细说明
- `parsed_config_detailed.json` - 从 verify_config.exe 解析的当前配置

### 脚本文件
- `parse_register_docs.py` - 解析 1.txt，生成 register_docs.json
- `parse_verify_output.py` - 运行 verify_config.exe 并解析输出
- `update_configs.bat` - 一键更新所有配置文件

## 示例输出

### verify_config.exe 输出（JSON 格式）
```json
{
  "registers": [
    {
      "reg_num": 0,
      "hex_address": "0x00",
      "value": "0x57",
      "value_decimal": 87,
      "value_binary": "0b01010111"
    },
    ...
  ]
}
```

### register_docs.json 结构
```json
{
  "0": {
    "number": 0,
    "hex_address": "0x00",
    "name": "IIC地址与低频时钟配置",
    "description": "...",
    "bit_fields": {
      "I2C_A": {
        "bits": "[7:6]",
        "description": "I²C设备地址补码配置",
        "options": [...]
      }
    },
    "notes": ["..."]
  }
}
```

## 故障排除

### verify_config.exe 编译失败
- 检查 gcc 是否安装
- 检查路径是否正确（Core/Src/pcap04_reg.c, Core/Inc）

### register_docs.json 为空或不完整
- 检查 1.txt 文件格式是否正确
- 运行 `parse_register_docs.py` 查看错误信息

### GUI 中看不到详细说明
- 确保已运行 `parse_register_docs.py` 生成 `register_docs.json`
- 检查文件是否在正确位置（test 目录下）

### 当前值不显示
- 确保已运行 `parse_verify_output.py` 生成 `parsed_config_detailed.json`
- 确保 `verify_config.exe` 已编译并可以运行

## 工作流程

```
1.txt (寄存器说明文档)
    ↓
parse_register_docs.py
    ↓
register_docs.json
    ↓
    ┌─────────────────┐
    │   config_gui.py │
    └─────────────────┘
    ↑              ↑
    │              │
verify_config.c   ui_options.json
    ↓              (已存在)
verify_config.exe
    ↓
parse_verify_output.py
    ↓
parsed_config_detailed.json
```

## 下一步改进

- [ ] 支持更多寄存器的位域解析
- [ ] 添加配置验证功能
- [ ] 支持配置对比（当前配置 vs 默认配置）
- [ ] 添加配置导入/导出功能

