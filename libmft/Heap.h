#ifndef __HEAP_H__
#define __HEAP_H__

typedef struct HeapBlock
{
	unsigned char *data;
	unsigned long current;
	unsigned long size;
	HeapBlock *end;
	HeapBlock *next;
} HeapBlock;

HeapBlock* CreateHeap(unsigned long size);
int FreeHeap(HeapBlock* block);
unsigned char * AllocData(HeapBlock* block, unsigned long size);
wchar_t * AllocAndCopyString(HeapBlock* block, wchar_t * heap, unsigned long size);
int FreeAllBlocks(HeapBlock* block);
int ReUseBlocks(HeapBlock* block, int clear);
#endif // !__HEAP_H__