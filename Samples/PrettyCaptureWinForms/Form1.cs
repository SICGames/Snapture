using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using com.HellstormGames.ScreenCapture;
using com.HellstormGames.Imaging;

namespace PrettyCaptureWinForms
{
    public partial class Form1 : Form
    {

        public Bitmap CapturedBitmap { get; private set; }
        public Snapture Snapture { get; private set; }  

        public Form1()
        {
            InitializeComponent();
            comboBox1.SelectedIndexChanged += ComboBox1_SelectedIndexChanged;
            this.Load += Form1_Load;
            this.FormClosing += Form1_FormClosing;
            button1.Click += Button1_Click;
        }

        private void Button1_Click(object sender, EventArgs e)
        {
            Snapture.CaptureDesktop();
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            Snapture.Release();

        }

        private void Form1_Load(object sender, EventArgs e)
        {
            Snapture = new Snapture();
            Snapture.onFrameCaptured += Snapture_onFrameCaptured;
            var dpi = Snapture.MonitorInfo.Monitors[0].Dpi;

            Snapture.SetBitmapResolution((int)dpi.X);

            Snapture.Start(FrameCapturingMethod.GDI);

        }

        private void Snapture_onFrameCaptured(object sender, FrameCapturedEventArgs e)
        {
            if(e.ScreenCapturedBitmap != null)
            {
                var bitmap = e.ScreenCapturedBitmap.Clone();
                pictureBox1.Image = (Bitmap)bitmap;
                e.ScreenCapturedBitmap.Dispose();
            }
        }

        private void ComboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {
            
        }
    }
}
