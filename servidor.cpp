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
   	 char identificador[MSG_SIZE];
    
    	int on, ret;

    
    
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
                                        eliminaJugadoresPartida()
                                    }
                                    
                                }
                                else{
                                    
                                    sprintf(identificador,"<%d>: %s",i,buffer);
                                    bzero(buffer,sizeof(buffer));

                                    strcpy(buffer,identificador);

                                    printf("%s\n", buffer);

                                    for(j=0; j<numClientes; j++)
                                        if(arrayClientes[j] != i)
                                            send(arrayClientes[j],buffer,sizeof(buffer),0);

                                    
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