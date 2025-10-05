#include "fifo.h"
#include "msg.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



queue_t listasReadyPrioridade[3]; // array com as listas

void mlfq_scheduler(uint32_t current_time_ms, queue_t *rq, queue_t listaReadyPrioridade[], pcb_t **cpu_task) {

    while (rq->head != NULL) { // estou a percorrer a lista dada para colocar na posiçao 0 do  meu array de lsitas
        pcb_t *nova_tarefa = dequeue_pcb(rq);// RETIRO A TAREFA e coloco no array de listas
        nova_tarefa->prioridade = 0;
        nova_tarefa->slice_start_ms = 0;
        enqueue_pcb(&listaReadyPrioridade[0], nova_tarefa); // vai guardando a tarefa
    }


    if (*cpu_task) {


        (*cpu_task)->ellapsed_time_ms += TICKS_MS;
        (*cpu_task)->slice_start_ms += TICKS_MS;

        if ((*cpu_task)->ellapsed_time_ms >= (*cpu_task)->time_ms) { // se atingiu o limite que pediu antes
            msg_t msg = {
                .pid = (*cpu_task)->pid,
                .request = PROCESS_REQUEST_DONE,
                .time_ms = current_time_ms
            };
            if (write((*cpu_task)->sockfd, &msg, sizeof(msg_t)) != sizeof(msg_t)) {
                perror("write");
            }
            free(*cpu_task);
            *cpu_task = NULL;
        }else {
            int prioridade = (*cpu_task)->prioridade;


            if ((*cpu_task)->slice_start_ms >= 500) {// se passou do timesgile

                // se utilisar o tempo do slice ent vamos diminuir a sua prioridade
                if (prioridade < 2) {// PARA NAO ATINGIR O 3
                    (*cpu_task)->prioridade++;  // BAIXO prioridade


                }

                (*cpu_task)->slice_start_ms = 0;  //atualizar e colocar na lsita
                enqueue_pcb(&listaReadyPrioridade[(*cpu_task)->prioridade], *cpu_task);

                *cpu_task = NULL;// coloco o cpu livre
            }
        }
    }

    if (*cpu_task == NULL) {
        for (int i = 0; i < 3; i++) {

            if (listaReadyPrioridade[i].head != NULL) { // percorro a lista

                *cpu_task = dequeue_pcb(&listaReadyPrioridade[i]); // tiro a proxima tarefa da lista com maior prioridade
               // (*cpu_task)->slice_start_ms = 0;
                printf("PRIORIDADE: %d | PID: %d\n", (*cpu_task)->prioridade, (*cpu_task)->pid);
                break;
            }
        }
    }
}