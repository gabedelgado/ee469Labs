for all parts os must be built (cd into os folder and run "make")

after os is made, for example, cd into example, run "make" and then "make run"

after os is made, for q2, cd into q2, run "make", then "make run" 

after os is made, for prio_test, cd into prio_test, run "make", then "make run"

I do believe that my scheduler is working correctly but does have a different order than the expected output.
I think this is because of the way I reorganize the pcb's in the queues, I only remove and put them back in if their priority actually changed ( for when all the runQueue pcb's need to decay estcpu).
If their priority is the same as before the recalculation, I don't remove and put them back in their respective queue.
This can completely change the order in which the pcb's run.

Otherwise, it picks the correct pcb based on the runQueues, updates them when necessary, etc. 

 

