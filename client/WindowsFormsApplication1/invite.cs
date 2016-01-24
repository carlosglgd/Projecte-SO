using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net.Sockets;

namespace WindowsFormsApplication1
{
    public partial class invite : Form
    {
        Socket server;
        string master;
        int id;

        public invite(string master, int id, Socket server)
        {
            InitializeComponent();
            this.server = server;
            this.master = master;
            this.id = id;
            label1.Text = master + " te ha invitado a una partida";
        }

        private void button2_Click(object sender, EventArgs e)
        {

        }
    }
}
