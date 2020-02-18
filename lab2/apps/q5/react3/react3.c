#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

void main (int argc, char * argv[]){

	sem_t s_procs_completed;
    int reactcount = 0;
    sem_t s_so2;
    sem_t s_o2;
    sem_t s_h2;
    int i;

    reactcount = dstrtol(argv[4], NULL, 10);
    s_procs_completed = dstrtol(argv[5], NULL, 10);
    s_so2 = dstrtol(argv[3], NULL, 10);
    s_h2 = dstrtol(argv[1], NULL, 10);
    s_o2 = dstrtol(argv[2], NULL, 10);

    
    for(i = 0; i < reactcount; i++){
        sem_wait(s_o2);
        sem_wait(s_so2);
        sem_wait(s_h2);

        Printf("(%d) H2 + O2 + SO2 -> H2SO4 reacted, PID: %d", i + 1, getpid());
    }
    
    sem_signal(s_procs_completed);

}
