//
//	memory.c
//
//	Routines for dealing with memory management.

//static char rcsid[] = "$Id: memory.c,v 1.1 2000/09/20 01:50:19 elm Exp elm $";

#include "ostraps.h"
#include "dlxos.h"
#include "process.h"
#include "memory.h"
#include "queue.h"

// num_pages = size_of_memory / size_of_one_page
static uint32 freemap[MEM_MAX_PAGES / 32];
static int page_refcounters[MEM_MAX_PAGES];
static uint32 pagestart;
static int nfreepages;
static int freemapmax;

//----------------------------------------------------------------------
//
//	This silliness is required because the compiler believes that
//	it can invert a number by subtracting it from zero and subtracting
//	an additional 1.  This works unless you try to negate 0x80000000,
//	which causes an overflow when subtracted from 0.  Simply
//	trying to do an XOR with 0xffffffff results in the same code
//	being emitted.
//
//----------------------------------------------------------------------
static int negativeone = 0xFFFFFFFF;
static inline uint32 invert (uint32 n) {
  return (n ^ negativeone);
}

//----------------------------------------------------------------------
//
//	MemoryGetSize
//
//	Return the total size of memory in the simulator.  This is
//	available by reading a special location.
//
//----------------------------------------------------------------------
int MemoryGetSize() {
  return (*((int *)DLX_MEMSIZE_ADDRESS));
}


//----------------------------------------------------------------------
//
//	MemoryModuleInit
//
//	Initialize the memory module of the operating system.
//      Basically just need to setup the freemap for pages, and mark
//      the ones in use by the operating system as "VALID", and mark
//      all the rest as not in use.
//
//----------------------------------------------------------------------
void MemoryModuleInit() {
  int i;
  freemapmax = (MEM_MAX_PAGES / 32) - 1;
  pagestart = (int)(lastosaddress / MEM_PAGESIZE) + 1;
  nfreepages = (MEM_MAX_PAGES / 32) - pagestart;
  for (i = 0; i <= freemapmax; i++){
    freemap[i] = 0;
  }
    //Q3: set os pages to 1
  for (i = 0;i < pagestart; i++){
    page_refcounters[i] = 1;
  }

  for (i = pagestart; i < MEM_MAX_PAGES; i++){
    MemorySetFreemap(i);
  }
}


//----------------------------------------------------------------------
//
// MemoryTranslateUserToSystem
//
//	Translate a user address (in the process referenced by pcb)
//	into an OS (physical) address.  Return the physical address.
//
//----------------------------------------------------------------------
uint32 MemoryTranslateUserToSystem (PCB *pcb, uint32 addr) {
  int pagenum;
  int offset;
  uint32 pte;
  if (addr > MEM_MAX_VIRTUAL_ADDRESS){
    // printf("over maxaddress in %d", GetPidFromAddress(pcb));
    ProcessKill();
  }
  pagenum = addr >> MEM_L1FIELD_FIRST_BITNUM;
  offset = addr & MEM_ADDRESS_OFFSET_MASK;
  pte = pcb->pagetable[pagenum];
  //printf("translating %d, pagenum: %d\n", addr, pagenum);
  if ((pte & 0x1) != 1){
    //printf("page not valid\n");
    pcb->currentSavedFrame[PROCESS_STACK_FAULT] = addr;
    MemoryPageFaultHandler(pcb);
    return MEM_FAIL;
  }
  // printf("stackaddress is %d\n", pcb->currentSavedFrame[PROCESS_STACK_USER_STACKPOINTER]);
  return ((pte & MEM_PTE_MASK) | offset);
}


