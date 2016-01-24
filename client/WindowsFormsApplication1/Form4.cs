using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace WindowsFormsApplication1
{
    public partial class Form4 : Form
    {
        //declaramos socket server como variable global
        public Socket server;

        public Form4(Socket server)
        {
            InitializeComponent();
            this.server = server;
            byte[] bytes = new byte[800];
            ThreadStart ts = delegate { Juego(bytes); };
            Thread t = new Thread(ts);
            t.Start();
        }

        private void Form4_Load(object sender, EventArgs e)
        {

        }
        public void Juego(byte[] bytes)
        {
            Boolean continuar = true;
            while (continuar)
            {
                try
                {
                    Thread.Sleep(200);
                    server.Receive(bytes);
                    string resultado = Encoding.ASCII.GetString(bytes);
                    string[] jugadores = resultado.Split(':');
                    players(jugadores);
                }

                catch (NullReferenceException)
                {
                    MessageBox.Show("NULLREFERENCEEXCEPTION");
                    continuar = false;
                }
                catch (SocketException)
                {
                    MessageBox.Show("SOCKETEXCEPTION");
                    continuar = false;
                }
                catch (FormatException)
                {
                    MessageBox.Show("FORMATEXCEPTION");
                    continuar = false;
                }
            }
        }
        delegate void playersDelegate(string[] jugadores);

        public void players(string[] jugadores)
        {
            if (listBox1.InvokeRequired)
            {
                playersDelegate d = new playersDelegate(players);
                this.Invoke(d, new object[] { jugadores });
            }
            else
            {
                int i = 2;//Para que liste solo los jugadores
                listBox1.Items.Clear();
                textBox2.Text = jugadores[0];
                textBox3.Text = jugadores[1];
                while (i < jugadores.Count())
                {
                    if (jugadores[i] != "*") listBox1.Items.Add(jugadores[i]);
                    else i = jugadores.Count();
                    i++;
                }
            }
        }
        private void button3_Click(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            byte[] crea = System.Text.Encoding.ASCII.GetBytes("3:");
            server.Send(crea);
        }

        private void button2_Click(object sender, EventArgs e)
        {

        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            try
            {

                byte[] escudo = System.Text.Encoding.ASCII.GetBytes("A:");
                server.Send(escudo);
            }
            catch (SocketException)
            {
                MessageBox.Show("Error al conectarse");
            }
        }

        private void button2_Click_1(object sender, EventArgs e)
        {
            try
            {

                byte[] recarga = System.Text.Encoding.ASCII.GetBytes("B:");
                server.Send(recarga);
            }
            catch (SocketException)
            {
                MessageBox.Show("Error al conectarse");
            }
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void button3_Click_1(object sender, EventArgs e)
        {
            try
            {

                byte[] ataca = System.Text.Encoding.ASCII.GetBytes("c:"+textBox1.Text);
                server.Send(ataca);
            }
            catch (SocketException)
            {
                MessageBox.Show("Error al conectarse");
            }
        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
