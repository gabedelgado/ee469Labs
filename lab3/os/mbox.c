#include "ostraps.h"
#include "dlxos.h"
#include "process.h"
#include "synch.h"
#include "queue.h"
#include "mbox.h"

static mbox mboxes[MBOX_NUM_MBOXES];
static mbox_message messages[MBOX_NUM_BUFFERS];

//-------------------------------------------------------
//
// void MboxModuleInit();
//
// Initialize all mailboxes.  This process does not need
// to worry about synchronization as it is called at boot
// time.  Only initialize necessary items here: you can
// initialize others in MboxCreate.  In other words, 
// don't waste system resources like locks and semaphores
// on unused mailboxes.
//
//-------------------------------------------------------

void MboxModuleInit() {
	int i;
	for (i = 0; i < MBOX_NUM_MBOXES; i++){
		mboxes[i].inuse = false;
	}
	for (i = 0; i < MBOX_NUM_BUFFERS; i++){
		messages[i].inuse = false;
	}
}

//-------------------------------------------------------
//
// mbox_t MboxCreate();
//
// Allocate an available mailbox structure for use. 
//
// Returns the mailbox handle on success
// Returns MBOX_FAIL on error.
//
//-------------------------------------------------------
mbox_t MboxCreate() {
	mbox_t m;
	uint32 intrval;

	intrval = DisableIntrs();
	for(m = 0; m < MBOX_NUM_MBOXES; m++){
		if(mboxes[m].inuse == false){
			mboxes[m].inuse = true;
			break;
		}
	}
	RestoreIntrs(intrval);
	if (m == MBOX_NUM_MBOXES) return MBOX_FAIL;

	if (mboxInit(&mboxes[m]) == SYNC_FAIL) return MBOX_FAIL;
	return m;
}

int mboxInit(mbox * m){
	int i = 0;
	if (!m) return SYNC_FAIL;
	if (AQueueInit(&m->msg_queue) == QUEUE_FAIL) {
		printf("ERROR: could not initialize mbox queue");
		exitsim();
	}
	if ((m->lock = LockCreate()) == SYNC_FAIL) {
		printf("ERROR: could not initialize mbox lock");
		exitsim();
	}
	if ((m->notfull = CondCreate(m->lock)) == INVALID_COND){
		printf("ERROR: could not initialize notfull mbox cond var");
		exitsim();
	}
	if ((m->not_empty = CondCreate(m->lock)) == INVALID_COND){
		printf("ERROR: could not initialize notfull mbox cond var");
		exitsim();
	}
	for (i = 0; i < 30; i++){
		m->procs[i] = -1;
	}
	return SYNC_SUCCESS;

}

//-------------------------------------------------------
// 
// void MboxOpen(mbox_t);
//
// Open the mailbox for use by the current process.  Note
// that it is assumed that the internal lock/mutex handle 
// of the mailbox and the inuse flag will not be changed 
// during execution.  This allows us to get the a valid 
// lock handle without a need for synchronization.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxOpen(mbox_t handle) {
	int pid = GetCurrentPid();
	int i = 0;
	printf("opening mailbox for pid (%d)\n", pid);
	LockHandleAcquire(mboxes[handle].lock);
	while(mboxes[handle].procs[i] != -1){i++;}
	if (i >= 30){
		printf("ERROR: more than 30 procs using mbox");
		exitsim();
	}
	mboxes[handle].procs[i] = pid;
	LockHandleRelease(mboxes[handle].lock);
	return MBOX_SUCCESS;
}

//-------------------------------------------------------
//
// int MboxClose(mbox_t);
//
// Close the mailbox for use to the current process.
// If the number of processes using the given mailbox
// is zero, then disable the mailbox structure and
// return it to the set of available mboxes.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxClose(mbox_t handle) {
	int pid = GetCurrentPid();
	int i = 0;
	LockHandleAcquire(mboxes[handle].lock);
	while(mboxes[handle].procs[i] != pid){
		if (i == 29){
			printf("ERROR: could not find pid in procs list for mbox");
			exitsim();	
			break;
		}	
		i++;
	}

	mboxes[handle].procs[i] = -1;
	i = 0;
	while (mboxes[handle].procs[i] == -1){
		if (i == 29){
			mboxes[handle].inuse = false;
			break; 
		}
		i++;
	}

	LockHandleRelease(mboxes[handle].lock);
	return MBOX_SUCCESS;
}

