#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include<signal.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include "servidor.hpp"

/** 
   El servidor ofrece el servicio de un chat
 */

void manejador(int signum);
void salirCliente(int socket, fd_set * readfds, int * numClientes, int arrayClientes[]);



int main ( )
{
  
	/*---------------------------------------------------- 
		Descriptor del socket y buffer de datos                
	-----------------------------------------------------*/
	int sd, new_sd;
	struct sockaddr_in sockname, from;
	char buffer[MSG_SIZE];
	socklen_t from_len;
    fd_set readfds, auxfds;
   	int salida;
   	int arrayClientes[MAX_CLIENTS];
    int numClientes = 0;

   	//contadores

    int i,j,k;
	int recibidos;

    vector<struct jugadores> vjugadores;
    vector<struct partidas> vpartidas;

   	char identificador[MSG_SIZE];
    
    int on, ret;

    srand(time(NULL));
    
    
	/* --------------------------------------------------
		Se abre el socket 
	---------------------------------------------------*/
  	sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1)
	{
		perror("No se puede abrir el socket cliente\n");
    	exit (1);	
	}
    
    	// Activaremos una propiedad del socket para permitir· que otros
    	// sockets puedan reutilizar cualquier puerto al que nos enlacemos.
    	// Esto permite· en protocolos como el TCP, poder ejecutar un
    	// mismo programa varias veces seguidas y enlazarlo siempre al
   	 // mismo puerto. De lo contrario habrÌa que esperar a que el puerto
    	// quedase disponible (TIME_WAIT en el caso de TCP)
    on=1;
    ret = setsockopt( sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));



	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(PORT);
	sockname.sin_addr.s_addr = INADDR_ANY;

	if (bind (sd, (struct sockaddr *) &sockname, sizeof (sockname)) == -1)
	{
		perror("Error en la operación bind");
		exit(1);
	}
	

   	/*---------------------------------------------------------------------
		Del las peticiones que vamos a aceptar sólo necesitamos el 
		tamaño de su estructura, el resto de información (familia, puerto, 
		ip), nos la proporcionará el método que recibe las peticiones.
   	----------------------------------------------------------------------*/
	from_len = sizeof (from);


	if(listen(sd,1) == -1){
		perror("Error en la operación de listen");
		exit(1);
	}

	
    printf("El servidor está esperando conexiones...\n");	//Inicializar los conjuntos fd_set
    	
	FD_ZERO(&readfds);
    FD_ZERO(&auxfds);
    FD_SET(sd,&readfds);
    FD_SET(0,&readfds);
    
   	
    //Capturamos la señal SIGINT (Ctrl+c)
    signal(SIGINT,manejador);
    
	/*-----------------------------------------------------------------------
		El servidor acepta una petición
	------------------------------------------------------------------------ */
	while(1){
            
            //Esperamos recibir mensajes de los clientes (nuevas conexiones o mensajes de los clientes ya conectados)
            
            auxfds = readfds;
            
            salida = select(FD_SETSIZE,&auxfds,NULL,NULL,NULL);
            
            if(salida > 0){
                
                
                for(i=0; i<FD_SETSIZE; i++){
                    
                    //Buscamos el socket por el que se ha establecido la comunicación
                    if(FD_ISSET(i, &auxfds)) {
                        
                        if( i == sd){
                            
                            if((new_sd = accept(sd, (struct sockaddr *)&from, &from_len)) == -1){
                                perror("Error aceptando peticiones");
                            }
                            else
                            {
                                if(numClientes < MAX_CLIENTS){
                                    arrayClientes[numClientes] = new_sd;
                                    numClientes++;
                                    FD_SET(new_sd,&readfds);
                                
                                    strcpy(buffer, "+Ok. Usuario conectado.\n");

                                    printf("Cliente <%d> conectado.\n", new_sd);
                                
                                    send(new_sd, buffer, sizeof(buffer), 0);

                                    bzero(buffer, sizeof(buffer), 0);
                                    sprintf(buffer, "------------------ OPCIONES ------------------\nUSUARIO usuario\nPASSWORD contraseña\nREGISTRO -u usuario -p contraseña\nINICIAR-PARTIDA\nPEDIR-CARTA\nPLANTARME\nSALIR\n----------------------------------------------\n");
                                    send(new_sd, buffer, sizeof(buffer), 0);
                                    close(new_sd);
                                }
                                else
                                {
                                    bzero(buffer,sizeof(buffer));
                                    strcpy(buffer,"Demasiados clientes conectados\n");
                                    send(new_sd,buffer,sizeof(buffer),0);
                                    close(new_sd);
                                }
                                
                            }
                            
                            
                        }
                        else if (i == 0){

                            /* APAGAR SERVIDOR */

                            //Se ha introducido información de teclado
                            bzero(buffer, sizeof(buffer));
                            fgets(buffer, sizeof(buffer),stdin);
                            
                            //Controlar si se ha introducido "SALIR", cerrando todos los sockets y finalmente saliendo del servidor. (implementar)
                            if(strcmp(buffer,"SALIR\n") == 0){

                                bzero(buffer, sizeof(buffer));
						        strcpy(buffer,"Desconexión servidor\n");
                             
                                for (j = 0; j < numClientes; j++){
						            
                                    send(arrayClientes[j],buffer , sizeof(buffer),0);
                                    salirCliente(arrayClientes[j], &readfds, &numClientes, arrayClientes);
                                }
                                
                                printf("Servidor apagado.\n");
                                close(sd);
                                exit(-1);
                                
                                
                            }
                            //Mensajes que se quieran mandar a los clientes (implementar)
                            
                        } 
                        else{
                            bzero(buffer,sizeof(buffer));
                            
                            recibidos = recv(i,buffer,sizeof(buffer),0);
                            
                            if(recibidos > 0){

                                /* SALIDA DEL CLIENTE */
                                
                                if(strcmp(buffer,"SALIR\n") == 0){


                                    int estadoJugador = 0;

                                    for(int j = 0; j < vjugadores.size(); j++) {
                                        
                                        if(vjugadores[j].identificadorUsuario === i) {
                                            estadoJugador = vjugadores[j].estado;
                                        
                                        }
                                    }

                                    if(estadoJugador == 4) {

                                        int idJugador2 = 0;

                                        for(int j = 0; j < vpartidas.size(); j++) {
                                            if( vpartidas[j].jugador1.identificadorUsuario == i ){

                                                idJugador2 = vpartidas[j].jugador2.identificadorUsuario;

                                            } else if( vpartidas[j].jugador2.identificadorUsuario == i ) {

                                                idJugador2 = vpartidas[j].jugador1.identificador;

                                            }
                                        }

                                        // Alerta al otro usuario de que su rival ha abandonado la partida.
                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "+Ok. Tu oponente ha terminado la partida");
                                        send(idJugador2, buffer, sizeof(buffer), 0);

                                        // Saca a los jugadores de la partida.
                                        eliminaJugadoresPartida(vjugadores, i, idJugador2, vpartidas, vbaraja);
                                    } else if ( estadoJugador == 3 ) {

                                        eliminaJugador(vjugadores, i, vpartidas, vbaraja);

                                    }

                                    salirCliente(i, &readfds, &numClientes, arrayClientes);
                                    
                                } else if(strncmp(buffer, "USUARIO", strlen("USUARIO ")) == 0 ) {

                                    //Obtenemos el nombre del usuario
                                    char jugador[250];
                                    sscanf(buffer, "Usuario %s", jugador);
                                    int introducirRes = IntroducirUsuarioRegistrado(vjugadores, i, jugador);

                                    if( introducirRes == 1 ) {
                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "+Ok. USUARIO correcto.");
                                        send(i, buffer, sizeof(buffer), 0);

                                        printf("Cliente <%d> conectado con usuario correctamente.\n", i);

                                    } else if( introducirRes == 2 ) {
                                        
                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "-ERR. USUARIO incorrecto.");
                                        send(i, buffer, sizeof(buffer), 0);

                                    } else if( introducirRes == 3 ) {

                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "-ERR. Demasiados clientes conectados.");
                                        send(i, buffer, sizeof(buffer), 0);

                                    }

                                } else if(strncmp(buffer, "PASSWORD ", strlen())){
                                    
                                    char contrasena[250];
                                    sscanf(buffer, "PASSWORD %s", contrasena);

                                    if( IntroducirContraseña(vjugadores, i, contraseña) == true ){ //Contraseña
                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "+OK. Usuario validado");
                                        send(i, buffer, sizeof(buffer), 0);

                                        printf("Cliente <%d> conectado con contraseña correctamente.\n", i);
                                    } else {
                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "-ERR. Error en la validacion");
                                        send(i, buffer, sizeof(buffer), 0);
                                    }
                                
                                } else if(strncmp(buffer, "REGISTRO ", strlen("REGISTRO ")) == 0) {

                                    char contrasena[250];
                                    char usuario[250];
                                    sscanf(buffer, "REGISTRO -u %s -p %s", usuario, contrasena);

                                    if( RegistrarJugadorFichero(usuario, contrasena) == true ){ //El usuario fue registrado
                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "+OK. Usuario registrado correctamente");
                                        send(i, buffer, sizeof(buffer), 0);

                                        printf("Cliente <%d> registrado correctamente.\n", i);
                                    } else {
                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "-ERR. El nombre de usuario ya ha sido utilizado");
                                        send(i, buffer, sizeof(buffer), 0);
                                    }

                                } else if( strncmp(buffer, "INICIAR-PARTIDA", strlen("INICIAR-PARTIDA")) == 0 ) {

                                    bool conectado = false;
                                    conectado = ConectadoConUsuarioYContrasña(vjugadores, i);

                                    if(conectado) {
                                        int aux, j, b;

                                        int num = rand() % (200 - 60 + 1) + 60;
                                        
                                        res = meterJugadorEnPartida(vjugadores, i, vpartidas, &j, num);

                                        printf("Jugador <%d> buscando partida.\n", i);

                                        if( res == 1 ) {
                                            printf("Los jugadores <%d> y <%d> han encontrado partida.\n", i, j);

                                            bzero(buffer, sizeof(buffer));
                                            sprintf(buffer, "+Ok. Empieza la partida. NÚMERO OBJETIVO: %d\n", num);
                                            send(i, buffer, sizeof(buffer), 0);

                                            bzero(buffer, sizeof(buffer));
                                            sprintf(buffer, "+Ok. Empieza la partida. NÚMERO OBJETIVO: %d\n", num);
                                            send(j, buffer, sizeof(buffer), 0);

                                        } else if(res == 2) {
                                            
                                            bzero(buffer, sizeof(buffer));
                                            sprintf(buffer, "+Ok. Esperando otro jugador");
                                            send(i, buffer, sizeof(buffer), 0);
                                        } else if(res == 0) {

                                            printf("Cliente <%d> ha dejado de buscar partida. Se ha alcanzado el máximo de clientes jugando simultáneamente.\n", i);

                                            bzero(buffer, sizeof(buffer));
                                            sprintf(buffer, "-ERR. Demasiados clientes jugando al BlackJack.");
                                            send(i, buffer, sizeof(buffer), 0);
                                        }
                                    } else if (!conectado) {
                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "-ERR. No puedes iniciar partida sin antes loguearte.");
                                        send(i, buffer, sizeof(buffer), 0);
                                    }
                                } else if( strncmp(buffer, "TIRAR-DADOS", strlen("TIRAR-DADOS")) == 0 ){

                                    bool conectado = false;
                                    conectado = ConectadoConUsuarioYContraseña(vjugadores, i);

                                    if (conectado) { // Jugador autenticado
                                        bool turnoJugador, plantadoJugador;
                                        int estadoJugador = 0, puntosJugador = 0;

                                        // Comprobar turno del jugador
                                        for(int a = 0; a < vjugadores.size(); a++) {
                                            if( vjugadores[a].identificadorUsuario == i ){
                                                estadoJugador = vjugadores[a].estado;
                                                turnoJugador = vjugadores[a].turno;
                                                plantadoJugador = vjugadores[a].plantado;
                                                puntosJugador = vjugadores[a].puntos;
                                            }
                                        }

                                        if (estadoJugador == 4) {

                                            if (!plantadoJugador) {

                                                if ( turnoJugador ) {
                                                    int idJugador2 = 0, objetivo = 0;
                                                    bool plantadoJugador2;

                                                    for( int h = 0; h < vpartidas.size(); h++ ) {

                                                        if( vpartidas[h].jugador1.identificadorUsuario == i) {

                                                            objetivo = vpartidas[h].objetivo;
                                                            idJugador2 = vpartidas[h].jugador2.identificadorUsuario;
                                                            plantadoJugador2 = vpartidas[h].jugador2.plantado;

                                                        } else if( vpartidas[h].jugador2.identificadorUsuario == i ) {

                                                            objetivo = vpartidas[h].objetivo;
                                                            idJugador2 = vpartidas[h].jugador1.identificadorUsuario;
                                                            plantadoJugador2 = vpartidas[h].jugador1.plantado;
                                                        }

                                                    }

                                                    for(int l = 0; l < vjugadores.size(); l++) {
                                                        if (vjugadores[l].identificadorUsuario == i) {
                                                            
                                                            if( puntosJugador < objetivo ) {
                                                                int tiradas;

                                                                sscanf(buffer, "TIRAR-DADOS %d", &tiradas);

                                                                if(tiradas == 2){
                                                                    int n1 = tirarDados();
                                                                    int n2 = tirarDados();

                                                                    puntosJugador += n1 + n2;

                                                                    vjugadores[l].puntos = puntosJugador;

                                                                    bzero(buffer, sizeof(buffer));
                                                                    sprintf(buffer, "+Ok.[<DADO 1>, <%d>; <DADO 2>, <%d>; <PUNTUACIÓN TOTAL>, <%d>]", n1, n2, puntosJugador);
                                                                    send(i, buffer, sizeof(buffer), 0);

                                                                    bzero(buffer, sizeof(buffer));
                                                                    sprintf(buffer, "+Ok.[<TIRADA DEL RIVAL>, <%d>; <PUNTUACIÓN TOTAL DEL RIVAL>, <%d>]", n1 + n2, puntosJugador);
                                                                    send(idJugador2, buffer, sizeof(buffer), 0);

                                                                } else if (tiradas == 1){

                                                                    int n1 = tirarDados();

                                                                    puntosJugador += n1;

                                                                    vjugadores[l].puntos = puntosJugador;

                                                                    bzero(buffer, sizeof(buffer));
                                                                    sprintf(buffer, "+Ok.[<DADO 1>, <%d>; <PUNTUACIÓN TOTAL>, <%d>]", n1, puntosJugador);
                                                                    send(i, buffer, sizeof(buffer), 0);

                                                                    bzero(buffer, sizeof(buffer));
                                                                    sprintf(buffer, "+Ok.[<TIRADA DEL RIVAL>, <%d>; <PUNTUACIÓN TOTAL DEL RIVAL>, <%d>]", n1, puntosJugador);
                                                                    send(idJugador2, buffer, sizeof(buffer), 0);

                                                                } else {
                                                                    bzero(buffer, sizeof(buffer));
                                                                    sprintf(buffer, "-Err. El usuario puede tirar 1 o 2 dados.");
                                                                    send(i, buffer, sizeof(buffer), 0);
                                                                }

                                                            } else if(puntosJugador > objetivo) {
                                                                bzero(buffer, sizeof(buffer));
                                                                sprintf(buffer, "-ERR. Excedido el valor de %d.", objetivo);
                                                                send(i, buffer, sizeof(buffer), 0);

                                                            } else {
                                                                bzero(buffer, sizeof(buffer));
                                                                sprintf(buffer, "-ERR. No puedes tirar más dados, ya que tu suma es igual a %d.", objetivo);
                                                                send(i, buffer, sizeof(buffer), 0);
                                                            }
                                                        }
                                                    }
                                                } else { // No es el turno de este jugador

                                                    bzero(buffer, sizeof(buffer));
                                                    sprintf(buffer, "-ERR. Turno del jugador contrario. Espera al otro jugador.");
                                                    send(i, buffer, sizeof(buffer), 0);
                                                }
                                            } else { // Este jugador se ha plantado
                                                bzero(buffer, sizeof(buffer));
                                                sprintf(buffer, "-ERR. No puedes tirar dados. Te has plantado.");
                                                send(i, buffer, sizeof(buffer), 0);
                                            }
                                        } else { // El jugador no está en una partida
                                            bzero(buffer, sizeof(buffer));
                                            sprintf(buffer, "-ERR. No puedes tirar dados fuera de una partida.");
                                            send(i, buffer, sizeof(buffer), 0);
                                        }
                                    } else { // Jugador no logueado

                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "-ERR. No puedes tirar dados sin antes loguearte.");
                                        send(i, buffer, sizeof(buffer), 0);
                                    }

                                } else if (strncmp(buffer, "NO-TIRAR-DADOS", strlen("NO-TIRAR-DADOS")) == 0){

                                    bool conectado = false;
                                    conectado = ConectadoConUsuarioY

                                }
                                                                
                                
                            }
                            //Si el cliente introdujo ctrl+c
                            if(recibidos== 0)
                            {
                                printf("El socket %d, ha introducido ctrl+c\n", i);
                                //Eliminar ese socket
                                salirCliente(i,&readfds,&numClientes,arrayClientes);
                            }
                        }
                    }
                }
            }
		}

	close(sd);
	return 0;
	
}

void salirCliente(int socket, fd_set * readfds, int * numClientes, int arrayClientes[]){
  
    printf("Socket de cliente <%d> desconectado.\n", socket);

    char buffer[250];
    int j;
    
    close(socket);
    FD_CLR(socket,readfds);
    
    //Re-estructurar el array de clientes
    for (j = 0; j < (*numClientes) - 1; j++)
        if (arrayClientes[j] == socket)
            break;
    for (; j < (*numClientes) - 1; j++)
        (arrayClientes[j] = arrayClientes[j+1]);
    
    (*numClientes)--;

}


void manejador (int signum){
    printf("\nSe ha recibido la señal de salida\n");
    signal(SIGINT,manejador);
    
    //Implementar lo que se desee realizar cuando ocurra la excepción de ctrl+c en el servidor
}