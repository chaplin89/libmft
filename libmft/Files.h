#ifndef __FILES_H__
#define __FILES_H__
#include <Windows.h>

typedef struct
{
	const wchar_t* file_name;
	unsigned long file_name_lenght;
	ULARGE_INTEGER parent_id;
	unsigned long inode;
	unsigned short flags;

	FILETIME ctime;
	FILETIME atime;
	FILETIME mtime;
	FILETIME rtime;

	unsigned long long file_size;
	unsigned long long alloc_file_size;
	unsigned long attributes;
	unsigned long obj_attrib;
} FileInformation;

typedef struct
{
	const wchar_t* file_name;
	unsigned short file_name_lenght;
	unsigned short flags;
	ULARGE_INTEGER parent_id;
} SearchFileInfo;

typedef struct
{
	const wchar_t* file_name;
	unsigned short file_name_lenght;
	unsigned short flags;
	ULARGE_INTEGER parent_id;
	ULARGE_INTEGER file_size;
} ShortFileInfo;

typedef struct
{
	const wchar_t* file_name;
	unsigned short file_name_lenght;
	unsigned short flags;
	ULARGE_INTEGER parent_id;

	bool has_data;
	ULARGE_INTEGER file_size;

	FILETIME creation_time;
	FILETIME access_time;
	FILETIME write_time;
	FILETIME change_time;
	ULARGE_INTEGER allocated_file_size;
	unsigned long file_attributes;
} FileInfo;

// NTFSRecordHeader
// Reference:
// 1. https://doxygen.reactos.org/d9/d95/structNTFS__RECORD__HEADER.html
typedef struct
{
	// Type: 'FILE' 'INDX' 'BAAD' 'HOLE' 'CHKD'
	unsigned long type;

	// Update sequence array
	unsigned short usa_offset; 
	unsigned short usa_count;

	// Update sequence number
	USN usn;
} NTFSRecordHeader;

// FileRecordHeader
// Reference: 
// 1. https://flatcap.org/linux-ntfs/ntfs/concepts/file_record.html
// 2. https://doxygen.reactos.org/de/d12/struct__FILE__RECORD__HEADER.html
typedef struct
{
	NTFSRecordHeader ntfs;
	unsigned short sequence_number;
	unsigned short link_count;
	// Offset of the first attribute from the begin of this structure
	unsigned short attributes_offset;
	// 0x0001 InUse; 0x0002 Directory
	unsigned short flags; 
	unsigned long bytes_in_use;
	unsigned long bytes_allocated;
	ULARGE_INTEGER base_file_record;
	unsigned short attributes_count;
} FileRecordHeader;

#endif // !__FILES_H__
