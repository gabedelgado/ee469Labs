#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "circbuff.h"

void main (int argv, char * argv[]){

// if not full, head will be index position of first available slot 
// if empty, it will be !isFull && head == tail

	circbuff * thebuffer;
	uint32 h_mem;
	sem_t s_procs_completed;
	lock_t lock;
	
	h_mem = dstrol(argv[1], NULL, 10);
	s_procs_completed = dstrol(argv[2], NULL, 10);
	lock = dstrol(argv[3], NULL, 10);
	
	if((thebuffer = (circuff *)shmat(h_mem)) == NULL){
		Printf("could not map the virtual address to memory in ");
		Printf(argv[0]); 
		Printf("exiting");
		Exit();
	}
	
	// for loop to iterate through the length of the message we are 		// putting in the circbuff ("helloworld" has 10 char)
	
		// inside for loop, need to take control of lock, or wait
		// or wait for it to be available
		// then produce the next character needed, ie
		
	char thestring[10] = "helloworld"
	
	int i = 0;
	for (i = 0; i < 10; i++){
		while(lock_acquire(lock) != SYNC_SUCCESS); //wait to get lock
		
		//check if buffer is full
		if (thebuffer->isFull){
			//buffer is full       ^^^ 10 is the buffer size
			lock_release(lock);
			i--;
			// release lock, and decrement i so that it tries putting in that character again next iteration
		}
		else {
			// double check my circular buffer logic please, 
			// haven't tested
			
			thebuffer->buffer[thebuffer->head] = thestring[i];
			thebuffer->head = thebuffer->head + 1;
			if (thebuffer->head == thebuffer->tail){thebuffer->isFull = 1;}
			else{thebuffer->isFull = 0;}
			lock_release(lock);
		}
		
		
	}
	sem_signal(s_procs_completed);
	Exit();
	

}
