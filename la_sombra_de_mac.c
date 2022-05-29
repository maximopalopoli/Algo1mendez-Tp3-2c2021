#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "la_sombra_de_mac.h"
#include "utiles.h"

#define MOVER_ARRIBA 'W'
#define MOVER_IZQUIERDA 'A'
#define MOVER_ABAJO 'S'
#define MOVER_DERECHA 'D'
#define AUXILIAR_INICIO 'F'

#define LETRA_MAC 'M'
#define LETRA_BLOO 'B'
#define PUNTOS_VIDA 200
#define MAX_VIDAS 3
#define PUNTOS_REVIVIR 50

#define PUERTA 'D'
#define ESCALERAS 'E'
#define LLAVE 'L'
#define MONEDA 'C'
#define POZO 'W'
#define INTERRUPTOR 'O'
#define PORTAL 'P'
#define VELA 'V'
#define PARED 'I'
#define LETRA_VIDA 'V'
#define LETRA_NADA 'N'

#define PUERTAS_LVL123 1

#define MAX_COORDENADAS_VELA 9
#define NO_ENCONTRADO -1
#define JUEGO_PERDIDO -1
#define JUEGO_GANADO 1
#define JUGANDO 0
#define NIVEL_GANADO 1

#define COORDENADA_MENOS_UNO_VELA -1
#define COORDENADA_MAS_UNO_VELA 1
#define TOPE_RANGO_VELA 2


//DE UTILIDAD

/*
*   La función recibe un caracter y devuelve true
* si el movimiento recibido concuerda con la convención
* propuesta, sino false.
* -> W: Si el personaje debe moverse para la arriba. 
* -> A: Si el personaje debe moverse para la izquierda.
* -> S: Si el personaje debe moverse para la abajo.
* -> D: Si el personaje debe moverse para la derecha.
* -> F: Como auxiliar para no mover al personaje sin que la persona vea el tablero
* -> V: Para canjear una vida por 200 puntos (en caso de ser posible)
*/
bool es_movimiento_valido(char movimiento){
    return ((movimiento == MOVER_ARRIBA) || (movimiento == MOVER_IZQUIERDA) || (movimiento == MOVER_ABAJO) || (movimiento == MOVER_DERECHA) || (movimiento == AUXILIAR_INICIO) || (movimiento == LETRA_VIDA));
}

/*
*   Función que dados una altura máxima y un ancho máximo
* devuelve una coordenada aleatoria. Una posición nunca podrá
* tener un valor negativo, ni ser mayor a sus máximos.
*/
coordenada_t coordenada_aleatoria(int max_alto, int max_ancho){
    coordenada_t coordenada_aleatoria_interna;
    coordenada_aleatoria_interna.fila = rand () % max_alto;
    coordenada_aleatoria_interna.col = rand () % max_ancho;
    
    return coordenada_aleatoria_interna;
}

/*
*    Función que recibe dos coordenadas, devuelve true si las
* coordenadas son iguales, sino false.
*/
bool coordenadas_iguales(coordenada_t a, coordenada_t b){ 
    return((a.fila == b.fila) && (a.col == b.col));
}

/*
*    La función recibe un vector de elementos, su respectivo tope y una coordenada,
* devuelve la posición del elemento del vector, que coincida con la coordenada pasada 'posicion',
* si no se encuentra ningún elemento, se devolverá -1. 
*   pre: el tope debe estar inicializado
*/
int buscar_elemento(elemento_t elementos[MAX_ELEMENTOS], int tope, coordenada_t posicion){
    int posicion_elemento = NO_ENCONTRADO;
    int i = 0;
    bool encontrado = false;

    while((i < tope) && (encontrado == false)){
        if(coordenadas_iguales(elementos[i].coordenada, posicion)){
            posicion_elemento = i;
            encontrado = true;
        }
        i ++;
    }

    return posicion_elemento;
}

/*
*   La función recibe un vector de paredes, su tope y una coordenada y retorna
*   si en esa coordenada en ese nivel hay una pared
*   pre: el tope debe ser mayor o igual a cero y la coordenada tiene que estar en rango
*/
bool hay_pared(coordenada_t paredes[MAX_PAREDES], int tope_paredes, coordenada_t coordenada){
    bool encontrado = false;

    for (int i=0 ; i < tope_paredes ; i++){
        if(coordenadas_iguales(paredes[i], coordenada)){
            encontrado = true;
        }
    }

    return encontrado;
}

/*
*   El procedimiento recibe los topes de obstáculos y de herramientas y el número de nivel y los inicializa en cero
*/
void inicializar_topes(int* tope_obstaculos, int* tope_herramientas){
    (*tope_obstaculos) = 0;
    (*tope_herramientas) = 0;
}

/*
*   La función recibe un vector de herramientas con su tope y una coordenada y retorna true si 
*   en ese nivel hay una escalera
*   pre: el tope tiene que estar inicializado
*/
bool hay_escalera(elemento_t herramientas[MAX_ELEMENTOS], int tope_herramientas, coordenada_t coordenada){
    bool encontrado = false;

    for (int i=0 ; i < tope_herramientas ; i++){
        if(coordenadas_iguales(herramientas[i].coordenada, coordenada) && (herramientas[i].tipo == ESCALERAS)){
            encontrado = true;
        }
    }

    return encontrado;
}

/*
*   La función recibe un juego_t, una coordenada y el número de nivel y determina 
*   si en esa coordenada en ese nivel hay algo, ya sea un elemento, pared, el personaje o la sombra 
*/
bool hay_algo(juego_t juego, int numero_nivel, coordenada_t coordenada){
    return (
      (buscar_elemento(juego.niveles[numero_nivel].obstaculos, juego.niveles[numero_nivel].tope_obstaculos, coordenada) != NO_ENCONTRADO) ||     // Verifica si hay obstáculos
      (buscar_elemento(juego.niveles[numero_nivel].herramientas, juego.niveles[numero_nivel].tope_herramientas, coordenada) != NO_ENCONTRADO) || // Verifica si hay herramientas
      (hay_pared(juego.niveles[numero_nivel].paredes, juego.niveles[numero_nivel].tope_paredes, coordenada) == true) ||               // Verifica si hay paredes
      (coordenadas_iguales(juego.personaje.posicion, coordenada) == true) ||                                                          // Verifica si está Mac
      (coordenadas_iguales(juego.sombra.posicion, coordenada) == true)                                                                // Verifica si está Bloo
    );
}

