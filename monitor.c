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
#include "monitor.h"

/* VARIAVEIS COMPARTILHADAS */
int forno = 0;
int espera[] = { 0, 0, 0, 0, 0, 0 };
int threads_ativas = 0;

/* MUTEXES E VARIAVEIS CONDICIONAIS */
pthread_cond_t casal[6];
pthread_mutex_t monitor = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexContador = PTHREAD_MUTEX_INITIALIZER;

/* FUNCOES AUXILIARES */

/*
 * Identifica o nome do personagem a partir do identificador da Thread.
 * Parametros:
 *  * id - identificador da thread
 * Retorno:
 *  * Nome do personagem.
 */
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

/*
 * Verifica se existe pelo menos uma pessoa do casal aguardando para usar o forno.
 * Parametros:
 *  * casal - indice do casal a ser verificado.
 * Retorno:
 *  * 1 se tem pelo menos uma pessoa do casal em espera.
 *  * 0 em caso contrário.
 */
int esta_esperando(int casal) {

	return (espera[casal] != 0 || espera[casal + 3] != 0);

}

/*
 * Determina o indice do casal de maior precedência, baseado no identificador da thread/personagem.
 * Parametros:
 *  * id - identificador da thread/personagem.
 * Retorno:
 *  * Inteiro com o indice do casal de maior precedência (0, 1 ou 2).
 */
int maior_precedencia(int id) {

	int indice;

	if (id == 0) {
		indice = 2;
	} else {
		indice = (id - 1) % 3;
	}

	return indice;

}

/*
 * Determina o indice do casal de menor precedência, baseado no identificador da thread/personagem.
 * Parametros:
 *  * id - identificador da thread/personagem.
 * Retorno:
 *  * Inteiro com o indice do casal de menor precedência (0, 1 ou 2).
 */
int menor_precedencia(int id) {
	return (id + 1) % 3;
}

/*
 * Determina o indice do casal a qual pertence a thread/personagem.
 * Parametros:
 *  * id - identificador da thread/personagem.
 * Retorno:
 *  * Inteiro com o indice do casal. (0, 1 ou 2).
 */
int mesma_precedencia(int id) {
	return (id % 3);
}

/* FUNCOES DO MONITOR */

/*
 * Verifica se o forno está disponível ou não para uso. Caso o forno esteja disponível esta função marca o mesmo para uso pela thread/personagem
 * identificada pelo parâmetro #id. Caso o forno esteja em uso por outra thread, a thread atual registra a sua espera em uma variável de condição
 * do casal. Para garantir que a ordem de acesso seja a ordem de chegada entre pessoas de um mesmo casal, foi utilizada uma segunda variável de condição (por casal).
 * Quando um personagem deseja usar o forno e seu namorado(a) já está esperando ela deve aguardar na segunda variável de condição.
 * Parametros:
 *  * id - identificador da thread/personagem.
 * Retorno:
 *  * Inteiro com o indice do casal de maior precedência (0, 1 ou 2).
 */
void esperar(int id) {
	pthread_mutex_lock(&monitor);

	printf("%s quer usar o forno.\n", nome(id));

	if (forno != 0) {
		if (!esta_esperando(id % 3)) {
			espera[id]++;

			// Aguarda na variável de condição primária do casal
			pthread_cond_wait(&(casal[id % 3]), &monitor);

		} else {

			espera[id]++;
			// Aguarda na variável de condição secundária do casal
			pthread_cond_wait(&(casal[(id % 3) + 3]), &monitor);

			// Aguarda na variável de condição primária do casal
			pthread_cond_wait(&(casal[id % 3]), &monitor);
		}
	}

	// Marca o forno para uso
	forno = 1;

	// Avisa que não está mais esperando
	if (espera[id] == 1) {
		espera[id] = 0;
	}

	pthread_mutex_unlock(&monitor);

}

/*
 * Libera o forno e define a próxima thread a executar de acordo com a política de prioridades circular.
 * Para evitar inanição é sempre respeitada a seguinte ordem: thread de maior prioridade > thread de menor prioridade > thread de igual prioridade.
 * Em cada um dos casos é liberado a variável de condição primária, permitindo que a thread do casal que chegou 1º execute. Também é liberado a
 * variável de condição secundária, permitindo que o membro do casal que chegou em 2º (caso exista), passe a ocupar a variável condicional primária.
 * Parametros:
 *  * id - identificador da thread/personagem.
 */
