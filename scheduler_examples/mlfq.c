//
// Created by HP on 30/09/2025.
//
#include "fifo.h"

#include <stdio.h>
#include <stdlib.h>

#include "msg.h"
#include <unistd.h>

#include "queue.h"

queue_t listasReadyPrioridade[8];

void mlfq_scheduler(uint32_t current_time_ms, queue_t listaReadyPrioridade[], pcb_t **cpu_task) {


    if (*cpu_task) {  // Se o CPU está ocupado com uma tarefa

        (*cpu_task)->ellapsed_time_ms += TICKS_MS;      // Incrementa o tempo que a tarefa já esteve a correr.
        (*cpu_task)->slice_start_ms+= TICKS_MS;

// perguntar ao prof se eu posso tirar este if pk mlfq so termina quando nao houver mais nenhuma task
        if ((*cpu_task)->ellapsed_time_ms >= (*cpu_task)->time_ms) {//- Se o tempo decorrido é maior ou igual ao tempo total necessário para a tarefa (time_ms), então a tarefa terminou.


            msg_t msg = {
                .pid = (*cpu_task)->pid,
                .request = PROCESS_REQUEST_DONE,
                .time_ms = current_time_ms
            };
            if (write((*cpu_task)->sockfd, &msg, sizeof(msg_t)) != sizeof(msg_t)) { //- Envia a mensagem através do socket associado à tarefa.

                perror("write");
            }
            // Application finished and can be removed (this is FIFO after all)
            free((*cpu_task));
            (*cpu_task) = NULL; //marca cmo cpu livre

        }else if ((*cpu_task)->slice_start_ms>500) { // se passou do time slice interrompo e coloco no fim da fila deixando o cpu livre e diminuo a prioridade


             if (*cpu_task != NULL && (*cpu_task)->prioridade<=7) {
                 (*cpu_task)->prioridade++; //baixo a prioridade
            }

            int prioridadeTarefa=(*cpu_task)->prioridade;

                (*cpu_task)->slice_start_ms=0;
                enqueue_pcb(&listaReadyPrioridade[prioridadeTarefa], *cpu_task);
                *cpu_task = NULL;


            //enqueue_pcb(&listaReadyPrioridade[priodiddaeTarefa],*cpu_task); // coloco a tarefa na lista correspondente
           // (*cpu_task) = NULL;

        }
    }
    if (*cpu_task == NULL) {
       //escolher a proxima tarefa com base na sua perioridade e se estivar vazia ou nao a lista
       for (int i = 0; i < 8; i++) {


           if (listaReadyPrioridade[i].head!=NULL ) { // estou a tirar a proxima tarefa da fila que ainda está completa com prioridade mais alta
               (*cpu_task) = dequeue_pcb(&listaReadyPrioridade[i]);
                   (*cpu_task)->slice_start_ms = 0;
               break;

           }
       }

    }
}