/*
*   La función recibe un juego_t, el número de nivel y los máximos de alto y de ancho y devuelve
*   una coordenada en la que no hay elementos, paredes o personajes
*/
coordenada_t coordenada_aleatoria_elemento(juego_t juego, int max_alto, int max_ancho, int numero_nivel){
    coordenada_t coordenada_aleatoria_interna = coordenada_aleatoria(max_alto, max_ancho);
    
    while(hay_algo(juego, numero_nivel, coordenada_aleatoria_interna) == true){
        coordenada_aleatoria_interna = coordenada_aleatoria(max_alto, max_ancho);
    }

    return coordenada_aleatoria_interna;
}

/*
*   La función recibe un un vector de obstáculos y su tope, el vector de herramientas y su tope y una coordenada y devuelve la letra del tipo de elemento
*   que hay en esa posición, ya sea un obstáculo o una herramienta
*   pre: los topes deben estar inicializados
*/
char letra_elemento(elemento_t obstaculos[MAX_ELEMENTOS], int tope_obstaculos, elemento_t herramientas[MAX_ELEMENTOS], int tope_herramientas , coordenada_t coordenada){
    int pos_obstaculo = buscar_elemento(obstaculos, tope_obstaculos, coordenada);
    int pos_herramienta = buscar_elemento(herramientas, tope_herramientas, coordenada);

    if (pos_obstaculo != NO_ENCONTRADO){
        return (obstaculos[pos_obstaculo].tipo);
    }else if (pos_herramienta != NO_ENCONTRADO){
        return (herramientas[pos_herramienta].tipo);
    }else {
        return LETRA_NADA;
    }
}

/*
*   La función recibe un vector de obstáculos y su tope, el vector de herramientas y su tope y una coordenada y devuelve true si está en un rango de 
*   una casilla respecto de una vela
*   pre: los topes deben estar inicializados
*/
bool efecto_vela(elemento_t obstaculos[MAX_ELEMENTOS], int tope_obstaculos, elemento_t herramientas[MAX_ELEMENTOS], int tope_herramientas, coordenada_t coordenada){
    coordenada_t vector_coordenadas_internas[MAX_COORDENADAS_VELA];
    int pos_vector = 0;
    for (int i = COORDENADA_MENOS_UNO_VELA; i < TOPE_RANGO_VELA; i++){    
        for (int j = COORDENADA_MENOS_UNO_VELA; j < TOPE_RANGO_VELA; j++){
            coordenada_t coordenada_llenar_matriz = coordenada;

            if (i == COORDENADA_MENOS_UNO_VELA){
                coordenada_llenar_matriz.fila ++;
            }
            if (i == COORDENADA_MAS_UNO_VELA){
                coordenada_llenar_matriz.fila --;
            }

            if(j == COORDENADA_MENOS_UNO_VELA){
                coordenada_llenar_matriz.col --;
            }
            if (j == COORDENADA_MAS_UNO_VELA){
                coordenada_llenar_matriz.col ++;
            }

            vector_coordenadas_internas[pos_vector] = coordenada_llenar_matriz;
            pos_vector++;
        }
    }

    bool hay_vela = false;
    for (int i=0 ; i < MAX_COORDENADAS_VELA ; i++){
        if (letra_elemento(obstaculos, tope_obstaculos, herramientas, tope_herramientas, vector_coordenadas_internas[i]) == VELA){
            hay_vela = true;
        }
    }

    return hay_vela;
}

/*
*   La función recibe un vector de paredes con su tope y un vector de herramientas con su tope y devuelve una coordenada
*   que sea apta para una escalera (que coincida con la de una pared)
*/
coordenada_t coordenada_escalera(coordenada_t paredes[MAX_PAREDES], int tope_paredes, elemento_t herramientas[MAX_ELEMENTOS], int tope_herramientas){
    coordenada_t coordenada_aleatoria_interna;
    
    bool encontrado = false;
    while(encontrado == false){
        coordenada_aleatoria_interna = coordenada_aleatoria(MAX_FILAS, MAX_COLUMNAS);
        int i = 0;
        while(i < tope_paredes){
            if ((coordenadas_iguales(coordenada_aleatoria_interna, paredes[i])) && (buscar_elemento(herramientas, tope_herramientas, coordenada_aleatoria_interna) == NO_ENCONTRADO)){
                encontrado = true;
            }
            i++;
        }
    }

    return coordenada_aleatoria_interna;
}

/*
*    La función recibe una coordenada, devuelve true
* si la posición está dentro de las dimensiones dadas, sino false.
* Una posición nunca podrá tener un valor negativo y no tendrá un 
* valor más alto que los máximos.
*/
bool esta_dentro_rango(coordenada_t posicion, int max_alto, int max_ancho){
    return ((posicion.col >= 0) && (posicion.col < max_ancho) && (posicion.fila >= 0) && (posicion.fila < max_alto));
}

/*
*   El procedimiento recibe la coordenada del personaje y un movimiento y actualiza la posición
*   de acuerdo al movimiento recibido
*/
void realizar_movimiento_personaje(coordenada_t* pos_recibida, char movimiento){
    if (movimiento == MOVER_ARRIBA){
        (*pos_recibida).fila --;
    }else if (movimiento == MOVER_ABAJO){
        (*pos_recibida).fila ++;
    }else if (movimiento == MOVER_IZQUIERDA){
        (*pos_recibida).col --;
    }else if (movimiento == MOVER_DERECHA){
        (*pos_recibida).col ++;
    }
}

/*
*   El procedimiento recibe la coordenada de la sombra y un movimiento y actualiza la posición
*   de acuerdo al movimiento recibido
*/
void realizar_movimiento_sombra(coordenada_t* pos_recibida, char movimiento){
    if (movimiento == MOVER_ARRIBA){
        (*pos_recibida).fila --;
    }else if (movimiento == MOVER_ABAJO){
        (*pos_recibida).fila ++;
    }else if (movimiento == MOVER_IZQUIERDA){
        (*pos_recibida).col ++;
    }else if (movimiento == MOVER_DERECHA){
        (*pos_recibida).col --;
    }
}

