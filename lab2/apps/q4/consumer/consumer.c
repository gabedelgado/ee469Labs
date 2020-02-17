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
    cond_t notemptycond;
    cond_t fullcond;

    int i = 0;
    char thestring[11] = "hello world";
	
	h_mem = dstrtol(argv[1], NULL, 10);
	s_procs_completed = dstrtol(argv[2], NULL, 10);
	lock = dstrtol(argv[3], NULL, 10);
	notemptycond = dstrtol(argv[4], NULL, 10);
    fullcond = dstrtol(argv[5], NULL, 10);

	if((thebuffer = (circbuff *)shmat(h_mem)) == NULL){
		Printf("could not map the virtual address to memory in ");
		Printf(argv[0]); 
		Printf("exiting");
		Exit();
	}

    // consumer will pull out 10 characters (strlen("helloworld"))
    
    
    for (i = 0; i < 11; i++){
		if (lock_acquire(lock) != SYNC_SUCCESS){
			Printf("couldnt acquire lock");
		} //wait to get lock

        if (!(thebuffer->isFull || (thebuffer->head != thebuffer->tail))){
            //characters NOT available to pull out
            cond_wait(notemptycond);
        }
        
        if (lock_acquire(lock) != SYNC_SUCCESS){
			Printf("couldnt acquire lock");
		} 
        
        Printf("Consumer %d removed: %c\n", getpid(), thebuffer->buffer[thebuffer->tail]);
        thebuffer->tail = (thebuffer->tail + 1) % 10;
        if (thebuffer->isFull){
            thebuffer->isFull = 0;
            cond_signal(fullcond);
        }
        lock_release(lock);

    }
	sem_signal(s_procs_completed);
	Exit();
}