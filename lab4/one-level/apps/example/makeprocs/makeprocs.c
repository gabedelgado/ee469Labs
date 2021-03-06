#include "usertraps.h"
#include "misc.h"

#define HELLO_WORLD "hello_world.dlx.obj"
#define MAXADDRESS "maxvaddress.dlx.obj"
#define OUTSIDEPAGES "outsidepages.dlx.obj"
#define GROWSTACK "growcallstack.dlx.obj"
#define BIGCOUNT "bigcount.dlx.obj"

void main (int argc, char *argv[])
{
  int num_hello_world = 0;             // Used to store number of processes to create
  int i;                               // Loop index variable
  int z;
  sem_t s_procs_completed;             // Semaphore used to wait until all spawned processes have completed
  char s_procs_completed_str[10];      // Used as command-line argument to pass page_mapped handle to new processes
  sem_t s_bigcount_completed;
  char s_bigcount_completed_str[10];

  if (argc != 2) {
    Printf("Usage: %s <number of hello world processes to create>\n", argv[0]);
    Exit();
  }

  // Convert string from ascii command line argument to integer number
  num_hello_world = dstrtol(argv[1], NULL, 10); // the "10" means base 10
  Printf("makeprocs (%d): Creating %d hello_world processes\n", getpid(), num_hello_world);

  // Create semaphore to not exit this process until all other processes 
  // have signalled that they are complete.
  if ((s_procs_completed = sem_create(0)) == SYNC_FAIL) {
    Printf("makeprocs (%d): Bad sem_create\n", getpid());
    Exit();
  }

  if ((s_bigcount_completed = sem_create(-29)) == SYNC_FAIL) {
    Printf("makeprocs (%d): Bad sem_create\n", getpid());
    Exit();
  }
  // Setup the command-line arguments for the new processes.  We're going to
  // pass the handles to the semaphore as strings
  // on the command line, so we must first convert them from ints to strings.
  ditoa(s_procs_completed, s_procs_completed_str);
  ditoa(s_bigcount_completed, s_bigcount_completed_str);
  // Create Hello World processes
  Printf("--------------------START Q2 TEST 1---------------------------------------------------------\n");
  Printf("makeprocs (%d): Creating %d hello world's in a row, but only one runs at a time\n", getpid(), num_hello_world);
  for(i=0; i<num_hello_world; i++) {
    Printf("makeprocs (%d): Creating hello world #%d\n", getpid(), i);
    process_create(HELLO_WORLD, s_procs_completed_str, NULL);
    if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
      Printf("Bad semaphore s_procs_completed (%d) in %s\n", s_procs_completed, argv[0]);
      Exit();
    }
  }
  Printf("-------------------------------------------------------------------------------------\n");
  
  
  Printf("-------------START Q2 TEST 3 --------------------------------------------------------------------\n");
  process_create(OUTSIDEPAGES, s_procs_completed_str, NULL);
  sem_wait(s_procs_completed);
  Printf("-------------------------------------------------------------------------------------\n");
  

  Printf("--------------------START Q2 TEST 4--------------------------------------------------------\n");
  Printf("makeprocs (%d): calling grow stack process\n", getpid());
  process_create(GROWSTACK, s_procs_completed_str, NULL);
  sem_wait(s_procs_completed);
  Printf("-------------------------------------------------------------------------------------\n");
  

  Printf("------------------------START Q2 TEST 5---------------------------------------------------\n");
  Printf("makeprocs (%d): calling hello world process 100 times\n", getpid());
  for(i=0; i<100; i++) {
    //Printf("makeprocs (%d): Creating hello world #%d\n", getpid(), i);
    process_create(HELLO_WORLD, s_procs_completed_str, NULL);
    if (sem_wait(s_procs_completed) != SYNC_SUCCESS) {
      Printf("Bad semaphore s_procs_completed (%d) in %s\n", s_procs_completed, argv[0]);
      Exit();
    }
  }
  Printf("-------------------------------------------------------------------------------------\n");
  

  Printf("----------------------START Q2 TEST 6----------------------------------------------------\n");
  for(z=0; z < 30; z++) {
    process_create(BIGCOUNT, s_bigcount_completed_str, NULL);
    Printf("made bigcount #%d\n", z);
    // sem_wait(s_bigcount_completed);
  }
  if (sem_wait(s_bigcount_completed) != SYNC_SUCCESS) {
    Printf("Bad semaphore s_procs_completed (%d) in %s\n", s_procs_completed, argv[0]);
    Exit();
  }
  Printf("-------------------------------------------------------------------------------------\n");
  
  
  Printf("---------------------------START Q2 TEST 2, WILL KILL SIMULATION----------------------------------------------\n");
  process_create(MAXADDRESS, s_procs_completed_str, NULL);
  sem_wait(s_procs_completed);
  Printf("-------------------------------------------------------------------------------------\n");
  Printf("makeprocs (%d): All other processes completed, exiting main process.\n", getpid());

}
