#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "utiles.h"
#include "la_sombra_de_mac.h"

#define MAX_POCOS_MOVIMIENTOS 300
#define MIN_MUCHOS_MOVIMIENTOS 400
#define MAX_MOVIMIENTOS_LOGRO 200
#define MIN_PUNTOS_LOGRO 700

#define JUEGO_PERDIDO -1
#define JUEGO_GANADO 1
#define JUGANDO 0

#define MAX_NOMBRE 100
#define MAX_GANO 3
#define SI_GANO 'Si'
#define NO_GANO 'No'
#define MAX_LINEA 150
#define ERROR -1
#define FORMATO_LECTURA "%[^;];%i;%i;%i;%[^\n]\n"
#define FORMATO_ESCRITURA "%s;%i;%i;%i;%s\n"
#define N_LEIDOS 5
#define MAX_PARTIDAS 100
#define LEIDOS_CONFIG 3

#define VELAS_LVL1 5
#define POZOS_LVL1 15
#define INTERRUPTORES_LVL1 1
#define PORTALES_LVL1 0
#define VELAS_LVL2 10
#define POZOS_LVL2 20
#define INTERRUPTORES_LVL2 2
#define PORTALES_LVL2 2
#define VELAS_LVL3 12
#define POZOS_LVL3 30
#define INTERRUPTORES_LVL3 4
#define PORTALES_LVL3 4

#define ESCALERAS_LVL1 10
#define MONEDAS_LVL1 10
#define LLAVES_LVL1 0
#define ESCALERAS_LVL2Y3 15
#define MONEDAS_LVL2Y3 15
#define LLAVES_LVL2Y3 1

typedef struct partida {
char jugador[MAX_NOMBRE];
int nivel_llegado;
int puntos;
int vidas_restantes;
bool gano;
} partida_t;

/*
*	El procedimiento recibe un numero_t y el número de cada elemento de la partida y le asigna a cada posicion del struct el valor de cada elemento
*/
void rellenar_numero_elementos(numero_t* numero_elementos, int n_pozos, int n_velas, int n_interruptores, int n_portales, int n_monedas, int n_escaleras, int n_llaves){
	(*numero_elementos).n_pozos = n_pozos;
	(*numero_elementos).n_velas = n_velas;
	(*numero_elementos).n_interruptores = n_interruptores;
	(*numero_elementos).n_portales = n_portales;
	(*numero_elementos).n_monedas = n_monedas;
	(*numero_elementos).n_escaleras = n_escaleras;
	(*numero_elementos).n_llaves = n_llaves;
}

/*
*	El procedimiento recibe el vector_numero_elementos y lo inicializa según el nivel que sea (usa los valores predeterminados para cada número de elementos)
*/
void inicializar_numero_elementos(numero_t vector_numero_elementos[MAX_NIVELES]){
	rellenar_numero_elementos(&vector_numero_elementos[0], POZOS_LVL1, VELAS_LVL1, INTERRUPTORES_LVL1, PORTALES_LVL1, MONEDAS_LVL1, ESCALERAS_LVL1, LLAVES_LVL1);
	rellenar_numero_elementos(&vector_numero_elementos[1], POZOS_LVL2, VELAS_LVL2, INTERRUPTORES_LVL2, PORTALES_LVL2, MONEDAS_LVL2Y3, ESCALERAS_LVL2Y3, LLAVES_LVL2Y3);
	rellenar_numero_elementos(&vector_numero_elementos[2], POZOS_LVL3, VELAS_LVL3, INTERRUPTORES_LVL3, PORTALES_LVL3, MONEDAS_LVL2Y3, ESCALERAS_LVL2Y3, LLAVES_LVL2Y3);
}



