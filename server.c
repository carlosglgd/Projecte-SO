
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <mysql.h>
#include <pthread.h>
#include <unistd.h>



#define MAX 800

typedef struct{
	int con;
}conexion;
conexion conexiones[100];

void BaseDeDatos(){	//Creamos una conexión al servidor MYSQL
	conn = mysql_init(NULL);
	if (conn == NULL) {
		printf("Error al crear la conexión con la DB %u %s\n", mysql_errno(conn),
				mysql_error(conn));
		exit(1);
	}
	conn = mysql_real_connect(conn, "localhost", "root", "mysql", "carlos", 0, NULL, 0);
	if (conn == NULL) {
		printf("Error al inicializar la conexión con la DB %u %s\n", mysql_errno(conn),
				mysql_error(conn));
		exit(1);
	}
}

void ListaConectados(int con){ //numero de conectados
	char msg[200];
	sprintf(msg,"1%s",listaconectados);
	write(con, msg, strlen(msg));
	printf("%s", msg);
}
int login(char datos[MAX], int con, int n){
	char *username, *password, *opcion;
	int correcto;
	pthread_t thread;
	// Estructura especial para almacenar resultados de consultas
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	username = strtok (datos, ":");
	password = strtok (NULL, ":");
	err=mysql_query (conn, "SELECT * FROM jugador");
	if (err!=0)
	{
		printf ("Error al consultar datos de la base %u %s\n",mysql_errno(conn), mysql_error(conn));
		exit (1);
	}
	resultado = mysql_store_result (conn);
	row = mysql_fetch_row (resultado);
	if (row == NULL)
		printf ("No se han obtenido datos en la consulta\n");
	else
	{
		while (row !=NULL)
		{
			// obtenemos la siguiente fila
			int p;
			p = strcmp(username,row[0]);
			int p2;
			p2 = strcmp(password,row[1]);
			if( (p == 0) && (p2 == 0) )
			{
				correcto = 1;
				printf("Correcto\n");
			}
			row = mysql_fetch_row (resultado);
		}
		if(correcto == 1)
		{
			printf("Usuario registrado correctamente\n");
			sprintf(conectados[n].id,"%s\n", username);
			printf("el username antes de copiarlo en el struct es: %s\n", username);
			sprintf(listaconectados,"%s:%s\n",listaconectados, username);
			printf("%s nombre del logeado\n", &conectados[n].id);
			pthread_t thread[10];
			int i=1;
			printf("EL threat se ha creado1\n");
			printf("EL threat se ha creado2\n");
			pthread_create(&thread[i], NULL, busca, (void*) &conectados[n]);
			printf("EL threat se ha creado");

			return 0;
		}
		else
		{
			printf("El usuario no esta en la base de datos\n");
			return 1;
		}
	}
}

int registro(char datos[MAX], int con, int n){


	char *username, *user, *mail, *password, *opcion, consulta[MAX];
	int encontrado = 0, err, p,i;
	// Estructura especial para almacenar resultados de consultas
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	printf("%d\n", n);
	username = strtok (datos, ":");
	password = strtok (NULL, ":");
	mail = strtok (NULL, ":");
	user = username;
	printf("Seguimos2 opcion: %s username : %s \n", username, password);
	err = mysql_query (conn, "SELECT * FROM jugador");
	resultado = mysql_store_result (conn);
	row = mysql_fetch_row (resultado);
	if (row == NULL){
		printf("No se han podido obtener consultas");
	}
	else
	{
		printf("probamos con el row\n");
		encontrado = 0;
		while (row !=NULL){
			//Buscamos si existe un usuario con ese nombre
			p = strcmp(username, row[0]);
			if (p == 0){
				encontrado = 1; //si el nombre ya existe ha de sacar un mensaje
			}
			i = strcmp(mail, row[2]);
			if (i == 0){
				encontrado = 1; //si el nombre ya existe ha de sacar un mensaje
			}
			row = mysql_fetch_row (resultado);
		}

		printf("%d encontrado\n",encontrado);
		printf("Se comprueba que el mail o nombre no existan\n");
		if (encontrado == 1)
		{
			printf("el usuario o email ya existen");
			printf("2\n");
			return 1;
		}
		if (encontrado == 0){
			printf("introducimos el usuario en la base de datos\n");
			strcpy (consulta, "INSERT INTO jugador VALUES ('");
			strcat (consulta, username);
			strcat (consulta, "','");
			strcat (consulta, password);
			strcat (consulta, "','");
			strcat (consulta, mail);
			strcat (consulta, "')");
			printf("Seguimos3\n");
			err = mysql_query(conn, consulta);
			if (err!=0)
			{
				printf ("Error al introducir datos la base %u %s\n",
				mysql_errno(conn), mysql_error(conn));
				exit (1);
				return 1;
			}
			else{
				printf("Se ha introducido correctamente el usuario\n");
				sprintf(conectados[n].id,"%s\n", user);
				sprintf(listaconectados,"%s: %s\n",listaconectados, username);
				printf("%s nombre del logeado\n", conectados[n].id);
					pthread_t thread;
					int *fichero[10];
					fichero[1] = (int *) malloc(sizeof(int));
					*fichero[1] = conectados[n].id;
					pthread_create(&thread, NULL, busca, (void*) fichero[1]);
				return 0;
			}


		}


	}
}

