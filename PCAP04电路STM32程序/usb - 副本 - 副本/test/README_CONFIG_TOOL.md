# PCAP04 配置工具使用说明

## 概述

本工具集提供了完整的 PCAP04 寄存器配置解决方案，包括：
1. **C 代码解析器** - 从 `pcap04_reg.c` 提取当前配置
2. **GUI 配置工具** - 可视化界面配置寄存器
3. **代码生成器** - 根据 GUI 配置反向生成 C 代码

## 文件说明

- `parse_c_code.py` - 解析 C 代码，提取当前配置
- `config_gui.py` - GUI 配置界面（基于 tkinter）
- `code_generator.py` - 代码生成器，根据配置生成 C 代码
- `pcap04_config_tool.py` - 主程序入口，整合所有功能

## 使用方法

### 1. 启动 GUI 工具（推荐）

```bash
python pcap04_config_tool.py --mode gui
```

或者直接运行：

```bash
python config_gui.py
```

### 2. 解析当前 C 代码配置

```bash
python pcap04_config_tool.py --mode parse --input Core/Src/pcap04_reg.c --output current_config.json
```

### 3. 从配置文件生成 C 代码

```bash
python pcap04_config_tool.py --mode generate --config gui_config.json --output generated_config.c
```

## GUI 工具使用步骤

1. **启动 GUI**
   ```bash
   python config_gui.py
   ```

2. **加载配置**
   - 工具会自动加载 `ui_options.json`（寄存器配置选项）
   - 如果存在 `current_config.json`，会自动加载当前配置值

3. **配置寄存器**
   - 在左侧树形列表中选择要配置的寄存器
   - 在右侧配置面板中修改各字段的值
   - 支持下拉选择（枚举值）和文本输入（数值）

4. **保存配置**
   - 点击"保存配置"按钮，将当前配置保存到 `gui_config.json`

5. **生成代码**
   - 点击"生成代码"按钮，在底部预览生成的 C 代码
   - 可以选择保存到文件

6. **导出 JSON**
   - 点击"导出 JSON"按钮，将配置导出为 JSON 文件

## 配置文件格式

### ui_options.json
包含所有寄存器的配置选项，从 `1.txt` 解析生成。

### gui_config.json
GUI 工具保存的配置格式：
```json
{
  "Reg0": {
    "I2C_A": "3",
    "OLF_FTUNE": "5",
    "OLF_CTUNE": "PCAP04_REG0_OLF_CTUNE_200k"
  },
  "Reg1": {
    "OX_DIS": "PCAP04_REG1_OX_DIS_ENABLE",
    "OX_DIV4": "PCAP04_REG1_OX_DIV4_NO_DIV",
    "OX_RUN": "PCAP04_REG1_OX_RUN_PERMANENT"
  }
}
```

### current_config.json
从 C 代码解析出的当前配置（可选）。

## 生成的 C 代码格式

生成的代码格式与 `pcap04_reg.c` 中的 `PCAP04_DefaultConfigItems[]` 数组格式一致：

```c
static const PCAP04_RegConfigItem_t PCAP04_DefaultConfigItems[] = {
    // Register 0 (0x00) - IIC地址与低频时钟配置
    {PCAP04_REG_ADDR_0,
     PCAP04_REG0_VALUE(
         3,                              // 地址位I2C_A
         5,                              // 低频时钟微调 OLF_FTUNE
         PCAP04_REG0_OLF_CTUNE_200k     // 低频时钟粗调 OLF_CTUNE
     )},
    // ...
};
```

## 特殊寄存器处理

以下寄存器跨多个寄存器，需要特殊处理：

- **Reg7-8**: `C_AVRG` 配置（13位，跨两个寄存器）
- **Reg9-11**: `CONV_TIME` 配置（23位，跨三个寄存器）
- **Reg12-13**: `DISCHARGE_TIME` 配置（10位，跨两个寄存器）
- **Reg14-15**: `PRECHARGE_TIME` 配置（10位，跨两个寄存器）
- **Reg16-17**: `FULLCHARGE_TIME` 配置（10位，跨两个寄存器）
- **Reg21-22**: `R_TRIG_PREDIV` 配置（10位，跨两个寄存器）

这些寄存器在 GUI 中会显示为单个配置项，代码生成器会自动处理跨寄存器的情况。

## 依赖要求

- Python 3.6+
- tkinter（GUI 工具需要，通常随 Python 安装）
- 标准库：json, os, sys, re, argparse

## 注意事项

1. **配置文件路径**：工具会在脚本目录下查找配置文件
2. **寄存器编号**：寄存器编号从 0 开始（Reg0 对应寄存器 0）
3. **固定值寄存器**：某些寄存器（如 Reg24-26 TDC 配置）为固定值，无需配置
4. **代码生成**：生成的代码可能需要手动调整某些常量名称，确保与头文件中的宏定义一致

## 故障排除

### GUI 无法启动
- 检查是否安装了 tkinter：`python -m tkinter`
- Windows 用户可能需要安装 tkinter：`pip install tk`

### 配置文件未找到
- 确保 `ui_options.json` 存在于脚本目录
- 运行 `test_parse copy.py` 生成 `ui_options.json`

### 生成的代码编译错误
- 检查常量名称是否与头文件中的宏定义一致
- 某些寄存器可能需要手动调整代码格式

## 开发说明

### 扩展代码生成器

要支持新的寄存器类型，需要：

1. 在 `code_generator.py` 的 `reg_macro_map` 中添加寄存器映射
2. 在 `_generate_register_code` 方法中添加对应的代码生成逻辑
3. 更新常量映射表（如需要）

### 添加新的配置选项

1. 更新 `ui_options.json`（通过解析 `1.txt` 生成）
2. 在 GUI 中会自动显示新的配置选项
3. 代码生成器需要相应更新以支持新选项

## 示例工作流程

1. **首次使用**：
   ```bash
   # 1. 解析寄存器说明文档，生成 UI 配置
   python test_parse\ copy.py
   
   # 2. 解析当前 C 代码配置（可选）
   python pcap04_config_tool.py --mode parse
   
   # 3. 启动 GUI 工具
   python config_gui.py
   ```

2. **配置寄存器**：
   - 在 GUI 中选择寄存器并修改配置
   - 保存配置到 `gui_config.json`

3. **生成代码**：
   - 在 GUI 中点击"生成代码"
   - 或使用命令行：`python pcap04_config_tool.py --mode generate`

4. **使用生成的代码**：
   - 将生成的代码复制到 `pcap04_reg.c` 的 `PCAP04_DefaultConfigItems[]` 数组中
   - 重新编译项目