void liberar(int id) {

	pthread_mutex_lock(&monitor);

	printf("%s vai comer.\n", nome(id));
	forno = 0;

	// quantos casais diferentes tem pessoas esperando?
	// se pelo menos uma pessoa de cada casal esta esperando, temos um deadlock.
	if (!(esta_esperando(0) && esta_esperando(1) && esta_esperando(2))) {
		if (esta_esperando(maior_precedencia(id))) {
			pthread_cond_signal(&(casal[maior_precedencia(id)]));
			pthread_cond_signal(&(casal[maior_precedencia(id) + 3]));
		} else if (esta_esperando(menor_precedencia(id))) {
			pthread_cond_signal(&(casal[menor_precedencia(id)]));
			pthread_cond_signal(&(casal[menor_precedencia(id) + 3]));
		} else {
			pthread_cond_signal(&(casal[mesma_precedencia(id)]));
			pthread_cond_signal(&(casal[mesma_precedencia(id) + 3]));
		}
	} else {
		// Caso ocorra uma deadlock não libera ninguém, mas atualiza a posição de espera do(a) namorado(a),
		// liberando a variável condicional secundária do casal
		pthread_cond_signal(&(casal[mesma_precedencia(id) + 3]));
	}

	pthread_mutex_unlock(&monitor);

}

/*
 * Verifica se ocorreu um deadlock e em caso positivo escolhe aleatoriamente um casal para ser liberado.
 * Sempre é liberado o membro do casal que chegou primeiro (esperando na variável de condição primária do casal).
 * Parametros:
 *  * id - identificador da thread/personagem.
 * Retorno:
 *  * Inteiro com o indice do casal de maior precedência (0, 1 ou 2).
 */
void verifica() {

	if (forno == 0 && esta_esperando(0) && esta_esperando(1) && esta_esperando(2)) {

		srand(time(NULL ));
		int i = rand();

		pthread_cond_signal(&(casal[i % 3]));
		pthread_cond_signal(&(casal[(i % 3) + 3]));

		printf("Raj detectou um deadlock, liberando um membro do casal [%s e %s].\n", nome(i % 3), nome((i % 3) + 3));

	}

}

/* FUNCOES ESPECIFICAS DO PROBLEMA */

/*
 * Utilização do forno pelo personagem. Duração de 1 segundo, conforme especificação.
 * Parametros:
 *  * id - identificador da thread/personagem.
 */
void esquentar_algo(int id) {

	printf("%s começa a esquentar algo.\n", nome(id));
	sleep(1);
}

/*
 * Espera um tempo aleatório entre 3 e 6 segundos.
*/
void comer() {

	srand(time(NULL ));
	int i = rand();

	// Comer (ou fazer outra atividade) gasta um tempo aleatório entre 3 e 6 segundos.
	sleep(3 + (i % 3));
}

/* FUNCOES RELACIONADAS AS THREADAS */

/*
 * Função para inicialização da thread do personagem Raj. Essa thread controla a ocorrência de deadlocks, chamando a função verificar a cada 5 segundos.
 * Parametros:
 *  * arg - argumentos para função de inicialização das threads.
*/
void *inicia_raj(void *arg) {

	while (threads_ativas > 0) {
		verifica();
		sleep(5);
	}

	pthread_exit(NULL );

}


/*
 * Função para inicialização da threads de cada personagem (exceto Raj). Executa a chamada as funções do monitor, usando o forno de uma forma
 * consistente.
 * Parametros:
 *  * arg - argumentos para função de inicialização das threads. Usado para recuperar o identificador de cada thread e o número de iterações.
*/
void *inicia_casais(void *arg) {

	pthread_mutex_lock(&mutexContador);
	threads_ativas++;
	pthread_mutex_unlock(&mutexContador);

	ThreadArg *t = (ThreadArg *) arg;

	while ((*t).num_iteracoes > 0) {

		esperar((*t).id);
		esquentar_algo((*t).id);
		liberar((*t).id);
		comer();

		(*t).num_iteracoes--;
	}

	pthread_mutex_lock(&mutexContador);
	threads_ativas--;
	pthread_mutex_unlock(&mutexContador);

	pthread_exit(NULL );

}