/*
*   La función recibe una coordenada del personaje y un movimiento y devuelve true si en caso de efectuarse un 
*   movimiento el personaje se sale de rango
*/
bool escalera_a_vacio (coordenada_t pos_personaje, char movimiento){
    coordenada_t coordenada_interna = pos_personaje;
    realizar_movimiento_personaje(&coordenada_interna, movimiento);
    
    return ((!esta_dentro_rango(coordenada_interna, MAX_FILAS, MAX_COLUMNAS)));
}

/*
*   El procedimiento recibe un vector de elementos, su tope y una coordenada a eliminar, y elimina
*   el elemento que tenga una coordenada igual a la recibida
*/
void eliminar_elemento(elemento_t elementos[MAX_ELEMENTOS], int* tope_elemento, coordenada_t coordenada_a_eliminar){
    int pos_elemento = buscar_elemento(elementos, (*tope_elemento), coordenada_a_eliminar);
    
    for(int i=pos_elemento ; i < (*tope_elemento) - 1 ; i++){
        elementos[i] = elementos[i+1];
    }

    (*tope_elemento) --;
}

/*
*   La función recibe un movimiento y devuelve el movimiento opuesto (Por ejemplo si
*   el movimiento es 'W' devuelve 'S')
*/
char movimiento_invertido(char movimiento){
    char movimiento_invertido_interno;

    if(movimiento == MOVER_ARRIBA){
        movimiento_invertido_interno = MOVER_ABAJO;
    }
    if (movimiento == MOVER_ABAJO){
        movimiento_invertido_interno = MOVER_ARRIBA;
    }
    if (movimiento == MOVER_IZQUIERDA){
        movimiento_invertido_interno = MOVER_DERECHA;
    }
    if (movimiento == MOVER_DERECHA){
        movimiento_invertido_interno = MOVER_IZQUIERDA;
    }

    return movimiento_invertido_interno;
}

/*
*   El procedimiento recibe un juego_t y un movimiento y mueve al personaje
*   de acuerdo a las convenciones
*/
void mover_personaje_mac(juego_t* juego, char movimiento){
    coordenada_t pos_nueva_personaje = (*juego).personaje.posicion;
    realizar_movimiento_personaje(&pos_nueva_personaje, movimiento);

    if ((esta_dentro_rango(pos_nueva_personaje, MAX_FILAS, MAX_COLUMNAS)) && (hay_pared((*juego).niveles[(*juego).nivel_actual - 1].paredes, (*juego).niveles[(*juego).nivel_actual - 1].tope_paredes, pos_nueva_personaje) == false)){
        (*juego).personaje.posicion = pos_nueva_personaje;
    }
    if ((esta_dentro_rango(pos_nueva_personaje, MAX_FILAS, MAX_COLUMNAS)) && (hay_pared((*juego).niveles[(*juego).nivel_actual - 1].paredes, (*juego).niveles[(*juego).nivel_actual - 1].tope_paredes, pos_nueva_personaje) == true) && (hay_escalera((*juego).niveles[(*juego).nivel_actual - 1].herramientas, (*juego).niveles[(*juego).nivel_actual - 1].tope_herramientas, pos_nueva_personaje) == true)){
        (*juego).personaje.posicion = pos_nueva_personaje;
    }
}

/*
*   El procedimiento recibe un juego_t y un movimiento y mueve a la sombra
*   de acuerdo a las convenciones
*/
void mover_personaje_bloo(juego_t* juego, char movimiento){
    coordenada_t pos_nueva_sombra = (*juego).sombra.posicion;
    
    if ((*juego).personaje.interruptor_apretado == false){
        realizar_movimiento_sombra(&pos_nueva_sombra, movimiento);
    }else {
        realizar_movimiento_personaje(&pos_nueva_sombra, movimiento);
    }

    if ((esta_dentro_rango(pos_nueva_sombra, MAX_FILAS, MAX_COLUMNAS)) && (hay_pared((*juego).niveles[(*juego).nivel_actual - 1].paredes, (*juego).niveles[(*juego).nivel_actual - 1].tope_paredes, pos_nueva_sombra) == false) && ((*juego).sombra.esta_viva == true)){
        (*juego).sombra.posicion = pos_nueva_sombra;
    }
}

/*
*   La función recibe un vector de paredes con su tope, un vector de herramientas con su tope y una coordenada y devuelve true
*   si en frente hay una pared pero no una escalera
*/
bool hay_pared_no_escalera (coordenada_t paredes[MAX_PAREDES], int tope_paredes, elemento_t herramientas[MAX_ELEMENTOS], int tope_herramientas, coordenada_t coordenada){
    bool encontrado = false;
    
    for (int i=0 ; i < tope_paredes ; i++){
        if(coordenadas_iguales(paredes[i], coordenada)){
            encontrado = true;
        }
    }

    if (encontrado == false){
        return false;
    }else {
        bool coinciden = false;

        for (int i=0 ; i < tope_herramientas ; i++){
            if(coordenadas_iguales(herramientas[i].coordenada, coordenada)){
                coinciden = true;
            }
        }
        
        return !coinciden;
    }
}

/*
*   Pre: La coordenada no debe salirse de los parámetros, ya sea excediendo
*   el máximo de filas o columnas o poseer un número negativo.  
*   Post: Devuelve la coordenada de Bloo (Estando dentro de los limites establecidos).
*/
coordenada_t coordenada_para_bloo(coordenada_t ref_coordenada_mac){
    coordenada_t coordenada_interna;
    coordenada_interna.fila = ref_coordenada_mac.fila;
    coordenada_interna.col = (MAX_COLUMNAS - 1) - ref_coordenada_mac.col;
    
    return coordenada_interna;
}