void imprimir_final(juego_t juego, int numero_movimientos){
    if (estado_juego(juego) == -1){
        if (juego.sombra.esta_viva == true){
            printf("Perdiste. Mac no llegó a salvo a casa y Bloo decidió no volver a jugar este juego nunca más :(. Mirá el lado bueno, vos podés volver a jugarlo, todavía no cobramos por partida jugada, así que a cambiar el destino de ambos!\n");
        }else {
            printf("Perdiste, ni Mac ni Bloo volvieron a casa, en la Mansion Foster los siguen esperando :(. Mirá el lado bueno, vos podés volver a jugarlo, todavía no cobramos por partida jugada, así que a cambiar el destino de ambos!\n");
        }
    }

    if (estado_juego(juego) == 1){
        printf(" \
     ######      ###    ##    ##    ###     ######  ######## ########          \n \
    ##    ##    ## ##   ###   ##   ## ##   ##    ##    ##    ##                \n \
    ##         ##   ##  ####  ##  ##   ##  ##          ##    ##                \n \
    ##   #### ##     ## ## ## ## ##     ##  ######     ##    ######            \n \
    ##    ##  ######### ##  #### #########       ##    ##    ##                \n \
    ##    ##  ##     ## ##   ### ##     ## ##    ##    ##    ##       ####     \n \
     ######   ##     ## ##    ## ##     ##  ######     ##    ######## ####     \n \
                                                                       ##      \n \
                                                                      ##       \n \
                                                                               \n \
    ######## ######## ##       ####  ######  #### ########     ###    ########  ########  ######  \n \
    ##       ##       ##        ##  ##    ##  ##  ##     ##   ## ##   ##     ## ##       ##    ## \n \
    ##       ##       ##        ##  ##        ##  ##     ##  ##   ##  ##     ## ##       ##       \n \
    ######   ######   ##        ##  ##        ##  ##     ## ##     ## ##     ## ######    ######  \n \
    ##       ##       ##        ##  ##        ##  ##     ## ######### ##     ## ##             ## \n \
    ##       ##       ##        ##  ##    ##  ##  ##     ## ##     ## ##     ## ##       ##    ## \n \
    ##       ######## ######## ####  ######  #### ########  ##     ## ########  ########  ######  \n \
                                                                                                  \n");

        if (numero_movimientos < MAX_POCOS_MOVIMIENTOS){
            printf("Mac y Bloo llegaron a casa sanos y salvos, y a Bloo le pareció que el juego había sido entretenido a pesar de morir un par de veces, por lo que volverían a jugarlo\n");
        }else if ((numero_movimientos >= MAX_POCOS_MOVIMIENTOS) && (numero_movimientos < MIN_MUCHOS_MOVIMIENTOS)){
            printf("Mac y Bloo llegaron a casa sanos y salvos, pero Bloo decidió que no era una buena idea volver a jugar este juego porque murió unas cuantas veces\n");
        }else {
            printf("Mac y Bloo llegaron a casa sanos y salvos, y a Bloo le pareció un juego aburrido, y sumado a que murió varias veces decidió que no era una buena idea volver a jugar este juego\n");
        }

        int num_logros = 0;

        printf("Logros ganados en esta partida:\n");

        if (numero_movimientos <= MAX_MOVIMIENTOS_LOGRO){
            printf("Rápido y furioso: Logrado tras llegar a la meta en %i movimientos o menos\n", MAX_MOVIMIENTOS_LOGRO);
            num_logros ++;
        }

        if (juego.personaje.puntos < 0){
            printf("En deuda: Logrado tras llegar a la meta con un número de puntos negativo (de dudoso mérito, no suma como logro realmente)\n");
            num_logros ++;
        }

        if (juego.personaje.puntos > MIN_PUNTOS_LOGRO){
            printf("Millonario: Logrado tras llegar a la meta agarrando todas las monedas y con un poco de suerte ;)\n");
            num_logros ++;
        }

        if (num_logros == 0){
            printf("Ninguno :(. Podés volver a jugar e intentar ganar alguno, probá ganando en menos de %i intentos.\n", MAX_MOVIMIENTOS_LOGRO);
        }
    }
}

/*	
*	Recibe una partida_t y un string y le pide los datos de la partida a agregar al jugador
*	Pre: La primer letra debe estar en mayúscula (o en minúscula si así están los nombres en el listado). El nombre no puede 
*	tener tíldes ni caracteres especiales, en ese caso, van a contar como más grande que la z (ej ö, ï, é, etc.)
*/
void pedir_datos(partida_t* partida_a_agregar, char gano_agregar[MAX_GANO]){
	printf("Buenas, para agregar una partida te pido ingreses los datos\n");
	printf("Nombre del jugador\n");
	scanf("%s", (*partida_a_agregar).jugador);
	printf("Nivel al que llegó\n");
	scanf("%i", &((*partida_a_agregar).nivel_llegado));
	printf("Puntos obtenidos\n");
	scanf("%i", &((*partida_a_agregar).puntos));
	printf("Número de vidas que le quedaban\n");
	scanf("%i", &((*partida_a_agregar).vidas_restantes));
	printf("Ganó? (Si/No)\n");
	scanf("%s", gano_agregar);
	printf("\n");
}

