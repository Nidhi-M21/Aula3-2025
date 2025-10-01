cmake_minimum_required(VERSION 3.25)
project(scheduling C)

set(CMAKE_C_STANDARD 11)

add_executable(scheduler ossim.c queue.c fifo.c
        sjf.c
        mlfq.c
        sjf.h
        rr.c
        rr.h
        mlfq.h
)

add_executable(app app.c
        sjf.c
        sjf.h
        queue.c
        fifo.c
        rr.c
        rr.h
        mlfq.h

)

add_executable(app-io app-io.c burst_queue.c
        sjf.c
        mlfq.c
        sjf.h
        queue.c
        rr.c
        rr.h
        mlfq.h

)
