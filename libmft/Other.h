#ifndef __Other_H__
#define __Other_H__
#include <memory>

#include "Common.h"
#include "Attributes.h"
#include "Disk.h"
#include "Fetch.h"

// TODO: clean this s*it

enum
{
	kPOSIXName = 0,
	kWIN32Name = 1,
	kDOSName = 2,
	kWIN32DOSName = 3
};

bool FixFileRecord(FileRecordHeader* file);

unsigned long ReadClusters(std::shared_ptr<DiskHandle> disk, unsigned long long lcn, unsigned long count, unsigned char * buffer);
unsigned long ReadExternalData(std::shared_ptr<DiskHandle> disk, NonResidentAttribute * attr, unsigned long long vcn, unsigned long count, unsigned char * buffer);
unsigned long ReadData(std::shared_ptr<DiskHandle> disk, Attribute * attr, unsigned char * buffer);

unsigned long ReadMFTLCN(std::shared_ptr<DiskHandle> disk, unsigned long long lcn, unsigned long count, unsigned char* buffer, fetchCallback fetch);
unsigned long ProcessBuffer(std::shared_ptr<DiskHandle> disk, unsigned char * buffer, unsigned long size, fetchCallback fetch);
#endif // !__Other_H__
