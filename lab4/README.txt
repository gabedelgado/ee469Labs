making and running will be the same as the labs before.
cd into os, execute "make" command inside os directory
cd into apps/example, execute "make" command inside apps/example directory
finally, execute "make run" command inside apps/example directory

the example folder runs the tests asked of us for q2 and q4 printing the relevant information on screen

for q4, five hello world's are spawned by makeprocs and then each has one fork inside of it, so 10 hello worlds in total will run
there will also be 2 page tables printed per process, so 20 page tables printed total (two for parent & child after fork, two for each rop handler call to parent/child)

this is the same for one-level, fork, and heap-mgmt