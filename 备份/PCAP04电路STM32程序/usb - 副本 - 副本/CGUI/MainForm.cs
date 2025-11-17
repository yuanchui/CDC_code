using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.IO.Ports;
using System.Linq;
using System.Windows.Forms;

namespace USBMatrixDisplay
{
    public partial class MainForm : Form
    {
        private SerialPort? serialPort;
        private MatrixDisplayPanel matrixPanel = null!;
        private ComboBox portComboBox = null!;
        private MacStyleButton connectButton = null!;
        private MacStyleButton disconnectButton = null!;
        private MacStyleButton refreshButton = null!;
        private MacStyleButton clearButton = null!;
        private MacStyleLabel statusLabel = null!;
        private MacStyleLabel receivedDataLabel = null!;
        private TextBox dataTextBox = null!;
        private RadioButton hexModeRadio = null!;
        private RadioButton stringModeRadio = null!;
        private CheckBox autoReconnectCheckBox = null!;
        private System.Windows.Forms.Timer autoReconnectTimer = null!;
        private System.Windows.Forms.Timer connectionMonitorTimer = null!;
        private Dictionary<string, string> portNameMap = new Dictionary<string, string>(); // DisplayName -> PortName
        private string? lastConnectedPort = null;
        private int receivedBytesCount = 0;
        private enum DisplayMode { Hex, String }
        private DisplayMode displayMode = DisplayMode.String; // 显示模式（默认字符串）
        private bool autoReconnectEnabled = false; // 自动重连开关
        private bool isResizing = false;
        private System.Text.StringBuilder textBuffer = new System.Text.StringBuilder(); // 文本缓冲区，用于累积接收到的文本数据
        private int matrixIndex = 0; // 当前矩阵填充位置
        private Panel? splitterPanel = null!; // 可拖拽的分隔条
        private bool foundStartMarker = false; // 是否已找到开头标志
        private int scanCount = 0; // 扫描次数（用于最多第二次矫正）
        private const string START_MARKER = "--- Starting 16x16 Matrix Scan"; // 开头标志
        private const string END_MARKER = "--- 16x16 Matrix Scan Complete"; // 结尾标志
        private System.Windows.Forms.Timer uiUpdateTimer = null!; // UI更新定时器，用于批量更新减少卡顿
        private Queue<byte[]> dataQueue = new Queue<byte[]>(); // 数据队列，用于批量处理
        private MacStylePanel? matrixContainer = null!;
        private MacStylePanel? dataContainer = null!;
        private int splitterPosition = 50; // 分隔条位置百分比（50%表示左右各占一半）
        private bool isDragging = false;

        public MainForm()
        {
            InitializeComponent();
        }

