#ifndef __BOOTBLOCK_H__
#define __BOOTBLOCK_H__

/// <summary>
/// This is the beginning of disk.
/// Contains general information about the volume,
/// and how to load the O.S.
/// </summary>
/// <remarks>
/// This can be found at the beginning of the disk 
/// or, equivalently, following the $BOOT file info.
/// There is also a backup copy at the end of the volume.
/// </remarks>
#pragma pack(push,1)
typedef struct
{
	// Offset to the begin of the bootloader code. See code field.
	unsigned char jump[3];
	// OEM ID, i.e.: "NTFS" string
	unsigned char format[8];
	// How many bytes for one sector
	unsigned short bytes_per_sector;
	// How many bytes for one cluster
	unsigned char sectors_per_cluster;
	// Reserved sectors before the beginning of the partition (must be 0 or the mount fail)
	unsigned short reserved_sectors;
	// Must be 0 or the mount fail.
	unsigned char must_be_zero_1[3];
	// Must be 0 or the mount fail.
	unsigned short must_be_zero_2;
	// Media descriptor (0xF8 means HD)
	unsigned char media_type;
	// Must be 0 or the mount fail.
	unsigned short must_be_zero_3;
	// Sectors per track (not used)
	unsigned short sectors_per_track;
	// Number of heads (not used)
	unsigned short number_of_heads;
	// Partition offset (not used)
	unsigned long partition_offset;
	// Must be 0 or the mount fail.
	unsigned long must_be_zero_4;
	// Not used
	unsigned long unused_1;
	// Number of sectors in the volume
	unsigned long long total_sectors;
	// Logical cluster number of the virtual cluster number 0 of the file $MFT
	unsigned long long mft_start_lcn;
	// Logical cluster number of the virtual cluster number 0 of the file $MFTMirr
	unsigned long long mft_2_start_lcn;
	// Generally clusters/MFT record. If field < 0 then means 2^(-1*-field) bytes.
	char clusters_per_file_record;
	// Not used/padding
	unsigned char unused_2[3];
	// Generally clusters per index record. If field < 0 then means 2^(-1*-field) bytes.
	char clusters_per_index_block;
	// Not used/padding
	unsigned char unused_3[3];
	// Serial number 
	unsigned long long volume_serial_number;
	// Unused
	unsigned long unused_4;
	// Here follow the code
	unsigned char code[0x1AA];
	// The constant 0xAA55
	unsigned short boot_signature;
} BootBlock;
#pragma pack(pop)
#endif // !__BOOTBLOCK_H__