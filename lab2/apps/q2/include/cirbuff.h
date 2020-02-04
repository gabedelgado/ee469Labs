#ifndef __USERPROG__
#define __USERPROG__

typedef struct circbuff {
	int head; 
	int tail;
	char buffer[10];
	int isFull;
} circbuff



#endif