/*
*   La función recibe el juego y devuelve true si no hay ni paredes ni elementos en las coordenadas del personaje y de la sombra
*/
bool hay_elemento_o_pared(juego_t* juego){
    return(
      (hay_pared((*juego).niveles[(*juego).nivel_actual - 1].paredes, (*juego).niveles[(*juego).nivel_actual - 1].tope_paredes, (*juego).personaje.posicion) == true) ||                // Verifica si hay una pared donde está el personaje
      (hay_pared((*juego).niveles[(*juego).nivel_actual - 1].paredes, (*juego).niveles[(*juego).nivel_actual - 1].tope_paredes, (*juego).sombra.posicion) == true) ||                   // Verifica si hay una pared donde está la sombra
      (buscar_elemento((*juego).niveles[(*juego).nivel_actual - 1].obstaculos, (*juego).niveles[(*juego).nivel_actual - 1].tope_obstaculos, (*juego).personaje.posicion) != NO_ENCONTRADO) ||      // Verifica si hay un obstaculo donde está el personaje
      (buscar_elemento((*juego).niveles[(*juego).nivel_actual - 1].herramientas, (*juego).niveles[(*juego).nivel_actual - 1].tope_herramientas, (*juego).personaje.posicion) != NO_ENCONTRADO) ||  // Verifica si hay una herramienta donde está el personaje
      (buscar_elemento((*juego).niveles[(*juego).nivel_actual - 1].obstaculos, (*juego).niveles[(*juego).nivel_actual - 1].tope_obstaculos, (*juego).sombra.posicion) != NO_ENCONTRADO) ||         // Verifica si hay un obstaculo donde está la sombra
      (buscar_elemento((*juego).niveles[(*juego).nivel_actual - 1].herramientas, (*juego).niveles[(*juego).nivel_actual - 1].tope_herramientas, (*juego).sombra.posicion) != NO_ENCONTRADO)        // Verifica si hay una herramienta donde está la sombra
    );
}

/*
*   El procedimiento recibe un juego_t y reasigna de forma aleatoria las coordenadas de Mac y Bloo
*   (Además mata a Bloo si su coordenada esta en el rango de una vela)
*/
void resetear_coordenadas(juego_t* juego){
    if ((*juego).nivel_actual <= MAX_NIVELES){
        (*juego).personaje.posicion = coordenada_aleatoria(MAX_FILAS, MAX_COLUMNAS);
        (*juego).sombra.posicion = coordenada_para_bloo((*juego).personaje.posicion);
        
        while(hay_elemento_o_pared(juego)){
            (*juego).personaje.posicion = coordenada_aleatoria(MAX_FILAS, MAX_COLUMNAS);
            (*juego).sombra.posicion = coordenada_para_bloo((*juego).personaje.posicion);
        }

        if (efecto_vela((*juego).niveles[(*juego).nivel_actual - 1].obstaculos, (*juego).niveles[(*juego).nivel_actual - 1].tope_obstaculos, (*juego).niveles[(*juego).nivel_actual - 1].herramientas, (*juego).niveles[(*juego).nivel_actual - 1].tope_herramientas, (*juego).sombra.posicion)){
            (*juego).sombra.esta_viva = false;
        }
    }
}

/*
*   La función recibe un vector de paredes(con su tope), un vector de herramientas(con su tope), una coordenada y un movimiento y devuelve si hay una pared (y no hay 
*   una escalera) tras un movimiento en ese sentido
*/
bool solo_pared_adelante (coordenada_t paredes[MAX_PAREDES], int tope_paredes, elemento_t herramientas[MAX_ELEMENTOS], int tope_herramientas, coordenada_t coordenada, char movimiento){
    coordenada_t coordenada_interna = coordenada;
    realizar_movimiento_personaje(&coordenada_interna, movimiento);

    return hay_pared_no_escalera(paredes, tope_paredes, herramientas, tope_herramientas, coordenada_interna);
}

/*
*   El procedimiento recibe al struct del personaje y le suma una vida en caso de
*   que se cumplan las condiciones
*/
void sumar_vida(personaje_t* personaje){
    if (((*personaje).vida < MAX_VIDAS) && ((*personaje).puntos >= PUNTOS_VIDA)){
        (*personaje).vida ++;
        (*personaje).puntos = (*personaje).puntos - PUNTOS_VIDA;
    }
}

/*
*   La función recibe dos coordenadas y devuelve la distancia manhattan entre ambas
*/
int distancia_manhattan(coordenada_t coordenada_1, coordenada_t coordenada_2){
    return (abs(coordenada_1.fila - coordenada_2.fila) + abs(coordenada_1.col - coordenada_2.col));
}

/*
*   La funcion recibe un juego_t y devuelve true en caso de estar a distancia manhattan 1
*   o menos respecto de la puerta
*/
bool esta_cerca_puerta(juego_t juego){
    coordenada_t coordenada_puerta;
    for (int i=0 ; i < juego.niveles[juego.nivel_actual - 1].tope_herramientas ; i++){
        if (juego.niveles[juego.nivel_actual - 1].herramientas[i].tipo == PUERTA){
            coordenada_puerta = juego.niveles[juego.nivel_actual - 1].herramientas[i].coordenada;
        }
    }

    return ((distancia_manhattan(juego.personaje.posicion, coordenada_puerta) <= 1) && (distancia_manhattan(juego.sombra.posicion, coordenada_puerta) <= 1));
}


//PARA INICIALIZAR

/*
*   El procedimiento recibe un juego_t, el número de nivel y el número de obstáculos de
*   ese nivel y lo rellena hasta el tope
*/
void rellenar_vector_obstaculos(juego_t* juego, int n_velas, int n_pozos, int n_interruptores, int n_portales, int numero_nivel){
    int i = 0;

    int total_obstaculos = n_velas + n_pozos + n_interruptores + n_portales;

    while(i < total_obstaculos){
        if (i < n_velas){
            (*juego).niveles[numero_nivel].obstaculos[i].tipo = VELA;
            (*juego).niveles[numero_nivel].obstaculos[i].coordenada = coordenada_aleatoria_elemento((*juego), MAX_FILAS, MAX_COLUMNAS, numero_nivel);
        }else if ((i >= n_velas) && (i < (n_velas+n_pozos))){
            (*juego).niveles[numero_nivel].obstaculos[i].tipo = POZO;
            (*juego).niveles[numero_nivel].obstaculos[i].coordenada = coordenada_aleatoria_elemento((*juego), MAX_FILAS, MAX_COLUMNAS, numero_nivel);
        }else if (i >= (n_velas+n_pozos) && (i < (n_velas + n_pozos + n_interruptores))){
            (*juego).niveles[numero_nivel].obstaculos[i].tipo = INTERRUPTOR;
            (*juego).niveles[numero_nivel].obstaculos[i].coordenada = coordenada_aleatoria_elemento((*juego), MAX_FILAS, MAX_COLUMNAS, numero_nivel);
        }else {
            (*juego).niveles[numero_nivel].obstaculos[i].tipo = PORTAL;
            (*juego).niveles[numero_nivel].obstaculos[i].coordenada = coordenada_aleatoria_elemento((*juego), MAX_FILAS, MAX_COLUMNAS, numero_nivel);
        }
        i ++;
        (*juego).niveles[numero_nivel].tope_obstaculos ++;
    }
}

