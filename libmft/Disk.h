#ifndef __DISK_H__
#define __DISK_H__
#include <Windows.h>
#include <memory>

#include "Common.h"
#include "BootBlock.h"
#include "Disk.h"
#include "Files.h"
#include "Heap.h"

enum FSType
{
	kNTFSDisk = 1,
	kFAT32Disk = 2,
	kFatDisk = 4,
	kEXT2 = 8,
	kUnknown = 0xff99ff99
};

typedef struct
{
	HANDLE handle;
	unsigned long type;
	unsigned long is_long;
	unsigned long files_size;
	unsigned long real_files;
	wchar_t dos_device;
	HeapBlock* heap_block;

	union
	{
		struct
		{
			BootBlock boot_sector;
			unsigned long bytes_per_file_record;
			unsigned long bytes_per_cluster;
			unsigned long size_mft;
			unsigned long entry_count;

			// Ofset from the beginning
			ULARGE_INTEGER mft_location;

			unsigned char *mft;
			unsigned char *bitmap;
		} ntfs;

		struct
		{
			unsigned long fat;
		} FAT;

		union
		{
			FileInfo *long_info;
			ShortFileInfo *short_info;
			SearchFileInfo *search_info;
		};
	};
} DiskHandle;

class Disk 
{
	std::shared_ptr<DiskHandle> disk_;
public:
	int OpenDisk(const wchar_t* disk);
	int OpenDisk(wchar_t dos_device);
	int CloseDisk();
	bool ReparseDisk(FetchOption option);

	std::shared_ptr<DiskHandle> GetDisk();

	~Disk();
};
#endif // !__DISK_H__
