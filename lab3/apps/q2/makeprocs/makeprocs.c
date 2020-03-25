#ifndef __USERPROG__
#define __USERPROG__
#endif

#include "usertraps.h"
#include "misc.h"

void main (int argc, char * argv[]){
    int num_S2 = 0;
    int num_CO = 0;
    int numprocs_react_S2;
    int numprocs_react_4CO;
    int numprocs_react_S_2O2;
    int i = 0;

    mbox_t mbox_S;
    mbox_t mbox_O2;
    mbox_t mbox_CO;
    mbox_t mbox_S2;
    mbox_t mbox_C2;

    sem_t s_procs_completed;
    
    char mbox_S_str[10];
    char mbox_O2_str[10];
    char mbox_CO_str[10];
    char mbox_S2_str[10];
    char mbox_C2_str[10];
    char s_procs_completed_str[10];
    char message[] = "message";

    if (argc != 3){
        Printf("Usage is <num S2> <num CO>");
        Exit();
    }

    num_S2 = dstrtol(argv[1], NULL, 10);
    num_CO = dstrtol(argv[2], NULL, 10);

    numprocs_react_4CO = num_CO / 4;
    numprocs_react_S2 = num_S2;

    if ((num_S2 * 2) <= numprocs_react_4CO){
        numprocs_react_S_2O2 = num_S2 * 2;
    }
    else {
        numprocs_react_S_2O2 = numprocs_react_4CO;
    }

    if ((mbox_S = mbox_create()) == MBOX_FAIL){
        Printf("mbox_create fail 1");
        Exit();
    }
    if (mbox_open(mbox_S) == MBOX_FAIL){
        Printf("mbox_open fail 1");
        Exit();
    }

    if ((mbox_O2 = mbox_create()) == MBOX_FAIL){
        Printf("mbox_create fail 1");
        Exit();
    }
    if (mbox_open(mbox_O2) == MBOX_FAIL){
        Printf("mbox_open fail 1");
        Exit();
    }

    if ((mbox_CO = mbox_create()) == MBOX_FAIL){
        Printf("mbox_create fail 1");
        Exit();
    }
    if (mbox_open(mbox_CO) == MBOX_FAIL){
        Printf("mbox_open fail 1");
        Exit();
    }
    
    if ((mbox_S2 = mbox_create()) == MBOX_FAIL){
        Printf("mbox_create fail 1");
        Exit();
    }
    if (mbox_open(mbox_S2) == MBOX_FAIL){
        Printf("mbox_open fail 1");
        Exit();
    }

    // DO WE NEED A C2 MAILBOX

    if ((s_procs_completed = sem_create(-(num_CO + num_S2 + numprocs_react_4CO + numprocs_react_S2 + numprocs_react_S_2O2 - 1))) == SYNC_FAIL){
        Printf("semc_create failed");
        Exit();
    }

    ditoa(s_procs_completed, s_procs_completed_str);
    ditoa(mbox_CO, mbox_CO_str);
    ditoa(mbox_C2, mbox_C2_str);
    ditoa(mbox_O2, mbox_O2_str);
    ditoa(mbox_S2, mbox_S2_str);
    ditoa(mbox_S, mbox_S_str);

    for (i = 0; i < num_S2; i++){
        process_create("genS2.dlx.obj", mbox_S2_str, s_procs_completed_str, NULL);
    }
    Printf("here 1\n");
    for (i = 0; i < num_CO; i++){
        process_create("genCO.dlx.obj", mbox_CO_str, s_procs_completed_str, NULL);
    }
    for (i = 0; i < numprocs_react_4CO; i++){
        process_create("react4CO.dlx.obj", mbox_CO_str, mbox_O2_str ,s_procs_completed_str, NULL);
    }
    for (i = 0; i < numprocs_react_S2; i++){
        process_create("reactS2.dlx.obj", mbox_S2_str, mbox_S_str, s_procs_completed_str, NULL);
    }
    Printf("here 2\n");
    for (i = 0; i < numprocs_react_S_2O2; i++){
        process_create("reactS2O2.dlx.obj", mbox_S_str, mbox_O2_str,s_procs_completed_str, NULL);
    }
    Printf("all processes created\n");
    
    if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
        Printf("bad sem s_procs_completed");
    }

    Printf("gen and react processes completed");
}