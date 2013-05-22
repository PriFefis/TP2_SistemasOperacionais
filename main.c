/*========================================================================================================================================
 TRABALHO PRATICO 2 - Sistemas Operacionais
 @GUSTAVO HENRIQUE DOS REIS
 gureis@dcc.ufmg.br

 @PRISCILLA FERNANDA VASCONCELOS
 prisvasconcelosdcc.ufmg.br

 *MAIN.C - Arquivo principal do programa que implementa a sincronização de processos do problema do Forno (The Big Bang Theory).

 ========================================================================================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include "monitor.h"

// SIMULAÇÕES DO PROGRAMA

/*
 * Inicializa as threads em uma ordem aleatória.
 * Parametros:
 *  * threads[] - cada uma das threads a ser iniciada.
 *  *thread_args[] - estrutura de dados com argumentos para as threads.
 */
void execucao_aleatoria(pthread_t threads[], ThreadArg thread_args[]) {

	int i = 0;
	int personagem[] = { 0, 0, 0, 0, 0, 0 };
	while (i < 6) {
		srand(time(NULL ));
		int n = rand() % 6;

		if (personagem[n] == 0) {
			pthread_create(&(threads[i]), NULL, inicia_casais, &thread_args[i]);
			personagem[i]++;
			i++;
		}
	}

}

/*
 * Inicializa as threads em uma sequência ordenada.
 * Parametros:
 *  * threads[] - cada uma das threads a ser iniciada.
 *  * thread_args[] - estrutura de dados com argumentos para as threads.
 */
void execucao_sequencial(pthread_t threads[], ThreadArg thread_args[]) {
	int i;

	for (i = 0; i < NUM_THREADS; i++) {
		pthread_create(&(threads[i]), NULL, inicia_casais, &thread_args[i]);
	}

}

/* FUNÇÃO PRINCIPAL DO PROGRAMA */
int main(int argc, char **argv) {

	/*parametros de entrada*/
	int num_iteracoes;

	/*Leitura dos parametros de linha de comando*/

	if (argc == 2) {
		num_iteracoes = atoi(argv[1]);

	} else {
		printf("TP2: parametros invalidos, consulte o arquivo de documentacao.\n");
		exit(1);
	}

	/* CRIAÇÃO DAS THREADSA */
	pthread_t threads[NUM_THREADS];
	pthread_t thread_raj;

	int i;
	ThreadArg thread_args[6];

	for (i = 0; i < NUM_THREADS; i++) {
		thread_args[i].id = i;
		thread_args[i].num_iteracoes = num_iteracoes;
	}

	/* EXECUÇÃO SEQUENCIAL */
	execucao_sequencial(threads, thread_args);

	/* EXECUÇÃO ALEATÓRIA */
	execucao_aleatoria(threads, thread_args);

	/* CRIA A THREAD DO RAJ SEPARADAMENTE */
	sleep(1);
	pthread_create(&thread_raj, NULL, inicia_raj, NULL );

	/*	AGUARDA QUE TODAS AS THREADS TERMINEM SUA EXECUÇÃO */
	for (i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL );
	}

	pthread_join(thread_raj, NULL );

	return EXIT_SUCCESS;
}

