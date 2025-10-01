//
// Created by HP on 29/09/2025.
//
#include "fifo.h"

#include <stdio.h>
#include <stdlib.h>

#include "msg.h"
#include <unistd.h>

#include "queue.h"

queue_elem_t *encontraTarefaTempoMIN(queue_t* q){
    queue_elem_t *tarefaAtual= q->head;
    queue_elem_t *tarefaComTempMin= tarefaAtual; // guardei como min o head so para começar a comparar

    if (!q || !q->head) {
        return NULL;
    }

    while (tarefaAtual!= NULL) {
        if (tarefaAtual->pcb->time_ms<tarefaComTempMin->pcb->time_ms ) {
            tarefaComTempMin= tarefaAtual;
        }

        tarefaAtual= tarefaAtual->next;
    }

    return tarefaComTempMin;
}

void sjf_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task) {
    if (*cpu_task) {  // Se o CPU está ocupado com uma tarefa

        (*cpu_task)->ellapsed_time_ms += TICKS_MS;      // Incrementa o tempo que a tarefa já esteve a correr.
        if ((*cpu_task)->ellapsed_time_ms >= (*cpu_task)->time_ms) {//- Se o tempo decorrido é maior ou igual ao tempo total necessário para a tarefa (time_ms), então a tarefa terminou.

            // Task finished
            // Send msg to application
            msg_t msg = {
                .pid = (*cpu_task)->pid, //identificador da tarefa
                .request = PROCESS_REQUEST_DONE, //código que indica fim da tarefa é um enum
                .time_ms = current_time_ms //atualiza o tempo
            };
            if (write((*cpu_task)->sockfd, &msg, sizeof(msg_t)) != sizeof(msg_t)) { //- Envia a mensagem através do socket associado à tarefa se der erro
                perror("write");
            }
            // Application finished and can be removed (this is FIFO after all)
            free((*cpu_task));
            (*cpu_task) = NULL; //marca cmo cpu livre
        }
    }
    if (*cpu_task == NULL) {// If CPU is idle está livre

       queue_elem_t* ptrParaTarefaMin= encontraTarefaTempoMIN(rq); // huardo o nó que contem a tarefa mini

        if (ptrParaTarefaMin!=NULL) {
            queue_elem_t* tarefaParaExecutar =remove_queue_elem(rq, ptrParaTarefaMin); // elimino o nó associado E RETORNO O PONTEIRO PARA A TEREFA faço isto pk tenho que remover essa tarefa da stack
           *cpu_task=tarefaParaExecutar->pcb;  //
            free(tarefaParaExecutar);   // libertar apenas o nó, não a tarefa
        }else {
            printf("nao conseguiu encontarar tarefa min");
        }

    }
}

