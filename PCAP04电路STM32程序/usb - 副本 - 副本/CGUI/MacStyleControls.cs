using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

namespace USBMatrixDisplay
{
    // Mac风格圆角按钮
    public class MacStyleButton : Button
    {
        private bool isHovered = false;
        private bool isPressed = false;

        public MacStyleButton()
        {
            this.FlatStyle = FlatStyle.Flat;
            this.FlatAppearance.BorderSize = 0;
            this.BackColor = Color.FromArgb(0, 122, 255); // Mac蓝色
            this.ForeColor = Color.White;
            this.Font = new Font("Segoe UI", 11, FontStyle.Regular);
            this.Cursor = Cursors.Hand;
            this.Padding = new Padding(20, 8, 20, 8);
            this.Size = new Size(100, 36);
        }

        protected override void OnMouseEnter(EventArgs e)
        {
            base.OnMouseEnter(e);
            isHovered = true;
            this.Invalidate();
        }

        protected override void OnMouseLeave(EventArgs e)
        {
            base.OnMouseLeave(e);
            isHovered = false;
            isPressed = false;
            this.Invalidate();
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown(e);
            isPressed = true;
            this.Invalidate();
        }

        protected override void OnMouseUp(MouseEventArgs e)
        {
            base.OnMouseUp(e);
            isPressed = false;
            this.Invalidate();
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);

            Graphics g = e.Graphics;
            g.SmoothingMode = SmoothingMode.AntiAlias;
            g.PixelOffsetMode = PixelOffsetMode.HighQuality;

            Rectangle rect = this.ClientRectangle;
            rect.Width--;
            rect.Height--;

            Color buttonColor = this.BackColor;
            if (isPressed)
            {
                buttonColor = Color.FromArgb(
                    Math.Max(0, buttonColor.R - 20),
                    Math.Max(0, buttonColor.G - 20),
                    Math.Max(0, buttonColor.B - 20));
            }
            else if (isHovered)
            {
                buttonColor = Color.FromArgb(
                    Math.Min(255, buttonColor.R + 10),
                    Math.Min(255, buttonColor.G + 10),
                    Math.Min(255, buttonColor.B + 10));
            }

            using (GraphicsPath path = GetRoundedRectangle(rect, 8))
            {
                using (SolidBrush brush = new SolidBrush(buttonColor))
                {
                    g.FillPath(brush, path);
                }

                // 添加阴影效果
                if (!isPressed)
                {
                    using (GraphicsPath shadowPath = GetRoundedRectangle(
                        new Rectangle(rect.X + 1, rect.Y + 2, rect.Width, rect.Height), 8))
                    {
                        using (SolidBrush shadowBrush = new SolidBrush(Color.FromArgb(30, 0, 0, 0)))
                        {
                            g.FillPath(shadowBrush, shadowPath);
                        }
                    }
                }
            }

            // 绘制文字
            TextRenderer.DrawText(g, this.Text, this.Font, rect, this.ForeColor,
                TextFormatFlags.HorizontalCenter | TextFormatFlags.VerticalCenter);
        }

