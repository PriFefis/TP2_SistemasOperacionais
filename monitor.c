/*========================================================================================================================================
 TRABALHO PRATICO 2 - Sistemas Operacionais
 @GUSTAVO HENRIQUE DOS REIS
 gureis@dcc.ufmg.br

 @PRISCILLA FERNANDA VASCONCELOS
 prisvasconcelosdcc.ufmg.br

 *MONITOR.H - Implementa funções relacionadas a manipulacao de um monitor para sincronização de processos.

 ========================================================================================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "monitor.h"

/* VARIAVEIS COMPARTILHADAS */
int forno = 0;
int espera[] = { 0, 0, 0, 0, 0, 0 };
int threads_ativas = 0;

/* MUTEXES E VARIAVEIS CONDICIONAIS */
pthread_cond_t casal[3];
pthread_mutex_t monitor = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexContador = PTHREAD_MUTEX_INITIALIZER;

/* FUNCOES AUXILIARES */
char* nome(int id) {

  char* nome;

	switch (id) {
	case 0:
		nome = "Sheldon";
		break;
	case 1:
		nome = "Howard";
		break;
	case 2:
		nome = "Leonard";
		break;
	case 3:
		nome = "Amy";
		break;
	case 4:
		nome = "Bernadette";
		break;
	case 5:
		nome = "Penny";
		break;
	default:
		break;
	}

	return nome;
}

int esta_esperando(int casal) {

	return (espera[casal] != 0 || espera[casal + 3] != 0);

}

int maior_precedencia(int id) {

	int indice;

	if (id == 0) {
		indice = 2;
	} else {
		indice = (id - 1) % 3;
	}

	return indice;

}

int menor_precedencia(int id) {
	return (id + 1) % 3;
}

int mesma_precedencia(int id) {
	return (id % 3);
}

/* FUNCOES DO MONITOR */
void esperar(int id) {
	pthread_mutex_lock(&monitor);

	printf("%s quer usar o forno.\n", nome(id));

	if (forno != 0) {
		espera[id]++;
		pthread_cond_wait(&(casal[id % 3]), &monitor);
	}

	// Marca o forno para uso
	forno = 1;

	// Avisa que não está mais esperando
	if (espera[id] == 1) {
		espera[id] = 0;
	}

	pthread_mutex_unlock(&monitor);

}

void liberar(int id) {

	pthread_mutex_lock(&monitor);

	printf("%s vai comer.\n", nome(id));
	forno = 0;

	// quantos casais diferentes tem pessoas esperando?
	// se pelo menos uma pessoa de cada casal esta esperando, temos um deadlock.
	if (!(esta_esperando(0) && esta_esperando(1) && esta_esperando(2))) {
		if (esta_esperando(maior_precedencia(id))) {
			pthread_cond_signal(&(casal[maior_precedencia(id)]));
		} else if (esta_esperando(menor_precedencia(id))) {
			pthread_cond_signal(&(casal[menor_precedencia(id)]));
		} else {
			pthread_cond_signal(&(casal[mesma_precedencia(id)]));

		}
	}

	pthread_mutex_unlock(&monitor);

}

void verifica() {

	if (forno == 0 && esta_esperando(0) && esta_esperando(1) && esta_esperando(2)) {

		srand(time(NULL ));
		int i = rand();

		pthread_cond_signal(&(casal[i % 3]));

		printf("Raj detectou um deadlock, liberando um membro do casal %d (%s e %s).\n", i % 3, nome(i % 3),
				nome((i % 3) + 3));

	}

}

/* FUNCOES ESPECIFICAS DO PROBLEMA */
void esquentar_algo(int id) {

	printf("%s começa a esquentar algo.\n", nome(id));
	sleep(1);
}

void comer(int tid) {

	srand(time(NULL ));
	int i = rand();

	// Comer (ou fazer outra atividade) gasta um tempo aleatório entre 3 e 6 segundos.
	sleep(3 + (i % 3));
}

/* FUNCOES RELACIONADAS AS THREADAS */
void *inicia_raj(void *arg) {

	while (threads_ativas > 0) {
		verifica();
		sleep(3);
	}

	pthread_exit(NULL );

}

void *inicia_casais(void *arg) {

	pthread_mutex_lock(&mutexContador);
	threads_ativas++;
	pthread_mutex_unlock(&mutexContador);

	ThreadArg *t = (ThreadArg *) arg;

	while ((*t).num_iteracoes > 0) {

		esperar((*t).id);
		esquentar_algo((*t).id);
		liberar((*t).id);
		comer((*t).id);

		(*t).num_iteracoes--;
	}

	pthread_mutex_lock(&mutexContador);
	threads_ativas--;
	pthread_mutex_unlock(&mutexContador);

	pthread_exit(NULL );

}
