#ifndef __ATTRIBUTES_H__
#define __ATTRIBUTES_H__
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
	// Use StandardInformationAttribute
	kStandardInformation = 0x10,
	// Use AttributeList
	kAttributeList = 0x20,
	// Use FileNameAttribute
	kFileName = 0x30,
	// Use ObjectIDAttribute, NTFS 3.0+ -> before was VolumeVersion
	kObjectID = 0x40,
	// Use SecurityDescriptorAttribute
	kSecurityDescripter = 0x50,
	// Use VolumeNameAttribute
	kVolumeName = 0x60,
	// Use VolumeInformationAttribute
	kVolumeInformation = 0x70,
	// Use DataAttribute
	kData = 0x80,
	// Use IndexRootAttribute
	kIndexRoot = 0x90,
	// Use IndexAllocationAttribute
	kIndexAllocation = 0xA0,
	// Use BitmapAttribute
	kBitmap = 0xB0,
	// Use ReparsePointAttribute, NTFS 3.0+ -> before was SymbolicLink
	kReparsePoint = 0xC0,
	// Use EAInformationAttribute
	kEAInformation = 0xD0,
	// Use EAAttribute
	kEA = 0xE0,
	// Does not exists in NTFS 3.0+
	kPropertySet = 0xF0,
	// Use LoggedUtilityStreamAttribute
	kLoggedUtilityStream = 0x100
};

/// <summary> 
/// Common set of information that are present whether
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
/// <remarks>
/// The attribute itself can be accessed using values_offset and value_lenght.
/// The offset if counted from the beginning of the attribute.
/// </remarks>
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
/// Every file is associated with a unique GUID, this attribute contains this info.
/// </summary>
/// <remarks> No minimum size, maximum size = 256 bytes. </remarks>
typedef struct
{
	// Uniquely identify the file
	unsigned char guid_object_id[16];
	// Volume where file is created
	unsigned char guid_birth_volume_id[16];
	// Original Object ID
	unsigned char guid_birth_object_id[16];
	// Maybe unused
	unsigned char guid_domain_id[16];
} ObjectIDAttribute;

/// <summary> 
/// Simply the name of the volume.
///</summary>
/// <remarks> Between 2 and 256 bytes. </remarks>
typedef struct
{
	// Unicode; Null-terminated
	wchar_t volume_name[1];
} VolumeNameAttribute;

/// <summary> 
/// The data, plain and simple.
/// </summary>
typedef struct
{
	unsigned char data_begin[1];
} DataAttribute;

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

enum Permission : unsigned long
{
	kReadOnly = 0x001,
	kHidden = 0x002,
	kSystem = 0x004,
	KArchive = 0x0020,
	kDevice = 0x0040,
	kNormal = 0x0080,
	kTemporary = 0x0100,
	kSparseFile = 0x0200,
	kReparsePointFile = 0x0400,
	kCompressed = 0x0800,
	kOffline = 0x1000,
	kNotContentIndexed = 0x2000,
	kEncrypted = 0x4000,

	kDirectory = 0x10000000,
	kIndexView = 0x20000000
};

/// <summary> 
/// Basic info that are saw in the "Property" of a file.
/// </summary>
/// <remarks> 
/// Belong to a resident attribute that has type == kFileName.
/// </remarks>
typedef struct
{
	// MFT Index of a directory
	unsigned long long parent_reference;
	//Updated when filename changes
	long long creation_time;
	// Updated when the file changes
	long long write_time;
	// Updated when file is written
	long long mft_changed_time;
	// Updated when file is accessed
	long long read_time;
	// Size on disk (multiple of clusters size)
	unsigned long long allocated_size;
	// Real size (size of the unnamed data attribute) -> Only if VCN is zero
	unsigned long long data_size;

	Permission dos_file_permission;
	unsigned long used_by_ea_and_reparse;

	// Lenght of the name, in characters (not in bytes)
	unsigned char characters_count;
	// 0x01 Long 0x02 Short 0x00 Posix?
	unsigned char name_type;
	// Filename begin
	wchar_t name[1];
} FileNameAttribute;

/// <summary> 
/// </summary>
/// <remarks> 
/// Belong to a resident attribute that has type == kStandardInformation.
/// </remarks>
typedef struct
{
	long long creation_time;
	long long write_time;
	long long mft_changed_time;
	long long read_time;

	Permission dos_file_permission;

	// NTFS 3.0+

	// Maximum version of the file or 0 to disable, see below.
	unsigned long max_version_number;
	// File's version (if any).
	unsigned long version_number;
	// Class ID from class ID index
	unsigned long class_id;
	// ID of the user that own the file.
	unsigned long owner_id;
	unsigned long security_id;
	unsigned long quota_id;
	unsigned long long quota_charge;
	// Index in $UsnJrnl file.
	long long usn;
}  StandardInformationAttribute;

Attribute *FindFirstAttribute(FileRecordHeader *file, AttributeType type);
Attribute *FindNextAttribute(Attribute *attribute, AttributeType type);

void ReadExternalAttribute(std::shared_ptr<DiskHandle> disk, NonResidentAttribute *attribute, unsigned long long virtual_cluster_number, unsigned long count, unsigned char* buffer);
void ReadAttribute(std::shared_ptr<DiskHandle> disk, Attribute *attribute, unsigned char* buffer);

#endif // !__ATTRIBUTES_H__
