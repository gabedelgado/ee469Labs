#include "usertraps.h"
#include "misc.h"

void main(int argc, char * argv[]){
    sem_t s_procs_completed;
    mbox_t mbox_S;
    mbox_t mbox_O2;
    int message;

    s_procs_completed = dstrtol(argv[3], NULL, 10);
    mbox_S = dstrtol(argv[1], NULL, 10);
    mbox_O2 = dstrtol(argv[2], NULL, 10);

    Printf("S + 2 O2 -> SO4 reacted, PID: %d.\n", getpid());
    
    if (mbox_open(mbox_S) == MBOX_FAIL) {
        Printf("reactS2O2 Could not open the mailbox!\n", getpid());
        Exit();
    }
    if (mbox_open(mbox_O2) == MBOX_FAIL) {
        Printf("reactS2O2 Could not open the mailbox!\n", getpid());
        Exit();
    }

    mbox_recv(mbox_S, sizeof(int), (void *)&message);
    mbox_recv(mbox_O2, sizeof(int), (void *)&message);
    mbox_recv(mbox_O2, sizeof(int), (void *)&message);
    
    // Printf("PID(%d) releasing SO4 into the atmosphere.\n", getpid());

    if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
        Printf("genCO: Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
        Exit();
    }

}