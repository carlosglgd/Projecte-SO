
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

pthread_mutex_t mutex;
int err, cliente, zas;
int part = 0;//almacena un numero que se va incrementando asignando donde guarda los datos un jugador de esa partida
char listaconectados[MAX];
int numerodepartidas;
int numeroJugadores[MAX];
int disparos[MAX], zp;
MYSQL *conn;

typedef struct{ //Estructura que almacena los jugadores conectados y pasar la conexion
	char id[50];
	int con;
	int n;
	}Tconectado;
Tconectado conectados[MAX];

typedef struct{	//Almacenamos la conexion
	int con;
	int n;
}conexion;

typedef struct{	//Almacena los datos de una partida que se esta creando
	int n;
	char participantes;	//lista con los participantes, se dividira en dos puntos
	char *nombrepartida;
	char accepted;
}Tpartida;
Tpartida partidas[MAX];

typedef struct{//Estos datos se los paso al thread del juego
	char idJugador[20];//Habra que eliminar el personaje cuando pierda, para ello he pensado que borre el vector y lo vuelva a poner
	char idPartida[20];
	int con;
	int numPart; // guarda el numero de partida
	int escudo;
	int vidas;
	int balas;
}Juego;
Juego jugada[MAX];

typedef struct{//Estos datos se los paso al thread del juego
	char idJugador[20];
	char idPartida[20];
	int con;
	int numPart; // guarda el numero de partida
}Start;

void *jugar(void *ju){//Le hemos de pasar la conexion, el nombre, un id de la partida que este jugando
	int con, nchar = 0,numpart, z, found = 0, pierde = 0, vivos = 0, f = 0;//Hacer un struct de partidas en las que se almacene el nombre de los jugadores
	char msg[200], *opcion, *nombre,*name, *partida, buf2[MAX], list[MAX], lives[4], bullets[4];
	Start *jogo;
	jogo = (Start *) ju;
	nombre = jogo ->idJugador;
	partida = jogo->idPartida;
	con = jogo -> con;
	numpart = jogo -> numPart;
	sprintf(jugada[numpart].idJugador,"%s", nombre);
	sprintf(jugada[numpart].idPartida,"%s", partida);
	jugada[numpart].con = con;
	jugada[numpart].numPart = numpart;
	jugada[numpart].vidas = 6;//Si quieres cambia rle numero de vidas hay que hacerlo aqui
	while(pierde == 0){
		nchar=read(con,msg, 100);
		opcion = strtok(msg, ":");
		if (opcion == "A"){//escudo, le paso el nombre, lo borra de la lista de jugadores a los que han disparado
			jugada[numpart].escudo = 1;
			numeroJugadores[numpart]--;
		}
		if (opcion == "B"){//recarga, pasamos el nombre para que me sume una bala en esa partida
			jugada[numpart].escudo = 0;
			jugada[numpart].balas ++;
			numeroJugadores[numpart]--;
		}
		if (opcion == "C"){//dispara, restamos una bala, y añadimos el nombre de la pers a quien disparamos en una lista
			jugada[numpart].escudo = 0;
			name = strtok(NULL,":");
			sprintf(nombre,"%s",name);
			disparos[numpart]++;
			numeroJugadores[numpart]--;
			if (jugada[numpart].balas>0){
				while(numeroJugadores[numpart]!=0){
					//Espera a que todos hayan enviado sus opciones, cuando sea 0 es que todos lo jugadores habrán enviado ya sus opciones
				}
				disparar(nombre, partida, numpart);
			jugada[numpart].balas --;
			}
		}
		//Esperar a que todas las opciones disparar se hayan echo
		while(found == 0){
			if (disparos == 0){//Disparos guarda el numero de jugadores que le han dado a disparar
				//Hacer un booleano
				found == 1;//Esto hara que no se ejecute nada más hasta que se

			}
		}//Ya hemos esperado a que todos los jugadores hayan disparado
		//Funcion que cuando no te queden vidas te saque del form y te saque de las listas
		if (jugada[numpart].vidas == 0){
			pierdes(con);
			pierde = 1;
		}
		while (f < MAX){
			if (jugada[f].vidas != 0 && f != numpart){
				vivos++;
			}
			f++;
		}
		f=0;
		if(vivos == 0){// has ganado la partida ya que no queda nadie mas con vidas
			ganador(con);
			pierde = 1;
		}

		//Hacer algo que comprube cuantos jugadores quedan jugando y cuando sea uno le de como ganador




		else{
			numeroJugadores[numpart]++;
			sprintf(lives,"%d", jugada[numpart].vidas);
			sprintf(bullets,"%d", jugada[numpart].balas);
			sprintf(list,"%s:%s" ,lives, bullets);// Se envian las vidas balas y depues la lista con los demás jugadores
			while(z <= part){
				if (partida == jugada[z].idPartida && z != numpart){//Esta no es la lista correcta
					sprintf(list,":%s:%d", jugada[z].idJugador, jugada[z].vidas);//prepara a lista para enviar, cada uno recibe una lista distinta
				}
				z++;
			}

			write(con,list,strlen(list));
		}
		vivos = 0;
	}

}

