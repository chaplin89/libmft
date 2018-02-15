#ifndef __BOOTBLOCK_H__
#define __BOOTBLOCK_H__

#pragma pack(push,1)
typedef struct
{
	unsigned char jump[3];
	unsigned char format[8];
	unsigned short bytes_per_sector;
	unsigned char sectors_per_cluster;
	unsigned short boot_sectors;
	unsigned char mbz_1;
	unsigned short mbz_2;
	unsigned short reserved_1;
	unsigned char media_type;
	unsigned short mbz_3;
	unsigned short sectors_per_track;
	unsigned short number_of_heads;
	unsigned long partition_offset;
	unsigned long reserved_2[2];
	unsigned long long total_sectors;
	unsigned long long mft_start_lcn;
	unsigned long long mft_2_start_lcn;
	unsigned long clusters_per_file_record;
	unsigned long clusters_per_index_block;
	unsigned long long volume_serial_number;
	unsigned char code[0x1AE];
	unsigned short boot_signature;
} BootBlock;
#pragma pack(pop)

#endif // !__BOOTBLOCK_H__