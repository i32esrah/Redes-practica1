#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>

int main ( ){

    int Socket_Servidor;
    struct sockaddr_in Servidor, Cliente;
    char Datos[100];
    socklen_t Longitud;

    if ((Socket_Servidor = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
        printf("No se puede abrir el socket\n");
        exit(-1);
    }

    Servidor.sin_family = AF_INET;
	Servidor.sin_port = htons(2000);
	Servidor.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(Socket_Servidor, (struct sockaddr *)&Servidor, sizeof(Servidor)) == -1){
        printf("Error de asociacion del puerto\n");
        close(Socket_Servidor);
        exit(-1);
    }

    printf("Servidor UDP escuchando en puerto %d...\n", 2000);

    while(1){
        Longitud = sizeof(Cliente);
        int recibido = recvfrom(Socket_Servidor, Datos, sizeof(Datos), 0, (struct sockaddr *)&Cliente, &Longitud);
        if (recibido < 0 ){
            printf("Error al recibir\n");
            close(Socket_Servidor);
            exit(-1);
        }
        Datos[recibido] = '\0';

        //Obtener fecha actual
        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);
        char respuesta[100];

        if(strcmp(Datos, "DAY") == 0){
            strftime(respuesta, sizeof(respuesta), "%A, %d de %B de %Y", tm_info);
        }
        else if(strcmp(Datos, "TIME") == 0){
            strftime(respuesta, sizeof(respuesta), "%H:%M:%S", tm_info);
        }
        else if(strcmp(Datos, "DAYTIME") == 0){
            strftime(respuesta, sizeof(respuesta), "%A, %d de %B de %Y; %H:%M:%S", tm_info);
        }
        else{
            snprintf(respuesta, sizeof(respuesta), "Error, comando desconocido");
        }

        sendto(Socket_Servidor, respuesta, strlen(respuesta), 0, (struct sockaddr *)&Cliente, Longitud);
        printf("Peticion: '%s' enviada\n", Datos);
    }

    close(Socket_Servidor);

    return 0;
        
    
}