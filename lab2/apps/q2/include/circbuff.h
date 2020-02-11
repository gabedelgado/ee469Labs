#ifndef __USERPROG__
#define __USERPROG__

typedef struct circbuff {
	int head; 
	int tail;
	char buffer[10];
	int isFull;
} circbuff;


#define FILENAME1_TO_RUN "consumer.dlx.obj"
#define FILENAME2_TO_RUN "producer.dlx.obj"

#endif
