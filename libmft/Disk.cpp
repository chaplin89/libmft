#include "stdafx.h"
#include <memory>

#include "Disk.h"
#include "Heap.h"
#include "MFT.h"

int Disk::OpenDisk(wchar_t dos_device)
{
	wchar_t path[] = { L'\\', L'\\', L'.', L'\\', dos_device, L':', L'\0' };

	int error = OpenDisk(path);

	if (disk_ != nullptr)
		disk_->dos_device = dos_device;

	return error;
}

int Disk::OpenDisk(const wchar_t* diskPath)
{
	disk_ = std::shared_ptr<DiskHandle>(new DiskHandle());

	unsigned long read;
	memset(&*disk_, 0, sizeof(DiskHandle));

	disk_->handle = CreateFileW(diskPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (disk_->handle == INVALID_HANDLE_VALUE)
		return -1;

	ReadFile(disk_->handle, &disk_->ntfs.boot_sector, sizeof(BootBlock), &read, NULL);

	if (read != sizeof(BootBlock))
		return -1;

	if (strncmp("NTFS", (const char*)&disk_->ntfs.boot_sector.format, 4) != 0)
	{
		disk_->type = kUnknown;
		disk_->long_info = nullptr;
		return -1;
	}

	disk_->type = kNTFSDisk;
	disk_->ntfs.bytes_per_cluster = disk_->ntfs.boot_sector.bytes_per_sector * disk_->ntfs.boot_sector.sectors_per_cluster;

	if (disk_->ntfs.boot_sector.clusters_per_file_record < 0x80)
		disk_->ntfs.bytes_per_file_record = disk_->ntfs.boot_sector.clusters_per_file_record * disk_->ntfs.bytes_per_cluster;
	else
		disk_->ntfs.bytes_per_file_record = 1 << (0x100 - disk_->ntfs.boot_sector.clusters_per_file_record);

	disk_->ntfs.mft_location.QuadPart = disk_->ntfs.boot_sector.mft_start_lcn * disk_->ntfs.bytes_per_cluster;
	disk_->ntfs.mft = nullptr;
	disk_->heap_block = nullptr;
	disk_->is_long = FALSE;
	disk_->ntfs.size_mft = 0;
	return 0;
};

int Disk::CloseDisk()
{
	if (disk_ == nullptr)
		return 0;

	if (disk_->handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(disk_->handle);
		disk_->handle = INVALID_HANDLE_VALUE;
	}

	if (disk_->type == kNTFSDisk)
	{
		if (disk_->ntfs.mft != nullptr)
			delete disk_->ntfs.mft;
		disk_->ntfs.mft = nullptr;
		if (disk_->ntfs.bitmap != nullptr)
			delete disk_->ntfs.bitmap;
		disk_->ntfs.bitmap = nullptr;
	}

	if (disk_->heap_block != nullptr)
	{
		FreeHeap(disk_->heap_block);
		disk_->heap_block = nullptr;
	}

	if (disk_->is_long)
	{
		if (disk_->long_info != nullptr)
			delete disk_->long_info;
		disk_->long_info = nullptr;
	}
	else
	{
		if (disk_->short_info != nullptr)
			delete disk_->short_info;
		disk_->short_info = nullptr;
	}

	return 0;
};

bool Disk::ReparseDisk(FetchOption option)
{
	if (disk_ == nullptr)
		return false;

	if (disk_->type == kNTFSDisk)
	{
		if (disk_->ntfs.mft != nullptr)
			delete disk_->ntfs.mft;

		disk_->ntfs.mft = nullptr;
		if (disk_->ntfs.bitmap != nullptr)
			delete disk_->ntfs.bitmap;

		disk_->ntfs.bitmap = nullptr;
	}

	if (disk_->heap_block != nullptr)
		ReUseBlocks(disk_->heap_block, FALSE);

	if (disk_->is_long)
	{
		if (disk_->long_info != nullptr)
			delete disk_->long_info;

		disk_->long_info = nullptr;
	}
	else
	{
		if (disk_->short_info != nullptr)
			delete disk_->short_info;

		disk_->short_info = nullptr;
	}

	disk_->files_size = 0;
	disk_->real_files = 0;

	if (LoadMFT(disk_) != 0)
		ParseMFT(disk_, option);

	return true;
}

std::shared_ptr<DiskHandle> Disk::GetDisk()
{
	return disk_;
}

Disk::~Disk()
{
	CloseDisk();
}