using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Management;

namespace USBMatrixDisplay
{
    public class SerialPortInfo
    {
        public string PortName { get; set; } = "";
        public string DisplayName { get; set; } = "";
    }

    public static class SerialPortHelper
    {
        public static List<SerialPortInfo> GetSerialPortsWithDescription()
        {
            List<SerialPortInfo> ports = new List<SerialPortInfo>();
            
            try
            {
                // 获取所有串口名称
                string[] portNames = SerialPort.GetPortNames();
                
                // 使用WMI获取串口详细信息 - 查询串口设备
                using (ManagementObjectSearcher searcher = new ManagementObjectSearcher(
                    "SELECT Name, Description, DeviceID, PNPDeviceID FROM Win32_PnPEntity WHERE ClassGuid='{4d36e978-e325-11ce-bfc1-08002be10318}'"))
                {
                    Dictionary<string, string> portDescriptions = new Dictionary<string, string>();
                    
                    foreach (ManagementObject obj in searcher.Get())
                    {
                        string deviceId = obj["DeviceID"]?.ToString() ?? "";
                        string description = obj["Description"]?.ToString() ?? "";
                        string name = obj["Name"]?.ToString() ?? "";
                        string pnpDeviceId = obj["PNPDeviceID"]?.ToString() ?? "";
                        
                        // 从Name、Description或DeviceID中提取COM端口号
                        string searchText = !string.IsNullOrEmpty(name) ? name : 
                                         (!string.IsNullOrEmpty(description) ? description : deviceId);
                        
                        int comIndex = searchText.IndexOf("COM", StringComparison.OrdinalIgnoreCase);
                        if (comIndex >= 0)
                        {
                            int start = comIndex + 3;
                            int end = start;
                            while (end < searchText.Length && char.IsDigit(searchText[end]))
                            {
                                end++;
                            }
                            if (end > start)
                            {
                                string comPort = "COM" + searchText.Substring(start, end - start);
                                
                                // 优先使用Description，如果没有则使用Name，最后使用PNPDeviceID中的信息
                                string displayDesc = "";
                                if (!string.IsNullOrEmpty(description))
                                {
                                    displayDesc = description;
                                }
                                else if (!string.IsNullOrEmpty(name))
                                {
                                    displayDesc = name;
                                }
                                else if (!string.IsNullOrEmpty(pnpDeviceId))
                                {
                                    // 从PNPDeviceID中提取厂商信息
                                    if (pnpDeviceId.Contains("STMicroelectronics", StringComparison.OrdinalIgnoreCase))
                                    {
                                        displayDesc = "STMicroelectronics Virtual COM Port";
                                    }
                                    else if (pnpDeviceId.Contains("USB\\", StringComparison.OrdinalIgnoreCase))
                                    {
                                        displayDesc = "USB 串行设备";
                                    }
                                }
                                
                                if (!string.IsNullOrEmpty(displayDesc) && !portDescriptions.ContainsKey(comPort))
                                {
                                    portDescriptions[comPort] = displayDesc;
                                }
                            }
                        }
                    }
                    
                    // 也查询Win32_SerialPort获取更多信息
                    using (ManagementObjectSearcher serialSearcher = new ManagementObjectSearcher(
                        "SELECT DeviceID, Description, Name FROM Win32_SerialPort"))
                    {
                        foreach (ManagementObject obj in serialSearcher.Get())
                        {
                            string deviceId = obj["DeviceID"]?.ToString() ?? "";
                            string description = obj["Description"]?.ToString() ?? "";
                            string name = obj["Name"]?.ToString() ?? "";
                            
                            if (deviceId.StartsWith("COM", StringComparison.OrdinalIgnoreCase))
                            {
                                string comPort = deviceId;
                                if (!portDescriptions.ContainsKey(comPort))
                                {
                                    string displayDesc = !string.IsNullOrEmpty(description) ? description : name;
                                    if (!string.IsNullOrEmpty(displayDesc))
                                    {
                                        portDescriptions[comPort] = displayDesc;
                                    }
                                }
                            }
                        }
                    }
                    
                    // 组合端口名称和描述
                    foreach (string portName in portNames)
                    {
                        string displayName = portName;
                        if (portDescriptions.TryGetValue(portName, out string? description) && !string.IsNullOrEmpty(description))
                        {
                            displayName = $"{description} ({portName})";
                        }
                        
                        ports.Add(new SerialPortInfo
                        {
                            PortName = portName,
                            DisplayName = displayName
                        });
                    }
                }
            }
            catch
            {
                // 如果WMI查询失败，回退到基本方法
                string[] portNames = SerialPort.GetPortNames();
                foreach (string portName in portNames)
                {
                    ports.Add(new SerialPortInfo
                    {
                        PortName = portName,
                        DisplayName = portName
                    });
                }
            }
            
            return ports;
        }
    }
}

