//
// Created by HP on 29/09/2025.
//

#include "fifo.h"

#include <stdio.h>
#include <stdlib.h>

#include "msg.h"
#include <unistd.h>

#include "queue.h"

void rr_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task) {
    if (*cpu_task) {  // Se o CPU está ocupado com uma tarefa

        (*cpu_task)->ellapsed_time_ms += TICKS_MS;      // Incrementa o tempo que a tarefa já esteve a correr.
        (*cpu_task)->slice_start_ms+= TICKS_MS;


        if ((*cpu_task)->ellapsed_time_ms >= (*cpu_task)->time_ms) {//- Se o tempo decorrido é maior ou igual ao tempo total necessário para a tarefa (time_ms), então a tarefa terminou.

            // Task finished
            // Send msg to application
            msg_t msg = {
                .pid = (*cpu_task)->pid, //identificador da tarefa
                .request = PROCESS_REQUEST_DONE, //código que indica fim da tarefa
                .time_ms = current_time_ms //atualiza o tempo
            };
            if (write((*cpu_task)->sockfd, &msg, sizeof(msg_t)) != sizeof(msg_t)) { //- Envia a mensagem através do socket associado à tarefa.

                perror("write");
            }
            // Application finished and can be removed (this is FIFO after all)
            free((*cpu_task));
            (*cpu_task) = NULL; //marca cmo cpu livre

        }else if ((*cpu_task)->slice_start_ms>50) { // se passou do time slice interrompo e coloco no fim da fila deixando o cpu livre

           enqueue_pcb(rq, *cpu_task); // coloquei no final da lista;
            (*cpu_task)->slice_start_ms=0;
           (*cpu_task) = NULL; // e o cpu fica livre
       }
    }
    if (*cpu_task == NULL) {            // If CPU is idle
        *cpu_task = dequeue_pcb(rq);   // Get next task from ready queue (dequeue from head)

        // quando vem uma nova tarefa eu tenho que fazer reset no sou time_slice inicio


    }
}