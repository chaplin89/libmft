#include "stdafx.h"
#include "FixList.h"

LinkItem *fixlist = nullptr;
LinkItem *curfix = nullptr;

void AddToFixList(int entry, int data)
{
	curfix->entry = entry;
	curfix->data = data;
	curfix->next = new LinkItem;
	curfix = curfix->next;
	curfix->next = nullptr;
}

void CreateFixList()
{
	fixlist = new LinkItem;
	fixlist->next = nullptr;
	curfix = fixlist;
}

void ProcessFixList(std::shared_ptr<DiskHandle> disk)
{
	SearchFileInfo *info, *src;
	while (fixlist->next != nullptr)
	{
		info = &disk->search_info[fixlist->entry];
		src = &disk->search_info[fixlist->data];
		info->file_name = src->file_name;
		info->file_name_lenght = src->file_name_lenght;

		info->parent_id = src->parent_id;

		src->parent_id.QuadPart = 0;
		LinkItem *item;
		item = fixlist;
		fixlist = fixlist->next;
		delete item;
	}
	fixlist = nullptr;
	curfix = nullptr;
}