void pierdes(int con){
	char buf2[MAX];
	sprintf(buf2,"close");
	write(con,buf2,strlen(buf2));
}

void ganador(int con){//Debe guardar las variables del ganador asi como los datos de la partida
	char buf2[MAX];
	write(con,buf2,strlen(buf2));
}

void disparar(char nombre[20], char partida[20], int numpart){ //Dispara a un jugador
	int found = 0, g=0, l = 0;
	while(jugada[g].idJugador != nombre && jugada[g].idPartida != partida && g<=part){
		g++;
		if (jugada[g].idJugador == nombre && jugada[g].idPartida == partida){
			found = 1;
			l = g;
			g++;
		}
	}
	if (jugada[l].escudo == 0){//le quitamos una vida sino tiene el escudo activado
		jugada[l].vidas--;
	}
	disparos[numpart]--;


}




void unirsePartida(char datos[MAX], int con, int n){ //Permite unirse a una partida ya creada
	char *nombrepartida, lista[MAX], buf2[MAX], nombre[20];
	int s, found=0;
	pthread_t thread[MAX];
	Start jugar[zas];
	nombrepartida = strtok (datos, ":");//Contiene el nombre de la partida
	sprintf(nombre,"%s", conectados[n].id);//contiene el nombre del jugador
	while(s<numerodepartidas && found == 0){
		if(nombrepartida == partidas[s].nombrepartida){
			found = 1;
			s--;
		}
		s++;
	}
	if (partidas[s].n != 0 && found ==1){
//		conectados[n].numpart++; //***************VER COMO SE HACE EL INCREMENTO EN UN STRUCT***************************
		partidas[s].n --;//n es el numero de participantes, si este alcanza el maximo no te puedes unir
		sprintf(lista,"%s:%s", partidas[s].participantes, conectados[n].id);//Lista de jugadores que se han unido a la partida, añade un nuevo nombre
		sprintf(partidas[s].participantes, "%s", lista);
		sprintf(buf2,"%d",1);// ahora hemos de almacenar los datos en el strust
		//pthreat mutex
		part++;
		sprintf(jugada[part].idJugador,"%s" ,nombre);
		sprintf(jugada[part].idPartida,"%s", nombrepartida);
		jugada[part].con = con;
		jugada[part].numPart = part;
		pthread_create(&thread[part], NULL, jugar, (void*) &jugar[part]);
		write(con,buf2,strlen(buf2));//Una vez unido creamos el threat para que pueda jugar varias partidas
	}
	else {
		printf("ALGO FUE MAL\n");
	}
}
void CrearPartida(char datos[MAX], int con, int n){ //Crea una partida y la almacena
	printf("comprobacion 2\n");
	char *names ,*nombrepartida, nombre[20];
	int z = 0, encontrado = 0,p;
	printf("comprobacion 223\n");
	pthread_t thread[MAX];
	Start jugar[zas];
	MYSQL_RES *resultado;
	MYSQL_ROW row;
	printf("comprobacion 242\n");
	printf("%s\n", datos);
	printf("comprobacion 225\n");
	printf("comprobacion 22\n");
	nombrepartida = strtok (datos, ":");//guarda el nombre de la partida
	char *numInvitados = strtok (NULL, ":");
	int numero = (int)numInvitados;
	printf("%d", numero);
	printf("%s\n", nombrepartida);
	printf("comprobacion 25\n");
	sprintf(nombre,"%s", conectados[n].id);//contiene el nombre del jugador
	err = mysql_query (conn, "SELECT id FROM partida");
	resultado = mysql_store_result (conn);
	row = mysql_fetch_row (resultado);
	printf("comprobacion 33\n");
	if (row == NULL){
		printf("No se han podido obtener consultas");
	}

	printf("parte 221\n");
	printf("%s\n", datos);
	//guardar los nombres en una lista
	int lisata = 0;
	while (names =! '\0' && names != ""){

		printf("parte 221\n");
		names = strtok (NULL, ":");
		printf("%s\n", names);
		printf("parte 221\n");
		if (names =! '\0' && names != ""){
			printf("parte 228756\n");
			char ass[200];
			strncpy(ass, names, sizeof names);
			sprintf(partidas[z].participantes,"%s:", names);//ver porque peta esto
			printf("parte 221 y finaaal\n");
		}
		if (names == '\0' || names == ""){
					printf("esto e una prueba\n");
				}
	}
	printf("parte 227681\n");
	partidas[z].nombrepartida = nombrepartida;
	partidas[z].n = numero;
	numerodepartidas ++;
	printf("parte 2  %d\n", numerodepartidas);
	part++;//Hacer mutext cada variable global que se toque
	sprintf(jugada[part].idPartida,"%s", nombrepartida);
	jugada[part].con = con;
	jugada[part].numPart = part;
	printf("parte 29\n");
}






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

