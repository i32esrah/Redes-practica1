#ifndef SERVIDOR_HPP
#define SERVIDOR_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <iostream>
 
using namespace std;

/**
 * @brief Define el número máximo de clientes.
 */
#define MAX_CLIENTS 20

/**
 * @brief Define el tamaño máximo de línea.
 */
#define MSG_SIZE 250


/**
 * @brief Define el puerto del servidor.
 */
#define PORT 2000

/**
 * @struct jugadores
 * @brief Estructura de un jugador.
 *
 * @param usuario Nombre del jugador.
 * @param contraseña Contraseña del jugador.
 * @param turno Indica si es el turno del jugador (true) o es el turno del oponente (false).
 * @param estado Estado de conexión del jugador (0 = no conectado, 1 = usuario conectado, 2 = contraseña correcta, 3 = quiere empezar partida y 4 = jugando en una partida).
 * @param puntos Puntos del jugador en la partida.
 * @param plantado Indica si el jugador se ha plantado (true) o no (false).
 * @param identificadorPartida ID de la partida en la que está el jugador.
 * @param identificadorUsuario ID único del jugador (socket).
 */

struct jugadores
{
    string usuario;
    string contraseña;
    bool turno;
    int estado;
    int puntos;
    int contadorNoTirarDados;
    bool plantado;
    int identificadorPartida;
    int identificadorUsuario;

};


/**
 * @struct partidas
 * @brief Estructura de una partida.
 *
 * @param jugador1 Primer jugador de la partida.
 * @param jugador2 Segundo jugador de la partida.
 * @param objetivo Objetivo a alcanzar en la partida.
 */
struct partidas
{
    struct jugadores jugador1;
    struct jugadores jugador2;
    int objetivo;
};


#endif

bool ConectadoConUsuario(vector<struct jugadores> &vjugadores, int id, const char *jugador)
{
    for (int i = 0; i < vjugadores.size(); i++)
    {
        if ((vjugadores[i].identificadorUsuario == id) && (strcmp(vjugadores[i].usuario.c_str(), jugador) == 0))
        {
            return true;
        }
    }
    return false;
}


bool ConectadoConUsuarioYContraseña(vector<jugadores> &vjugadores, int id)
{
    for (int i = 0; i < vjugadores.size(); i++)
    {
        if (vjugadores[i].identificadorUsuario == id && vjugadores[i].estado >= 2)
        {
            return true;
        }
    }
    return false;
}

bool comprobarConexiones(vector<struct jugadores> vjugadores, int id)
{
    if (vjugadores.size() < MAX_CLIENTS)
    {
        return true;
    }
    return false;
}

int IntroducirUsuarioRegistrado(vector<struct jugadores> &vjugadores, int id, const char *jugador)
{

    for(int i = 0; i < vjugadores.size(); i++) {
        if( strncmp(jugador, vjugadores[i].usuario.c_str(), strlen(jugador)) == 0 ) {
            return 4;
        }
    }

    FILE *fichero;
    char linea[MSG_SIZE];

    fichero = fopen("usuarios.txt", "r");
    if (fichero == NULL)
    {
        printf("Error al abrir el archivo.\n");
        return false;
    }

    if (vjugadores.size() < MAX_CLIENTS)
    {
        while (fgets(linea, MSG_SIZE, fichero) != NULL)
        {
            char usuarioArchivo[MSG_SIZE];
            char contrasenaArchivo[MSG_SIZE];

            sscanf(linea, "%s %s", usuarioArchivo, contrasenaArchivo);

            if (strcmp(usuarioArchivo, jugador) == 0)
            {
                struct jugadores j;
                j.usuario = jugador;
                j.contraseña = string(contrasenaArchivo);
                j.estado = 1; // Conectado con usuario
                j.turno = false;
                j.puntos = 0;
                j.plantado = false;
                j.identificadorPartida = -1;
                j.identificadorUsuario = id;
                vjugadores.push_back(j);
                fclose(fichero);
                return 1; // El usuario es correcto y se ha introducido el jugador en el vector jugadores
            }
        }

        fclose(fichero);
        return 2; // El usuario introducido no es correcto
    }
    else
    {
        fclose(fichero);
        return 3; // No hay espacio en el vector para que se introduzca
    }
}

bool IntroducirContraseña(vector<struct jugadores> &vjugadores, int id, const char *contrasena)
{
    for (int i = 0; i < vjugadores.size(); i++)
    {
        if (vjugadores[i].identificadorUsuario == id)
        {
            if (vjugadores[i].contraseña == string(contrasena))
            {
                vjugadores[i].estado = 2; // Conectado con la contraseña
                return true;
            }
        }
    }
    return false;
}