        private void InitializeComponent()
        {
            // Mac风格窗体设置
            this.Text = "USB 数据接收 - 矩阵显示";
            this.Size = new Size(1200, 850);
            this.MinimumSize = new Size(1000, 700);
            this.StartPosition = FormStartPosition.CenterScreen;
            this.FormBorderStyle = FormBorderStyle.Sizable;
            this.MaximizeBox = true;
            this.BackColor = Color.FromArgb(245, 245, 250);
            this.Padding = new Padding(20);

            // 初始化自动重连定时器
            autoReconnectTimer = new System.Windows.Forms.Timer
            {
                Interval = 2000, // 每2秒检测一次
                Enabled = false
            };
            autoReconnectTimer.Tick += AutoReconnectTimer_Tick;

            // 初始化连接状态监控定时器
            connectionMonitorTimer = new System.Windows.Forms.Timer
            {
                Interval = 1000, // 每1秒检测一次连接状态
                Enabled = false
            };
            connectionMonitorTimer.Tick += ConnectionMonitorTimer_Tick;

            // 初始化UI更新定时器（用于批量更新，减少卡顿）
            uiUpdateTimer = new System.Windows.Forms.Timer
            {
                Interval = 50, // 每50ms批量更新一次UI
                Enabled = false
            };
            uiUpdateTimer.Tick += UiUpdateTimer_Tick;

            // 窗口大小改变事件（使用延迟处理优化性能）
            this.Resize += MainForm_Resize;
            this.ResizeBegin += (s, e) => 
            { 
                isResizing = true;
                // 暂停布局更新以减少拖拽时的卡顿
                this.SuspendLayout();
            };
            this.ResizeEnd += (s, e) => 
            { 
                isResizing = false;
                this.ResumeLayout(false);
                AdjustLayout();
            };

            // 串口选择面板
            MacStylePanel portPanel = new MacStylePanel
            {
                Location = new Point(20, 20),
                Size = new Size(750, 130),
                BackColor = Color.White,
                Anchor = AnchorStyles.Top | AnchorStyles.Left | AnchorStyles.Right
            };
            this.Controls.Add(portPanel);

            MacStyleLabel portLabel = new MacStyleLabel
            {
                Text = "串口:",
                Location = new Point(15, 15),
                Size = new Size(50, 23),
                TextAlign = ContentAlignment.MiddleLeft
            };
            portPanel.Controls.Add(portLabel);

            portComboBox = new ComboBox
            {
                Location = new Point(70, 12),
                Size = new Size(300, 28),
                DropDownStyle = ComboBoxStyle.DropDownList,
                FlatStyle = FlatStyle.Flat,
                Font = new Font("Segoe UI", 10),
                IntegralHeight = false,
                DrawMode = DrawMode.OwnerDrawVariable
            };
            portComboBox.DrawItem += PortComboBox_DrawItem;
            portComboBox.MeasureItem += PortComboBox_MeasureItem;
            RefreshPortList();
            portPanel.Controls.Add(portComboBox);

            connectButton = new MacStyleButton
            {
                Text = "连接",
                Location = new Point(385, 10),
                Size = new Size(90, 32),
                BackColor = Color.FromArgb(0, 200, 0) // 初始为亮绿色
            };
            connectButton.Click += ConnectButton_Click;
            portPanel.Controls.Add(connectButton);

            disconnectButton = new MacStyleButton
            {
                Text = "断开",
                Location = new Point(485, 10),
                Size = new Size(90, 32),
                BackColor = Color.FromArgb(128, 128, 128), // 初始为灰色
                Enabled = false
            };
            disconnectButton.Click += DisconnectButton_Click;
            portPanel.Controls.Add(disconnectButton);

            refreshButton = new MacStyleButton
            {
                Text = "刷新",
                Location = new Point(585, 10),
                Size = new Size(90, 32),
                BackColor = Color.FromArgb(142, 142, 147)
            };
            refreshButton.Click += (s, e) => RefreshPortList();
            portPanel.Controls.Add(refreshButton);

            // 状态显示和自动重连（第一行）
            statusLabel = new MacStyleLabel
            {
                Text = "状态: 未连接",
                Location = new Point(15, 50),
                Size = new Size(400, 20),
                ForeColor = Color.FromArgb(128, 128, 128) // 灰色
            };
            portPanel.Controls.Add(statusLabel);

            // 自动连接复选框（第一行，状态后面）
            autoReconnectCheckBox = new CheckBox
            {
                Text = "自动连接",
                Location = new Point(420, 48),
                Size = new Size(100, 23),
                Font = new Font("Segoe UI", 10),
                ForeColor = Color.FromArgb(50, 50, 50)
            };
            autoReconnectCheckBox.CheckedChanged += (s, e) =>
            {
                autoReconnectEnabled = autoReconnectCheckBox.Checked;
                autoReconnectTimer.Enabled = autoReconnectEnabled && (serialPort == null || !serialPort.IsOpen);
            };
            portPanel.Controls.Add(autoReconnectCheckBox);

            // 数据显示模式选择
            MacStylePanel modePanel = new MacStylePanel
            {
                Location = new Point(790, 20),
                Size = new Size(370, 130),
                BackColor = Color.White,
                Anchor = AnchorStyles.Top | AnchorStyles.Right
            };
            this.Controls.Add(modePanel);

            // 显示模式和单选按钮一行
            MacStyleLabel modeLabel = new MacStyleLabel
            {
                Text = "显示模式:",
                Location = new Point(15, 15),
                Size = new Size(80, 23)
            };
            modePanel.Controls.Add(modeLabel);

            hexModeRadio = new RadioButton
            {
                Text = "十六进制",
                Location = new Point(100, 15),
                Size = new Size(80, 23),
                Font = new Font("Segoe UI", 10),
                ForeColor = Color.FromArgb(50, 50, 50),
                Checked = false
            };
            hexModeRadio.CheckedChanged += (s, e) => 
            { 
                if (hexModeRadio.Checked) 
                {
                    displayMode = DisplayMode.Hex;
                    textBuffer.Clear();
                    matrixIndex = 0;
                    foundStartMarker = false;
                    scanCount = 0;
                }
            };
            modePanel.Controls.Add(hexModeRadio);

            stringModeRadio = new RadioButton
            {
                Text = "字符串",
                Location = new Point(190, 15),
                Size = new Size(80, 23),
                Font = new Font("Segoe UI", 10),
                ForeColor = Color.FromArgb(50, 50, 50),
                Checked = true
            };
            stringModeRadio.CheckedChanged += (s, e) => 
            { 
                if (stringModeRadio.Checked) 
                {
                    displayMode = DisplayMode.String;
                    textBuffer.Clear();
                    matrixIndex = 0;
                    foundStartMarker = false;
                    scanCount = 0;
                }
            };
            modePanel.Controls.Add(stringModeRadio);

            // 接收数据统计（第二行）
            receivedDataLabel = new MacStyleLabel
            {
                Text = "接收字节数: 0",
                Location = new Point(15, 50),
                Size = new Size(300, 20)
            };
            modePanel.Controls.Add(receivedDataLabel);

            // 16x16矩阵显示面板
            MacStyleLabel matrixLabel = new MacStyleLabel
            {
                Text = "16x16 数据矩阵",
                Location = new Point(20, 170),
                Size = new Size(200, 25),
                Font = new Font("Segoe UI", 12, FontStyle.Bold),
                Anchor = AnchorStyles.Top | AnchorStyles.Left
            };
            this.Controls.Add(matrixLabel);

            matrixContainer = new MacStylePanel
            {
                Location = new Point(20, 200),
                Size = new Size(560, 560),
                BackColor = Color.White,
                Padding = new Padding(15),
                Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left
            };
            this.Controls.Add(matrixContainer);

            matrixPanel = new MatrixDisplayPanel
            {
                Dock = DockStyle.Fill,
                BackColor = Color.Transparent
            };
            matrixContainer.Controls.Add(matrixPanel);

            // 可拖拽的分隔条
            splitterPanel = new Panel
            {
                BackColor = Color.FromArgb(200, 200, 205),
                Cursor = Cursors.VSplit,
                Width = 5,
                Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left
            };
            splitterPanel.MouseDown += SplitterPanel_MouseDown;
            splitterPanel.MouseMove += SplitterPanel_MouseMove;
            splitterPanel.MouseUp += SplitterPanel_MouseUp;
            splitterPanel.BringToFront(); // 确保分隔条在最上层
            this.Controls.Add(splitterPanel);

            // 原始数据显示区域
            MacStyleLabel dataLabel = new MacStyleLabel
            {
                Text = "接收的原始数据",
                Location = new Point(600, 170),
                Size = new Size(300, 25),
                Font = new Font("Segoe UI", 12, FontStyle.Bold),
                Anchor = AnchorStyles.Top | AnchorStyles.Right
            };
            this.Controls.Add(dataLabel);

            dataContainer = new MacStylePanel
            {
                Location = new Point(600, 200),
                Size = new Size(560, 560),
                BackColor = Color.White,
                Padding = new Padding(10),
                Anchor = AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Right
            };
            this.Controls.Add(dataContainer);

            dataTextBox = new TextBox
            {
                Dock = DockStyle.Fill,
                Multiline = true,
                ScrollBars = ScrollBars.Vertical,
                ReadOnly = true,
                Font = new Font("Consolas", 9),
                BorderStyle = BorderStyle.None,
                BackColor = Color.FromArgb(250, 250, 255),
                Margin = new Padding(0, 0, 0, 50),
                Cursor = Cursors.IBeam
            };
            // 启用拖拽选择文本
            dataTextBox.MouseDown += (s, e) =>
            {
                if (e.Button == MouseButtons.Left)
                {
                    dataTextBox.SelectionLength = 0;
                }
            };
            dataContainer.Controls.Add(dataTextBox);

            clearButton = new MacStyleButton
            {
                Text = "清空显示",
                Size = new Size(120, 32),
                BackColor = Color.FromArgb(142, 142, 147),
                Anchor = AnchorStyles.Bottom | AnchorStyles.Left
            };
            clearButton.Location = new Point(10, dataContainer.Height - clearButton.Height - 10);
            clearButton.Click += (s, e) =>
            {
                matrixPanel.Clear();
                dataTextBox.Clear();
                receivedBytesCount = 0;
                matrixIndex = 0;
                textBuffer.Clear();
                foundStartMarker = false;
                scanCount = 0;
                receivedDataLabel.Text = "接收字节数: 0";
            };
            dataContainer.Controls.Add(clearButton);
            
            // 初始布局调整
            AdjustLayout();
        }

