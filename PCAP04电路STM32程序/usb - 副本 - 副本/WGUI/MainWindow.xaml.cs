using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Threading;

namespace USBMatrixDisplay
{
    public partial class MainWindow : Window
    {
        private SerialPort? serialPort;
        private Dictionary<string, string> portNameMap = new Dictionary<string, string>();
        private string? lastConnectedPort = null;
        private int receivedBytesCount = 0;
        private enum DisplayMode { Hex, String }
        private DisplayMode displayMode = DisplayMode.String;
        private bool autoReconnectEnabled = false;
        private StringBuilder textBuffer = new StringBuilder();
        private int matrixIndex = 0;
        private bool foundStartMarker = false;
        private int scanCount = 0;
        private const string START_MARKER = "--- Starting 16x16 Matrix Scan";
        private const string END_MARKER = "--- 16x16 Matrix Scan Complete";
        private Queue<byte[]> dataQueue = new Queue<byte[]>();
        private double splitterPosition = 0.5; // 分隔条位置（0-1）
        private bool isDragging = false;
        private Point dragStartPoint;

        // 定时器
        private DispatcherTimer autoReconnectTimer = null!;
        private DispatcherTimer connectionMonitorTimer = null!;
        private DispatcherTimer uiUpdateTimer = null!;

        public MainWindow()
        {
            InitializeComponent();
            InitializeTimers();
            RefreshPortList();
            InitializeSplitter();
        }

        private void InitializeSplitter()
        {
            // 初始化分隔条位置为50%
            LeftColumn.Width = new GridLength(1, GridUnitType.Star);
            RightColumn.Width = new GridLength(1, GridUnitType.Star);
            splitterPosition = 0.5;
        }

        private void InitializeTimers()
        {
            // 自动重连定时器
            autoReconnectTimer = new DispatcherTimer
            {
                Interval = TimeSpan.FromSeconds(2),
                IsEnabled = false
            };
            autoReconnectTimer.Tick += AutoReconnectTimer_Tick;

            // 连接状态监控定时器
            connectionMonitorTimer = new DispatcherTimer
            {
                Interval = TimeSpan.FromSeconds(1),
                IsEnabled = false
            };
            connectionMonitorTimer.Tick += ConnectionMonitorTimer_Tick;

            // UI更新定时器
            uiUpdateTimer = new DispatcherTimer
            {
                Interval = TimeSpan.FromMilliseconds(50),
                IsEnabled = false
            };
            uiUpdateTimer.Tick += UiUpdateTimer_Tick;
        }

        private void RefreshPortList()
        {
            PortComboBox.Items.Clear();
            portNameMap.Clear();

            List<SerialPortInfo> ports = SerialPortHelper.GetSerialPortsWithDescription();

            foreach (var portInfo in ports)
            {
                PortComboBox.Items.Add(portInfo.DisplayName);
                portNameMap[portInfo.DisplayName] = portInfo.PortName;
            }

            if (ports.Count > 0)
            {
                PortComboBox.SelectedIndex = 0;
            }
        }

        private void RefreshButton_Click(object sender, RoutedEventArgs e)
        {
            RefreshPortList();
        }