/*	
*	Recibe un archivo arch y un archivo auxiliar, le pide los datos de la partida a agregar al jugador y agrega esa partida respetando el orden preestablecido
*/
void agregar_partida(FILE* arch, FILE* auxiliar){
	partida_t partida_a_agregar;
	char gano_agregar[MAX_GANO];

	pedir_datos(&partida_a_agregar, gano_agregar);
	
	partida_t partida;
	char gano[MAX_GANO];
	int leidos = fscanf(arch, FORMATO_LECTURA, partida.jugador, &(partida.nivel_llegado), &(partida.puntos), &(partida.vidas_restantes), gano);

	bool agregado = false;
	while(leidos == N_LEIDOS){
		if((strcmp(partida_a_agregar.jugador, partida.jugador) < 0) && (agregado == false)){
			fprintf(auxiliar, FORMATO_ESCRITURA, partida_a_agregar.jugador, partida_a_agregar.nivel_llegado, partida_a_agregar.puntos, partida_a_agregar.vidas_restantes, gano_agregar);
			agregado = true;
		}
		fprintf(auxiliar, FORMATO_ESCRITURA, partida.jugador, partida.nivel_llegado, partida.puntos, partida.vidas_restantes, gano);
		leidos = fscanf(arch, FORMATO_LECTURA, partida.jugador, &(partida.nivel_llegado), &(partida.puntos), &(partida.vidas_restantes), gano);
	}

	if (agregado == false){
		fprintf(auxiliar, FORMATO_ESCRITURA, partida_a_agregar.jugador, partida_a_agregar.nivel_llegado, partida_a_agregar.puntos, partida_a_agregar.vidas_restantes, gano_agregar);
		agregado = true;
	}
}

/*	
*	Recibe un archivo arch, un archivo auxiliar y el nombre de la partida a eliminar y elimina la partida cuyo nombre de jugador coincida con el recibido
*/
void eliminar_partida(FILE* arch, FILE* auxiliar, char nombre_a_eliminar[MAX_NOMBRE]){
	bool eliminado = false;
	partida_t partida;
	char gano[MAX_GANO];
	int leidos = fscanf(arch, FORMATO_LECTURA, partida.jugador, &(partida.nivel_llegado), &(partida.puntos), &(partida.vidas_restantes), gano);

	while(leidos == N_LEIDOS){
		if((strcmp(nombre_a_eliminar, partida.jugador) != 0) || (strcmp(nombre_a_eliminar, partida.jugador) == 0 && eliminado == true)){
			fprintf(auxiliar, FORMATO_ESCRITURA, partida.jugador, partida.nivel_llegado, partida.puntos, partida.vidas_restantes, gano);	
  	    }else {
  	    	eliminado = true;
  	    }
  	    leidos = fscanf(arch, FORMATO_LECTURA, partida.jugador, &(partida.nivel_llegado), &(partida.puntos), &(partida.vidas_restantes), gano);
	}	
}

/*	
*	Recibe un archivo arch y un archivo auxiliar y ordena el archivo recibido
*/
void ordenar_partidas(FILE* arch, FILE* auxiliar){
	int tope = 0;
	partida_t vector_partidas[MAX_PARTIDAS];
	char gano[MAX_GANO];
	int leidos = fscanf(arch, FORMATO_LECTURA, vector_partidas[tope].jugador, &(vector_partidas[tope].nivel_llegado), &(vector_partidas[tope].puntos), &(vector_partidas[tope].vidas_restantes), gano);

	while(leidos == N_LEIDOS){
		vector_partidas[tope].gano = strcmp(gano, "No");
		tope ++;
		leidos = fscanf(arch, "%[^;];%i;%i;%i;%[^\n]\n", vector_partidas[tope].jugador, &(vector_partidas[tope].nivel_llegado), &(vector_partidas[tope].puntos), &(vector_partidas[tope].vidas_restantes), gano);
	}
	
	partida_t aux;
	for (int i = 1; i < tope; i++){
		for (int j = 0; j < tope - 1; j++){
			if (strcmp(vector_partidas[j].jugador, vector_partidas[j+1].jugador) > 0){
				aux = vector_partidas[j];
				vector_partidas[j] = vector_partidas[j+1];
				vector_partidas[j+1] = aux;
			}
		}
	}

	int i = 0;
	while(i < tope){
		fprintf(auxiliar, "%s;%i;%i;%i;", vector_partidas[i].jugador, vector_partidas[i].nivel_llegado, vector_partidas[i].puntos, vector_partidas[i].vidas_restantes);
		
		if (vector_partidas[i].gano == true){
			strcpy(gano, "Si");
		}else {
			strcpy(gano, "No");
		}

		fprintf(auxiliar, "%s\n", gano);
		i++;
	}
}

