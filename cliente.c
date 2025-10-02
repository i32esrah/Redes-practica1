#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>


int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Uso: %s <IP> <PUERTO> <TIMEOUT> <COMANDO>\n", argv[0]);
        fprintf(stderr, "Ejemplo: %s 127.0.0.1 2000 5 DAYTIME\n", argv[0]);
        exit(EXIT_FAILURE);
    }


    int Socket_Cliente;
    struct sockaddr_in Servidor;
    char buffer[100];
    socklen_t Longitud = sizeof(Servidor);

    // Crear socket UDP
    if ((Socket_Cliente = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    Servidor.sin_family = AF_INET;
    Servidor.sin_port = htons(atoi(argv[2]));               // Puerto pasado como argumento
    Servidor.sin_addr.s_addr = inet_addr(argv[1]);          // IP pasada como argumento

    // Configurar timeout de recepción
    int timeout = atoi(argv[3]);
    struct timeval tv;
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    if (setsockopt(Socket_Cliente, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("setsockopt");
        close(Socket_Cliente);
        exit(EXIT_FAILURE);
    }

    char *comando = argv[4];
    int retries = 0;
    int success = 0;

    while (retries < 3 && !success) {

        printf("Enviando petición: %s (intento %d)\n", comando, retries + 1);
        int enviado = sendto(Socket_Cliente, comando, strlen(comando), 0, (struct sockaddr *)&Servidor, Longitud);
        if (enviado < 0) {
            printf("Error al enviar la petición\n");
            close(Socket_Cliente);
            exit(-1);
        }
        
        int n = recvfrom(Socket_Cliente, buffer, 100, 0, NULL, NULL);
        if (n >= 0) {
            buffer[n] = '\0';
            printf("Respuesta del servidor: %s\n", buffer);
        } else {
            retries++;
            printf("Timeout (%d/%d). Reintentando...\n", retries, 3);

            if(retries == 3){
                success = 1;
            }
        }
    }

    if (!success) {
        printf("Error: No se recibió respuesta después de %d intentos.\n", 3);
    }

    close(Socket_Cliente);
    return 0;
}
