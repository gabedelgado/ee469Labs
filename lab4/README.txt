- making and running will be the same as the labs before.
- cd into os, execute "make" command inside os directory
- cd into apps/example, execute "make" command inside apps/example directory
- finally, execute "make run" command inside apps/example directory

- the example folder runs the tests asked of us for q2 and q4 printing the relevant information on screen
- instructions are the same for one-level & fork

- for q4, a hello world program is run with a fork() inside, so there will be a total of 4 page tables printed
- 1 for parent after fork, 1 for child after fork, 1 for parent after rop handler, 1 for child after rop handler
- you will be able to see the pages are the same for child/parent after fork 
  and pte for user stack will be the same for parent process (with readonly bit changed) after rop handler call, 
  and a new page will be assigned to child process after rop handler is called

