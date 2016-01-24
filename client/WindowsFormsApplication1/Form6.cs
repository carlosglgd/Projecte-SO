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
    public partial class Form6 : Form
    {
        Socket server;
        
        string usuario;//guarda el nombre del usuario
        string listilla;
        int actualizado = 0;
        string[] jugadores;

        public Form6(Socket server, string user, string[] j)
        {
            InitializeComponent();
            this.server = server;
            jugadores = j;
            usuario = user;
            ponerListaJugadores(jugadores);
        }

        private void Form6_Load(object sender, EventArgs e)
        {
            

        }
        

        delegate void ponerListaJugadoresDelegate(string[] jugadores);

        void ponerListaJugadores(string[] jugadores)
        {
            try
            {
                if (listBox1.InvokeRequired)
            {
                ponerListaJugadoresDelegate d = new ponerListaJugadoresDelegate(ponerListaJugadores);
                this.Invoke(d, new object[] { jugadores });
            }
            else
            {
                int i = 0;

                    listilla = usuario + ":";
                    listBox1.Items.Clear();
                while (i < jugadores.Count())
                {

                        
                        if (jugadores[i] != "*")
                        {
                            listBox1.Items.Add(jugadores[i]);
                            listilla = listilla + ":" + jugadores[i];
                        }
                        else i = jugadores.Count();
                    i++;
                }
            }
            }
            catch (ObjectDisposedException) { }
        }

        private void button1_Click(object sender, EventArgs e)
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
            Close();
        }


        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void button3_Click(object sender, EventArgs e)
        {
                creaPartida f = new creaPartida(server, usuario, listilla);
                f.Show();
            }
        }
    }