/*
*   El procedimiento recibe un juego_t, el número de nivel y el vector de número de elementos e inicializa el vector de obstáculos
*   de cada nivel por separado
*/
void inicializar_obstaculos(juego_t* juego, int numero_nivel, numero_t vector_numero_elementos[MAX_NIVELES]){
    if (numero_nivel == 0){        
        rellenar_vector_obstaculos(juego, vector_numero_elementos[0].n_velas, vector_numero_elementos[0].n_pozos, vector_numero_elementos[0].n_interruptores, vector_numero_elementos[0].n_portales, numero_nivel);
    }else if (numero_nivel == 1){
        rellenar_vector_obstaculos(juego, vector_numero_elementos[1].n_velas, vector_numero_elementos[1].n_pozos, vector_numero_elementos[1].n_interruptores, vector_numero_elementos[1].n_portales, numero_nivel);
    }else {
        rellenar_vector_obstaculos(juego, vector_numero_elementos[2].n_velas, vector_numero_elementos[2].n_pozos, vector_numero_elementos[2].n_interruptores, vector_numero_elementos[2].n_portales, numero_nivel);
    }
}


/*
*   El procedimiento recibe un juego_t, el número de nivel y el número de herramientas de
*   ese nivel y lo rellena hasta el tope
*/
void rellenar_vector_herramientas(juego_t* juego, int n_escaleras, int n_monedas, int n_llaves, int n_puertas, int numero_nivel){
    int i = 0;

    int total_herramientas = n_escaleras + n_monedas + n_llaves + n_puertas;

    while(i < total_herramientas){
        if (i < n_escaleras){
            (*juego).niveles[numero_nivel].herramientas[i].tipo = ESCALERAS;
            (*juego).niveles[numero_nivel].herramientas[i].coordenada = coordenada_escalera((*juego).niveles[numero_nivel].paredes, (*juego).niveles[numero_nivel].tope_paredes, (*juego).niveles[numero_nivel].herramientas, (*juego).niveles[numero_nivel].tope_herramientas);
        }else if ((i >= n_escaleras) && (i < (n_escaleras+n_monedas))){
            (*juego).niveles[numero_nivel].herramientas[i].tipo = MONEDA;
            (*juego).niveles[numero_nivel].herramientas[i].coordenada = coordenada_aleatoria_elemento((*juego), MAX_FILAS, MAX_COLUMNAS, numero_nivel);
        }else if ((i >= (n_escaleras+n_monedas)) && (i < (n_escaleras+n_monedas+n_llaves))){
            (*juego).niveles[numero_nivel].herramientas[i].tipo = LLAVE;
            (*juego).niveles[numero_nivel].herramientas[i].coordenada = coordenada_aleatoria_elemento((*juego), MAX_FILAS, MAX_COLUMNAS, numero_nivel);
        }else {
            (*juego).niveles[numero_nivel].herramientas[i].tipo = PUERTA;
            (*juego).niveles[numero_nivel].herramientas[i].coordenada = coordenada_aleatoria_elemento((*juego), MAX_FILAS, MAX_COLUMNAS, numero_nivel);
        }
        i ++;
        (*juego).niveles[numero_nivel].tope_herramientas ++;
    }
}

/*
*   El procedimiento recibe un juego_t, el número de nivel y el vector de número de elementos e inicializa el vector de herramientas
*   de cada nivel por separado
*/
void inicializar_herramientas(juego_t* juego, int numero_nivel, numero_t vector_numero_elementos[MAX_NIVELES]){
    if (numero_nivel == 0){
        rellenar_vector_herramientas(juego, vector_numero_elementos[0].n_escaleras, vector_numero_elementos[0].n_monedas, vector_numero_elementos[0].n_llaves, PUERTAS_LVL123, numero_nivel);
    }else if (numero_nivel == 1){
        rellenar_vector_herramientas(juego, vector_numero_elementos[1].n_escaleras, vector_numero_elementos[1].n_monedas, vector_numero_elementos[1].n_llaves, PUERTAS_LVL123, numero_nivel);
    }else {
        rellenar_vector_herramientas(juego, vector_numero_elementos[2].n_escaleras, vector_numero_elementos[2].n_monedas, vector_numero_elementos[2].n_llaves, PUERTAS_LVL123, numero_nivel);
    }
}

/*
*   El procedimiento recibe un juego_t e inicializa las coordenadas de los personajes
*   (cuidando que no haya una pared)
*/
void inicializar_coordenadas(juego_t* juego){
    (*juego).personaje.posicion = coordenada_aleatoria(MAX_FILAS, MAX_COLUMNAS);
    (*juego).sombra.posicion = coordenada_para_bloo((*juego).personaje.posicion);
    
    while((hay_pared((*juego).niveles[(*juego).nivel_actual - 1].paredes, (*juego).niveles[(*juego).nivel_actual - 1].tope_paredes, (*juego).personaje.posicion) == true) || (hay_pared((*juego).niveles[(*juego).nivel_actual - 1].paredes, (*juego).niveles[(*juego).nivel_actual - 1].tope_paredes, (*juego).sombra.posicion) == true)){
        (*juego).personaje.posicion = coordenada_aleatoria(MAX_FILAS, MAX_COLUMNAS);
        (*juego).sombra.posicion = coordenada_para_bloo((*juego).personaje.posicion);
    }
}

/*
*   El procedimiento recibe un juego_t y el vector de número de elementos e inicializa los niveles
*/
void inicializar_niveles(juego_t* juego, numero_t vector_numero_elementos[MAX_NIVELES]){
    
    cargar_mapas();

    int i=0;
    while(i < MAX_NIVELES){
        (*juego).niveles[i].numero_nivel = i;
        obtener_mapa((*juego).niveles[i].paredes, &((*juego).niveles[i].tope_paredes));
        inicializar_coordenadas(juego);
        inicializar_topes(&((*juego).niveles[(*juego).niveles[i].numero_nivel].tope_obstaculos), &((*juego).niveles[(*juego).niveles[i].numero_nivel].tope_herramientas));
        inicializar_obstaculos(juego, (*juego).niveles[i].numero_nivel, vector_numero_elementos);
        inicializar_herramientas(juego, (*juego).niveles[i].numero_nivel, vector_numero_elementos);

        i++;
    }
}