//----------------------------------------------------------------------
//
//	MemoryMoveBetweenSpaces
//
//	Copy data between user and system spaces.  This is done page by
//	page by:
//	* Translating the user address into system space.
//	* Copying all of the data in that page
//	* Repeating until all of the data is copied.
//	A positive direction means the copy goes from system to user
//	space; negative direction means the copy goes from user to system
//	space.
//
//	This routine returns the number of bytes copied.  Note that this
//	may be less than the number requested if there were unmapped pages
//	in the user range.  If this happens, the copy stops at the
//	first unmapped address.
//
//----------------------------------------------------------------------
int MemoryMoveBetweenSpaces (PCB *pcb, unsigned char *system, unsigned char *user, int n, int dir) {
  unsigned char *curUser;         // Holds current physical address representing user-space virtual address
  int		bytesCopied = 0;  // Running counter
  int		bytesToCopy;      // Used to compute number of bytes left in page to be copied

  while (n > 0) {
    // Translate current user page to system address.  If this fails, return
    // the number of bytes copied so far.
    curUser = (unsigned char *)MemoryTranslateUserToSystem (pcb, (uint32)user);

    // If we could not translate address, exit now
    if (curUser == (unsigned char *)0) break;

    // Calculate the number of bytes to copy this time.  If we have more bytes
    // to copy than there are left in the current page, we'll have to just copy to the
    // end of the page and then go through the loop again with the next page.
    // In other words, "bytesToCopy" is the minimum of the bytes left on this page 
    // and the total number of bytes left to copy ("n").

    // First, compute number of bytes left in this page.  This is just
    // the total size of a page minus the current offset part of the physical
    // address.  MEM_PAGESIZE should be the size (in bytes) of 1 page of memory.
    // MEM_ADDRESS_OFFSET_MASK should be the bit mask required to get just the
    // "offset" portion of an address.
    bytesToCopy = MEM_PAGESIZE - ((uint32)curUser & MEM_ADDRESS_OFFSET_MASK);
    
    // Now find minimum of bytes in this page vs. total bytes left to copy
    if (bytesToCopy > n) {
      bytesToCopy = n;
    }

    // Perform the copy.
    if (dir >= 0) {
      bcopy (system, curUser, bytesToCopy);
    } else {
      bcopy (curUser, system, bytesToCopy);
    }

    // Keep track of bytes copied and adjust addresses appropriately.
    n -= bytesToCopy;           // Total number of bytes left to copy
    bytesCopied += bytesToCopy; // Total number of bytes copied thus far
    system += bytesToCopy;      // Current address in system space to copy next bytes from/into
    user += bytesToCopy;        // Current virtual address in user space to copy next bytes from/into
  }
  return (bytesCopied);
}

//----------------------------------------------------------------------
//
//	These two routines copy data between user and system spaces.
//	They call a common routine to do the copying; the only difference
//	between the calls is the actual call to do the copying.  Everything
//	else is identical.
//
//----------------------------------------------------------------------
int MemoryCopySystemToUser (PCB *pcb, unsigned char *from,unsigned char *to, int n) {
  return (MemoryMoveBetweenSpaces (pcb, from, to, n, 1));
}

int MemoryCopyUserToSystem (PCB *pcb, unsigned char *from,unsigned char *to, int n) {
  return (MemoryMoveBetweenSpaces (pcb, to, from, n, -1));
}

//---------------------------------------------------------------------
// MemoryPageFaultHandler is called in traps.c whenever a page fault 
// (better known as a "seg fault" occurs.  If the address that was
// being accessed is on the stack, we need to allocate a new page 
// for the stack.  If it is not on the stack, then this is a legitimate
// seg fault and we should kill the process.  Returns MEM_SUCCESS
// on success, and kills the current process on failure.  Note that
// fault_address is the beginning of the page of the virtual address that 
// caused the page fault, i.e. it is the vaddr with the offset zero-ed
// out.
//
// Note: The existing code is incomplete and only for reference. 
// Feel free to edit.
//---------------------------------------------------------------------
int MemoryPageFaultHandler(PCB *pcb) {
  uint32 faultaddress = pcb->currentSavedFrame[PROCESS_STACK_FAULT];
  uint32 stackaddr = pcb->currentSavedFrame[PROCESS_STACK_USER_STACKPOINTER];
  uint32 faultpagenum = faultaddress >> MEM_L1FIELD_FIRST_BITNUM;
  uint32 stackaddrpagenum = stackaddr >> MEM_L1FIELD_FIRST_BITNUM;
  uint32 newpage; 
  // printf("faultpage: %d  |||| stackpage: %d\n", faultpagenum, stackaddrpagenum);
  if (faultpagenum < stackaddrpagenum){
    printf("Segmentation Fault caused by PID: %d. Killing process.\n", GetPidFromAddress(pcb));
    ProcessKill();
  }
  else {
    printf("Allocating new page to stack for pid(%d)\n", GetPidFromAddress(pcb));
    newpage = MemoryAllocPage();
    pcb->pagetable[stackaddrpagenum] = MemorySetupPte(newpage);
  }
  return MEM_SUCCESS;
}


