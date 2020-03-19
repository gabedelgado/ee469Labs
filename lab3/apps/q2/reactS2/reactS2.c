
#include "usertraps.h"
#include "misc.h"

void main(int argc, char * argv[]){
    sem_t s_procs_completed;
    mbox_t mbox_S;
    mbox_t mbox_S2;
    int message;

    s_procs_completed = dstrtol(argv[3], NULL, 10);
    mbox_S2 = dstrtol(argv[1], NULL, 10);
    mbox_S = dstrtol(argv[2], NULL, 10);

    Printf("S2 -> S + S reacted, PID: %d.\n", getpid());
    if (mbox_open(mbox_S2) == MBOX_FAIL) {
        Printf("genS2 Could not open the mailbox!\n", getpid());
        Exit();
    }
    if (mbox_open(mbox_S) == MBOX_FAIL) {
        Printf("genS2 Could not open the mailbox!\n", getpid());
        Exit();
    }

    mbox_recv(mbox_S2, sizeof(int), (void *)&message);

    message = 1;

    if (mbox_send(mbox_S, sizeof(message), (void *)&message) == MBOX_FAIL) {
        Printf("genS2 Could not open the mailbox!\n", getpid());
        Exit();
    }
    if (mbox_send(mbox_S, sizeof(message), (void *)&message) == MBOX_FAIL) {
        Printf("genS2 Could not open the mailbox!\n", getpid());
        Exit();
    }
    
    // Printf("S injected in atmosphere, PID: %d.\n", getpid());
    // Printf("S injected in atmosphere, PID: %d.\n", getpid());
    
    if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
        Printf("genCO: Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
        Exit();
    }

}