        private void MainForm_Resize(object? sender, EventArgs e)
        {
            // 如果正在调整大小，延迟处理以避免卡顿
            if (isResizing)
            {
                return;
            }
            
            // 使用SuspendLayout优化性能
            this.SuspendLayout();
            try
            {
                AdjustLayout();
            }
            finally
            {
                this.ResumeLayout(false);
            }
        }

        private void ConnectionMonitorTimer_Tick(object? sender, EventArgs e)
        {
            // 检测串口连接状态
            if (serialPort == null)
            {
                connectionMonitorTimer.Enabled = false;
                return;
            }

            try
            {
                // 尝试读取串口状态
                bool isOpen = serialPort.IsOpen;
                
                // 检查串口是否还在可用列表中
                string[] availablePorts = SerialPort.GetPortNames();
                if (!isOpen || !availablePorts.Contains(lastConnectedPort ?? ""))
                {
                    // 连接已断开
                    this.Invoke(new Action(() =>
                    {
                        HandlePortDisconnection();
                    }));
                }
            }
            catch
            {
                // 如果检测失败，可能是连接断开
                this.Invoke(new Action(() =>
                {
                    HandlePortDisconnection();
                }));
            }
        }

        private void AdjustLayout()
        {
            if (matrixContainer == null || dataContainer == null || splitterPanel == null)
                return;

            // 调整矩阵容器和数据容器的位置和大小
            int availableWidth = this.ClientSize.Width - 40; // 减去左右边距
            int availableHeight = this.ClientSize.Height - 250; // 减去顶部控件和边距
            int startY = 200;
            
            // 根据分隔条位置计算左右宽度
            int splitterWidth = 5;
            int leftWidth = (int)(availableWidth * splitterPosition / 100.0) - splitterWidth / 2;
            int rightWidth = availableWidth - leftWidth - splitterWidth;
            int leftX = 20;
            int splitterX = leftX + leftWidth;
            int rightX = splitterX + splitterWidth;
            
            // 更新矩阵容器
            matrixContainer.Location = new Point(leftX, startY);
            matrixContainer.Size = new Size(leftWidth, availableHeight);
            
            // 更新分隔条
            splitterPanel.Location = new Point(splitterX, startY);
            splitterPanel.Height = availableHeight;
            
            // 更新数据容器
            dataContainer.Location = new Point(rightX, startY);
            dataContainer.Size = new Size(rightWidth, availableHeight);
            
            // 更新标签位置
            Control? matrixLabel = null;
            Control? dataLabel = null;
            foreach (Control ctrl in this.Controls)
            {
                if (ctrl is MacStyleLabel label)
                {
                    if (label.Text == "16x16 数据矩阵")
                    {
                        matrixLabel = label;
                    }
                    else if (label.Text == "接收的原始数据")
                    {
                        dataLabel = label;
                    }
                }
            }
            
            if (matrixLabel != null)
            {
                matrixLabel.Location = new Point(leftX, 170);
            }
            
            if (dataLabel != null)
            {
                dataLabel.Location = new Point(rightX, 170);
            }
            
            // 更新清空按钮位置
            if (clearButton != null)
            {
                clearButton.Location = new Point(10, dataContainer.Height - clearButton.Height - 10);
            }
        }

