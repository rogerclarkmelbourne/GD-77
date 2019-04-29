namespace TestTool
{
    partial class FormMain
    {
        /// <summary>
        /// Erforderliche Designervariable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Verwendete Ressourcen bereinigen.
        /// </summary>
        /// <param name="disposing">True, wenn verwaltete Ressourcen gelöscht werden sollen; andernfalls False.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Vom Windows Form-Designer generierter Code

        /// <summary>
        /// Erforderliche Methode für die Designerunterstützung.
        /// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
        /// </summary>
        private void InitializeComponent()
        {
            this.buttonStartStop = new System.Windows.Forms.Button();
            this.labelStatus = new System.Windows.Forms.Label();
            this.labelStatusText = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // buttonStartStop
            // 
            this.buttonStartStop.Location = new System.Drawing.Point(15, 12);
            this.buttonStartStop.Name = "buttonStartStop";
            this.buttonStartStop.Size = new System.Drawing.Size(75, 23);
            this.buttonStartStop.TabIndex = 0;
            this.buttonStartStop.Text = "Start";
            this.buttonStartStop.UseVisualStyleBackColor = true;
            this.buttonStartStop.Click += new System.EventHandler(this.buttonStartStop_Click);
            // 
            // labelStatus
            // 
            this.labelStatus.AutoSize = true;
            this.labelStatus.Location = new System.Drawing.Point(12, 428);
            this.labelStatus.Name = "labelStatus";
            this.labelStatus.Size = new System.Drawing.Size(40, 13);
            this.labelStatus.TabIndex = 2;
            this.labelStatus.Text = "Status:";
            // 
            // labelStatusText
            // 
            this.labelStatusText.AutoSize = true;
            this.labelStatusText.Location = new System.Drawing.Point(58, 428);
            this.labelStatusText.Name = "labelStatusText";
            this.labelStatusText.Size = new System.Drawing.Size(22, 13);
            this.labelStatusText.TabIndex = 3;
            this.labelStatusText.Text = "OK";
            // 
            // FormMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.labelStatusText);
            this.Controls.Add(this.labelStatus);
            this.Controls.Add(this.buttonStartStop);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "FormMain";
            this.Text = "TestTool";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.FormMain_FormClosing);
            this.Load += new System.EventHandler(this.FormMain_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button buttonStartStop;
        private System.Windows.Forms.Label labelStatus;
        private System.Windows.Forms.Label labelStatusText;
    }
}