void *server(void *id)
{

	int con, nchar = 0, n;
	char *ca, *op;
	char msg[100];
	char buf2[200];
	conexion *conect;
	conect = (conexion *) id;
	con = conect->con;
	n = conect->n;
	// Estructura especial para almacenar resultados de consultas
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	//Bucle infinito para estar leyendo siempre el clientes
	while(1){
		int devolver;
		nchar=read(con, msg, 1);
		msg[nchar]='\0';
		char OP = msg[0];
		switch (OP){
			case '0':	//Registrar usuarios
				nchar=read(con,msg, 100);
				devolver = registro(msg, con, n);
				sprintf(buf2,"%d",devolver);
				write(con,buf2,strlen(buf2));
			break;
			case '1':	//login de los usuarios
				nchar=read(con, msg, 100);
				devolver = login(msg, con, n);
				sprintf(buf2,"%d",devolver);
				write(con,buf2,strlen(buf2));
			break;
			case '2':	//listar los usuarios conectados
				ListaConectados(con);
			break;
			case '3':	//crea la partida cmprobando que el nombre no existe
				nchar = read(con, msg, 100);
				printf("primera comprovacion %s \n",msg);
				CrearPartida(msg, con, n);
				sprintf(buf2,"%d",devolver);
				write(con,buf2,strlen(buf2));
			break;
			case '4': //Mostrar partidas creadas
				listaPartidas(con);
			break;
			case '5': //Unirse a una partida
				nchar = read(con, msg, 100);
				unirsePartida(msg, con, n);
			break;
			case '6'://Le ha dado a recordar contraseña
				nchar = read(con, msg, 100);
				sprintf(buf2,"1");
				write(con,buf2,strlen(buf2));
			break;
			case '7'://le ha dado a crear partida y invitar a los usuarios
				nchar = read(con, msg, 300);
				printf("el mensage recibido es: %s\n");
				CrearPartida(msg, con, n);
			break;
			case 'a'://acepta jugar la partida
				nchar =read(con, msg, 300);
				aceptar(msg, con, n);
			case 'p'://rechaza la invitación
				nchar =read(con, msg, 300);//elimina la partida y sus datos
				eliminapartida(msg);
			break;
		}
		sprintf(msg,"");
		sprintf(buf2,"");
	}
}
int main(void) {
		pthread_mutex_init(&mutex,NULL);   // Inicialización de variables
		int nchar, con, remote_addr_len;
		struct sockaddr_in serv_adr, remote_addr;
		int s,r, i = 0;
    	pthread_t thread[MAX];
    	s=socket(AF_INET, SOCK_STREAM, 0);
    	if (s==-1) {
    		perror("socket");
    		exit(-1);
    	}

    	/********** Fem el bind al port amb INADDR_ANY */
    	serv_adr.sin_family = AF_INET;
    	serv_adr.sin_port = htons(50001);
    	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    	r=bind(s, (struct sockaddr *) &serv_adr, sizeof(serv_adr));
    	if (r==-1) {
    		perror("bind");
    		exit(-1);
    	}
    	r=listen(s, 1);
    	if (r==-1) {
    		perror("listen");
    		exit(-1);
    	}
		BaseDeDatos();
		conexion conexiones[MAX];
		while(1){
			remote_addr_len=sizeof(remote_addr);
			printf ("esperant connexio\n");
			con=accept(s,(struct sockaddr *)&remote_addr, &remote_addr_len);
			conexiones[cliente].con = con;
			i++;
			pthread_create(&thread[cliente], NULL, server, (void*) &conexiones[cliente]);
			pthread_mutex_lock(&mutex);
			cliente++;
			pthread_mutex_unlock(&mutex);
			printf("el númer de clientes que se han conectado hasta el momento es de: %d\n", cliente);
			}

		close(s);
	}
