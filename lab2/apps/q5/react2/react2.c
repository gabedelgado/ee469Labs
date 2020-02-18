#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

void main (int argc, char * argv[]){

	sem_t s_procs_completed;
    int sO4count = 0;
    sem_t s_so4;
    sem_t s_so2;
    sem_t s_o2;
    int i;

    sO4count = dstrtol(argv[2], NULL, 10);
    s_procs_completed = dstrtol(argv[3], NULL, 10);
    s_so4 = dstrtol(argv[1], NULL, 10);
    s_so2 = dstrtol(argv[5], NULL, 10);
    s_o2 = dstrtol(argv[4], NULL, 10);


    for(i = 0; i < so4count; i++){
        sem_wait(s_so4);
        Printf("SO4 -> SO2 + O2 reacted, PID: %d\n", getpid());
        sem_signal(s_so2);
        sem_signal(s_o2);
    }
    sem_signal(s_procs_completed);

}