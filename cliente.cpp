#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include "servidor.hpp"


void manejador(int signum){
    printf("\n-ERR. Se ha recibido la señal sigint. Para apagar el servidor introduzca SALIR.\n");
    
    
}


int main(){

/*---------------------------------------------------- 
		Descriptor del socket y buffer de datos                
	-----------------------------------------------------*/
	int sd;
	struct sockaddr_in sockname;
	char buffer[MSG_SIZE];

	socklen_t len_sockname;
    fd_set readfds, auxfds;
    int salida;
    int fin = 0;
	
    
	/* --------------------------------------------------
		Se abre el socket 
	---------------------------------------------------*/
  	
	if (( sd = socket(AF_INET, SOCK_STREAM, 0) ) == -1)
	{
		perror("No se puede abrir el socket cliente\n");
    		exit (1);	
	}

   
    
	/* ------------------------------------------------------------------
		Se rellenan los campos de la estructura con la IP del 
		servidor y el puerto del servicio que solicitamos
	-------------------------------------------------------------------*/
	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(PORT);
	sockname.sin_addr.s_addr =  inet_addr("127.0.0.1");

	/* ------------------------------------------------------------------
		Se solicita la conexión con el servidor
	-------------------------------------------------------------------*/
	len_sockname = sizeof(sockname);
	
	if (connect(sd, (struct sockaddr *)&sockname, len_sockname) == -1)
	{
		perror ("Error de conexión");
        close(sd);
		exit(EXIT_FAILURE);
	}
    
    //Inicializamos las estructuras
    FD_ZERO(&auxfds);
    FD_ZERO(&readfds);
    
    FD_SET(0,&readfds);
    FD_SET(sd,&readfds);

    //Capturamos la señal SIGINT (Ctrl+c)
    signal(SIGINT, manejador);

    
	/* ------------------------------------------------------------------
		Se transmite la información
	-------------------------------------------------------------------*/
	do
	{
        auxfds = readfds;
        if( (salida = select(sd+1,&auxfds,NULL,NULL,NULL)) < 0){
            perror("Error en la operación de select");
            close(sd);
            exit(EXIT_FAILURE);
        };
        
        //Tengo mensaje desde el servidor
        if(FD_ISSET(sd, &auxfds)){
            
            bzero(buffer,sizeof(buffer)); //Vaciamos el buffer
            if( recv(sd,buffer,sizeof(buffer),0) <= 0){
                perror("Error en la operación de recv");
                fin = 1;
                exit(EXIT_FAILURE);
            }
            else{
                printf("\n%s\n",buffer);
                
                if(strcmp(buffer,"Demasiados clientes conectados\n") == 0)
                    fin =1;
                
                if(strcmp(buffer,"Desconexión servidor\n") == 0)
                    fin =1;
            }
        }
    
            
        //He introducido información por teclado
        if(FD_ISSET(0,&auxfds)){
            bzero(buffer,sizeof(buffer));
                
            fgets(buffer,sizeof(buffer),stdin);
            
            //Si el usuario pide salir
            if(strcmp(buffer,"SALIR\n") == 0){
                    fin = 1;
                
            }
            
            //Enviamos respuesta al servidor
            send(sd,buffer,strlen(buffer),0);
                
        }
                
        
				
    }while(fin == 0);
		
    close(sd);

    return 0;

}