/*
*   El procedimiento recibe al struct del personaje y lo inicializa
*/
void inicializar_personaje(personaje_t* ref_personaje){
    (*ref_personaje).vida = MAX_VIDAS;
    (*ref_personaje).puntos = 0;
    (*ref_personaje).tiene_llave = false;
    (*ref_personaje).interruptor_apretado = false;
}

/*
*   El procedimiento recibe al struct de la sombra y lo inicializa
*/
void inicializar_sombra(sombra_t* sombra, elemento_t obstaculos[MAX_ELEMENTOS], int tope_obstaculos, elemento_t herramientas[MAX_ELEMENTOS], int tope_herramientas){
    (*sombra).esta_viva = true;
}



/* 
*   Inicializará el juego, cargando toda la información inicial, los datos del personaje y la sombra, y los 3 niveles.(Recibe también 
*   el vector con el número de elementos de cada uno)
*/
void inicializar_juego(juego_t* juego, numero_t vector_numero_elementos[MAX_NIVELES]){
    (*juego).nivel_actual = 1;
    inicializar_niveles(juego, vector_numero_elementos);
    inicializar_personaje(&((*juego).personaje));
    inicializar_sombra(&((*juego).sombra), (*juego).niveles[(*juego).nivel_actual - 1].obstaculos, (*juego).niveles[(*juego).nivel_actual - 1].tope_obstaculos, (*juego).niveles[(*juego).nivel_actual - 1].herramientas, (*juego).niveles[(*juego).nivel_actual - 1].tope_herramientas);
}

//PROCEDIMIENTO

/*
*   Procedimiento que se encargará de pedirle al usuario
*   que ingrese un movimiento hasta que sea válido.
*/
void pedir_movimiento(char* ref_movimiento){
    printf("Ingrese un movimiento\n");
    scanf(" %c", ref_movimiento);
    
    if (!es_movimiento_valido(*ref_movimiento)){
        printf("Ese movimiento no es válido, te pido que ingreses '%c', '%c', '%c' o '%c' para moverte para arriba, para la izquierda, para abajo o para la derecha, respectivamente\n", MOVER_ARRIBA, MOVER_IZQUIERDA, MOVER_ABAJO, MOVER_DERECHA);
        scanf(" %c", ref_movimiento);
    }
    
    while (!es_movimiento_valido(*ref_movimiento)){
        printf("Ese movimiento no es válido, te pido que ingreses un movimiento válido para continuar\n");
        scanf(" %c", ref_movimiento);
    } 
}

/*
*   El procedimiento recibe el personaje y la sombra por referencia y el movimiento,
*   y según este último, los mueve acordemente.
* 
*   El personaje se mueve de la forma:
*   -> W: Si el personaje debe moverse para la arriba. 
*   -> A: Si el personaje debe moverse para la izquierda.
*   -> S: Si el personaje debe moverse para la abajo.
*   -> D: Si el personaje debe moverse para la derecha.
*
*   La sombra se mueve de igual forma para el eje vertical, pero
*   para el eje horizontal se moverá de forma inversa:
*
*   -> A: Si el personaje debe moverse para la derecha.
*   -> D: Si el personaje debe moverse para la izquierda.
*/
void mover_personaje(juego_t* juego, char movimiento){
    mover_personaje_mac(juego, movimiento);
    mover_personaje_bloo(juego, movimiento);
}

/*
*   Procedimiento que rellena una matriz recibida de los elementos que hay en los vectores, así como las paredes y mac
*   y bloo
*/
void rellenar_matriz(juego_t juego, char dibujo[MAX_FILAS][MAX_COLUMNAS]){
    for (int i = 0; i < MAX_FILAS; ++i){
        for (int j = 0; j < MAX_COLUMNAS; ++j){
            dibujo[i][j] = ' ';
        }
    }

    for (int i=0 ; i < juego.niveles[juego.nivel_actual - 1].tope_paredes ; i++){
    coordenada_t coordenada_interna = juego.niveles[juego.nivel_actual - 1].paredes[i];
    dibujo[coordenada_interna.fila][coordenada_interna.col] = PARED;
    }

    for (int i=0 ; i < juego.niveles[juego.nivel_actual - 1].tope_obstaculos ; i++){
    coordenada_t coordenada_interna = juego.niveles[juego.nivel_actual - 1].obstaculos[i].coordenada;
    dibujo[coordenada_interna.fila][coordenada_interna.col] = juego.niveles[juego.nivel_actual - 1].obstaculos[i].tipo; //no hay que imprimir una cosa sola, sino todas. chequear si podes imprimir por tipo
    }

    for (int i=0 ; i < juego.niveles[juego.nivel_actual - 1].tope_herramientas ; i++){
    coordenada_t coordenada_interna = juego.niveles[juego.nivel_actual - 1].herramientas[i].coordenada;
    dibujo[coordenada_interna.fila][coordenada_interna.col] = juego.niveles[juego.nivel_actual - 1].herramientas[i].tipo; //no hay que imprimir una cosa sola, sino todas. chequear si podes imprimir por tipo
    }

    dibujo [juego.sombra.posicion.fila][juego.sombra.posicion.col] = LETRA_BLOO;
    dibujo [juego.personaje.posicion.fila][juego.personaje.posicion.col] = LETRA_MAC;
}

/*
*   Procedimiento que recibe el juego e imprime toda su información por pantalla.
*/
void imprimir_terreno(juego_t juego){
    char dibujo[MAX_FILAS][MAX_COLUMNAS];

    rellenar_matriz(juego, dibujo);

    for (int i = 0; i < MAX_FILAS; ++i){
        for (int j = 0; j < MAX_COLUMNAS; ++j){
            printf(" %c ", dibujo[i][j]);
        }
        printf("\n");
    }
}

/*
*   La función bool recibe el vector de paredes, el tope del vector paredes, el vector de herramientas, el tope de el vector de herramientas, una coordenada y un movimiento y
*   devuelve true si el movimiento no se puede realizar
*   pre: ambos topes deben estar inicializados
*/
bool no_se_puede_usar_escalera (coordenada_t paredes[MAX_PAREDES], int tope_paredes, elemento_t herramientas[MAX_ELEMENTOS], int tope_herramientas, coordenada_t coordenada, char movimiento){
    coordenada_t coordenada_interna = coordenada;
    bool pared_adelante = solo_pared_adelante(paredes, tope_paredes, herramientas, tope_herramientas, coordenada_interna, movimiento);
    bool escalera_a_vacio_adelante = escalera_a_vacio (coordenada_interna, movimiento);

    realizar_movimiento_personaje(&coordenada_interna, movimiento);
    
    bool pared_mas_adelante = solo_pared_adelante(paredes, tope_paredes, herramientas, tope_herramientas, coordenada_interna, movimiento);
    return (pared_adelante || escalera_a_vacio_adelante || pared_mas_adelante);
}

