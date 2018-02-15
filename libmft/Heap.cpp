#include "Heap.h"
#include <Windows.h>

HeapBlock* CreateHeap(unsigned long size)
{
	HeapBlock *tmp;
	tmp = (HeapBlock *)malloc(sizeof(HeapBlock));
	tmp->current = 0;
	tmp->size = size;
	tmp->next = nullptr;
	tmp->data = (unsigned char *)malloc(size);
	if (tmp->data != nullptr)
	{
		tmp->end = tmp;
		return tmp;
	}
	else
	{
		free(tmp);
		return nullptr;
	}
}

int FreeHeap(HeapBlock *block)
{
	if (block != nullptr)
	{
		FreeAllBlocks(block);
		return TRUE;
	}
	return FALSE;
}

wchar_t * AllocAndCopyString(HeapBlock* block, wchar_t * string, unsigned long size)
{
	HeapBlock *tmp, *back = nullptr;
	unsigned char * ret = nullptr;
	unsigned int t;
	unsigned int asize;
	unsigned long rsize = (size + 1) * sizeof(wchar_t);
	asize = ((rsize) & 0xfffffff8) + 8;

	if (asize <= rsize) 
		DebugBreak();

	tmp = block->end;

	if (tmp != nullptr)
	{
		t = tmp->size - tmp->current;
		if (t > asize)
			goto copy;
		back = tmp;


		tmp = tmp->next;
		if (tmp != nullptr)
		{
			t = tmp->size - tmp->current;
			if (t > asize)
			{
				block->end = tmp;
				goto copy;
			}

			back = tmp;
			tmp = tmp->next;
		}
	}
	tmp = (HeapBlock*)malloc(sizeof(HeapBlock));
	memset(tmp, 0, sizeof(HeapBlock));
	tmp->data = (unsigned char *)malloc(block->size);
	if (tmp->data != nullptr)
	{
		tmp->size = block->size;
		tmp->next = nullptr;

		if (back == nullptr)
			back = block->end;

		tmp->end = block;
		back->next = tmp;
		block->end = tmp;
		goto copy;
	}
	else
	{
		DebugBreak();
		return nullptr;
	}

copy:
	ret = &tmp->data[tmp->current];
	memcpy(ret, string, rsize);
	ret[rsize] = 0;
	ret[rsize + 1] = 0;
	tmp->current += asize;
	return (wchar_t *)ret;
}

unsigned char * AllocData(HeapBlock* block, unsigned long size)
{
	HeapBlock *tmp, *back;
	unsigned char * ret = nullptr;
	int t;

	tmp = block->end;
	if (tmp != nullptr)
	{
		t = tmp->size - tmp->current;
		if (t > (int)size)
		{
			ret = &tmp->data[tmp->current];
			tmp->current += size;
			return ret;
		}
		back = tmp;
	}
	tmp = (HeapBlock*)malloc(sizeof(HeapBlock));
	tmp->data = (unsigned char *)malloc(block->size);
	if (tmp->data != nullptr)
	{
		tmp->current = size;
		tmp->size = block->size;
		tmp->next = nullptr;

		back = block->end;
		tmp->end = block;
		back->next = tmp;
		block->end = tmp;

		return tmp->data;
	}
	else
		free(tmp);
	return nullptr;
}

int FreeAllBlocks(HeapBlock* block)
{
	HeapBlock *tmp, *back;
	tmp = block;

	while (tmp != nullptr)
	{
		free(tmp->data);
		back = tmp;
		tmp = tmp->next;
		free(back);
	}

	return TRUE;
}

int ReUseBlocks(HeapBlock* block, int clear)
{
	if (block != nullptr)
	{
		HeapBlock* tmp;
		tmp = block;
		while (tmp != nullptr)
		{
			tmp->current = 0;
			tmp = tmp->next;
			if (clear) memset(tmp->data, 0, tmp->size * sizeof(wchar_t));
		}
		block->end = block;
	}
	return TRUE;
}