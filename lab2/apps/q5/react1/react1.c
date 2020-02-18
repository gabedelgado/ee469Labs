#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

void main (int argc, char * argv[]){

	sem_t s_procs_completed;
    int h2ocount = 0;
    sem_t s_h2o;
    sem_t s_o2;
    sem_t s_h2;
    int i;

    h2ocount = dstrtol(argv[2], NULL, 10);
    s_procs_completed = dstrtol(argv[3], NULL, 10);
    s_h2o = dstrtol(argv[1], NULL, 10);
    s_h2 = dstrtol(argv[5], NULL, 10);
    s_o2 = dstrtol(argv[4], NULL, 10);

    //Printf("%dh20count in react1 %d", h2ocount);
    for(i = 0; i < h2ocount; i++){
        sem_wait(s_h2o);
        sem_wait(s_h2o);

        Printf("2 H2O -> 2 H2 + O2 reacted, PID: %d\n", getpid());
        sem_signal(s_h2);
        sem_signal(s_h2);
        sem_signal(s_o2);

    }
    sem_signal(s_procs_completed);

}
