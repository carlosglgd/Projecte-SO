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
    public partial class Form2 : Form
    {
        //declaramos socket server como variable global
        public Socket server;
        string usuario;

        public Form2(Socket server)
        {
            InitializeComponent();
            this.server = server;
        }


        private void Form2_Load(object sender, EventArgs e)
        {
           
        }

        private void button1_Click(object sender, EventArgs e)
        {
            //boton que añadira el nuevo usuario a la base de datos
            try
            {
                if (textBox3.Text == textBox4.Text)
                {
                    byte[] jugador = System.Text.Encoding.ASCII.GetBytes("0:" + textBox1.Text + ":" + textBox3.Text + ":" + textBox2.Text);
                    server.Send(jugador);
                    byte[] bytes = new byte[80];
                    server.Receive(bytes);
                    string resultado = Encoding.ASCII.GetString(bytes);
                    int resul;
                    resul = Convert.ToInt32(resultado);

                    if (resul == 0)
                    {
                        MessageBox.Show("Bienvenido al PIM PAM PUM");
                        usuario = textBox1.Text;
                        Form3 f = new Form3(server, usuario);
                        f.Show();

                    }
                    if (resul == 1)
                    {
                        MessageBox.Show("Usuario o email ya existen");
                    }
                }
                else
                {
                    MessageBox.Show("Las contraseñas no coinciden");
                }



            }
            catch
            {
                MessageBox.Show("Error al registrar");
            }
        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {
        }

        private void textBox3_TextChanged(object sender, EventArgs e)
        {

            textBox3.PasswordChar = '*';
        }

        private void textBox4_TextChanged(object sender, EventArgs e)
        {

            textBox4.PasswordChar = '*';
        }
    }
}
