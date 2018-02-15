#include "stdafx.h"
#include "Common.h"

#include "MFT.h"
#include "Attributes.h"
#include "Other.h"
#include "FixList.h"
#include "Heap.h"
#include "Fetch.h"
#include "DataRun.h"

unsigned long long LoadMFT(std::shared_ptr<DiskHandle> disk)
{
	if (disk == nullptr)
		return -1;

	if (disk->type != kNTFSDisk)
		return -1;

	auto read = 0UL;
	auto buffer = new unsigned char[disk->ntfs.bytes_per_cluster];
	auto record_header = (FileRecordHeader *)(buffer);

	ULARGE_INTEGER mft_offset = disk->ntfs.mft_location;

	SetFilePointer(disk->handle, mft_offset.LowPart, (long *)&mft_offset.HighPart, FILE_BEGIN);
	ReadFile(disk->handle, buffer, disk->ntfs.bytes_per_cluster, &read, NULL);

	FixFileRecord(record_header);

	if (record_header->ntfs.type != 'ELIF')
		return -1;

	auto dataAttribute = (NonResidentAttribute *)FindFirstAttribute(record_header, kData);
	if (dataAttribute == nullptr)
		return -1;

	disk->ntfs.size_mft = (unsigned long)dataAttribute->data_size;
	disk->ntfs.entry_count = disk->ntfs.size_mft / disk->ntfs.bytes_per_file_record;
	disk->ntfs.mft = buffer;

	return dataAttribute->data_size;
}

unsigned long ParseMFT(std::shared_ptr<DiskHandle> disk, FetchOption option)
{
	if (disk == nullptr)
		return -1;

	if (disk->type != kNTFSDisk)
		return -1;

	fetchCallback fetch = nullptr;

	CreateFixList();

	switch (option)
	{
	case FetchOption::kSearchInfo:
		disk->is_long = sizeof(SearchFileInfo);
		fetch = FetchSearchInfo;
		break;
	default:
	case FetchOption::kStandardInfo:
		disk->is_long = sizeof(FileInfo);
		fetch = FetchFileInfo;
		break;
	}

	auto file_header = (FileRecordHeader*)(disk->ntfs.mft);
	FixFileRecord(file_header);

	if (disk->heap_block == nullptr)
		disk->heap_block = CreateHeap(0x100000);
	auto data_attribute = (NonResidentAttribute *)FindFirstAttribute(file_header, kData);

	if (data_attribute != nullptr)
	{
		auto buffer = new unsigned char[kClusterPerRead * disk->ntfs.bytes_per_cluster];
		ReadMFTParse(disk, data_attribute, 0, unsigned long(data_attribute->high_vcn) + 1, buffer, fetch);
		delete buffer;
	}

	ProcessFixList(disk);
	return 0;
}

unsigned long ReadMFTLCN(std::shared_ptr<DiskHandle> disk, unsigned long long  lcn, unsigned long count, unsigned char* buffer, fetchCallback fetch)
{
	LARGE_INTEGER offset;

	unsigned long read = 0;
	unsigned long current_cluster = 0, current_byte = 0;

	offset.QuadPart = lcn * disk->ntfs.bytes_per_cluster;
	SetFilePointer(disk->handle, offset.LowPart, &offset.HighPart, FILE_BEGIN);

	count /= kClusterPerRead;

	for (unsigned int i = 1; i <= count; i++)
	{
		ReadFile(disk->handle, buffer, kClusterPerRead * disk->ntfs.bytes_per_cluster, &read, NULL);

		current_cluster += kClusterPerRead;
		current_byte += read;

		// Get current offset
		offset.HighPart = 0;
		offset.LowPart = SetFilePointer(disk->handle, 0, &offset.HighPart, FILE_CURRENT);

		ProcessBuffer(disk, buffer, read, fetch);

		// Set current offset
		SetFilePointer(disk->handle, offset.LowPart, &offset.HighPart, FILE_BEGIN);
	}

	ReadFile(disk->handle, buffer, (count - current_cluster)*disk->ntfs.bytes_per_cluster, &read, NULL);
	ProcessBuffer(disk, buffer, read, fetch);

	current_byte += read;
	return current_byte;
}

unsigned long ReadMFTParse(std::shared_ptr<DiskHandle> disk, NonResidentAttribute * attr, unsigned long long vcn, unsigned long count, unsigned char *buffer, fetchCallback fetch)
{
	unsigned long long  lcn, runcount;
	unsigned long readcount, left;
	unsigned long ret = 0;
	unsigned char *bytes = (unsigned char *)(buffer);
	unsigned char *data = new unsigned char[disk->ntfs.entry_count*disk->is_long];

	memset(data, 0, disk->ntfs.entry_count*disk->is_long);
	disk->long_info = (FileInfo *)data;

	for (left = count; left > 0; left -= readcount)
	{
		FindRun(attr, vcn, &lcn, &runcount);
		readcount = unsigned long(min(runcount, left));
		unsigned long n = readcount * disk->ntfs.bytes_per_cluster;

		if (lcn == 0)
			memset(bytes, 0, n);
		else
			ret += ReadMFTLCN(disk, lcn, readcount, buffer, fetch);

		vcn += readcount;
		bytes += n;
	}
	return ret;
}