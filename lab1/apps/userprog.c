#include "usertraps.h"

unsigned int result;
void main (int x)
{
  Printf("Hello World!\n"); 
  result = Getpid();
  Printf("current pid is %d", result);
  while(1); // Use CTRL-C to exit the simulator
}