        private GraphicsPath GetRoundedRectangle(Rectangle rect, int radius)
        {
            GraphicsPath path = new GraphicsPath();
            path.AddArc(rect.X, rect.Y, radius * 2, radius * 2, 180, 90);
            path.AddArc(rect.Right - radius * 2, rect.Y, radius * 2, radius * 2, 270, 90);
            path.AddArc(rect.Right - radius * 2, rect.Bottom - radius * 2, radius * 2, radius * 2, 0, 90);
            path.AddArc(rect.X, rect.Bottom - radius * 2, radius * 2, radius * 2, 90, 90);
            path.CloseAllFigures();
            return path;
        }
    }

    // Mac风格面板
    public class MacStylePanel : Panel
    {
        public MacStylePanel()
        {
            this.DoubleBuffered = true;
            this.BackColor = Color.FromArgb(245, 245, 250);
            this.SetStyle(ControlStyles.AllPaintingInWmPaint | 
                         ControlStyles.UserPaint | 
                         ControlStyles.DoubleBuffer | 
                         ControlStyles.ResizeRedraw, true);
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);

            Graphics g = e.Graphics;
            g.SmoothingMode = SmoothingMode.AntiAlias;

            Rectangle rect = this.ClientRectangle;
            rect.Width--;
            rect.Height--;

            using (GraphicsPath path = GetRoundedRectangle(rect, 12))
            {
                using (SolidBrush brush = new SolidBrush(this.BackColor))
                {
                    g.FillPath(brush, path);
                }

                // 添加边框
                using (Pen pen = new Pen(Color.FromArgb(220, 220, 225), 1))
                {
                    g.DrawPath(pen, path);
                }

                // 添加阴影
                using (GraphicsPath shadowPath = GetRoundedRectangle(
                    new Rectangle(rect.X + 2, rect.Y + 3, rect.Width, rect.Height), 12))
                {
                    using (SolidBrush shadowBrush = new SolidBrush(Color.FromArgb(20, 0, 0, 0)))
                    {
                        g.FillPath(shadowBrush, shadowPath);
                    }
                }
            }
        }

        private GraphicsPath GetRoundedRectangle(Rectangle rect, int radius)
        {
            GraphicsPath path = new GraphicsPath();
            path.AddArc(rect.X, rect.Y, radius * 2, radius * 2, 180, 90);
            path.AddArc(rect.Right - radius * 2, rect.Y, radius * 2, radius * 2, 270, 90);
            path.AddArc(rect.Right - radius * 2, rect.Bottom - radius * 2, radius * 2, radius * 2, 0, 90);
            path.AddArc(rect.X, rect.Bottom - radius * 2, radius * 2, radius * 2, 90, 90);
            path.CloseAllFigures();
            return path;
        }
    }

    // Mac风格标签
    public class MacStyleLabel : Label
    {
        public MacStyleLabel()
        {
            this.ForeColor = Color.FromArgb(50, 50, 50);
            this.Font = new Font("Segoe UI", 10, FontStyle.Regular);
            this.BackColor = Color.Transparent;
        }
    }

    // Mac风格文本框容器（用于包装TextBox实现圆角效果）
    public class MacStyleTextBoxContainer : Panel
    {
        private TextBox textBox;

        public TextBox TextBox => textBox;

        public MacStyleTextBoxContainer()
        {
            this.BackColor = Color.Transparent;
            this.Padding = new Padding(0);
            
            textBox = new TextBox
            {
                BorderStyle = BorderStyle.None,
                BackColor = Color.White,
                ForeColor = Color.FromArgb(50, 50, 50),
                Font = new Font("Segoe UI", 10, FontStyle.Regular),
                Dock = DockStyle.Fill,
                Margin = new Padding(8)
            };
            this.Controls.Add(textBox);
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);

            Graphics g = e.Graphics;
            g.SmoothingMode = SmoothingMode.AntiAlias;

            Rectangle rect = this.ClientRectangle;
            rect.Width--;
            rect.Height--;

            // 绘制白色背景
            using (GraphicsPath path = GetRoundedRectangle(rect, 8))
            {
                using (SolidBrush brush = new SolidBrush(Color.White))
                {
                    g.FillPath(brush, path);
                }

                // 绘制边框
                using (Pen pen = new Pen(Color.FromArgb(200, 200, 205), 1))
                {
                    g.DrawPath(pen, path);
                }
            }
        }

        private GraphicsPath GetRoundedRectangle(Rectangle rect, int radius)
        {
            GraphicsPath path = new GraphicsPath();
            path.AddArc(rect.X, rect.Y, radius * 2, radius * 2, 180, 90);
            path.AddArc(rect.Right - radius * 2, rect.Y, radius * 2, radius * 2, 270, 90);
            path.AddArc(rect.Right - radius * 2, rect.Bottom - radius * 2, radius * 2, radius * 2, 0, 90);
            path.AddArc(rect.X, rect.Bottom - radius * 2, radius * 2, radius * 2, 90, 90);
            path.CloseAllFigures();
            return path;
        }
    }
}

