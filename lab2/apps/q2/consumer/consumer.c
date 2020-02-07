#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "circbuff.h"

void main (int argc, char * argv[]){

// if not full, head will be index position of first available slot 
// if empty, it will be !isFull && head == tail

	circbuff * thebuffer;
	uint32 h_mem;
	sem_t s_procs_completed;
	lock_t lock;
	
	h_mem = dstrtol(argv[1], NULL, 10);
	s_procs_completed = dstrtol(argv[2], NULL, 10);
	lock = dstrtol(argv[3], NULL, 10);
	
	if((thebuffer = (circbuff *)shmat(h_mem)) == NULL){
		Printf("could not map the virtual address to memory in ");
		Printf(argv[0]); 
		Printf("exiting");
		Exit();
	}

    // consumer will pull out 10 characters (strlen("helloworld"))
    
    int i = 0;
    char thestring[10] = "helloworld";
    
    for (i = 0; i < 10; i++){
        while(lock_acquire(lock) != SYNC_SUCCESS);

        if (thebuffer->isFull || (thebuffer->head != thebuffer->tail)){
            //characters available to pull out
            Printf("Consumer %d removed: %c", getpid(), thebuffer->buffer[thebuffer->tail]);
            thebuffer->tail = (thebuffer->tail + 1) % 10;
            if (thebuffer->isFull){
                thebuffer->isFull = 0;
            }
            lock_release(lock);
        }
        else {
            //thebuffer is empty
            lock_release(lock);
            i--;
            // i-- so that for loop will try taking out the i-th letter again
        }

    }
	
}