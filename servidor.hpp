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


int IntroducirUsuarioRegistrado(vector<struct jugadores> &vjugadores, int id, const char *jugador)
{
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

