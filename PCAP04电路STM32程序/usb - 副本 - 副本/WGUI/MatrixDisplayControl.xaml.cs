using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace USBMatrixDisplay
{
    public partial class MatrixDisplayControl : UserControl
    {
        private byte[,] byteMatrix = new byte[16, 16];
        private float[,] floatMatrix = new float[16, 16];
        private TextBlock[,] cells = new TextBlock[16, 16];
        private const int CellPadding = 3;
        private bool isFloatMode = false;

        public MatrixDisplayControl()
        {
            InitializeComponent();
            InitializeMatrix();
            this.SizeChanged += MatrixDisplayControl_SizeChanged;
        }

        private void InitializeMatrix()
        {
            // 直接使用 Canvas 来布局
            MatrixCanvas.Children.Clear();

            for (int row = 0; row < 16; row++)
            {
                for (int col = 0; col < 16; col++)
                {
                    var border = new Border
                    {
                        Background = new SolidColorBrush(Color.FromRgb(245, 245, 250)),
                        CornerRadius = new CornerRadius(2),
                        Child = new TextBlock
                        {
                            Text = "00",
                            TextAlignment = TextAlignment.Center,
                            VerticalAlignment = VerticalAlignment.Center,
                            FontFamily = new FontFamily("Segoe UI"),
                            FontSize = 11,
                            FontWeight = FontWeights.Bold,
                            Foreground = new SolidColorBrush(Color.FromRgb(30, 30, 30))
                        }
                    };

                    var textBlock = (TextBlock)border.Child;
                    cells[row, col] = textBlock;

                    MatrixCanvas.Children.Add(border);
                }
            }

            UpdateCellLayout();
        }

        private void MatrixDisplayControl_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            UpdateCellLayout();
        }

        private void UpdateCellLayout()
        {
            if (cells[0, 0] == null || MatrixCanvas.Children.Count == 0) return;

            double availableWidth = this.ActualWidth;
            double availableHeight = this.ActualHeight;

            // 如果尺寸无效，跳过更新
            if (availableWidth <= 0 || availableHeight <= 0) return;

            // 计算整个矩阵应该占用的正方形大小（保持16x16的长宽比）
            // 取可用宽高的最小值，确保矩阵保持正方形
            double matrixSize = Math.Min(availableWidth, availableHeight);
            
            // 计算单元格大小（16个单元格 + 15个间距）
            double totalPadding = CellPadding * 15; // 15个间距（16个单元格之间有15个间距）
            double cellSize = Math.Max(10, (matrixSize - totalPadding) / 16);

            // 计算整个矩阵的实际大小
            double actualMatrixSize = cellSize * 16 + totalPadding;

            // 计算居中偏移量
            double offsetX = (availableWidth - actualMatrixSize) / 2;
            double offsetY = (availableHeight - actualMatrixSize) / 2;

            // 计算字体大小 - 进一步增大字体
            double fontSize = Math.Max(10, Math.Min(16, cellSize / 2.8));

            // 更新每个单元格的位置和大小
            int index = 0;
            foreach (UIElement element in MatrixCanvas.Children)
            {
                if (element is Border border)
                {
                    int row = index / 16;
                    int col = index % 16;
                    
                    // 设置单元格大小
                    border.Width = cellSize;
                    border.Height = cellSize;
                    
                    // 计算单元格位置（相对于矩阵左上角）
                    double cellX = col * (cellSize + CellPadding);
                    double cellY = row * (cellSize + CellPadding);
                    
                    // 使用 Canvas 设置位置（加上居中偏移）
                    Canvas.SetLeft(border, offsetX + cellX);
                    Canvas.SetTop(border, offsetY + cellY);

                    if (border.Child is TextBlock textBlock)
                    {
                        textBlock.FontSize = fontSize;
                    }
                    
                    index++;
                }
            }
        }

        public void SetValue(int row, int col, byte value)
        {
            if (row >= 0 && row < 16 && col >= 0 && col < 16)
            {
                isFloatMode = false;
                byteMatrix[row, col] = value;
                if (cells[row, col] != null)
                {
                    cells[row, col].Text = value.ToString("X2");
                    UpdateCellColor(cells[row, col], value / 255.0f);
                }
            }
        }

        public void SetFloatValue(int row, int col, float value)
        {
            if (row >= 0 && row < 16 && col >= 0 && col < 16)
            {
                isFloatMode = true;
                floatMatrix[row, col] = value;
                if (cells[row, col] != null)
                {
                    // 格式化显示
                    if (Math.Abs(value) >= 1000 || (Math.Abs(value) < 0.01 && value != 0))
                    {
                        cells[row, col].Text = value.ToString("E2");
                        cells[row, col].FontSize = 10;
                    }
                    else
                    {
                        cells[row, col].Text = value.ToString("F2");
                        cells[row, col].FontSize = 11;
                    }

                    // 根据值的范围设置颜色
                    float normalizedValue = NormalizeFloat(value);
                    UpdateCellColor(cells[row, col], normalizedValue);
                }
            }
        }

        private float NormalizeFloat(float value)
        {
            // 简单的归一化：假设值在-100到100之间，映射到0-1
            float normalized = (value + 100) / 200.0f;
            return Math.Max(0, Math.Min(1, normalized));
        }

        private void UpdateCellColor(TextBlock textBlock, float intensity)
        {
            // Mac风格渐变：从浅蓝到深蓝
            byte r = (byte)(245 - intensity * 50);
            byte g = (byte)(245 - intensity * 30);
            byte b = (byte)(255 - intensity * 20);

            var border = (Border)textBlock.Parent;
            border.Background = new SolidColorBrush(Color.FromRgb(r, g, b));
            textBlock.Foreground = new SolidColorBrush(Colors.Black);
            textBlock.FontWeight = FontWeights.Bold;
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

