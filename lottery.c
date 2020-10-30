/*
*  lottery.c - Implementacao do algoritmo Lottery Scheduling e sua API
*
*  Autores: SUPER_PROGRAMADORES_C
*  Projeto: Trabalho Pratico I - Sistemas Operacionais
*  Organizacao: Universidade Federal de Juiz de Fora
*  Departamento: Dep. Ciencia da Computacao
*
*
*/

#include "lottery.h"
#include <stdio.h>
#include <string.h>

//Nome unico do algoritmo. Deve ter 4 caracteres.
const char lottName[]="LOTT";

//=====Funcoes Auxiliares=====






//=====Funcoes da API=====

//Funcao chamada pela inicializacao do S.O. para a incializacao do escalonador
//conforme o algoritmo Lottery Scheduling
//Deve envolver a inicializacao de possiveis parametros gerais
//Deve envolver o registro do algoritmo junto ao escalonador
void lottInitSchedInfo() {
	SchedInfo* si = malloc(sizeof(SchedInfo));
	strcpy(si->name, lottName);
	si->initParamsFn = lottInitSchedParams;
	si->scheduleFn = lottSchedule;
	si->releaseParamsFn = lottReleaseParams;
	schedRegisterScheduler(si);
}

//Inicializa os parametros de escalonamento de um processo p, chamada 
//normalmente quando o processo e' associado ao slot de Lottery
void lottInitSchedParams(Process *p, void *params) {
	processSetSchedParams(p, params);
}

//Retorna o proximo processo a obter a CPU, conforme o algortimo Lottery 
Process* lottSchedule(Process *plist) {
	//...
	int sum;
	int total_tickets;
	int random;
	Process* p;
	LotterySchedParams* params;
	total_tickets = 0;
	p = plist;
	while(p != NULL) {
		params = processGetSchedParams(p);
		total_tickets += params->num_tickets;
		p = processGetNext(p);
	}

	random = rand()%total_tickets;

	sum = 0;
	p = plist;
	while(p != NULL) {
		params = processGetSchedParams(p);
		sum += params->num_tickets;

		if(random <= sum) {
			if(processGetStatus(p) != 2)
				return p;
			else {
				p = processGetNext(p);
				if(p == NULL)
					p = plist;
				while(p != NULL) {
					if(processGetStatus(p) != 2)
						return p;
					p = processGetNext(p);
				}
			}	
		}

		p = processGetNext(p);
	}

	return NULL;
}

//Libera os parametros de escalonamento de um processo p, chamada 
//normalmente quando o processo e' desassociado do slot de Lottery
//Retorna o numero do slot ao qual o processo estava associado
int lottReleaseParams(Process *p) {
	free(processGetSchedParams(p));
	return 0;
}

//Transfere certo numero de tickets do processo src para o processo dst.
//Retorna o numero de tickets efetivamente transfeirdos (pode ser menos)
int lottTransferTickets(Process *src, Process *dst, int tickets) {
	LotterySchedParams* params_src = processGetSchedParams(src);
	LotterySchedParams* params_dst = processGetSchedParams(dst);

	if(params_src->num_tickets <= tickets) {
		tickets = params_src->num_tickets;
		params_dst->num_tickets += params_src->num_tickets;
		params_src->num_tickets = 0; //PROCESSO NUNCA VAI SER SELECIONADO, ACHO MELHOR REVER ISSO DEPOIS
		return tickets;
	}

	params_src->num_tickets -= tickets;
	params_dst->num_tickets += tickets;

	return tickets;
}
