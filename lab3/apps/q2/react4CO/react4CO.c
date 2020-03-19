#include "usertraps.h"
#include "misc.h"

void main(int argc, char * argv[]){
    sem_t s_procs_completed;
    mbox_t mbox_CO;
    mbox_t mbox_O2;
    int message;

    s_procs_completed = dstrtol(argv[3], NULL, 10);
    mbox_CO = dstrtol(argv[1], NULL, 10);
    mbox_O2 = dstrtol(argv[2], NULL, 10);

    Printf("4 CO -> 2 O2 + 2 C2 reacted, PID: %d.\n", getpid());
    if (mbox_open(mbox_CO) == MBOX_FAIL) {
        Printf("genS2 Could not open the mailbox!\n", getpid());
        Exit();
    }
    if (mbox_open(mbox_O2) == MBOX_FAIL) {
        Printf("genS2 Could not open the mailbox!\n", getpid());
        Exit();
    }

    mbox_recv(mbox_CO, sizeof(int), (void *)&message);
    mbox_recv(mbox_CO, sizeof(int), (void *)&message);
    mbox_recv(mbox_CO, sizeof(int), (void *)&message);
    mbox_recv(mbox_CO, sizeof(int), (void *)&message);

    message = 1;

    if (mbox_send(mbox_O2, sizeof(message), (void *)&message) == MBOX_FAIL) {
        Printf("genS2 Could not open the mailbox!\n", getpid());
        Exit();
    }
    if (mbox_send(mbox_O2, sizeof(message), (void *)&message) == MBOX_FAIL) {
        Printf("genS2 Could not open the mailbox!\n", getpid());
        Exit();
    }

    if(sem_signal(s_procs_completed) != SYNC_SUCCESS) {
        Printf("genCO: Bad semaphore s_procs_completed (%d)!\n", getpid(), s_procs_completed);
        Exit();
    }

}