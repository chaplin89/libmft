#ifndef __MFT_H__
#define __MFT_H__
#include <memory>
#include "Common.h"

#include "Fetch.h"
#include "Disk.h"
#include "Attributes.h"

///<summary> 
/// Read the MFT.
/// </summary>
unsigned long long LoadMFT(std::shared_ptr<DiskHandle> disk);

///<summary> 
/// TODO
/// </summary>
unsigned long ReadMFTParse(std::shared_ptr<DiskHandle> disk, NonResidentAttribute * attr, unsigned long long vcn, unsigned long count, unsigned char* buffer, fetchCallback fetch);

///<summary> 
/// TODO
/// </summary>
unsigned long ParseMFT(std::shared_ptr<DiskHandle> disk, FetchOption option);

///<summary> 
/// TODO
/// </summary>
unsigned long ReadMFTLCN(std::shared_ptr<DiskHandle> disk, unsigned long long  lcn, unsigned long count, unsigned char* buffer, fetchCallback fetch);

#endif // !__MFT_H__
