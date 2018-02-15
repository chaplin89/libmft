#include "stdafx.h"

#include "FixList.h"
#include "Other.h"
#include "DataRun.h"
#include "Heap.h"
#include "Attributes.h"

bool FixFileRecord(FileRecordHeader* file)
{
	unsigned char* update_sequence_array_pointer = (unsigned char *)file + file->ntfs.usa_offset;
	unsigned short* sector = (unsigned short*)file;

	unsigned short* update_sequence_array = (unsigned short*)update_sequence_array_pointer;

	if (file->ntfs.usa_count > 4)
		return false;

	for (unsigned long i = 1; i < file->ntfs.usa_count; i++)
	{
		sector[255] = update_sequence_array[i];
		sector += 256;
	}

	return true;
}

void ReadVCN(std::shared_ptr<DiskHandle> disk, FileRecordHeader* file, AttributeType type, unsigned long long vcn, unsigned long count, unsigned char* buffer)
{
	NonResidentAttribute * non_resident = nullptr;

	//TODO

	if (non_resident == nullptr || (vcn < non_resident->low_vcn || vcn > non_resident->high_vcn))
		return;

	ReadExternalAttribute(disk, non_resident, vcn, count, buffer);
}

unsigned long ReadData(std::shared_ptr<DiskHandle> disk, Attribute * attribute, unsigned char * buffer)
{
	if (attribute->non_resident)
	{
		auto non_resident_attribute = (NonResidentAttribute *)attribute;
		return ReadExternalData(disk, non_resident_attribute, 0, unsigned long(non_resident_attribute->high_vcn) + 1, buffer);
	}
	else
	{
		auto resident_attribute = (ResidentAttribute *)attribute;
		auto begin_pointer = (unsigned char *)(resident_attribute)+resident_attribute->value_offset;
		memcpy(buffer, begin_pointer, resident_attribute->value_length);
	}

	return ~0;
}

unsigned long ReadExternalData(std::shared_ptr<DiskHandle> disk, NonResidentAttribute * attribute, unsigned long long  vcn, unsigned long count, unsigned char * buffer)
{
	unsigned long long lcn, runcount;
	unsigned long readcount;
	unsigned long ret = 0;

	for (unsigned long left = count; left > 0; left -= readcount)
	{
		FindRun(attribute, vcn, &lcn, &runcount);
		readcount = unsigned long(min(runcount, left));

		unsigned long n = readcount * disk->ntfs.bytes_per_cluster;

		if (lcn == 0)
			memset(buffer, 0, n);
		else
			ret += ReadClusters(disk, lcn, readcount, buffer);

		vcn += readcount;
		buffer += n;
	}
	return ret;
}

unsigned long ReadClusters(std::shared_ptr<DiskHandle> disk, unsigned long long  lcn, unsigned long count, unsigned char *buffer)
{
	LARGE_INTEGER offset;

	unsigned long read = 0;
	unsigned long iterations = 0, current_cluster = 0, current_byte = 0;

	offset.QuadPart = lcn*disk->ntfs.bytes_per_cluster;
	SetFilePointer(disk->handle, offset.LowPart, &offset.HighPart, FILE_BEGIN);

	iterations = count / kClusterPerRead;

	for (unsigned int i = 1; i <= iterations; i++)
	{
		ReadFile(disk->handle, &buffer[current_byte], kClusterPerRead * disk->ntfs.bytes_per_cluster, &read, NULL);

		current_cluster += kClusterPerRead;
		current_byte += read;
	}

	ReadFile(disk->handle, &buffer[current_byte], (count - current_cluster)*disk->ntfs.bytes_per_cluster, &read, NULL);
	current_byte += read;

	return current_byte;
}

unsigned long ProcessBuffer(std::shared_ptr<DiskHandle> disk, unsigned char * buffer, unsigned long size, fetchCallback fetch)
{
	unsigned char * end = buffer + size;
	unsigned char * data = (unsigned char *)disk->long_info + disk->is_long * disk->files_size;

	while (buffer < end)
	{
		auto record_header = (FileRecordHeader *)buffer;

		FixFileRecord(record_header);
		if (fetch(disk, record_header, data) > 0)
			disk->real_files++;

		buffer += disk->ntfs.bytes_per_file_record;
		data += disk->is_long;
		disk->files_size++;
	}
	return 0;
}