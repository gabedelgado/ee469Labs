#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"
#include "filenames.h"

void main (int argc, char * argv[]){

    int numSO4;
    sem_t sO4;
    sem_t parent;

    numSO4 = dstrtol(argv[2], NULL, 10);
    sO4 = dstrtol(argv[1],NULL,10);
    parent = dstrtol(argv[3],NULL,10);

    int number_molecules = numh20 % 1;


    for(int i = 0; i < number_molecules;++i){
        Print("SO4 injected into Radeon atmosphere, PID: %d",getpid());
        sem_signal(s_sO2);
        sem_signal(s_O2);
    }

    sem_signal(parent)
    
    return;
    
}