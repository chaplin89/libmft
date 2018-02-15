#ifndef __FIXLIST_H__
#define __FIXLIST_H__
#include <memory>

#include "Disk.h"

struct LinkItem
{
	unsigned int data;
	unsigned int entry;
	LinkItem *next;
};

void AddToFixList(int entry, int data);
void CreateFixList();
void ProcessFixList(std::shared_ptr<DiskHandle> disk);
#endif // !__FIXLIST_H__