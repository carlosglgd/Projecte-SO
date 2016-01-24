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
    public partial class Form3 : Form
    {

        //declaramos socket server como variable global
        public Socket server;
        string usuario;
        public void escoltacanal(byte[] ans)
        {
            try
            {

                char opcion = 'a';
                while ( opcion != '9')
                {
                    server.Receive(ans);
                    string resposta = Encoding.ASCII.GetString(ans);
                    opcion = resposta[0];
                    resposta = resposta.TrimEnd('\0');
                    switch (opcion)
                    {
                        case '1'://Lista de jugadores conectados
                            resposta = resposta.TrimStart('1');
                            string[] jugadores = resposta.Split(':');
                            Form6 f = new Form6(server, usuario, jugadores);
                            f.ShowDialog();
                            MessageBox.Show("Vamos a crear una partida");

                            break;
                        case 'D'://La partida ha sido aceptada por todos los jugadores
                            resposta = resposta.TrimStart('C');
                            string c = resposta.Split(':')[0];
                            break;

                        case 'T'://Una partida a la que nos han invitado se ha rechazado
                            resposta = resposta.TrimStart('T');
                            string d = resposta.Split(':')[0];
                            MessageBox.Show("algun cobarde ha abandonado");
                            break;

                        case 'Q'://Recibe una invitacion
                            resposta = resposta.TrimStart('Q');
                            string a = resposta.Split(':')[0];
                            int b = Convert.ToInt32(resposta.Split(':')[1]);
                            invite jk = new invite(a, b, server);
                            jk.ShowDialog();
                            break;
                    }
                }
            }
            catch (NullReferenceException) { MessageBox.Show("NULLREFERENCEEXCEPTION"); }
            catch (SocketException) { MessageBox.Show("SOCKETEXCEPTION"); }
            catch (FormatException) { MessageBox.Show("FORMATEXCEPTION"); }
        }
        public Form3(Socket server, string user)
        {
            InitializeComponent();
            this.server = server;
            usuario = user;
            byte[] ans = new byte[100];
            ThreadStart ts = delegate { escoltacanal(ans); };
            Thread t = new Thread(ts);
            t.Start();
        }

        private void Form3_Load(object sender, EventArgs e)
        {

        }

        private void toolStripLabel1_Click(object sender, EventArgs e)
        {

        }

        private void desconectarseToolStripMenuItem_Click(object sender, EventArgs e)
        {
            //string mensaje = "6";
            //server.Send(Encoding.ASCII.GetBytes(mensaje));
            this.Close();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            try
            {
                byte[] actualizar = System.Text.Encoding.ASCII.GetBytes("2:");
                server.Send(actualizar);
            }
            catch (SocketException)
            {
                MessageBox.Show("Error al conectarse");
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Form4 f = new Form4(server);
            f.Show();
        }

        private void button2_Click(object sender, EventArgs e)
        {

        }
    }
}
