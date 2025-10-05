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