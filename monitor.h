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

#define NUM_THREADS 6

typedef struct {

	int id;
	int num_iteracoes;

} ThreadArg;

/* FUNCOES AUXILIARES */

/*
 * Identifica o nome do personagem a partir do identificador da Thread.
 * Parametros:
 *  * id - identificador da thread
 * Retorno:
 *  * Nome do personagem.
 */
char* nome(int id);

/*
 * Verifica se existe pelo menos uma pessoa do casal aguardando para usar o forno.
 * Parametros:
 *  * casal - indice do casal a ser verificado.
 * Retorno:
 *  * 1 se tem pelo menos uma pessoa do casal em espera.
 *  * 0 em caso contrário.
 */
int esta_esperando(int casal);

/*
 * Determina o indice do casal de maior precedência, baseado no identificador da thread/personagem.
 * Parametros:
 *  * id - identificador da thread/personagem.
 * Retorno:
 *  * Inteiro com o indice do casal de maior precedência (0, 1 ou 2).
 */
int maior_precedencia(int id);

/*
 * Determina o indice do casal de menor precedência, baseado no identificador da thread/personagem.
 * Parametros:
 *  * id - identificador da thread/personagem.
 * Retorno:
 *  * Inteiro com o indice do casal de menor precedência (0, 1 ou 2).
 */
int menor_precedencia(int id);

/*
 * Determina o indice do casal a qual pertence a thread/personagem.
 * Parametros:
 *  * id - identificador da thread/personagem.
 * Retorno:
 *  * Inteiro com o indice do casal. (0, 1 ou 2).
 */
int mesma_precedencia(int id);

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
void esperar(int id);

/*
 * Libera o forno e define a próxima thread a executar de acordo com a política de prioridades circular.
 * Para evitar inanição é sempre respeitada a seguinte ordem: thread de maior prioridade > thread de menor prioridade > thread de igual prioridade.
 * Em cada um dos casos é liberado a variável de condição primária, permitindo que a thread do casal que chegou 1º execute. Também é liberado a
 * variável de condição secundária, permitindo que o membro do casal que chegou em 2º (caso exista), passe a ocupar a variável condicional primária.
 * Parametros:
 *  * id - identificador da thread/personagem.
 */
void liberar(int id);

/*
 * Verifica se ocorreu um deadlock e em caso positivo escolhe aleatoriamente um casal para ser liberado.
 * Sempre é liberado o membro do casal que chegou primeiro (esperando na variável de condição primária do casal).
 * Parametros:
 *  * id - identificador da thread/personagem.
 * Retorno:
 *  * Inteiro com o indice do casal de maior precedência (0, 1 ou 2).
 */
void verifica();

/* FUNCOES ESPECIFICAS DO PROBLEMA */

/*
 * Utilização do forno pelo personagem. Duração de 1 segundo, conforme especificação.
 * Parametros:
 *  * id - identificador da thread/personagem.
 */
void esquentar_algo(int id);

/*
 * Espera um tempo aleatório entre 3 e 6 segundos.
 */
void comer();

/* FUNCOES RELACIONADAS AS THREADAS */

/*
 * Função para inicialização da thread do personagem Raj. Essa thread controla a ocorrência de deadlocks, chamando a função verificar a cada 5 segundos.
 * Parametros:
 *  * arg - argumentos para função de inicialização das threads.
 */
void *inicia_raj(void *arg);

/*
 * Função para inicialização da threads de cada personagem (exceto Raj). Executa a chamada as funções do monitor, usando o forno de uma forma
 * consistente.
 * Parametros:
 *  * arg - argumentos para função de inicialização das threads. Usado para recuperar o identificador de cada thread e o número de iterações.
 */
void *inicia_casais(void *arg);

#endif /* MONITOR_H_ */
