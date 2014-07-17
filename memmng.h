#define CAN_FREE 1
#define DONT_FREE 0
#include "memmng.c"

void *MM_FindPtr(void *ptr);
void MM_FreePtr(void *ptr);
void *MM_GetPtr(unsigned length, unsigned attributes);
void MM_SortMem();
void DisplayP();
void MM_ChangePurge(void *ptr);