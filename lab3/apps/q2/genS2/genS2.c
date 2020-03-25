
#include "usertraps.h"
#include "misc.h"

void main(int argc, char * argv[]){
    sem_t s_procs_completed;
    mbox_t mbox_S2;
    int message = 1;
    Printf("got here at least\n");
    s_procs_completed = dstrtol(argv[2], NULL, 10);
    mbox_S2 = dstrtol(argv[1], NULL, 10);

    Printf("S2 injected in atmosphere, PID: %d.\n", getpid());
    
    if (mbox_open(mbox_S2) == MBOX_FAIL) {
        Printf("genS2 Could not open the mailbox!\n", getpid());
        Exit();
    }

    if (mbox_send(mbox_S2, sizeof(message), (void *)&message) == MBOX_FAIL) {
        Printf("Could not send message to mailbox %d in %s (%d)\n", mbox_S2, argv[0], getpid());
        Exit();
    }

    if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
        Printf("genS2: Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
        Exit();
    }

}