//---------------------------------------------------------------------
// You may need to implement the following functions and access them from process.c
// Feel free to edit/remove them
//---------------------------------------------------------------------

uint32 MemoryAllocPage(void) {
  int i;
  int bitcheck;
  uint32 mapcopy;
  int bitnum = 0;
  for (i = 0; i <= freemapmax; i++){
    if(freemap[i] != 0){
      bitnum = 0;
      mapcopy = freemap[i];
      bitcheck = mapcopy & 0x1;
      while (bitcheck == 0){
        mapcopy = mapcopy >> 1;
        bitcheck = mapcopy & 0x1;
        bitnum++;
      }
      freemap[i] = freemap[i] & invert(0x1 << bitnum);
      nfreepages--;
      //Q3 set page associated with available bit in freemap
      page_refcounters[i * 32 + bitnum] = 1;
      
      return (i * 32 + bitnum);
    }
  }
  return 0;
}


uint32 MemorySetupPte (uint32 page) {
  return ((page << MEM_L1FIELD_FIRST_BITNUM) | 0x1);
}


void MemoryFreePage(uint32 page) {
  MemorySetFreemap(page);
  nfreepages++;

}

void MemoryFreePte(uint32 pte){
  int pagenum;
  pagenum = pte >> MEM_L1FIELD_FIRST_BITNUM;

  if (page_refcounters[pagenum] < 1){
    //Kill Process Missing
    return MEM_FAIL;
  }
  else if (page_refcounters[pagenum] > 1){
    page_refcounters[pagenum] -= 1; // IS THIS RIGHT ?????
  }
  else if (page_refcounters[pagenum] == 0){
    MemoryFreePage(pagenum);
  }
}

void MemorySetFreemap(uint32 page){
  int freemapindex;
  int shiftbitnum;
  uint32 one = 0x1;
  freemapindex = page / 32;
  shiftbitnum = page % 32;
  freemap[freemapindex] = freemap[freemapindex] | (one << shiftbitnum);
}

int MemoryROPAccessHandler(PCB * pcb){

  uint32 fault_address = pcb->currentSavedFrame[PROCESS_STACK_FAULT];
  int l1_page_num = fault_address >> MEM_L1FIELD_FIRST_BITNUM;
  int phys_page_num = l1_page_num >> MEM_L1FIELD_FIRST_BITNUM;
  int pageGen;

  if (page_refcounters[phys_page_num] < 1){
    //Kill Process and return mem_fail
    return MEM_FAIL;
  }
  else if (page_refcounters[phys_page_num] == 1){
    pcb->pagetable[l1_page_num] &= invert(MEM_PTE_READONLY);
  }
  else{
    //generate new page
    pageGen = MemoryAllocPage();
    pcb->pagetable[l1_page_num] = MemorySetupPte(pageGen);
    //bcopy
    bcopy((char *)(fault_address),(char *)(pageGen * MEM_PAGESIZE),MEM_PAGESIZE);
    //setup pte for new page
    //decrease ref_count
    page_refcounters[phys_page_num] -= 1;
  }
  return MEM_SUCCESS;
}

void IncreaseRefCount(uint32 page){
  uint32 pageNum;
  pageNum = page >> MEM_L1FIELD_FIRST_BITNUM;
  page_refcounters[pageNum] += 1;
}

int malloc(PCB * pcb, int handle){
  return 0;
}
int mfree(PCB * pcb, int * handle){
  return 0;
}