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
        bool running = false;
        BackgroundWorker worker;
        bool stop_worker = false;
        bool form_close = false;

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

        void worker_DoWork(object sender, DoWorkEventArgs e)
        {
            stop_worker = false;

            while (!stop_worker)
            {
                Thread.Sleep(10);
            }
        }

        void worker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
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
                worker = new BackgroundWorker();
                worker.DoWork += new DoWorkEventHandler(worker_DoWork);
                worker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(worker_RunWorkerCompleted);
                worker.RunWorkerAsync();

                buttonStartStop.Text = "Stop";
                running = true;
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
