#include "fifo.h"
#include "msg.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



queue_t listasReadyPrioridade[3];

void mlfq_scheduler(uint32_t current_time_ms, queue_t *rq, queue_t listaReadyPrioridade[], pcb_t **cpu_task) {

    while (rq->head != NULL) {
        pcb_t *nova_tarefa = dequeue_pcb(rq);// RETIRO A TAREFA e coloco no array de listas
        nova_tarefa->prioridade = 0;
        nova_tarefa->slice_start_ms = 0;
        enqueue_pcb(&listaReadyPrioridade[0], nova_tarefa); // vai guardando a tarefa
    }

    // Se há tarefa no CPU
    if (*cpu_task) {
        printf("[DEBUG1] PID %d | prioridade %d | ellapsed %u | slice %u | time_ms %u\n",(*cpu_task)->pid,(*cpu_task)->prioridade,(*cpu_task)->ellapsed_time_ms,(*cpu_task)->slice_start_ms,(*cpu_task)->time_ms);

        (*cpu_task)->ellapsed_time_ms += TICKS_MS;
        (*cpu_task)->slice_start_ms += TICKS_MS;



        (*cpu_task)->ellapsed_time_ms += TICKS_MS;
        (*cpu_task)->slice_start_ms += TICKS_MS;


        printf("[DEBUG2] PID %d | ellapsed %u | slice %u\n",(*cpu_task)->pid,(*cpu_task)->ellapsed_time_ms,(*cpu_task)->slice_start_ms);


        if ((*cpu_task)->ellapsed_time_ms >= (*cpu_task)->time_ms) {


            printf("[DEBUG5] PID %d terminou. ellapsed=%u, time_ms=%u\n",(*cpu_task)->pid,(*cpu_task)->ellapsed_time_ms,(*cpu_task)->time_ms);

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
        }
        // Se passouo time slice
        else {
            int prioridade_atual = (*cpu_task)->prioridade;


            if ((*cpu_task)->slice_start_ms >= 500) {


                printf("[DEBUG3] PID %d excedeu slice de %u ms\n",
                       (*cpu_task)->pid, (*cpu_task)->slice_start_ms);

                // se utilisar o tempo do slice ent vamos diminuir a sua prioridade
                if (prioridade_atual < 2) {// PARA NAO ATINGIR O 3
                    (*cpu_task)->prioridade++;  // BAIXO prioridade
                    printf("[DEBUG4] PID %d mudou para prioridade %d\n", (*cpu_task)->pid, (*cpu_task)->prioridade);

                }else {
                    printf("[DEBUG8] PID %d está na fila final (prioridade %d)\n",(*cpu_task)->pid, (*cpu_task)->prioridade);
                }

                (*cpu_task)->slice_start_ms = 0;  //atualizar e colocar na lsita
                enqueue_pcb(&listaReadyPrioridade[(*cpu_task)->prioridade], *cpu_task);


                printf("[DEBUG7] PID %d enfileirado na prioridade %d\n",(*cpu_task)->pid, (*cpu_task)->prioridade);


                *cpu_task = NULL;
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