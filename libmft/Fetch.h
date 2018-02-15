#ifndef __FETCH_H__
#define __FETCH_H__
#include <memory>

#include "Disk.h"
#include "Files.h"

typedef unsigned long(__cdecl *fetchCallback)(std::shared_ptr<DiskHandle>, FileRecordHeader*, unsigned char *);
unsigned long FetchFileInfo(std::shared_ptr<DiskHandle> disk, FileRecordHeader* record_header, unsigned char * buf);
unsigned long FetchSearchInfo(std::shared_ptr<DiskHandle> disk, FileRecordHeader* record_header, unsigned char * data);
#endif // !__FETCH_H__
