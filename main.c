#include <stdlib.h>
#include "monitor.h"

// SIMULAÇÕES DO PROGRAMA
void execucao_aleatoria() {

	//	// Sheldon
	//	pthread_create(&(threads[0]), NULL, inicia_thread, &id_thread[0]);
	//	sleep(1);
	//
	//	// Amy
	//	pthread_create(&(threads[3]), NULL, inicia_thread, &id_thread[3]);
	////	sleep(1);
	//
	//	// Howard
	//	pthread_create(&(threads[1]), NULL, inicia_thread, &id_thread[1]);
	//	//sleep(1);
	//
	//	// Bernadette
	//	pthread_create(&(threads[4]), NULL, inicia_thread, &id_thread[4]);
	//	sleep(1);
	//
	////	// Leonard
	////	pthread_create(&(threads[2]), NULL, inicia_thread, &id_thread[2]);
	////	sleep(1);
	////
	////	// Penny
	////	pthread_create(&(threads[5]), NULL, inicia_thread, &id_thread[5]);
	////	sleep(1);

}

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

