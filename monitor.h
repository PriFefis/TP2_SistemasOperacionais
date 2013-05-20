/*========================================================================================================================================
 TRABALHO PRATICO 2 - Sistemas Operacionais
 @GUSTAVO HENRIQUE DOS REIS
 gureis@dcc.ufmg.br

 @PRISCILLA FERNANDA VASCONCELOS
 prisvasconcelosdcc.ufmg.br

 *MONITOR.H - Define as estruturas de dados e cabecalhos de funcoes relacionadas a manipulacao de um monitor
 para sincronização de processos.
 ========================================================================================================================================*/

#ifndef MONITOR_H_
#define MONITOR_H_

#include <pthread.h>
#define NUM_THREADS 6

typedef struct {

  int id;
	int num_iteracoes;

} ThreadArg;


/* FUNCOES AUXILIARES */
char* nome(int id);
int esta_esperando(int casal);
int maior_precedencia(int id);
int menor_precedencia(int id);
int mesma_precedencia(int id);

/* FUNCOES DO MONITOR */
void esperar(int id);
void liberar(int id);
void verifica();

/* FUNCOES ESPECIFICAS DO PROBLEMA */
void esquentar_algo(int id);
void comer(int tid);

/* FUNCOES RELACIONADAS AS THREADAS */
void *inicia_raj(void *arg);
void *inicia_casais(void *arg);

#endif /* MONITOR_H_ */
