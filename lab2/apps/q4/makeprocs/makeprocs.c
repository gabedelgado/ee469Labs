#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "circbuff.h"

void main (int argc, char * argv[]){

    int numprocs = 0; 
    int i;
    circbuff * buff;
    uint32 h_mem;
    sem_t s_procs_completed;
    char h_mem_str[10];
    char s_procs_completed_str[10];
    lock_t lock;
    char lock_str[10];
    cond_t fullcond;
    cond_t emptycond;
    char fullcond_str[10];
    char emptycond_str[10];



    if (argc != 2){
        Printf("usage: filetoexecute.xx : <number of processess to create>");
        Exit();
    }

    numprocs = dstrtol(argv[1], NULL, 10);
    Printf("Creating %d processes\n", numprocs);
      
    if ((h_mem = shmget()) == 0) {
        Printf("ERROR: could not allocate shared memory page in "); Printf(argv[0]); Printf(", exiting...\n");
        Exit();
    }

    if ((buff = (circbuff *)shmat(h_mem)) == NULL) {
        Printf("Could not map the shared page to virtual address in "); Printf(argv[0]); Printf(", exiting..\n");
        Exit();
    }

    if ((lock = lock_create()) == SYNC_FAIL){
        Printf("could not create lock correctly");
        Exit();
    }

    if ((s_procs_completed = sem_create(-1 * (2 * numprocs-1))) == SYNC_FAIL) {
        Printf("Bad sem_create in "); Printf(argv[0]); Printf("\n");
        Exit();
    }
    Printf("lock val in makeprocs %d", lock);
    if ((fullcond = cond_create(lock)) == SYNC_FAIL){
        Printf("could not create condition variable fullcond");
        Exit();
    }

    if ((emptycond = cond_create(lock)) == SYNC_FAIL){
        Printf("could not create condition variable emptycond");
        Exit();
    }

    ditoa(h_mem, h_mem_str);
    ditoa(s_procs_completed, s_procs_completed_str);
    ditoa(lock, lock_str);
    ditoa(emptycond, emptycond_str);
    ditoa(fullcond, fullcond_str);



    for(i = 0; i < numprocs; i++){
        process_create(FILENAME2_TO_RUN, h_mem_str, s_procs_completed_str, lock_str, emptycond_str, fullcond_str, NULL);
        process_create(FILENAME1_TO_RUN, h_mem_str, s_procs_completed_str, lock_str, emptycond_str, fullcond_str, NULL);
        Printf("2 processess created\n");
    }

    if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
        Printf("Bad semaphore s_procs_completed (%d) in ", s_procs_completed); Printf(argv[0]); Printf("\n");
        Exit();
    }

    Printf("All other processes completed, exiting main process.\n");

}