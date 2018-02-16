#include "Attributes.h"
#include "DataRun.h"
#include "Other.h"

Attribute *FindFirstAttribute(FileRecordHeader* file, AttributeType type)
{
	auto first_attribute_pointer = (unsigned char *)file + file->attributes_offset;
	auto *current_attribute = (Attribute *)first_attribute_pointer;

	for (int i = 1; i < file->attributes_count; i++)
	{
		if (current_attribute->type == type)
			return current_attribute;

		if (current_attribute->type < 1 || current_attribute->type>0x100)
			break;

		// FIXME: this check sucks.
		if (current_attribute->length > 0 && current_attribute->length < file->bytes_in_use)
			current_attribute = (Attribute *)((unsigned char *)current_attribute + current_attribute->length);
		else if (current_attribute->non_resident == TRUE)
			current_attribute = (Attribute *)((unsigned char *)current_attribute + sizeof(NonResidentAttribute));
	}
	return nullptr;
}

Attribute * FindNextAttribute(Attribute * attribute, AttributeType type)
{
	if (attribute == nullptr)
		return nullptr;

	auto next_attribute_pointer = (unsigned char *)attribute + attribute->length;
	auto next_attribute = (Attribute *)next_attribute_pointer;

	while (next_attribute->type > 0 && next_attribute->type < kIndexAllocation)
	{
		if (next_attribute->type == type)
			return next_attribute;

		if (next_attribute->length == 0)
			return nullptr;

		next_attribute_pointer = (unsigned char *)next_attribute + next_attribute->length;
		next_attribute = (Attribute* )next_attribute_pointer;
	}

	return nullptr;
}

void ReadExternalAttribute(std::shared_ptr<DiskHandle> disk, NonResidentAttribute * attribute, unsigned long long vcn, unsigned long count, unsigned char *buffer)
{
	unsigned long long lcn, runcount;
	unsigned long readcount, left;

	for (left = count; left > 0; left -= readcount)
	{
		FindRun(attribute, vcn, &lcn, &runcount);
		readcount = unsigned long(min(runcount, left));
		unsigned long n = readcount * disk->ntfs.bytes_per_cluster;

		if (lcn == 0)
			memset(buffer, 0, n);
		else
			ReadClusters(disk, lcn, readcount, buffer);

		vcn += readcount;
		buffer += n;
	}
}

void ReadAttribute(std::shared_ptr<DiskHandle> disk, Attribute * attribute, unsigned char* buffer)
{
	if (attribute->non_resident)
	{
		NonResidentAttribute *non_resident_attribute = (NonResidentAttribute *)(attribute);
		ReadExternalAttribute(disk, non_resident_attribute, 0, unsigned long(non_resident_attribute->high_vcn) + 1, buffer);
	}
	else
	{
		ResidentAttribute *resident_attribute = (ResidentAttribute *)(attribute);
		memcpy(buffer, (unsigned char *)(resident_attribute)+resident_attribute->value_offset, resident_attribute->value_length);
	}
}