void eliminarPartidasYaEmpezadas(){ //Elimina una partida de la lista de partidas creadas y que ya ha empezado
	int j = 0;
	while(j<=numerodepartidas){
		if (partidas[j].n = 0)
			partidas[j].nombrepartida = NULL;//Asi la partida no se listara, ver como eliminarlo
		j++;
	}
}
void listaPartidas(int con){ //Lista las partidas creadas pero no empezadas*********REVISAR***********
	int j = 0;
	char partidillas[MAX];
	while (j <= numerodepartidas){
		if (partidas[j].nombrepartida != NULL){
			sprintf(partidillas,"%s,%d", partidas[j].nombrepartida, partidas[j].n);
		}
		j++;
	}
	write(con, partidillas, strlen(partidillas));
}
void ListaConectados(int con){ //numero de conectados
	char msg[200];
	sprintf(msg,"1%s",listaconectados);
	write(con, msg, strlen(msg));
	printf("%s", msg);
}
void *busca(void *cone){
	printf("\nEstamos buscando partidas\n");
		int zi = 0, encontrado = 0, p, j[20], existe = 0,i=0, nchar = 0;
		conexion *param;
		param = (conexion *) cone;
		int n = param ->n;
		char username = "";
		username = conectados[n].id;
		int con = param->con;
		char *user, buf2[20];
		char usuari = "";
		char listax = "";
		while(zi <200){
			listax = partidas[zi].participantes;
			if (listax != '\0'){
				user = strtok(listax,":");
				sprintf(usuari, "%s", user);

				if (usuari == username){
					encontrado = 1;
				}
				else{
					while (encontrado == 0 && p < 100){
						user = strtok(NULL, ":");
						if (user == username){
							encontrado = 1;
						}
						p++;
					}
				}
				if(encontrado == 1){
					int t = 0;
					while ( t<20 ){
						if (zi == j[t]) {
							existe = 1;
						}
						else{
							t++;
						}
					}
				}
				if(encontrado == 1 && existe == 0){
					//enviar invitación a la partida que se encuentra en la posición zi
					char listilla = partidas[zi].participantes;
					user = strtok (listilla, ":");
					if (user != username){
						partidas[zi].participantes = user;
						sprintf(partidas[zi].participantes, ":");
					}
					while(user != "" && i< 20){
						user = strtok(NULL, ":");
						if (user != username){
							partidas[zi].participantes = user;
							sprintf(partidas[zi].participantes, ":");
						}
						i++;
					}//Esto copia los usuarios que aun faltan por logear en la lista
					sprintf(buf2,"Q:%s",partidas[zi].nombrepartida);
					write(con,buf2,strlen(buf2));
					i=0;

				}
			}
			zi++;
			encontrado = 0;
			existe = 0;
			if (zi == 199){
				zi=0;
			}
		}


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
eliminapartida(char id){
	int p=0;
	char *nombrepar;
	nombrepar = strtok(id,":");
	while(partidas[p].nombrepartida != nombrepar){
		p++;
	}
	partidas[p].n = 200;//supongo que estara vacia
	partidas[p].participantes = "nadie";
}
aceptar(char np, int con, int n){
	char *nombrepar = strtok(np,":");
	char buf2[MAX];
	int p = 0, rechazada = 0;

	while(partidas[p].nombrepartida != nombrepar){
		p++;
	}
	partidas[p].n--;
	sprintf(partidas[p].accepted,"%s:", conectados[n].id);
	if (partidas[p].n == 0){
		empezar(p);
	}
	else{//esto esperara a que los demas acepten
		while(rechazada = 0){
			if(partidas[p].n == 0){
				empezar(p, con ,n);
			}
			if (partidas[p].n == 200){
				rechazada = 1;
				sprintf(buf2,"T");
				write(con,buf2,strlen(buf2));

			}
		}

	}
}
empezar(int p, int con, int n){//debe inicializar la partida usando un thread
	pthread_t thread[MAX];
	Start jugar[zas];
	char buf2[200];
	char nombrepar = partidas[p].nombrepartida;
	part++;
	sprintf(jugada[part].idPartida,"%s", nombrepar);
	sprintf(jugada[part].idJugador,"%s", conectados[n].id);
	jugada[part].con = con;
	jugada[part].numPart = part;
	pthread_create(&thread[part], NULL, jugar, (void*) &jugar[part]);
	sprintf(buf2,"C:%s", partidas[p].accepted);
	write(con,buf2,strlen(buf2));//Una vez unido creamos el threat para que pueda jugar varias partidas
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
			conexiones[cliente].n = i;
			i++;
			pthread_create(&thread[cliente], NULL, server, (void*) &conexiones[cliente]);
			pthread_mutex_lock(&mutex);
			cliente++;
			pthread_mutex_unlock(&mutex);
			printf("el númer de clientes que se han conectado hasta el momento es de: %d\n", cliente);
			}

		close(s);
	}