        private void ConnectButton_Click(object sender, RoutedEventArgs e)
        {
            if (PortComboBox.SelectedItem == null)
            {
                MessageBox.Show("请选择串口！", "错误", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }

            string displayName = PortComboBox.SelectedItem.ToString() ?? "";
            if (portNameMap.TryGetValue(displayName, out string? portName))
            {
                ConnectToPort(portName);
            }
            else
            {
                MessageBox.Show("无法获取串口名称！", "错误", MessageBoxButton.OK, MessageBoxImage.Warning);
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
                ConnectButton.IsEnabled = false;
                ConnectButton.Background = new System.Windows.Media.SolidColorBrush(System.Windows.Media.Color.FromRgb(128, 128, 128));
                DisconnectButton.IsEnabled = true;
                DisconnectButton.Background = new System.Windows.Media.SolidColorBrush(System.Windows.Media.Color.FromRgb(255, 0, 0));
                PortComboBox.IsEnabled = false;

                // 重置数据缓冲区和解析状态
                textBuffer.Clear();
                matrixIndex = 0;
                receivedBytesCount = 0;
                foundStartMarker = false;
                scanCount = 0;
                ReceivedDataLabel.Content = "接收字节数: 0";

                // 显示完整名称
                string displayName = PortComboBox.SelectedItem?.ToString() ?? portName;
                StatusLabel.Content = $"状态: 已连接到 {displayName}";
                StatusLabel.Foreground = new System.Windows.Media.SolidColorBrush(System.Windows.Media.Color.FromRgb(0, 200, 0));

                autoReconnectTimer.IsEnabled = false;
                connectionMonitorTimer.IsEnabled = true;
            }
            catch (Exception ex)
            {
                if (!autoReconnectEnabled)
                {
                    MessageBox.Show($"连接失败: {ex.Message}", "错误", MessageBoxButton.OK, MessageBoxImage.Error);
                }
                else
                {
                    StatusLabel.Content = $"状态: 连接失败 - {ex.Message}";
                    StatusLabel.Foreground = new System.Windows.Media.SolidColorBrush(System.Windows.Media.Color.FromRgb(128, 128, 128));
                }
            }
        }

        private void DisconnectButton_Click(object sender, RoutedEventArgs e)
        {
            DisconnectPort();
        }

        private void DisconnectPort()
        {
            try
            {
                connectionMonitorTimer.IsEnabled = false;
                uiUpdateTimer.IsEnabled = false;

                lock (dataQueue)
                {
                    dataQueue.Clear();
                }

                if (serialPort != null)
                {
                    serialPort.DataReceived -= SerialPort_DataReceived;
                    serialPort.ErrorReceived -= SerialPort_ErrorReceived;

                    Thread.Sleep(50);

                    if (serialPort.IsOpen)
                    {
                        try
                        {
                            serialPort.Close();
                        }
                        catch { }
                    }

                    try
                    {
                        serialPort.Dispose();
                    }
                    catch { }

                    serialPort = null;
                }

                ConnectButton.IsEnabled = true;
                ConnectButton.Background = new System.Windows.Media.SolidColorBrush(System.Windows.Media.Color.FromRgb(0, 200, 0));
                DisconnectButton.IsEnabled = false;
                DisconnectButton.Background = new System.Windows.Media.SolidColorBrush(System.Windows.Media.Color.FromRgb(128, 128, 128));
                PortComboBox.IsEnabled = true;
                StatusLabel.Content = "状态: 未连接";
                StatusLabel.Foreground = new System.Windows.Media.SolidColorBrush(System.Windows.Media.Color.FromRgb(128, 128, 128));

                if (autoReconnectEnabled && !string.IsNullOrEmpty(lastConnectedPort))
                {
                    autoReconnectTimer.IsEnabled = true;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"断开连接失败: {ex.Message}", "错误", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void SerialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            if (serialPort == null) return;

            try
            {
                if (!serialPort.IsOpen) return;

                int bytesToRead = serialPort.BytesToRead;
                if (bytesToRead > 0)
                {
                    byte[] buffer = new byte[bytesToRead];
                    int bytesRead = serialPort.Read(buffer, 0, bytesToRead);

                    if (bytesRead > 0)
                    {
                        byte[] dataToQueue = new byte[bytesRead];
                        Array.Copy(buffer, 0, dataToQueue, 0, bytesRead);

                        lock (dataQueue)
                        {
                            dataQueue.Enqueue(dataToQueue);
                        }

                        if (!uiUpdateTimer.IsEnabled)
                        {
                            Dispatcher.BeginInvoke(new Action(() =>
                            {
                                uiUpdateTimer.IsEnabled = true;
                            }));
                        }
                    }
                }
            }
            catch (System.IO.IOException)
            {
                Dispatcher.BeginInvoke(new Action(() =>
                {
                    HandlePortDisconnection();
                }));
            }
            catch (Exception ex)
            {
                Dispatcher.BeginInvoke(new Action(() =>
                {
                    StatusLabel.Content = $"接收错误: {ex.Message}";
                    StatusLabel.Foreground = new System.Windows.Media.SolidColorBrush(System.Windows.Media.Color.FromRgb(128, 128, 128));
                    if (autoReconnectEnabled)
                    {
                        HandlePortDisconnection();
                    }
                }));
            }
        }

        private void SerialPort_ErrorReceived(object sender, SerialErrorReceivedEventArgs e)
        {
            Dispatcher.BeginInvoke(new Action(() =>
            {
                HandlePortDisconnection();
            }));
        }

        private void HandlePortDisconnection()
        {
            if (serialPort != null)
            {
                try
                {
                    uiUpdateTimer.IsEnabled = false;

                    lock (dataQueue)
                    {
                        dataQueue.Clear();
                    }

                    serialPort.DataReceived -= SerialPort_DataReceived;
                    serialPort.ErrorReceived -= SerialPort_ErrorReceived;

                    if (serialPort.IsOpen)
                    {
                        try
                        {
                            serialPort.Close();
                        }
                        catch { }
                    }

                    try
                    {
                        serialPort.Dispose();
                    }
                    catch { }

                    serialPort = null;
                }
                catch { }

                ConnectButton.IsEnabled = true;
                ConnectButton.Background = new System.Windows.Media.SolidColorBrush(System.Windows.Media.Color.FromRgb(0, 200, 0));
                DisconnectButton.IsEnabled = false;
                DisconnectButton.Background = new System.Windows.Media.SolidColorBrush(System.Windows.Media.Color.FromRgb(128, 128, 128));
                PortComboBox.IsEnabled = true;
                StatusLabel.Content = "状态: 连接已断开";
                StatusLabel.Foreground = new System.Windows.Media.SolidColorBrush(System.Windows.Media.Color.FromRgb(128, 128, 128));

                if (autoReconnectEnabled && !string.IsNullOrEmpty(lastConnectedPort))
                {
                    autoReconnectTimer.IsEnabled = true;
                }
            }
        }

        private void UiUpdateTimer_Tick(object? sender, EventArgs e)
        {
            List<byte[]> batchData = new List<byte[]>();

            lock (dataQueue)
            {
                while (dataQueue.Count > 0 && batchData.Count < 10)
                {
                    batchData.Add(dataQueue.Dequeue());
                }

                if (dataQueue.Count == 0)
                {
                    uiUpdateTimer.IsEnabled = false;
                }
            }

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
            ReceivedDataLabel.Content = $"接收字节数: {receivedBytesCount}";

            string displayText = "";
            switch (displayMode)
            {
                case DisplayMode.String:
                    try
                    {
                        string text = Encoding.ASCII.GetString(data, 0, length);
                        textBuffer.Append(text);
                        ParseTextFloatValues();
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
                    displayText = BitConverter.ToString(data, 0, length).Replace("-", " ");
                    break;
            }

            if (!string.IsNullOrEmpty(displayText))
            {
                if (DataTextBox.Text.Length > 10000)
                {
                    DataTextBox.Text = DataTextBox.Text.Substring(5000) + displayText;
                }
                else
                {
                    DataTextBox.AppendText(displayText);
                }
                // 滚动到底部 - 设置光标位置会自动滚动
                DataTextBox.CaretIndex = DataTextBox.Text.Length;
                DataTextBox.SelectionStart = DataTextBox.Text.Length;
                DataTextBox.SelectionLength = 0;
            }

            switch (displayMode)
            {
                case DisplayMode.String:
                    break;

                case DisplayMode.Hex:
                default:
                    for (int i = 0; i < length && i < 256; i++)
                    {
                        int row = i / 16;
                        int col = i % 16;
                        MatrixPanel.SetValue(row, col, data[i]);
                    }
                    break;
            }
        }

        private void ParseTextFloatValues()
        {
            string bufferText = textBuffer.ToString();

            int startMarkerIndex = bufferText.IndexOf(START_MARKER);
            if (startMarkerIndex >= 0)
            {
                if (!foundStartMarker || scanCount < 2)
                {
                    foundStartMarker = true;
                    scanCount++;

                    int afterMarkerIndex = bufferText.IndexOf('\n', startMarkerIndex);
                    if (afterMarkerIndex < 0)
                    {
                        afterMarkerIndex = bufferText.IndexOf('\r', startMarkerIndex);
                    }

                    if (afterMarkerIndex >= 0)
                    {
                        bufferText = bufferText.Substring(afterMarkerIndex + 1);
                        textBuffer.Clear();
                        textBuffer.Append(bufferText);
                        matrixIndex = 0;
                    }
                    else
                    {
                        if (startMarkerIndex > 0)
                        {
                            textBuffer.Remove(0, startMarkerIndex);
                        }
                        return;
                    }
                }
            }

            if (bufferText.Contains(END_MARKER))
            {
                foundStartMarker = false;
                scanCount = 0;

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
                        textBuffer.Clear();
                        textBuffer.Append(bufferText.Substring(endMarkerIndex));
                    }
                }
            }

            string[] lines = bufferText.Split(new[] { "\r\n", "\r", "\n" }, StringSplitOptions.None);

            for (int i = 0; i < lines.Length - 1; i++)
            {
                string line = lines[i].Trim();
                if (string.IsNullOrEmpty(line))
                    continue;

                if (line.Contains("Return value") ||
                    (line.Contains("---") && (line.Contains("Starting") || line.Contains("Complete"))) ||
                    (line.StartsWith(".") && line.EndsWith(".") && line.Length > 10))
                    continue;

                string[] values = line.Split(new[] { ' ', '\t' }, StringSplitOptions.RemoveEmptyEntries);

                foreach (string valueStr in values)
                {
                    string trimmed = valueStr.Trim();
                    if (string.IsNullOrEmpty(trimmed) || trimmed.Length > 20)
                        continue;

                    if (float.TryParse(trimmed, System.Globalization.NumberStyles.Float,
                        System.Globalization.CultureInfo.InvariantCulture, out float value))
                    {
                        if (matrixIndex < 256)
                        {
                            int row = matrixIndex / 16;
                            int col = matrixIndex % 16;
                            MatrixPanel.SetFloatValue(row, col, value);
                            matrixIndex++;

                            if (matrixIndex >= 256)
                            {
                                matrixIndex = 0;
                            }
                        }
                    }
                }
            }

            if (lines.Length > 0)
            {
                string remainingText = lines[lines.Length - 1];
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
            if (serialPort != null && serialPort.IsOpen)
            {
                autoReconnectTimer.IsEnabled = false;
                return;
            }

            try
            {
                RefreshPortList();

                string? targetPort = null;
                foreach (string displayName in PortComboBox.Items)
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
                    StatusLabel.Content = "状态: 检测到STMicroelectronics设备，正在自动连接...";
                    StatusLabel.Foreground = new System.Windows.Media.SolidColorBrush(System.Windows.Media.Color.FromRgb(255, 149, 0));

                    try
                    {
                        string? displayName = portNameMap.FirstOrDefault(x => x.Value == targetPort).Key;
                        if (displayName != null && PortComboBox.Items.Contains(displayName))
                        {
                            PortComboBox.SelectedItem = displayName;
                            ConnectToPort(targetPort);
                        }
                    }
                    catch { }
                }
            }
            catch { }
        }

        private void ConnectionMonitorTimer_Tick(object? sender, EventArgs e)
        {
            if (serialPort == null)
            {
                connectionMonitorTimer.IsEnabled = false;
                return;
            }

            try
            {
                bool isOpen = serialPort.IsOpen;
                string[] availablePorts = SerialPort.GetPortNames();
                if (!isOpen || !availablePorts.Contains(lastConnectedPort ?? ""))
                {
                    HandlePortDisconnection();
                }
            }
            catch
            {
                HandlePortDisconnection();
            }
        }

        private void HexModeRadio_Checked(object sender, RoutedEventArgs e)
        {
            if (HexModeRadio.IsChecked == true)
            {
                displayMode = DisplayMode.Hex;
                textBuffer.Clear();
                matrixIndex = 0;
                foundStartMarker = false;
                scanCount = 0;
            }
        }

        private void StringModeRadio_Checked(object sender, RoutedEventArgs e)
        {
            if (StringModeRadio.IsChecked == true)
            {
                displayMode = DisplayMode.String;
                textBuffer.Clear();
                matrixIndex = 0;
                foundStartMarker = false;
                scanCount = 0;
            }
        }

        private void AutoReconnectCheckBox_Checked(object sender, RoutedEventArgs e)
        {
            autoReconnectEnabled = true;
            autoReconnectTimer.IsEnabled = (serialPort == null || !serialPort.IsOpen);
        }

        private void AutoReconnectCheckBox_Unchecked(object sender, RoutedEventArgs e)
        {
            autoReconnectEnabled = false;
            autoReconnectTimer.IsEnabled = false;
        }

        private void ClearButton_Click(object sender, RoutedEventArgs e)
        {
            MatrixPanel.Clear();
            DataTextBox.Clear();
            receivedBytesCount = 0;
            matrixIndex = 0;
            textBuffer.Clear();
            foundStartMarker = false;
            scanCount = 0;
            ReceivedDataLabel.Content = "接收字节数: 0";
        }

        private void Splitter_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                isDragging = true;
                dragStartPoint = e.GetPosition(this);
                ((Border)sender).CaptureMouse();
            }
        }

