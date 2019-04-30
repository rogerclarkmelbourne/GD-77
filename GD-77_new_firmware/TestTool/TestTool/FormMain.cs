/*
 * Copyright (C)2019 Kai Ludwig, DG4KLU
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. The name of the author may not be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

using System;
using System.ComponentModel;
using System.Drawing;
using System.IO.Ports;
using System.Threading;
using System.Windows.Forms;

namespace TestTool
{
    public partial class FormMain : Form
    {
        const int BUTTON_PTT    = 0x00000001;
        const int BUTTON_SK1    = 0x00000002;
        const int BUTTON_SK2    = 0x00000004;
        const int BUTTON_ORANGE = 0x00000008;

        const int KEY_UP    = 0x00000100;
        const int KEY_DOWN  = 0x00002000;
        const int KEY_LEFT  = 0x00000200;
        const int KEY_RIGHT = 0x00000010;
        const int KEY_GREEN = 0x00000008;
        const int KEY_RED   = 0x00040000;
        const int KEY_0     = 0x00010000;
        const int KEY_1     = 0x00000001;
        const int KEY_2     = 0x00000002;
        const int KEY_3     = 0x00000004;
        const int KEY_4     = 0x00000020;
        const int KEY_5     = 0x00000040;
        const int KEY_6     = 0x00000080;
        const int KEY_7     = 0x00000400;
        const int KEY_8     = 0x00000800;
        const int KEY_9     = 0x00001000;
        const int KEY_STAR  = 0x00008000;
        const int KEY_HASH  = 0x00020000;

        bool running = false;
        BackgroundWorker worker;
        bool stop_worker = false;
        bool form_close = false;

        SerialPort port = null;
        
        public FormMain()
        {
            InitializeComponent();
        }

        private void FormMain_Load(object sender, EventArgs e)
        {
            this.Location = new Point((Screen.PrimaryScreen.WorkingArea.Width - this.Width) / 2, (Screen.PrimaryScreen.WorkingArea.Height - this.Height) / 2);

            loadCOMPortlist();
        }

        private void FormMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (running)
            {
                stop_worker = true;
                form_close = true;
                e.Cancel = true;
            }
        }

        void loadCOMPortlist()
        {
            string old_item = comboBoxCOMPorts.Text;
            comboBoxCOMPorts.Items.Clear();
            string[] ports = SerialPort.GetPortNames();
            foreach (string port in ports)
            {
                comboBoxCOMPorts.Items.Add(port);
            }
            if (comboBoxCOMPorts.Items.Contains(old_item))
            {
                comboBoxCOMPorts.Text = old_item;
            }
        }

        void UpdateStatus(string text)
        {
            if (labelStatusText.InvokeRequired)
                labelStatusText.Invoke(new MethodInvoker(delegate ()
                {
                    labelStatusText.Text = text;
                }));
            else
            {
                labelStatusText.Text = text;
            }
        }

        void SetCheckBox(CheckBox cb, bool value)
        {
            if (cb.InvokeRequired)
                cb.Invoke(new MethodInvoker(delegate ()
                {
                    cb.Checked = value;
                }));
            else
            {
                cb.Checked = value;
            }
        }

        void worker_DoWork(object sender, DoWorkEventArgs e)
        {
            byte[] sendbuffer = new byte[512];
            byte[] readbuffer = new byte[512];

            uint receive_data1 = 0;
            uint receive_data2 = 0;
            uint send_data1 = 0;
            uint send_data2 = 0;

            stop_worker = false;

            while (!stop_worker)
            {
                try
                {
                    send_data1 = 0x80000000;
                    if (checkBoxLED_green.Checked)
                    {
                        send_data1 |= 0x00000001;
                    }
                    if (checkBoxLED_red.Checked)
                    {
                        send_data1 |= 0x00000002;
                    }
                    send_data2 = 0x80000000;
                    if (checkBoxDISPLAY_test.Checked)
                    {
                        send_data2 |= 0x00000001;
                    }
                    if (checkBoxDISPLAY_light.Checked)
                    {
                        send_data2 |= 0x00000002;
                    }

                    sendbuffer[0] = (byte)'A';
                    sendbuffer[1] = (byte)((send_data1 & 0xFF000000) >> 24);
                    sendbuffer[2] = (byte)((send_data1 & 0x00FF0000) >> 16);
                    sendbuffer[3] = (byte)((send_data1 & 0x0000FF00) >> 8);
                    sendbuffer[4] = (byte)((send_data1 & 0x000000FF) >> 0);
                    sendbuffer[5] = (byte)((send_data2 & 0xFF000000) >> 24);
                    sendbuffer[6] = (byte)((send_data2 & 0x00FF0000) >> 16);
                    sendbuffer[7] = (byte)((send_data2 & 0x0000FF00) >> 8);
                    sendbuffer[8] = (byte)((send_data2 & 0x000000FF) >> 0);

                    port.Write(sendbuffer, 0, 9);
                    port.Read(readbuffer, 0, 9);

                    if (readbuffer[0] == 'A')
                    {
                        receive_data1 = (uint)((readbuffer[1] << 24) | (readbuffer[2] << 16) | (readbuffer[3] << 8) | (readbuffer[4] << 0));
                        receive_data2 = (uint)((readbuffer[5] << 24) | (readbuffer[6] << 16) | (readbuffer[7] << 8) | (readbuffer[8] << 0));

                        SetCheckBox(checkBoxButton_PTT, (receive_data1 & BUTTON_PTT) != 0);
                        SetCheckBox(checkBoxButton_SK1, (receive_data1 & BUTTON_SK1) != 0);
                        SetCheckBox(checkBoxButton_SK2, (receive_data1 & BUTTON_SK2) != 0);
                        SetCheckBox(checkBoxButton_ORANGE, (receive_data1 & BUTTON_ORANGE) != 0);

                        SetCheckBox(checkBoxKey_0, (receive_data2 & KEY_0) != 0);
                        SetCheckBox(checkBoxKey_1, (receive_data2 & KEY_1) != 0);
                        SetCheckBox(checkBoxKey_2, (receive_data2 & KEY_2) != 0);
                        SetCheckBox(checkBoxKey_3, (receive_data2 & KEY_3) != 0);
                        SetCheckBox(checkBoxKey_4, (receive_data2 & KEY_4) != 0);
                        SetCheckBox(checkBoxKey_5, (receive_data2 & KEY_5) != 0);
                        SetCheckBox(checkBoxKey_6, (receive_data2 & KEY_6) != 0);
                        SetCheckBox(checkBoxKey_7, (receive_data2 & KEY_7) != 0);
                        SetCheckBox(checkBoxKey_8, (receive_data2 & KEY_8) != 0);
                        SetCheckBox(checkBoxKey_9, (receive_data2 & KEY_9) != 0);
                        SetCheckBox(checkBoxKey_green, (receive_data2 & KEY_GREEN) != 0);
                        SetCheckBox(checkBoxKey_red, (receive_data2 & KEY_RED) != 0);
                        SetCheckBox(checkBoxKey_up, (receive_data2 & KEY_UP) != 0);
                        SetCheckBox(checkBoxKey_down, (receive_data2 & KEY_DOWN) != 0);
                        SetCheckBox(checkBoxKey_left, (receive_data2 & KEY_LEFT) != 0);
                        SetCheckBox(checkBoxKey_right, (receive_data2 & KEY_RIGHT) != 0);
                        SetCheckBox(checkBoxKey_star, (receive_data2 & KEY_STAR) != 0);
                        SetCheckBox(checkBoxKey_hash, (receive_data2 & KEY_HASH) != 0);
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message);
                    break;
                }

                Thread.Sleep(10);
            }
        }

        void worker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            port.Close();

            buttonStartStop.Text = "Start";
            running = false;

            if (form_close)
            {
                Close();
            }
        }

        private void buttonStartStop_Click(object sender, EventArgs e)
        {
            if (!running)
            {
                try
                {
                    port = new SerialPort(comboBoxCOMPorts.Text, 115200, Parity.None, 8, StopBits.One);
                    port.ReadTimeout = 1000;
                    port.Open();

                    worker = new BackgroundWorker();
                    worker.DoWork += new DoWorkEventHandler(worker_DoWork);
                    worker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(worker_RunWorkerCompleted);
                    worker.RunWorkerAsync();

                    buttonStartStop.Text = "Stop";
                    running = true;
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }
            }
            else
            {
                stop_worker = true;
            }
        }

        private void buttonRefreshCOMPortlist_Click(object sender, EventArgs e)
        {
            loadCOMPortlist();
        }
    }
}
