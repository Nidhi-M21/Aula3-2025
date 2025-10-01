//
// Created by HP on 01/10/2025.
//

#ifndef ROUNDROUBIN_H
#define ROUNDROUBIN_H

#include <stdint.h>
#include "queue.h"

void rr_scheduler(uint32_t current_time_ms, queue_t *rq, pcb_t **cpu_task);

#endif //ROUNDROUBIN_H