/*
*	Recibe el archivo de configuración y el vector con las configuraciones predeterminadas y las cambia de acuerdo a lo escrito en el archivo
*/
void configurar_juego(FILE* config, numero_t vector_numero_elementos[MAX_NIVELES]){
	int nivel_elemento;
	char etiqueta[MAX_NOMBRE];
	int cantidad;
	int leidos = fscanf(config, "N%i_%[^=]=%i\n", &nivel_elemento, etiqueta, &cantidad);

	while(leidos == LEIDOS_CONFIG){
		if (strcmp(etiqueta, "POZOS") == 0){
			vector_numero_elementos[nivel_elemento - 1].n_pozos = cantidad;
		}
		if (strcmp(etiqueta, "VELAS") == 0){
			vector_numero_elementos[nivel_elemento - 1].n_velas = cantidad;
		}
		if (strcmp(etiqueta, "INTERRUPTORES") == 0){
			vector_numero_elementos[nivel_elemento - 1].n_interruptores = cantidad;
		}
		if (strcmp(etiqueta, "PORTALES") == 0){
			vector_numero_elementos[nivel_elemento - 1].n_portales = cantidad;
		}
		if (strcmp(etiqueta, "MONEDAS") == 0){
			vector_numero_elementos[nivel_elemento - 1].n_monedas = cantidad;
		}
		if (strcmp(etiqueta, "ESCALERAS") == 0){
			vector_numero_elementos[nivel_elemento - 1].n_escaleras = cantidad;
		}
		if (strcmp(etiqueta, "LLAVES") == 0){
			vector_numero_elementos[nivel_elemento - 1].n_llaves = cantidad;
		}

		leidos = fscanf(config, "N%i_%[^=]=%i\n", &nivel_elemento, etiqueta, &cantidad);
	}
}

int main(int argc , char *argv []){

	bool partida_agregada = false;

	if (argc > 2){
		FILE* arch = fopen(argv[2],"r");
		if (!arch){
			perror("No se pudo abrir el archivo :( \n");
			return ERROR;
		}

		FILE* auxiliar = fopen("auxiliar.csv", "w");
		if (!auxiliar){
			fclose(arch);
			perror("No se pudo crear el auxiliar :( \n");
			return ERROR;
		}
	
		if (strcmp(argv[1], "agregar_partida") == 0){
			agregar_partida(arch, auxiliar);
			partida_agregada = true;
		}

		if (strcmp(argv[1], "eliminar_partida") == 0){
			eliminar_partida(arch, auxiliar, argv[3]);
		}

		if (strcmp(argv[1], "ordenar_partidas") == 0){
			ordenar_partidas(arch, auxiliar);
		}

		
		fclose(arch);
		fclose(auxiliar);
		rename("auxiliar.csv", argv[2]);
	}


	numero_t vector_numero_elementos[MAX_NIVELES];
	inicializar_numero_elementos(vector_numero_elementos);

	if (argc == 2){
		if (strcmp(argv[0], "config_juego")){
			FILE* config = fopen("config.txt","r");
			if (!config){
				perror("No se pudo abrir el archivo de configuraciones :( \n");
				return ERROR;
			}

			configurar_juego(config, vector_numero_elementos);

			fclose(config);
		}
	}

	
	if (partida_agregada == false){
		char quiere_jugar;
		printf("quiere_jugar? (S/N)\n");
		scanf("%c", &quiere_jugar);

		if (quiere_jugar == 'S'){
		    srand ((unsigned)time(NULL));

		    juego_t juego;

		    inicializar_juego(&juego, vector_numero_elementos);

		    printf("                                                                                                             \n \
			Contexto: Mac y a Bloo a volver de la mansión luego de haber pasado una tarde explorando la ciudad.                  \n \
			Deberás guiarlos en su camino ingresando una dirección en la que caminar, para poder encontrar la llave que          \n \
			abre la puerta de la mansión. Bloo, para hacer la vuelta de forma divertida, va a caminar en espejo con respecto     \n \
			a Mac. Es decir, si Mac se dirige en algun sentido horizontal (izquierda o derecha), Bloo irá en el camino           \n \
			contrario. Presione F para empezar\n");

		    printf("\n");

		    int numero_movimientos = 0;
		    while (estado_juego(juego) == JUGANDO){
		        realizar_jugada(&juego);
		        if ((estado_nivel(juego) == JUEGO_GANADO) && (juego.nivel_actual < MAX_NIVELES)){
		            juego.nivel_actual ++;
		        }
		        printf("%i movimientos\n", numero_movimientos);
		        numero_movimientos ++;
		    }

		    imprimir_final(juego, numero_movimientos);
		}
	}
	

	return 0;
}