#ifndef __ATTRIBUTES_H__
#define __ATTRIBUTES_H__
#include <Windows.h>
#include <memory>

#include "Disk.h"
#include "Files.h"

/// <summary> 
/// Type of the attribute.
/// </summary>
/// <remarks>
/// Depending on the type, some info can be accessed using the right 
/// structure.
/// </remarks>
enum AttributeType
{
	kStandardInformation = 0x10,
	kAttributeList = 0x20,
	kFileName = 0x30,
	kObjectId = 0x40,
	kSecurityDescripter = 0x50,
	kVolumeName = 0x60,
	kVolumeInformation = 0x70,
	kData = 0x80,
	kIndexRoot = 0x90,
	kIndexAllocation = 0xA0,
	kBitmap = 0xB0,
	kReparsePoint = 0xC0,
	kEAInformation = 0xD0,
	kEA = 0xE0,
	kPropertySet = 0xF0,
	kLoggedUtilityStream = 0x100
};

/// <summary> 
/// Common set of information that are present wether
/// the attribute is resident or non-resident.
/// </summary>
typedef struct
{
	// Type of the attribute
	AttributeType type;
	// Lenght of the attribute
	unsigned long length;
	// True if attribute is "non-resident" type.
	unsigned char non_resident;
	// Lenght of the name of the attribute
	unsigned char name_length;
	// Offset counted starting from the beginning
	unsigned short name_offset;
	// 0x001 = Compressed
	unsigned short flags;
	// Attribute index
	unsigned short attribute_number;
} Attribute;

/// <summary> 
/// Extend the Attribute structure adding information that are present
/// only in resident attributes.
/// </summary>
typedef struct
{
	// Attribute information
	Attribute attribute;
	// Lenght
	unsigned long value_length;
	// Offset counted starting from the attribute
	unsigned short value_offset;
	// 0x0001 Indexed
	unsigned short flags;
} ResidentAttribute;

/// <summary> 
/// Extend the Attribute structure adding information that are present
/// only in non-resident attributes.
/// </summary>
typedef struct
{
	Attribute attribute;

	// Virtual cluster number
	unsigned long long low_vcn;
	unsigned long long high_vcn;

	unsigned short run_array_offset;
	unsigned char compression_unit;
	unsigned char aligment_or_reserved[5];
	unsigned long long allocated_size;
	unsigned long long data_size;
	unsigned long long initialized_size;
	unsigned long long compressed_size;
} NonResidentAttribute;

/// <summary> 
/// </summary>
/// <remarks> 
/// Belong to a non-resident attribute that has type == kAttributeList
/// </remarks>
typedef struct
{
	AttributeType attribute;
	unsigned short length;
	unsigned char name_length;
	unsigned short name_offset;
	unsigned long long low_vcn;
	unsigned long long file_reference_number;
	unsigned short attribute_number;
	unsigned short aligment_or_reserved[3];
} AttributeList;

/// <summary> 
/// </summary>
/// <remarks> 
/// Belong to a resident attribute that has type == kFileName.
/// </remarks>
typedef struct
{
	// MFT Index of a directory
	unsigned long long directory_file_reference_number;
	//Updated when filename changes
	FILETIME creation_time;
	// Updated when the file changes
	FILETIME change_time;
	// Updated when file is written
	FILETIME last_write_time;
	// Updated when file is accessed
	FILETIME last_access_time;

	unsigned long long allocated_size;
	unsigned long long data_size;
	unsigned long file_attributes;
	unsigned long aligment_or_reserved;

	// Lenght of the "name" field
	unsigned char name_length;
	// 0x01 Long 0x02 Short 0x00 Posix?
	unsigned char name_type;
	// Filename
	wchar_t name[1];
} FileNameAttribute;

/// <summary> 
/// </summary>
/// <remarks> 
/// Belong to a resident attribute that has type == kStandardInformation.
/// </remarks>
typedef struct
{
	FILETIME creation_time;
	FILETIME change_time;
	FILETIME last_write_time;
	FILETIME last_access_time;
	unsigned long file_attributes;
	unsigned long aligment_or_reserved_or_unknown[3];

	// NTFS 3.0+
	unsigned long quota_id;
	unsigned long security_id;
	unsigned long long quota_charge;
	long long usn;
}  StandardInformation;


Attribute *FindFirstAttribute(FileRecordHeader *file, AttributeType type);
Attribute *FindNextAttribute(Attribute *attribute, AttributeType type);

void ReadExternalAttribute(std::shared_ptr<DiskHandle> disk, NonResidentAttribute *attribute, unsigned long long virtual_cluster_number, unsigned long count, unsigned char* buffer);
void ReadAttribute(std::shared_ptr<DiskHandle> disk, Attribute *attribute, unsigned char* buffer);
#endif // !__ATTRIBUTES_H__
