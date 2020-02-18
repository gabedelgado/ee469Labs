#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"
#include "filenames.h"

void main (int argc, char * argv[]){

    int numh20 = 0; 
    int nums04 = 0;
    int i;
    // sem_t s_procs_completed;
    // char s_procs_completed_str[10];
    sem_t s_h2o;
    sem_t s_so4;
    sem_t s_h2;
    sem_t s_o2;
    sem_t s_so2;

    char s_h2o_str[10];
    char s_so4_str[10];
    char s_h2_str[10];
    char s_o2_str[10];
    char s_so2_str[10];

    char numh20_str[10];
    char numso4_str[10];

    if (argc != 3){
        Printf("usage: filetoexecute.xx : <number of processess to create>");
        Exit();
    }

    numh20 = dstrtol(argv[1], NULL, 10);
    nums04 = dstrtol(argv[2], NULL, 10);
    
    //Printf("Creating %d processes\n", numprocs);


    
    // if ((s_procs_completed = sem_create(-1 * (2 * numprocs-1))) == SYNC_FAIL) {
    //     Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
    //     Exit();
    // }
    
    
    if ((s_h2o = sem_create(-1) == SYNC_FAIL)) {
        Printf("Bad sem_create in s_h2o"); Printf("\n");
        Exit();
    }

    if ((s_so4 = sem_create(-1) == SYNC_FAIL)) {
        Printf("Bad sem_create in s_so4"); Printf("\n");
        Exit();
    }

    if ((s_h2 = sem_create(-1) == SYNC_FAIL)) {
        Printf("Bad sem_create in s_h2"); Printf("\n");
        Exit();
    }

    if ((s_o2 = sem_create(-1) == SYNC_FAIL)) {
        Printf("Bad sem_create in s_o2"); Printf("\n");
        Exit();
    }

    if ((s_so2 = sem_create(-1) == SYNC_FAIL)) {
        Printf("Bad sem_create in s_so2"); Printf("\n");
        Exit();
    }
    // ditoa(s_procs_completed, s_procs_completed_str);
    
    ditoa(s_h2o, s_h2o_str);
    ditoa(s_so4, s_so4_str);
    ditoa(s_h2, s_h2_str);
    ditoa(s_o2, s_o2_str);
    ditoa(s_so2, s_so2_str);
    ditoa(numh20, numh20_str);
    ditoa(numh20, numh20_str);

    // NEED TO CALCULATE NUMBER OF REACTIONS EXPECTED FOR EACH REACT PROCESS, FROM THE NUMBER OF H20 AND S04 ???

    process_create(REACT1_TO_RUN, s_h2o_str, NULL);
    process_create(REACT2_TO_RUN, s_so4_str, NULL);
    process_create(REACT3_TO_RUN, s_h2_str, s_o2_str, s_so2_str, NULL);
    
    process_create(INJECT1_TO_RUN, NULL);
    process_create(INJECT2_TO_RUN, NULL);

    // for(i = 0; i < numprocs; i++){
    //     process_create(FILENAME2_TO_RUN, h_mem_str, s_procs_completed_str, lock_str, emptycond_str, fullcond_str, NULL);
    //     process_create(FILENAME1_TO_RUN, h_mem_str, s_procs_completed_str, lock_str, emptycond_str, fullcond_str, NULL);
    //     Printf("2 processess created\n");
    // }

    if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
        Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf("\n");
        Exit();
    }

    Printf("All other processes completed, exiting main process.\n");

}