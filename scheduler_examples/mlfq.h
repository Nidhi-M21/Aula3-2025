//
// Created by HP on 01/10/2025.
//

#ifndef SCHEDULING_MLFQ_H
#define SCHEDULING_MLFQ_H
#include <stdint.h>
#include "queue.h"

void mlfq_scheduler(uint32_t current_time_ms,queue_t *rq ,queue_t listaReadyPrioridade[], pcb_t **cpu_task) ;
#endif //SCHEDULING_MLFQ_H