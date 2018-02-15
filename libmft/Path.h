#ifndef __PATH_H__
#define __PATH_H__
#include "Files.h"
#include "Disk.h"

const wchar_t* GetFileName(FileRecordHeader* file);
wchar_t* GetPath(std::shared_ptr<DiskHandle> disk, int id);
wchar_t* GetCompletePath(std::shared_ptr<DiskHandle> disk, int id);
#endif // !__PATH_H__

