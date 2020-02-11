#include "lab2-api.h"
#include "usertraps.h"
#include "misc.h"

#include "circbuff.h"

void main(int argc, char *argv[])
{
	circbuff * thebuffer;
	int h_mem;
	sem_t s_procs_completed;
	lock_t lock;
	char thestring[11] = "hello world";
	int i = 0;
	
	h_mem = dstrtol(argv[1], NULL, 10);
	s_procs_completed = dstrtol(argv[2], NULL, 10);
	lock = dstrtol(argv[3], NULL, 10);
	
	thebuffer = (circbuff *)shmat(h_mem);

	if(thebuffer == NULL){
		Printf("could not map the virtual address to memory in ");
		Printf(argv[0]); 
		Printf("exiting");
		Exit();
	}
	
	for (i = 0; i < 11; i++){
		if (lock_acquire(lock) != SYNC_SUCCESS)
		{
			Printf("couldnt acquire lock");
		} //wait to get lock
		
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
			Printf("Producer %d inserted: %c\n", getpid(), thestring[i]);
			thebuffer->head = (thebuffer->head + 1) % 10;
			if (thebuffer->head == thebuffer->tail){thebuffer->isFull = 1;}
			else{thebuffer->isFull = 0;}
			lock_release(lock);
		}
	}

	sem_signal(s_procs_completed);
	Exit();
}
