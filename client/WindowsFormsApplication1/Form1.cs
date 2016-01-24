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
    public partial class Form1 : Form
    {
        IPEndPoint remoteEP;
        Socket server;
        string usuario;
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            // hacemos que el socket se conecte una vez se habra el programa
            server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            IPAddress direction = IPAddress.Parse("192.168.56.101");
            remoteEP = new IPEndPoint(direction, 50001);

            //usamos el try para conectar con el servidor, sino lo consigure ejecuta el catch que mostrara en pantalla al usuario un error
            try
            {
                server.Connect(remoteEP);//Intentamos conectar el socket

            }

            catch (SocketException)
            {
                //Si hay excepcion imprimimos error y salimos del programa con return 
                MessageBox.Show("No he podido conectar con el servidor");
                return;
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (textBox1.Text != "" && textBox2.Text != "")
            {
                try
                {

                    byte[] login = System.Text.Encoding.ASCII.GetBytes("1:" + textBox1.Text + ":" + textBox2.Text);
                    server.Send(login);
                    byte[] bytes = new byte[80];
                    server.Receive(bytes);
                    string resultado = Encoding.ASCII.GetString(bytes);
                    int resul;
                    resul = Convert.ToInt32(resultado);

                    if (resul == 0)
                    {
                        usuario = textBox1.Text;
                        MessageBox.Show("Bienvenido al PIM PAM PUM");
                        Form3 f = new Form3(server, usuario);
                        f.Show();

                    }
                    if (resul == 1)
                    {
                        MessageBox.Show("Usuario o contraseña incorrectos");
                    }

                }
                catch (SocketException)
                {
                    MessageBox.Show("Error al conectarse");
                }
            }
            else
            {
                MessageBox.Show("Introduce un usuario y una contraseña");
            }
            
        }

        private void button3_Click(object sender, EventArgs e)
        {
            try
            {

                

                if (textBox1 == null)
                {

                    MessageBox.Show("introduzca el usuario y se le recordara la contraseña por correo");

                }
                if (textBox1 != null)
                {
                    byte[] login = System.Text.Encoding.ASCII.GetBytes("6:" + textBox1.Text);
                    server.Send(login);
                    byte[] bytes = new byte[80];
                    server.Receive(bytes);
                    string resultado = Encoding.ASCII.GetString(bytes);
                    int resul;
                    resul = Convert.ToInt32(resultado);
                    if (resul == 0)
                    {
                        MessageBox.Show("El usuario no esta en la base de datos");
                    }
                    else
                    {
                        MessageBox.Show("Falta por implementar");
                        //Aqui hemos de enviar el correo
                    }
                }

            }
            catch (SocketException)
            {
                MessageBox.Show("Error al conectarse");
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            Form2 f = new Form2(server);
            f.Show();

        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {
            textBox2.PasswordChar = '*';
        }
        
        }
    }