        private void SplitterPanel_MouseDown(object? sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                isDragging = true;
                if (splitterPanel != null)
                {
                    splitterPanel.Capture = true;
                }
            }
        }

        private void SplitterPanel_MouseMove(object? sender, MouseEventArgs e)
        {
            if (isDragging && matrixContainer != null && dataContainer != null && splitterPanel != null)
            {
                int availableWidth = this.ClientSize.Width - 40;
                // 计算鼠标在窗口中的绝对位置
                Point mousePos = this.PointToClient(Control.MousePosition);
                int mouseX = mousePos.X;
                int newPosition = (int)((mouseX - 20) * 100.0 / availableWidth);
                
                // 限制分隔条位置在10%到90%之间
                newPosition = Math.Max(10, Math.Min(90, newPosition));
                splitterPosition = newPosition;
                
                AdjustLayout();
            }
        }

        private void SplitterPanel_MouseUp(object? sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                isDragging = false;
                if (splitterPanel != null)
                {
                    splitterPanel.Capture = false;
                }
            }
        }

        private void RefreshPortList()
        {
            portComboBox.Items.Clear();
            portNameMap.Clear();
            
            List<SerialPortInfo> ports = SerialPortHelper.GetSerialPortsWithDescription();
            
            foreach (var portInfo in ports)
            {
                portComboBox.Items.Add(portInfo.DisplayName);
                portNameMap[portInfo.DisplayName] = portInfo.PortName;
            }
            
            if (ports.Count > 0)
            {
                portComboBox.SelectedIndex = 0;
            }
        }

        private void PortComboBox_DrawItem(object? sender, DrawItemEventArgs e)
        {
            if (e.Index < 0) return;
            
            e.DrawBackground();
            
            if (sender is ComboBox comboBox && e.Index < comboBox.Items.Count)
            {
                string displayName = comboBox.Items[e.Index]?.ToString() ?? "";
                using (Brush brush = new SolidBrush(e.ForeColor))
                {
                    e.Graphics.DrawString(displayName, comboBox.Font, brush, e.Bounds);
                }
            }
            
            e.DrawFocusRectangle();
        }

        private void PortComboBox_MeasureItem(object? sender, MeasureItemEventArgs e)
        {
            if (sender is ComboBox comboBox && e.Index >= 0 && e.Index < comboBox.Items.Count)
            {
                string displayName = comboBox.Items[e.Index]?.ToString() ?? "";
                SizeF size = e.Graphics.MeasureString(displayName, comboBox.Font);
                e.ItemHeight = (int)size.Height + 4;
                e.ItemWidth = (int)size.Width;
            }
        }

        private void ConnectButton_Click(object? sender, EventArgs e)
        {
            if (portComboBox.SelectedItem == null)
            {
                MessageBox.Show("请选择串口！", "错误", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }

            string displayName = portComboBox.SelectedItem.ToString() ?? "";
            if (portNameMap.TryGetValue(displayName, out string? portName))
            {
                ConnectToPort(portName);
            }
            else
            {
                MessageBox.Show("无法获取串口名称！", "错误", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        private void ConnectToPort(string portName)
        {
            try
            {
                serialPort = new SerialPort(portName)
                {
                    BaudRate = 115200,
                    DataBits = 8,
                    Parity = Parity.None,
                    StopBits = StopBits.One,
                    ReadTimeout = 1000,
                    WriteTimeout = 1000
                };

                serialPort.DataReceived += SerialPort_DataReceived;
                serialPort.ErrorReceived += SerialPort_ErrorReceived;
                serialPort.Open();

                lastConnectedPort = portName;
                connectButton.Enabled = false;
                connectButton.BackColor = Color.FromArgb(128, 128, 128); // 灰色
                disconnectButton.Enabled = true;
                disconnectButton.BackColor = Color.FromArgb(255, 0, 0); // 红色
                portComboBox.Enabled = false;
                
                // 重置数据缓冲区和解析状态
                textBuffer.Clear();
                matrixIndex = 0;
                receivedBytesCount = 0;
                foundStartMarker = false;
                scanCount = 0;
                receivedDataLabel.Text = "接收字节数: 0";
                
                // 显示完整名称
                string displayName = portComboBox.SelectedItem?.ToString() ?? portName;
                statusLabel.Text = $"状态: 已连接到 {displayName}";
                statusLabel.ForeColor = Color.FromArgb(0, 200, 0); // 亮绿色
                
                autoReconnectTimer.Enabled = false; // 连接成功后停止自动重连检测
                connectionMonitorTimer.Enabled = true; // 启动连接状态监控
            }
            catch (Exception ex)
            {
                if (!autoReconnectEnabled) // 只有在非自动重连模式下才显示错误消息
                {
                    MessageBox.Show($"连接失败: {ex.Message}", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                else
                {
                    statusLabel.Text = $"状态: 连接失败 - {ex.Message}";
                    statusLabel.ForeColor = Color.FromArgb(128, 128, 128); // 灰色
                }
            }
        }

        private void DisconnectButton_Click(object? sender, EventArgs e)
        {
            DisconnectPort();
        }

        private void DisconnectPort()
        {
            try
            {
                connectionMonitorTimer.Enabled = false; // 停止连接状态监控
                uiUpdateTimer.Enabled = false; // 停止UI更新定时器
                
                // 清空数据队列
                lock (dataQueue)
                {
                    dataQueue.Clear();
                }
                
                if (serialPort != null)
                {
                    // 先移除事件处理器，避免在关闭时触发事件导致卡住
                    serialPort.DataReceived -= SerialPort_DataReceived;
                    serialPort.ErrorReceived -= SerialPort_ErrorReceived;
                    
                    // 等待一小段时间，让正在处理的事件完成
                    System.Threading.Thread.Sleep(50);
                    
                    if (serialPort.IsOpen)
                    {
                        try
                        {
                            serialPort.Close();
                        }
                        catch { } // 忽略关闭时的异常
                    }
                    
                    try
                    {
                        serialPort.Dispose();
                    }
                    catch { } // 忽略释放时的异常
                    
                    serialPort = null;
                }

                connectButton.Enabled = true;
                connectButton.BackColor = Color.FromArgb(0, 200, 0); // 亮绿色
                disconnectButton.Enabled = false;
                disconnectButton.BackColor = Color.FromArgb(128, 128, 128); // 灰色
                portComboBox.Enabled = true;
                statusLabel.Text = "状态: 未连接";
                statusLabel.ForeColor = Color.FromArgb(128, 128, 128); // 灰色

                // 如果启用了自动重连，启动定时器
                if (autoReconnectEnabled && !string.IsNullOrEmpty(lastConnectedPort))
                {
                    autoReconnectTimer.Enabled = true;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"断开连接失败: {ex.Message}", "错误", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void SerialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            // 检查串口状态，避免在断开时处理数据
            if (serialPort == null)
                return;

            try
            {
                // 再次检查串口是否打开
                if (!serialPort.IsOpen)
                    return;

                int bytesToRead = serialPort.BytesToRead;
                if (bytesToRead > 0)
                {
                    byte[] buffer = new byte[bytesToRead];
                    int bytesRead = serialPort.Read(buffer, 0, bytesToRead);

                    // 将数据加入队列，由定时器批量处理，减少UI更新频率
                    if (this.IsHandleCreated && !this.IsDisposed && bytesRead > 0)
                    {
                        // 创建正确大小的数组
                        byte[] dataToQueue = new byte[bytesRead];
                        Array.Copy(buffer, 0, dataToQueue, 0, bytesRead);
                        
                        lock (dataQueue)
                        {
                            dataQueue.Enqueue(dataToQueue);
                        }
                        
                        // 启动UI更新定时器（如果未启动）
                        if (!uiUpdateTimer.Enabled)
                        {
                            this.BeginInvoke(new Action(() =>
                            {
                                uiUpdateTimer.Enabled = true;
                            }));
                        }
                    }
                }
            }
            catch (System.IO.IOException)
            {
                // 串口已断开，这是正常情况，不需要处理
                if (this.IsHandleCreated && !this.IsDisposed)
                {
                    this.BeginInvoke(new Action(() =>
                    {
                        HandlePortDisconnection();
                    }));
                }
            }
            catch (Exception ex)
            {
                // 其他错误，记录但不阻塞
                if (this.IsHandleCreated && !this.IsDisposed)
                {
                    this.BeginInvoke(new Action(() =>
                    {
                        statusLabel.Text = $"接收错误: {ex.Message}";
                        statusLabel.ForeColor = Color.FromArgb(128, 128, 128); // 灰色
                        // 如果发生错误，可能是设备断开，触发断开处理
                        if (autoReconnectEnabled)
                        {
                            HandlePortDisconnection();
                        }
                    }));
                }
            }
        }

        private void SerialPort_ErrorReceived(object sender, SerialErrorReceivedEventArgs e)
        {
            // 串口错误，可能是设备断开
            if (this.IsHandleCreated && !this.IsDisposed)
            {
                this.BeginInvoke(new Action(() =>
                {
                    HandlePortDisconnection();
                }));
            }
        }

        private void HandlePortDisconnection()
        {
            // 处理串口意外断开
            if (serialPort != null)
            {
                try
                {
                    uiUpdateTimer.Enabled = false; // 停止UI更新定时器
                    
                    // 清空数据队列
                    lock (dataQueue)
                    {
                        dataQueue.Clear();
                    }
                    
                    // 先移除事件处理器
                    serialPort.DataReceived -= SerialPort_DataReceived;
                    serialPort.ErrorReceived -= SerialPort_ErrorReceived;
                    
                    if (serialPort.IsOpen)
                    {
                        try
                        {
                            serialPort.Close();
                        }
                        catch { } // 忽略关闭时的异常
                    }
                    
                    try
                    {
                        serialPort.Dispose();
                    }
                    catch { } // 忽略释放时的异常
                    
                    serialPort = null;
                }
                catch { }

                connectButton.Enabled = true;
                connectButton.BackColor = Color.FromArgb(0, 200, 0); // 亮绿色
                disconnectButton.Enabled = false;
                disconnectButton.BackColor = Color.FromArgb(128, 128, 128); // 灰色
                portComboBox.Enabled = true;
                statusLabel.Text = "状态: 连接已断开";
                statusLabel.ForeColor = Color.FromArgb(128, 128, 128); // 灰色

                // 如果启用了自动重连，启动定时器
                if (autoReconnectEnabled && !string.IsNullOrEmpty(lastConnectedPort))
                {
                    autoReconnectTimer.Enabled = true;
                }
            }
        }

        private void UiUpdateTimer_Tick(object? sender, EventArgs e)
        {
            // 批量处理队列中的数据
            List<byte[]> batchData = new List<byte[]>();
            
            lock (dataQueue)
            {
                while (dataQueue.Count > 0 && batchData.Count < 10) // 每次最多处理10个数据包
                {
                    batchData.Add(dataQueue.Dequeue());
                }
                
                // 如果队列为空，停止定时器
                if (dataQueue.Count == 0)
                {
                    uiUpdateTimer.Enabled = false;
                }
            }
            
            // 处理批量数据
            foreach (byte[] data in batchData)
            {
                if (serialPort != null && serialPort.IsOpen)
                {
                    ProcessReceivedData(data, data.Length);
                }
            }
        }

        private void ProcessReceivedData(byte[] data, int length)
        {
            receivedBytesCount += length;
            receivedDataLabel.Text = $"接收字节数: {receivedBytesCount}";

            // 使用SuspendLayout优化性能，减少重绘
            this.SuspendLayout();
            dataTextBox.SuspendLayout();
            matrixPanel.SuspendLayout();
            
            try
            {
                // 更新原始数据显示
                string displayText = "";
                switch (displayMode)
                {
                    case DisplayMode.String:
                        // 显示为字符串（ASCII编码），保留换行格式
                        try
                        {
                            string text = System.Text.Encoding.ASCII.GetString(data, 0, length);
                            // 累积到缓冲区
                            textBuffer.Append(text);
                            
                            // 按行解析浮点数值并填充矩阵
                            ParseTextFloatValues();
                            
                            // 显示文本（保留换行，不可打印字符替换为点号）
                            displayText = new string(text.Select(c => 
                                char.IsControl(c) && c != '\r' && c != '\n' ? '.' : c).ToArray());
                        }
                        catch
                        {
                            displayText = "无法解析为字符串";
                        }
                        break;
                        
                    case DisplayMode.Hex:
                    default:
                        // 显示为十六进制
                        displayText = BitConverter.ToString(data, 0, length).Replace("-", " ");
                        break;
                }

                // 追加显示文本（保留换行）
                if (!string.IsNullOrEmpty(displayText))
                {
                    if (dataTextBox.TextLength > 10000)
                    {
                        dataTextBox.Text = dataTextBox.Text.Substring(5000) + displayText;
                    }
                    else
                    {
                        dataTextBox.AppendText(displayText);
                    }
                    dataTextBox.SelectionStart = dataTextBox.TextLength;
                    dataTextBox.ScrollToCaret();
                }

                // 更新16x16矩阵显示
                switch (displayMode)
                {
                    case DisplayMode.String:
                        // 字符串模式：已在ParseTextFloatValues中处理
                        break;
                        
                    case DisplayMode.Hex:
                    default:
                        // 十六进制模式：每个字节显示
                        for (int i = 0; i < length && i < 256; i++)
                        {
                            int row = i / 16;
                            int col = i % 16;
                            matrixPanel.SetValue(row, col, data[i]);
                        }
                        break;
                }
            }
            finally
            {
                matrixPanel.ResumeLayout(false);
                dataTextBox.ResumeLayout(false);
                this.ResumeLayout(false);
            }
        }

        private void ParseTextFloatValues()
        {
            // 从文本缓冲区中按行解析浮点数值
            string bufferText = textBuffer.ToString();
            
            // 检测开头标志（最多在第二次扫描时矫正）
            int startMarkerIndex = bufferText.IndexOf(START_MARKER);
            if (startMarkerIndex >= 0)
            {
                // 找到开头标志，重置解析状态（允许在第二次扫描时矫正）
                if (!foundStartMarker || scanCount < 2)
                {
                    foundStartMarker = true;
                    scanCount++;
                    
                    // 找到开头标志后的第一个换行符位置
                    int afterMarkerIndex = bufferText.IndexOf('\n', startMarkerIndex);
                    if (afterMarkerIndex < 0)
                    {
                        afterMarkerIndex = bufferText.IndexOf('\r', startMarkerIndex);
                    }
                    
                    if (afterMarkerIndex >= 0)
                    {
                        // 从开头标志之后开始解析
                        bufferText = bufferText.Substring(afterMarkerIndex + 1);
                        textBuffer.Clear();
                        textBuffer.Append(bufferText);
                        matrixIndex = 0; // 重置矩阵索引
                    }
                    else
                    {
                        // 如果开头标志在最后但还没有换行，等待更多数据
                        // 但先清空开头标志之前的内容
                        if (startMarkerIndex > 0)
                        {
                            textBuffer.Remove(0, startMarkerIndex);
                        }
                        return; // 等待更多数据
                    }
                }
            }
            
            // 检测结尾标志，重置状态准备下一次扫描
            if (bufferText.Contains(END_MARKER))
            {
                // 找到结尾标志后，重置状态以便下一次扫描可以重新矫正
                foundStartMarker = false;
                scanCount = 0; // 重置扫描计数，允许下一次扫描时重新矫正
                
                // 清空缓冲区中结尾标志之后的内容
                int endMarkerIndex = bufferText.IndexOf(END_MARKER);
                if (endMarkerIndex >= 0)
                {
                    int afterEndIndex = bufferText.IndexOf('\n', endMarkerIndex);
                    if (afterEndIndex < 0)
                    {
                        afterEndIndex = bufferText.IndexOf('\r', endMarkerIndex);
                    }
                    if (afterEndIndex >= 0)
                    {
                        textBuffer.Clear();
                        textBuffer.Append(bufferText.Substring(afterEndIndex + 1));
                    }
                    else
                    {
                        // 如果结尾标志在最后但还没有换行，保留结尾标志等待更多数据
                        textBuffer.Clear();
                        textBuffer.Append(bufferText.Substring(endMarkerIndex));
                    }
                }
            }
            
            string[] lines = bufferText.Split(new[] { "\r\n", "\r", "\n" }, StringSplitOptions.None);
            
            // 处理每一行（除了最后一行，因为它可能不完整）
            for (int i = 0; i < lines.Length - 1; i++)
            {
                string line = lines[i].Trim();
                if (string.IsNullOrEmpty(line))
                    continue;
                
                // 跳过明显的非数据行（包含特殊字符或文本）
                if (line.Contains("Return value") || 
                    (line.Contains("---") && (line.Contains("Starting") || line.Contains("Complete"))) ||
                    (line.StartsWith(".") && line.EndsWith(".") && line.Length > 10))
                    continue;
                
                // 按空格分割，支持一行多个值
                string[] values = line.Split(new[] { ' ', '\t' }, StringSplitOptions.RemoveEmptyEntries);
                
                foreach (string valueStr in values)
                {
                    string trimmed = valueStr.Trim();
                    // 跳过空字符串和明显的非数字字符串
                    if (string.IsNullOrEmpty(trimmed) || trimmed.Length > 20)
                        continue;
                    
                    // 尝试解析为浮点数
                    if (float.TryParse(trimmed, System.Globalization.NumberStyles.Float, 
                        System.Globalization.CultureInfo.InvariantCulture, out float value))
                    {
                        // 填充到矩阵中（按行优先顺序）
                        if (matrixIndex < 256)
                        {
                            int row = matrixIndex / 16;
                            int col = matrixIndex % 16;
                            matrixPanel.SetFloatValue(row, col, value);
                            matrixIndex++;
                            
                            // 如果矩阵已满，重置索引（循环覆盖）
                            if (matrixIndex >= 256)
                            {
                                matrixIndex = 0;
                            }
                        }
                    }
                }
            }
            
            // 保留最后一行（可能不完整）在缓冲区中
            if (lines.Length > 0)
            {
                string remainingText = lines[lines.Length - 1];
                // 如果最后一行包含开头标志，需要特殊处理
                if (remainingText.Contains(START_MARKER))
                {
                    int markerIndex = remainingText.IndexOf(START_MARKER);
                    int afterMarkerIndex = remainingText.IndexOf('\n', markerIndex);
                    if (afterMarkerIndex < 0)
                    {
                        afterMarkerIndex = remainingText.IndexOf('\r', markerIndex);
                    }
                    if (afterMarkerIndex >= 0)
                    {
                        remainingText = remainingText.Substring(afterMarkerIndex + 1);
                    }
                    else
                    {
                        // 如果开头标志在最后一行但还没有换行，保留整个缓冲区等待更多数据
                        return;
                    }
                }
                
                textBuffer.Clear();
                textBuffer.Append(remainingText);
            }
            else
            {
                textBuffer.Clear();
            }
        }

        private void AutoReconnectTimer_Tick(object? sender, EventArgs e)
        {
            // 如果已经连接，停止检测
            if (serialPort != null && serialPort.IsOpen)
            {
                autoReconnectTimer.Enabled = false;
                return;
            }

            // 检查是否有STMicroelectronics Virtual COM Port设备
            try
            {
                // 更新串口列表
                RefreshPortList();
                
                // 查找包含"STMicroelectronics Virtual COM Port"的串口
                string? targetPort = null;
                foreach (string displayName in portComboBox.Items)
                {
                    if (displayName != null && displayName.Contains("STMicroelectronics Virtual COM Port"))
                    {
                        if (portNameMap.TryGetValue(displayName, out string? portName))
                        {
                            targetPort = portName;
                            break;
                        }
                    }
                }

                if (targetPort != null)
                {
                    // 找到目标设备，尝试自动连接
                    this.Invoke(new Action(() =>
                    {
                        statusLabel.Text = $"状态: 检测到STMicroelectronics设备，正在自动连接...";
                        statusLabel.ForeColor = Color.FromArgb(255, 149, 0);
                    }));

                    try
                    {
                        // 选择并连接目标串口
                        string? displayName = portNameMap.FirstOrDefault(x => x.Value == targetPort).Key;
                        if (displayName != null && portComboBox.Items.Contains(displayName))
                        {
                            portComboBox.SelectedItem = displayName;
                            ConnectToPort(targetPort);
                        }
                    }
                    catch
                    {
                        // 连接失败，继续等待下次检测
                    }
                }
            }
            catch
            {
                // 检测失败，继续等待下次检测
            }
        }

        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            autoReconnectTimer?.Stop();
            autoReconnectTimer?.Dispose();
            connectionMonitorTimer?.Stop();
            connectionMonitorTimer?.Dispose();
            uiUpdateTimer?.Stop();
            uiUpdateTimer?.Dispose();
            
            if (serialPort != null && serialPort.IsOpen)
            {
                serialPort.Close();
                serialPort.Dispose();
            }
            base.OnFormClosing(e);
        }
    }

    // 16x16矩阵显示面板（支持浮点数）
    public class MatrixDisplayPanel : Panel
    {
        private byte[,] byteMatrix = new byte[16, 16];
        private float[,] floatMatrix = new float[16, 16];
        private Label[,] labels = new Label[16, 16];
        private const int CellPadding = 3;
        private bool isFloatMode = false;

        public MatrixDisplayPanel()
        {
            this.DoubleBuffered = true;
            this.BackColor = Color.Transparent;
            this.Resize += MatrixDisplayPanel_Resize;
            InitializeMatrix();
        }

        private void InitializeMatrix()
        {
            for (int row = 0; row < 16; row++)
            {
                for (int col = 0; col < 16; col++)
                {
                    Label label = new Label
                    {
                        Text = "00",
                        TextAlign = ContentAlignment.MiddleCenter,
                        BorderStyle = BorderStyle.None,
                        BackColor = Color.FromArgb(245, 245, 250),
                        ForeColor = Color.FromArgb(30, 30, 30),
                        Font = new Font("Segoe UI", 7, FontStyle.Bold) // 使用粗体增强可读性
                    };
                    labels[row, col] = label;
                    this.Controls.Add(label);
                }
            }
            UpdateCellLayout();
        }

        private void MatrixDisplayPanel_Resize(object? sender, EventArgs e)
        {
            UpdateCellLayout();
        }

        private void UpdateCellLayout()
        {
            if (labels[0, 0] == null) return;

            // 使用SuspendLayout优化性能
            this.SuspendLayout();
            try
            {
                // 计算可用空间（减去内边距）
                int availableWidth = this.ClientSize.Width - (CellPadding * 2);
                int availableHeight = this.ClientSize.Height - (CellPadding * 2);

                // 计算单元格大小（确保16x16能完整显示）
                int cellWidth = (availableWidth - (CellPadding * 15)) / 16;
                int cellHeight = (availableHeight - (CellPadding * 15)) / 16;
                
                // 使用较小的值确保正方形
                int cellSize = Math.Max(10, Math.Min(cellWidth, cellHeight)); // 最小10px
                
                // 计算字体大小（根据单元格大小动态调整）
                int fontSize = Math.Max(6, Math.Min(12, cellSize / 4));
                
                // 计算总宽度和高度，用于居中
                int totalWidth = cellSize * 16 + CellPadding * 15;
                int totalHeight = cellSize * 16 + CellPadding * 15;
                int offsetX = (availableWidth - totalWidth) / 2 + CellPadding;
                int offsetY = (availableHeight - totalHeight) / 2 + CellPadding;

                for (int row = 0; row < 16; row++)
                {
                    for (int col = 0; col < 16; col++)
                    {
                        if (labels[row, col] != null)
                        {
                            labels[row, col].SuspendLayout();
                            labels[row, col].Location = new Point(
                                offsetX + col * (cellSize + CellPadding),
                                offsetY + row * (cellSize + CellPadding)
                            );
                            labels[row, col].Size = new Size(cellSize, cellSize);
                            
                            // 动态调整字体大小（保持粗体）
                            if (labels[row, col].Font != null)
                            {
                                labels[row, col].Font.Dispose();
                            }
                            labels[row, col].Font = new Font("Segoe UI", fontSize, FontStyle.Bold);
                            labels[row, col].ResumeLayout(false);
                        }
                    }
                }
            }
            finally
            {
                this.ResumeLayout(false);
                this.Invalidate(); // 强制重绘以清除残影
            }
        }

        public void SetValue(int row, int col, byte value)
        {
            if (row >= 0 && row < 16 && col >= 0 && col < 16)
            {
                isFloatMode = false;
                byteMatrix[row, col] = value;
                if (labels[row, col] != null)
                {
                    labels[row, col].Text = value.ToString("X2");
                    UpdateCellColor(labels[row, col], value / 255.0f);
                }
            }
        }

        public void SetFloatValue(int row, int col, float value)
        {
            if (row >= 0 && row < 16 && col >= 0 && col < 16)
            {
                isFloatMode = true;
                floatMatrix[row, col] = value;
                if (labels[row, col] != null)
                {
                    // 格式化显示：如果值很大或很小，使用科学计数法
                    float fontSize = labels[row, col].Font?.Size ?? 7;
                    FontStyle fontStyle = FontStyle.Bold; // 使用粗体增强可读性
                    
                    if (Math.Abs(value) >= 1000 || (Math.Abs(value) < 0.01 && value != 0))
                    {
                        labels[row, col].Text = value.ToString("E2");
                        fontSize = 6;
                    }
                    else
                    {
                        labels[row, col].Text = value.ToString("F2");
                        fontSize = 7;
                    }
                    
                    // 更新字体（保持粗体）
                    if (labels[row, col].Font != null)
                    {
                        labels[row, col].Font.Dispose();
                    }
                    labels[row, col].Font = new Font("Segoe UI", fontSize, fontStyle);

                    // 根据值的范围设置颜色（归一化到0-1）
                    float normalizedValue = NormalizeFloat(value);
                    UpdateCellColor(labels[row, col], normalizedValue);
                }
            }
        }

        private float NormalizeFloat(float value)
        {
            // 简单的归一化：假设值在-100到100之间，映射到0-1
            // 可以根据实际数据范围调整
            float normalized = (value + 100) / 200.0f;
            return Math.Max(0, Math.Min(1, normalized));
        }

        private void UpdateCellColor(Label label, float intensity)
        {
            // Mac风格渐变：从浅蓝到深蓝
            int r = (int)(245 - intensity * 50);
            int g = (int)(245 - intensity * 30);
            int b = (int)(255 - intensity * 20);
            label.BackColor = Color.FromArgb(r, g, b);

            // 文字颜色：始终使用黑色
            label.ForeColor = Color.Black;
            label.Font = new Font(label.Font.FontFamily, label.Font.Size, FontStyle.Bold);
        }

        public void Clear()
        {
            for (int row = 0; row < 16; row++)
            {
                for (int col = 0; col < 16; col++)
                {
                    if (isFloatMode)
                    {
                        SetFloatValue(row, col, 0);
                    }
                    else
                    {
                        SetValue(row, col, 0);
                    }
                }
            }
        }
    }
}
