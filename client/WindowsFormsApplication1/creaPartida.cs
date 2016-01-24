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
    public partial class creaPartida : Form
    {
        public Socket server;
        string usuario;//guarda el nombre del usuario
        string listilla;
        string listaInvitados;
        int cont = 0;

        public creaPartida(Socket server, string user, string list)
        {
            InitializeComponent();
            this.server = server;
            usuario = user;
            listilla = list;
            byte[] bytes = new byte[800];
            string[] jugadores = listilla.Split(':');
            ponerListaJugadores(jugadores);
        }
        delegate void ponerListaJugadoresDelegate(string[] jugadores);

        public void ponerListaJugadores(string[] jugadores)
        {
            if (listBox1.InvokeRequired)
            {
                ponerListaJugadoresDelegate d = new ponerListaJugadoresDelegate(ponerListaJugadores);
                this.Invoke(d, new object[] { jugadores });
            }
            else
            {
                int i = 0;
                listBox1.Items.Clear();
                listBox2.Items.Clear();
                listBox2.Items.Add(usuario);
                listaInvitados = listaInvitados + ":" + usuario;
                cont++;
                while (i < jugadores.Count())
                {
                    if (jugadores[i] != "*") listBox1.Items.Add(jugadores[i]);
                    else i = jugadores.Count();
                    i++;
                }
            }
        }


        private void label3_Click(object sender, EventArgs e)
        {

        }

        private void listBox1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (listBox1.SelectedItems.Count < 100)
            {
                try
                {
                    string playerid = listBox1.SelectedItem.ToString();
                    if (playerid != usuario)
                    {
                        listBox2.Items.Add(playerid);
                        listaInvitados = listaInvitados + ":"+ playerid ;
                        cont++;
                    }
                }
                catch (NullReferenceException)
                {
                    MessageBox.Show("NULLREFERENCEEXCEPTION");
                }
            }

        }

        private void listBox2_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (textBox1.Text != "" && textBox1.Text != null)
            {
                string namepar = textBox1.Text;
                string msg = "7:" + namepar + ":" + cont + ":" + listaInvitados;
                server.Send(Encoding.ASCII.GetBytes(msg));
                MessageBox.Show("ahora cierro el form");
                Close();
            }
            else
            {
                MessageBox.Show("introduzca un nombre de partida");
            }
        }

        private void creaPartida_Load(object sender, EventArgs e)
        {

        }
    }
}
