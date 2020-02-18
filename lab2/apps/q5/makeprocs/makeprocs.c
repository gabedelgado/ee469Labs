#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"
#include "filenames.h"

void main (int argc, char * argv[]){

    int numh20 = 0; 
    int nums04 = 0;
    int h2count = 0;
    int o2count = 0;
    int so2count = 0;
    int react3count = 0;
    sem_t s_procs_completed;
    char s_procs_completed_str[10];
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
    char react3count_str[10];
    char numh20str2[10];

    if (argc != 3){
        Printf("usage: filetoexecute.xx : <number of processess to create>");
        Exit();
    }

    numh20 = dstrtol(argv[1], NULL, 10);
    nums04 = dstrtol(argv[2], NULL, 10);
    
    //Printf("Creating %d processes\n", numprocs);

    h2count = (int)(numh20/2) * 2;
    o2count = (int)(numh20 / 2) + nums04;
    so2count = nums04;

    react3count = h2count;

    if (o2count <= h2count && o2count <= so2count){
        react3count = o2count;
    }
    else if (h2count <= o2count && h2count <= so2count){
        react3count = h2count;
    }
    else {
        react3count = so2count;
    }
    
    if ((s_h2o = sem_create(0) == SYNC_FAIL)) {
        Printf("Bad sem_create in s_h2o"); Printf("\n");
        Exit();
    }

    if ((s_so4 = sem_create(0) == SYNC_FAIL)) {
        Printf("Bad sem_create in s_so4"); Printf("\n");
        Exit();
    }

    if ((s_h2 = sem_create(0) == SYNC_FAIL)) {
        Printf("Bad sem_create in s_h2"); Printf("\n");
        Exit();
    }

    if ((s_o2 = sem_create(0) == SYNC_FAIL)) {
        Printf("Bad sem_create in s_o2"); Printf("\n");
        Exit();
    }

    if ((s_so2 = sem_create(0) == SYNC_FAIL)) {
        Printf("Bad sem_create in s_so2"); Printf("\n");
        Exit();
    }

    if ((s_procs_completed = sem_create(-4)) == SYNC_FAIL) {
        Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
        Exit();
    }
    // ditoa(s_procs_completed, s_procs_completed_str);
    
    ditoa(s_h2o, s_h2o_str);
    ditoa(s_so4, s_so4_str);
    ditoa(s_h2, s_h2_str);
    ditoa(s_o2, s_o2_str);
    ditoa(s_so2, s_so2_str);
    ditoa((int)(numh20 / 2), numh20_str);
    ditoa(nums04, numso4_str);
    ditoa(react3count, react3count_str);
    ditoa(s_procs_completed, s_procs_completed_str);
    ditoa(numh20, numh20str2);


    // NEED TO CALCULATE NUMBER OF REACTIONS EXPECTED FOR EACH REACT PROCESS, FROM THE NUMBER OF H20 AND S04 ???


    process_create(REACT1_TO_RUN, s_h2o_str, numh20_str, s_procs_completed_str, s_o2_str, s_h2_str, NULL);
    process_create(REACT2_TO_RUN, s_so4_str, numso4_str, s_procs_completed_str, s_o2_str, s_so2_str, NULL);
    process_create(REACT3_TO_RUN, s_h2_str, s_o2_str, s_so2_str, react3count_str, s_procs_completed_str, NULL);
    
    process_create(INJECT1_TO_RUN, s_h2o_str, numh20str2, s_procs_completed_str, NULL);
    process_create(INJECT2_TO_RUN, s_so4_str, numso4_str, s_procs_completed_str, NULL);

    // MAYBE MAIN SEMAPHORE FOR ALL OF THE PROCESSES TO SIGNAL COMPLETION

    if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
        Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf("\n");
        Exit();
    }

    Printf("%d H2O's left over. %d H2's left over. %d O2's left over.", numh20 % 2, h2count - react3count, o2count - react3count);
    Printf(" %d SO2's left over. %d H2SO4's created.\n", so2count - react3count, react3count);
}