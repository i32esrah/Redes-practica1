#ifndef SERVIDOR_HPP
#define SERVIDOR_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

/**
 * @brief Define el número máximo de clientes.
 */
#define MAX_CLIENTS 20

/**
 * @brief Define el tamaño máximo de línea.
 */
#define MSG_SIZE 200


/**
 * @brief Define el puerto del servidor.
 */
#define PORT 1865

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


/**
 * @brief Comprueba si un jugador con un ID de socket dado está conectado con un nombre de usuario concreto.
 * 
 * @param vjugadores Vector con la lista de jugadores conectados.
 * @param id Identificador del socket del cliente.
 * @param jugador Nombre de usuario que se quiere comprobar.
 * @return true Si el jugador con ese ID y nombre de usuario está conectado.
 * @return false Si no existe coincidencia.
 */
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

/**
 * @brief Comprueba si un jugador está conectado y ha introducido correctamente su contraseña.
 * 
 * @param vjugadores Vector con la lista de jugadores conectados.
 * @param id Identificador del socket del cliente.
 * @return true Si el jugador tiene un estado >= 2 (contraseña validada).
 * @return false En caso contrario.
 */
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


/**
 * @brief Verifica si aún hay espacio disponible para aceptar nuevas conexiones.
 * 
 * @param vjugadores Vector actual de jugadores.
 * @param id Identificador del socket (no se usa en la comprobación).
 * @return true Si hay menos jugadores que el máximo permitido.
 * @return false Si el servidor está lleno.
 */
bool comprobarConexiones(vector<struct jugadores> vjugadores, int id)
{
    if (vjugadores.size() < MAX_CLIENTS)
    {
        return true;
    }
    return false;
}


/**
 * @brief Intenta conectar a un usuario registrado en el sistema.
 * Busca el nombre de usuario en el archivo "usuarios.txt".  
 * Si lo encuentra, lo agrega al vector de jugadores y asigna sus datos iniciales.
 * 
 * @param vjugadores Vector de jugadores conectados.
 * @param id Identificador del socket del cliente.
 * @param jugador Nombre de usuario que intenta conectarse.
 * @return 1 Si el usuario se ha conectado correctamente.  
 * @return 2 Si el usuario no existe en el fichero.  
 * @return 3 Si no hay espacio disponible en el vector.  
 * @return 4 Si el usuario ya estaba conectado.  
 */
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
                j.contadorNoTirarDados = 0;
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


/**
 * @brief Comprueba si la contraseña introducida por un jugador es correcta.
 * 
 * @param vjugadores Vector con los jugadores conectados.
 * @param id Identificador del socket del cliente.
 * @param contrasena Contraseña introducida por el usuario.
 * @return true Si la contraseña coincide con la almacenada.  
 * @return false En caso contrario.
 */
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

/**
 * @brief Registra un nuevo jugador en el fichero "usuarios.txt".
 * Verifica si el usuario ya existe. Si no, lo añade con su contraseña.
 * 
 * @param jugador Nombre de usuario a registrar.
 * @param contrasena Contraseña del nuevo usuario.
 * @return true Si el registro fue exitoso.  
 * @return false Si el usuario ya existe o hay un error de archivo.
 */
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

/**
 * @brief Elimina del sistema la partida asociada a dos jugaodres.
 * 
 * @param id1 Identificador del primer jugador.
 * @param id2 Identificador del segundo jugador.
 * @param vpartidas Vector de partidas activas.
 */
void eliminaJugadoresPartida(int id1, int id2, vector<struct partidas> &vpartidas)
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
}

/**
 * @brief Elimina completamente a un jugador del sistema y de cualquier partida activa.
 * 
 * @param vjugadores Vector de jugadores conectados.
 * @param id Identificador del jugador a eliminar.
 * @param vpartidas Vector de partidas activas.
 */
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

/**
 * @brief Asigna un jugador a una partida disponible o crea una nueva si no hay.
 * 
 * @param vjugadores Vector con todos los jugadores.
 * @param id Identificador del jugador que busca partida.
 * @param vpartidas Vector de partidas actuales.
 * @param id2 Puntero que recibe el ID del oponente si se encuentra.
 * @param obj Valor objetivo (puntos a alcanzar en la partida).
 * @return 1 Si se empareja con otro jugador.  
 * @return 2 Si se crea una nueva partida a la espera de oponente.  
 * @return 0 Si no se puede crear o unir a ninguna partida.
 */
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
                            vjugadores[j2].puntos = 0;
                            vjugadores[j2].plantado = false;
                            vjugadores[j2].estado = 4;
                            vjugadores[j2].contadorNoTirarDados = 0;
                            (*id2) = vjugadores[j2].identificadorUsuario;
                        }
                    }

                    // Cambiar el estado a jugando (4) y el identificador de la partida del jugador 2
                    vjugadores[j].puntos = 0;
                    vjugadores[j].plantado = false;
                    vjugadores[j].turno = false;
                    vjugadores[j].contadorNoTirarDados = 0;
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



/**
 * @brief Simula el lanzamiento de un dado.
 * 
 * @return int Número aleatorio entre 1 y 6.
 */
int tirarDados(){

    return 1 + rand() % 6;

}

/**
 * @brief Genera una representación en texto de la tabla de resultados entre dos jugadores.
 * 
 * @param j1 Puntuación del jugador 1.
 * @param j2 Puntuación del jugador 2.
 * @return string Cadena con formato de tabla mostrando ambos puntajes.
 */
string tablaResultado(int j1, int j2, string usuario1, string usuario2) {

    ostringstream oss;

    oss << "+-------------------------------+\n" << "| " << usuario1 << "\t| " << usuario2 << "\t|\n" << "+-------------------------------+\n" << "| " << j1 << "\t\t| " << j2 << "\t\t|\n" << "+-------------------------------+";;
    string res = oss.str();
    return res;
}
