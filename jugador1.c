/*************************************************************************
	Sistemas Operativos.
	Colas de mensajes. Battleship, jugador1 (bnp1)
	Autor: Carlos Miguel López Loarca

	tipo:
		0 = ver "tipo_mensaje"
		1 = jugador 1 ha iniciado.
		2 = jugador 2 ha iniciado.
	tipo_mensaje:
		0 = esperar al otro jugador.
		1 = turno del otro jugador.
		2 = ganador jugador 1.
		3 = ganador jugador 2.
	
*************************************************************************/

#include "sys/types.h"
#include "sys/ipc.h"
#include "sys/msg.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"


#define LLAVE 34856
#define ESTADO_GANAR_P2 3
#define ESTADO_GANAR 2
#define ESTADO_JUGADOR 1
#define ESTADO_ENVIAR 0


typedef struct mensaje
{
	long tipo;
	int fila;
	int columna;
	int tipo_mensaje;
} mensaje;

char matriz1[3][3] =
	{{' ', ' ', ' '},
	 {' ', ' ', ' '},
	 {' ', ' ', ' '}};
int i, j;
int fil, col;
int cont = 0;
int bandera = 0;

void mostrarMatrices(void);

int main()
{
	//variables necesarias
	int id01;
	int tam;

	//estructuras de mensajes
	mensaje enviar;
	mensaje recibir;

	//inicializando matrices

	//muestra los tableros vacios
	mostrarMatrices();

	//pide las coordenadas de los barcos para jugador1
	printf("espere a que ambos jugadores esten listo y presione cualquier tecla");
	getchar();

	//indicar que jugador1 ha iniciado
	enviar.tipo = ESTADO_JUGADOR;
	enviar.tipo_mensaje = ESTADO_ENVIAR;

	//longitud del mensaje
	tam = sizeof(mensaje) - sizeof(long);

	//inicializando la cola de mensajes
	id01 = msgget(LLAVE, 0600 | IPC_CREAT);
	
	//envia el mensaje al jugador2
	msgsnd(id01, &enviar, tam, 0);

	//esperando a jugador2...
	printf("\nesperando a jugador 2...\n");
	msgrcv(id01, &recibir, tam, 2, 0); //recibe el mensaje de jugador2
	if (recibir.tipo_mensaje != 0)
	{ //fallo confirmacion de jugador2
		printf("\nerror al iniciar, intente de nuevo....\n");
		msgctl(id01, IPC_RMID, 0); //elimina la cola de mensajes
		return 0;
	}

	//muestra los tableros
	mostrarMatrices();

	//inicio del juego
	while (1)
	{
		printf("\nIngrese las Coordenadas:\n");
		printf("fila: ");
		scanf("%d", &fil);
		printf("columna: ");
		scanf("%d", &col);
		if (fil > 0 && fil <= 3 && col > 0 && col <= 3)
		{
			if (matriz1[fil][col] == 'X' || matriz1[fil][col] == '0')
			{
				printf("casilla ocupada!!!\n");
			}
			else
			{ //verifica si hundio un barco del jugador2
				matriz1[fil - 1][col - 1] = 'X';
				int ganar = 0;
				if (matriz1[0][0] == 'X' && matriz1[1][1] == 'X' && matriz1[2][2] == 'X' ||
					matriz1[2][0] == 'X' && matriz1[1][1] == 'X' && matriz1[0][2] == 'X' ||
					matriz1[0][1] == 'X' && matriz1[1][1] == 'X' && matriz1[2][1] == 'X' ||
					matriz1[1][0] == 'X' && matriz1[1][1] == 'X' && matriz1[1][2] == 'X' ||
					matriz1[0][0] == 'X' && matriz1[0][1] == 'X' && matriz1[0][2] == 'X' ||
					matriz1[0][0] == 'X' && matriz1[0][1] == 'X' && matriz1[0][2] == 'X' ||
					matriz1[2][0] == 'X' && matriz1[2][1] == 'X' && matriz1[2][2] == 'X' ||
					matriz1[0][2] == 'X' && matriz1[1][2] == 'X' && matriz1[2][2] == 'X')
				{
					ganar = 1;
				}

				mostrarMatrices();

				//verificando si gano el jugador1
				if (ganar == 1)
				{
					printf("\n¡¡ganador jugador 1!!\n");
					enviar.fila = fil-1;
					enviar.columna = col-1;
					enviar.tipo_mensaje = ESTADO_GANAR;
					msgsnd(id01, &enviar, tam, 0);
					return 0;
				}

				//turno de jugador 1
				enviar.tipo = 1;
				enviar.fila = fil-1;
				enviar.columna = col-1;
				enviar.tipo_mensaje = 1;
				msgsnd(id01, &enviar, tam, 0);

				//esperando al jugador 2
				printf("\nesperando a jugador 2...\n");
				msgrcv(id01, &recibir, tam, 0, 0);
				matriz1[recibir.fila][recibir.columna] = '0';
				//verifica si hundio un barco del jugador1
				
				mostrarMatrices();

				//verificando si gano jugador 2
				if (recibir.tipo_mensaje == ESTADO_GANAR_P2)
				{
					printf("\n¡¡ganador jugador 2!!\n");
					msgctl(id01, IPC_RMID, 0);
					return 0;
				}
			} //fin del else
		}
		else
		{
			printf("¡la casilla (%d,%d) no existe!\n", fil, col);
		}
	} //fin del while
}

void mostrarMatrices(void)
{
	system("clear");
	printf("JUGADOR 1\n");
	for (size_t i = 0; i < 3; i++)
	{
		for (size_t j = 0; j < 3; j++)
		{
			printf(matriz1[i, j]);
			if (j != 2)
			{
				printf(" | ");
			}
		}
		printf("\n");
		if (i != 2)
		{
			printf("_________\n");
		}
	}
}