/*
*   El procedimiento recibe el juego, un movimiento y las letras de la posicion de bloo y mac y realiza las acciones de mac
*/
void realizar_acciones_mac(juego_t* juego, char movimiento, char letra_pos_mac, char letra_pos_bloo){
    if (letra_pos_mac == VELA){
        eliminar_elemento((*juego).niveles[(*juego).nivel_actual - 1].obstaculos, &((*juego).niveles[(*juego).nivel_actual - 1].tope_obstaculos), (*juego).personaje.posicion);
    }

    if (letra_pos_mac == POZO){
        (*juego).personaje.vida --;

        resetear_coordenadas(juego);        
    } 

    if (letra_pos_mac == INTERRUPTOR){
        if ((*juego).personaje.interruptor_apretado == false){
            (*juego).personaje.interruptor_apretado = true;
        }else {
            (*juego).personaje.interruptor_apretado = false;
        }
    }  



    if (letra_pos_mac == ESCALERAS){
        
        if ((no_se_puede_usar_escalera((*juego).niveles[(*juego).nivel_actual - 1].paredes, (*juego).niveles[(*juego).nivel_actual - 1].tope_paredes, (*juego).niveles[(*juego).nivel_actual - 1].herramientas, (*juego).niveles[(*juego).nivel_actual - 1].tope_herramientas, (*juego).personaje.posicion, movimiento))){
            movimiento = movimiento_invertido(movimiento);

            mover_personaje_mac(juego, movimiento);
        }else {

            eliminar_elemento((*juego).niveles[(*juego).nivel_actual - 1].herramientas, &((*juego).niveles[(*juego).nivel_actual - 1].tope_herramientas), (*juego).personaje.posicion);
            
            realizar_movimiento_personaje(&((*juego).personaje.posicion), movimiento);

            if ((!solo_pared_adelante((*juego).niveles[(*juego).nivel_actual - 1].paredes, (*juego).niveles[(*juego).nivel_actual - 1].tope_paredes, (*juego).niveles[(*juego).nivel_actual - 1].herramientas, (*juego).niveles[(*juego).nivel_actual - 1].tope_herramientas, (*juego).personaje.posicion, movimiento)) && (!escalera_a_vacio((*juego).personaje.posicion, movimiento)) && hay_escalera((*juego).niveles[(*juego).nivel_actual - 1].herramientas, (*juego).niveles[(*juego).nivel_actual - 1].tope_herramientas, (*juego).personaje.posicion)) {
                coordenada_t pos_mac = (*juego).personaje.posicion;
                mover_personaje_mac(juego, movimiento);

                eliminar_elemento((*juego).niveles[(*juego).nivel_actual - 1].herramientas, &((*juego).niveles[(*juego).nivel_actual - 1].tope_herramientas), pos_mac);
            }

            letra_pos_mac = letra_elemento((*juego).niveles[(*juego).nivel_actual - 1].obstaculos, (*juego).niveles[(*juego).nivel_actual - 1].tope_obstaculos, (*juego).niveles[(*juego).nivel_actual - 1].herramientas, (*juego).niveles[(*juego).nivel_actual - 1].tope_herramientas, (*juego).personaje.posicion);
            realizar_acciones_mac(juego, movimiento, letra_pos_mac, letra_pos_bloo);
        }
    }

    if ((letra_pos_mac == MONEDA) && (letra_pos_bloo != MONEDA)){
        int puntos_a_sumar = rand () % 11 + 10; //Da un número de puntos entre 10 y 20
        (*juego).personaje.puntos = (*juego).personaje.puntos + puntos_a_sumar;

        eliminar_elemento((*juego).niveles[(*juego).nivel_actual - 1].herramientas, &((*juego).niveles[(*juego).nivel_actual - 1].tope_herramientas), (*juego).personaje.posicion);
    }

    if (letra_pos_mac == LLAVE){
        (*juego).personaje.tiene_llave = true;

        eliminar_elemento((*juego).niveles[(*juego).nivel_actual - 1].herramientas, &((*juego).niveles[(*juego).nivel_actual - 1].tope_herramientas), (*juego).personaje.posicion);
    }
}

/*
*   El procedimiento recibe el juego y las letras de la posicion de bloo y mac y realiza las acciones de bloo
*/
void realizar_acciones_bloo(juego_t* juego, char letra_pos_bloo, char letra_pos_mac){
    if (efecto_vela((*juego).niveles[(*juego).nivel_actual - 1].obstaculos, (*juego).niveles[(*juego).nivel_actual - 1].tope_obstaculos, (*juego).niveles[(*juego).nivel_actual - 1].herramientas, (*juego).niveles[(*juego).nivel_actual - 1].tope_herramientas, (*juego).sombra.posicion) == true){
        (*juego).sombra.esta_viva = false;
    }

    if (letra_pos_bloo == PORTAL){
        coordenada_t aux;
        aux = (*juego).personaje.posicion;
        (*juego).personaje.posicion = (*juego).sombra.posicion;
        (*juego).sombra.posicion = aux;
    }

    if ((letra_pos_bloo == MONEDA) && (letra_pos_mac != MONEDA)){
        int puntos_a_sumar = rand () % 11 + 10; //Da un número de puntos entre 10 y 20
        (*juego).personaje.puntos = (*juego).personaje.puntos + puntos_a_sumar;

        eliminar_elemento((*juego).niveles[(*juego).nivel_actual - 1].herramientas, &((*juego).niveles[(*juego).nivel_actual - 1].tope_herramientas), (*juego).sombra.posicion);
    }
}