        private void Splitter_MouseMove(object sender, MouseEventArgs e)
        {
            if (isDragging && e.LeftButton == MouseButtonState.Pressed)
            {
                Point currentPoint = e.GetPosition(this);
                double deltaX = currentPoint.X - dragStartPoint.X;
                double totalWidth = this.ActualWidth - 40; // 减去左右边距

                if (totalWidth > 0)
                {
                    double newPosition = splitterPosition + (deltaX / totalWidth);
                    newPosition = Math.Max(0.1, Math.Min(0.9, newPosition)); // 限制在10%到90%之间
                    splitterPosition = newPosition;

                    // 更新列宽（使用Star单位保持比例）
                    double leftRatio = splitterPosition;
                    double rightRatio = 1 - splitterPosition;
                    LeftColumn.Width = new GridLength(leftRatio, GridUnitType.Star);
                    RightColumn.Width = new GridLength(rightRatio, GridUnitType.Star);

                    dragStartPoint = currentPoint;
                }
            }
        }

        private void Splitter_MouseUp(object sender, MouseButtonEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Released)
            {
                isDragging = false;
                ((Border)sender).ReleaseMouseCapture();
            }
        }

        protected override void OnClosing(System.ComponentModel.CancelEventArgs e)
        {
            autoReconnectTimer?.Stop();
            connectionMonitorTimer?.Stop();
            uiUpdateTimer?.Stop();

            if (serialPort != null && serialPort.IsOpen)
            {
                try
                {
                    serialPort.Close();
                    serialPort.Dispose();
                }
                catch { }
            }

            base.OnClosing(e);
        }
    }
}