#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"
#include "filenames.h"

void main (int argc, char * argv[]){

    int numh20;
    sem_t h20;
    sem_t parent;

    numh20 = dstrtol(argv[2], NULL, 10);
    h20 = dstrtol(argv[1],NULL,10);
    parent = dstrtol(argv[3],NULL,10);

    int number_molecules = numh20;


    for(int i = 0; i < number_molecules;++i){
        Print("H2O injected into Radeon atmosphere, PID: %d",getpid());
        if ((i+1) % 2 == 0){
            sem_signal(s_h2);
            sem_signal(s_h2);
            sem_signal(s_o2);
        }
    }

    sem_signal(parent)

    return;
    
}