/*
*   El procedimiento recibe un juego_t y un movimiento y realiza la acción pertinente
*/
void realizar_accion(juego_t* juego, char movimiento){
    char letra_pos_mac = letra_elemento((*juego).niveles[(*juego).nivel_actual - 1].obstaculos, (*juego).niveles[(*juego).nivel_actual - 1].tope_obstaculos, (*juego).niveles[(*juego).nivel_actual - 1].herramientas, (*juego).niveles[(*juego).nivel_actual - 1].tope_herramientas, (*juego).personaje.posicion);
    char letra_pos_bloo = letra_elemento((*juego).niveles[(*juego).nivel_actual - 1].obstaculos, (*juego).niveles[(*juego).nivel_actual - 1].tope_obstaculos, (*juego).niveles[(*juego).nivel_actual - 1].herramientas, (*juego).niveles[(*juego).nivel_actual - 1].tope_herramientas, (*juego).sombra.posicion);

    if (coordenadas_iguales((*juego).personaje.posicion, (*juego).sombra.posicion) && ((*juego).sombra.esta_viva == false)){
        (*juego).sombra.esta_viva = true;
        (*juego).personaje.puntos = (*juego).personaje.puntos - PUNTOS_REVIVIR;
    }

    if (movimiento == LETRA_VIDA){
        sumar_vida(&((*juego).personaje));
    }

    realizar_acciones_mac(juego, movimiento, letra_pos_mac, letra_pos_bloo);

    realizar_acciones_bloo(juego, letra_pos_bloo, letra_pos_mac);

    if ((letra_pos_bloo == MONEDA) && (letra_pos_mac == MONEDA)){
        int puntos_a_sumar = rand () % 11 + 10; //Da un número de puntos entre 10 y 20
        (*juego).personaje.puntos = (*juego).personaje.puntos + puntos_a_sumar;

        eliminar_elemento((*juego).niveles[(*juego).nivel_actual - 1].herramientas, &((*juego).niveles[(*juego).nivel_actual - 1].tope_herramientas), (*juego).sombra.posicion);
    }
}

/*
*   El procedimiento recibe un juego_t e imprime el status de los personajes (y el nivel actual)
*/
void imprimir_status(juego_t juego){
    printf("--------------------------------------------------------------------------\n");
    printf("Nivel %i\n", juego.nivel_actual);

    if (juego.personaje.tiene_llave == true && juego.personaje.interruptor_apretado == true){
        printf("La posición del personaje es fila %i columna %i, tiene %i vidas, %i puntos, tiene la llave y el interruptor está apretado\n", juego.personaje.posicion.fila, juego.personaje.posicion.col, juego.personaje.vida, juego.personaje.puntos);
    }else if (juego.personaje.tiene_llave == true && juego.personaje.interruptor_apretado == false){
        printf("La posición del personaje es fila %i columna %i, tiene %i vidas, %i puntos, tiene la llave y el interruptor no está apretado\n", juego.personaje.posicion.fila, juego.personaje.posicion.col, juego.personaje.vida, juego.personaje.puntos);
    }else if (juego.personaje.tiene_llave == false && juego.personaje.interruptor_apretado == true){
        printf("La posición del personaje es fila %i columna %i, tiene %i vidas, %i puntos, no tiene la llave y el interruptor está apretado\n", juego.personaje.posicion.fila, juego.personaje.posicion.col, juego.personaje.vida, juego.personaje.puntos);
    }else {
        printf("La posición del personaje es fila %i columna %i, tiene %i vidas, %i puntos, no tiene la llave y el interruptor no está apretado\n", juego.personaje.posicion.fila, juego.personaje.posicion.col, juego.personaje.vida, juego.personaje.puntos);
    }

    if (juego.sombra.esta_viva == true){
        printf("La posición de la sombra es fila %i columna %i y está viva\n", juego.sombra.posicion.fila, juego.sombra.posicion.col);
    }else {
        printf("La posición de la sombra es fila %i columna %i y no está viva\n", juego.sombra.posicion.fila, juego.sombra.posicion.col);
    }
}

/*
*   El procedimiento recibe el juego y cambia de nivel si se ganó el nivel y no estan en el nivel 3
*/
void cambiar_nivel(juego_t* juego){
    if ((estado_nivel(*juego) == NIVEL_GANADO) && ((*juego).nivel_actual < MAX_NIVELES)){
        (*juego).nivel_actual ++;
        resetear_coordenadas(juego); 
        (*juego).personaje.interruptor_apretado = false;
        (*juego).personaje.tiene_llave = false;
    }
}

/*
 * Moverá el personaje, y realizará la acción necesaria en caso de chocar con un elemento
 */
void realizar_jugada(juego_t* juego){
    char movimiento;
    pedir_movimiento(&movimiento);        //pide un movimiento válido al jugador
    mover_personaje(juego, movimiento);  //mueve al personaje
    realizar_accion(juego, movimiento); //ejecuta la acción de acuerdo al elemento con el que interactúe
    cambiar_nivel(juego);              //cambia de nivel si se cumplen las condiciones
    system("clear");                  //limpia la terminal
    imprimir_terreno(*juego);        //imprime matriz
    imprimir_status(*juego);        //imprime los datos de los personajes
}

//DEFINIR SI GANA


/*
 * El nivel se dará por terminado, si ambos personajes pasaron por la puerta teniendo la 
 * llave correspondiente.
 * Devolverá:
 * -> 0 si el estado es jugando.
 * -> 1 si el estado es ganado.
 */
int estado_nivel(juego_t juego){
    if ((esta_cerca_puerta(juego)) && (juego.personaje.tiene_llave == true) && (juego.sombra.esta_viva == true)){
        return NIVEL_GANADO;
    }else if((esta_cerca_puerta(juego)) && (juego.nivel_actual == 1) && (juego.sombra.esta_viva == true)){
        return NIVEL_GANADO;
    }else {
        return JUGANDO;
    }
}

/*
*   La función recibe un juego_t y devuelve true si el juego se ganó
*/
bool gano_juego(juego_t juego){
    return ((estado_nivel(juego) == NIVEL_GANADO) && (juego.nivel_actual == MAX_NIVELES));
}

/*
*   Recibe un juego con todas sus estructuras válidas.
*
*   El juego se dará por ganado, si terminó todos los niveles. O perdido, si el personaje queda
*   sin vida. 
*   Devolverá:
*   -> 0 si el estado es jugando. 
*   -> -1 si el estado es perdido.
*   -> 1 si el estado es ganado.
*/
int estado_juego(juego_t juego){
    if (juego.personaje.vida <= 0){
        return JUEGO_PERDIDO;
    }else if (gano_juego(juego)){
        return JUEGO_GANADO;
    }else {
        return JUGANDO;
    }
}