bool RegistrarJugadorFichero(char *jugador, char *contrasena)
{
    FILE *fichero;
    char linea[MSG_SIZE];
    bool usuarioEncontrado = false;

    
    fichero = fopen("usuarios.txt", "r");
    if (fichero == NULL){
        printf("No existe el archivo usuarios.txt. Procede a crearlo.\n");
        fichero = fopen("usuarios.txt", "w");
        if (fichero == NULL)
        {
            printf("Error al abrir el archivo.\n");
            return false;
        }
        fclose(fichero);
        
    }

    fichero = fopen("usuarios.txt", "r");
    if (fichero == NULL){
        printf("Error al abrir el archivo.\n");
        return false;
    }
    
    while (fgets(linea, MSG_SIZE, fichero) != NULL){

        char usuarioArchivo[MSG_SIZE];
        char contrasenaArchivo[MSG_SIZE];

        sscanf(linea, "%s %s", usuarioArchivo, contrasenaArchivo);

        if (strcmp(usuarioArchivo, jugador) == 0)
        {
            usuarioEncontrado = true;
            break;
        }
    }

    fclose(fichero);

    if (usuarioEncontrado) // El nombre de usuario ya esta registrado en el fichero de texto
    {
        return false;
    }

    fichero = fopen("usuarios.txt", "a");
    if (fichero == NULL)
    {
        printf("Error al abrir el archivo.\n");
        return false;
    }

    fprintf(fichero, "%s %s\n", jugador, contrasena); // Escribir el nuevo usuario seguido de espacio y su contrasena

    fclose(fichero);
    return true;
}


void eliminaJugadoresPartida(vector<struct jugadores> &vjugadores, int id1, int id2, vector<struct partidas> &vpartidas)
{
    for (int i = 0; i < vpartidas.size(); i++)
    {
        // Ambos jugadores están en la misma partida
        if ((vpartidas[i].jugador1.identificadorUsuario == id1 && vpartidas[i].jugador2.identificadorUsuario == id2) ||
            (vpartidas[i].jugador1.identificadorUsuario == id2 && vpartidas[i].jugador2.identificadorUsuario == id1))
        {

            vpartidas.erase(vpartidas.begin() + i); // Eliminamos la partida
        }
    }

    // Eliminar a ambos jugadores del vector de jugadores
    for (int i = 0; i < vjugadores.size();)
    {
        if (vjugadores[i].identificadorUsuario == id1 || vjugadores[i].identificadorUsuario == id2)
        {
            vjugadores.erase(vjugadores.begin() + i); // Eliminar el jugador
        }
        else
        {
            i++; // Incrementar si no se ha eliminado un jugador del vector
        }
    }
}

int meterJugadorEnPartida(vector<struct jugadores> &vjugadores, int id, vector<struct partidas> &vpartidas, int *id2, int obj)
{
    // Comprueba si hay un jugador esperando emparejamiento
    for (int i = 0; i < vpartidas.size(); i++)
    {
        //Si solo hay un jugador esperando emparejamiento
        if (vpartidas[i].jugador1.estado == 3 && vpartidas[i].jugador2.estado == 0)
        {
            for (int j = 0; j < vjugadores.size(); j++)
            {
                if (vjugadores[j].identificadorUsuario == id)
                {

                    vpartidas[i].jugador2 = vjugadores[j];
                    vpartidas[i].jugador1.puntos = 0;
                    vpartidas[i].jugador2.puntos = 0;
                    vpartidas[i].jugador1.contadorNoTirarDados = 0;
                    vpartidas[i].jugador2.contadorNoTirarDados = 0;
                    vpartidas[i].jugador2.plantado = false;
                    vpartidas[i].jugador2.turno = false;
                    vpartidas[i].objetivo = obj;



                    // Cambiar el estado a jugando (4) del jugador 1
                    for (int j2 = 0; j2 < vjugadores.size(); j2++)
                    {
                        if (vjugadores[j2].identificadorUsuario == vpartidas[i].jugador1.identificadorUsuario)
                        {
                            vjugadores[j2].estado = 4;
                            (*id2) = vjugadores[j2].identificadorUsuario;
                        }
                    }

                    // Cambiar el estado a jugando (4) y el identificador de la partida del jugador 2
                    vjugadores[j].estado = 4;
                    vjugadores[j].identificadorPartida = i;
                    return 1;
                }
            }
        }
    }
    // Crea una nueva partida si hay menos de 10 partidas
    if (vpartidas.size() < 10)
    {
        partidas p;
        for (int j = 0; j < vjugadores.size(); j++)
        {
            if (vjugadores[j].identificadorUsuario == id)
            {
                vjugadores[j].estado = 3; // Cambiamos el estado del jugador a buscando partida
                vjugadores[j].turno = true;
                vjugadores[j].plantado = false;
                vjugadores[j].identificadorPartida = vpartidas.size();
                p.jugador1 = vjugadores[j];
                p.jugador2.estado = 0;
            }
        }
        vpartidas.push_back(p);
        return 2;
    }
    return 0;
}


void eliminaJugador(vector<struct jugadores> &vjugadores, int id, vector<struct partidas> &vpartidas)
{
    // Elimino el vector de la partida
    for (int i = 0; i < vpartidas.size(); i++)
    {
        if (vpartidas[i].jugador1.identificadorUsuario == id)
        {
            vpartidas.erase(vpartidas.begin() + i);
        }

        if (vpartidas[i].jugador2.identificadorUsuario == id)
        {
            vpartidas.erase(vpartidas.begin() + i);
        }
    }

    // Elimino al jugador del vector jugadores
    for (int i = 0; i < vjugadores.size(); i++)
    {
        if (vjugadores[i].identificadorUsuario == id)
        {
            vjugadores.erase(vjugadores.begin() + i);
        }
    }
}

int tirarDados(){

    return 1 + rand() % 6;

}

