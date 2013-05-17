#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_THREADS 2

pthread_mutex_t monitor = PTHREAD_MUTEX_INITIALIZER;

int forno = 0; 

int espera[] = { 0, 0, 0, 0, 0, 0 };

pthread_cond_t casal[3];

//id
//0,3
//1,4
//2,5
// tid = thread id - identificador de cada thread/pessoa

void esperar(int tid) {
  pthread_mutex_lock(&monitor);

	if (forno != 0) {
		espera[tid]++;
		pthread_cond_wait(&(casal[tid % 3]), &monitor);
	}

	// Marca o forno para uso
	forno = 1;

	// Avisa que não está mais esperando
	if (espera[tid] == 1) {
		espera[tid] = 0;
	}

	pthread_mutex_unlock(&monitor);

}

void usar(int tid) {

	sleep(2);

}

void comer(int tid) {

	sleep(1);
}

int estaEsperando(int casal) {

	return (espera[casal] != 0 || espera[casal + 3] != 0);

}

void liberar(int tid) {

	pthread_mutex_lock(&monitor);

	forno = 0;

	// quantos casais diferentes tem pessoas esperando?
	// se pelo menos uma pessoa de cada casal esta esperando, temos um deadlock.
	if (!(estaEsperando(0) && estaEsperando(1) && estaEsperando(2))) {
		if (estaEsperando((tid - 1) % 3)) {
			pthread_cond_signal(&(casal[tid - 1 % 3]));
		} else {
			pthread_cond_signal(&(casal[tid + 1 % 3]));
		}
	}

	pthread_mutex_unlock(&monitor);

}

void *inicia_thread(void *arg) {

	int *n = (int *) arg;

	printf("T%d - Esperar\n", *n);
	esperar(*n);
	printf("T%d - Usar\n", *n);
	usar(*n);
	printf("T%d - Liberar\n", *n);
	liberar(*n);
	printf("T%d - Comer\n", *n);
	comer(*n);

	pthread_exit(NULL );

}

int main(void) {

	pthread_t threads[NUM_THREADS];

	int i;
	int id_thread[] = { 0, 1, 2, 3, 4, 5, 6 };

	for (i = 0; i < NUM_THREADS; i++) {
		pthread_create(&(threads[i]), NULL, inicia_thread, &id_thread[i]);
	}

	for (i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL );
	}

	return EXIT_SUCCESS;
}

