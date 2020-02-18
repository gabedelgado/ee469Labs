#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"
#include "filenames.h"

void main (int argc, char * argv[]){

    int numSO4;
    sem_t sO4;
    sem_t parent;
    int number_molecules;
    int i;
    numSO4 = dstrtol(argv[2], NULL, 10);
    sO4 = dstrtol(argv[1],NULL,10);
    parent = dstrtol(argv[3],NULL,10);

    number_molecules = numSO4;


    for(i = 0; i < number_molecules;++i){
        Printf("SO4 injected into Radeon atmosphere, PID: %d\n",getpid());
        sem_signal(sO4);
    }

    sem_signal(parent);
    
}