//-------------------------------------------------------
//
// int MboxSend(mbox_t handle,int length, void* message);
//
// Send a message (pointed to by "message") of length
// "length" bytes to the specified mailbox.  Messages of
// length 0 are allowed.  The call 
// blocks when there is not enough space in the mailbox.
// Messages cannot be longer than MBOX_MAX_MESSAGE_LENGTH.
// Note that the calling process must have opened the 
// mailbox via MboxOpen.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//-------------------------------------------------------
int MboxSend(mbox_t handle, int length, void* message) {
	Link * l;
	int pid = GetCurrentPid();
	int i = 0;
	uint32 intrval;
	LockHandleAcquire(mboxes[handle].lock);
	// check that pid is in list of procs using mbox
	while ( mboxes[handle].procs[i] != pid){
		printf("procs[i] = %d ||| currentpid is %d\n", mboxes[handle].procs[i], pid);
		if (i == 29){
			printf("currentpid was not on procs list\n");
			exitsim();
			break;
		}
		i++;
	}
	
	if (AQueueLength(&mboxes[handle].msg_queue) >= MBOX_MAX_BUFFERS_PER_MBOX){	
		CondHandleWait(mboxes[handle].notfull);
		LockHandleAcquire(mboxes[handle].lock);
	}
	
	intrval = DisableIntrs();
	for (i = 0; i < MBOX_NUM_BUFFERS; i++){
		if (messages[i].inuse == false){
			messages[i].inuse = true;
			break;
		}
	}
	RestoreIntrs(intrval);

	
	if (i == MBOX_NUM_BUFFERS) return MBOX_FAIL;
	bcopy(message, messages[i].buffer, length);
	messages[i].length = length;
	if ((l = AQueueAllocLink ((void *)&messages[i])) == NULL) {
      	printf("FATAL ERROR: could not allocate link for message queue in mboxsend!\n");
      	exitsim();
    }
    if (AQueueInsertLast (&mboxes[handle].msg_queue, l) != QUEUE_SUCCESS) {
      	printf("FATAL ERROR: could not insert new link into mbox queue in mboxsend!\n");
      	exitsim();
    }
	if (AQueueLength(&mboxes[handle].msg_queue) == 1){
		CondHandleSignal(mboxes[handle].not_empty);
	}
	LockHandleRelease(mboxes[handle].lock);	
  	return MBOX_SUCCESS;
}

//-------------------------------------------------------
//
// int MboxRecv(mbox_t handle, int maxlength, void* message);
//
// Receive a message from the specified mailbox.  The call 
// blocks when there is no message in the buffer.  Maxlength
// should indicate the maximum number of bytes that can be
// copied from the buffer into the address of "message".  
// An error occurs if the message is larger than maxlength.
// Note that the calling process must have opened the mailbox 
// via MboxOpen.
//   
// Returns MBOX_FAIL on failure.
// Returns number of bytes written into message on success.
//
//-------------------------------------------------------
int MboxRecv(mbox_t handle, int maxlength, void* message) {
  	Link * l;
	int pid = GetCurrentPid();
	int i = 0;
	mbox_message * inboundmsg;

	LockHandleAcquire(mboxes[handle].lock);
	// check that pid is in list of procs using mbox
	while ( mboxes[handle].procs[i] != pid){
		if (i == 29){
			printf("currentpid was not on procs list mboxrecv\n");
			exitsim();
			break;
		}
		i++;
	}
	if (AQueueLength(&mboxes[handle].msg_queue) == 0){
		CondHandleWait(mboxes[handle].not_empty);
		LockHandleAcquire(mboxes[handle].lock);
	}
	l = AQueueFirst(&mboxes[handle].msg_queue);
	inboundmsg = (mbox_message *)AQueueObject(l);
	if (inboundmsg->length > maxlength){
		printf("there was an issue with maxlength v message length\n");
		return MBOX_FAIL;
	}
	bcopy(inboundmsg->buffer, message, inboundmsg->length);
	if (AQueueLength(&mboxes[handle].msg_queue) == MBOX_MAX_BUFFERS_PER_MBOX){
		CondHandleSignal(mboxes[handle].notfull);
	}
	LockHandleRelease(mboxes[handle].lock);
  	return MBOX_SUCCESS;
}

//--------------------------------------------------------------------------------
// 
// int MboxCloseAllByPid(int pid);
//
// Scans through all mailboxes and removes this pid from their "open procs" list.
// If this was the only open process, then it makes the mailbox available.  Call
// this function in ProcessFreeResources in process.c.
//
// Returns MBOX_FAIL on failure.
// Returns MBOX_SUCCESS on success.
//
//--------------------------------------------------------------------------------
int MboxCloseAllByPid(int pid) {
  	int i;
	int q;
	int notused = 0;
	
	for (i = 0; i < MBOX_NUM_MBOXES; i++){
		LockHandleAcquire(mboxes[i].lock);
		for (q = 0; q < 30; q++){
			if (mboxes[i].procs[q] == pid){
				mboxes[i].procs[q] = -1;
				notused++;
			}
			else if (mboxes[i].procs[q] == -1){
				notused++;
			}
		}
		if (notused == 30){ // no one using this mailbox
			//make mailbox available
			mboxes[i].inuse = false;
		}
		LockHandleRelease(mboxes[i].lock);
		notused = 0;
	}	  
	return MBOX_SUCCESS;
}
