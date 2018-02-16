

#include "Fetch.h"
#include "Attributes.h"
#include "FixList.h"
#include "Other.h"
#include "Heap.h"

unsigned long FetchSearchInfo(std::shared_ptr<DiskHandle> disk, FileRecordHeader* file, unsigned char * buf)
{
	SearchFileInfo *data = (SearchFileInfo*)buf;
	auto attribute_pointer = (unsigned char *)(file)+file->attributes_offset;
	auto attribute = (Attribute *)attribute_pointer;

	int stop = min(8, file->attributes_count);

	if (file->ntfs.type != 'ELIF')
		return 0;

	data->flags = file->flags;

	for (int i = 0; i < stop; i++)
	{
		if (attribute->type < 0 || attribute->type>0x100)
			break;

		FileNameAttribute *name_attribute;

		switch (attribute->type)
		{
		case kFileName:
			name_attribute = (FileNameAttribute*)((unsigned char *)(attribute)+((ResidentAttribute *)(attribute))->value_offset);
			if (name_attribute->name_type & kWIN32Name || name_attribute->name_type == 0)
			{
				name_attribute->name[name_attribute->characters_count] = L'\0';
				data->file_name = AllocAndCopyString(disk->heap_block, name_attribute->name, name_attribute->characters_count);
				data->file_name_lenght = name_attribute->characters_count;
				data->parent_id.QuadPart = name_attribute->parent_reference;
				data->parent_id.HighPart &= 0x0000ffff;

				if (file->base_file_record.LowPart != 0)
					AddToFixList(file->base_file_record.LowPart, disk->files_size);

				return sizeof(SearchFileInfo);
			}
			break;
		default:
			break;
		};

		if (attribute->length > 0 && attribute->length < file->bytes_in_use)
			attribute = (Attribute *)((unsigned char *)attribute + attribute->length);
		else if (attribute->non_resident == TRUE)
			attribute = (Attribute *)((unsigned char *)attribute + sizeof(NonResidentAttribute));
	}

	return 0;
}

unsigned long FetchFileInfo(std::shared_ptr<DiskHandle> disk, FileRecordHeader* file, unsigned char * buffer)
{
	int hasFilename = false;
	auto file_info = (FileInfo *)buffer;
	auto attribute_pointer = (unsigned char *)file + file->attributes_offset;
	Attribute * attribute = (Attribute *)attribute_pointer;

	int attributes_number = min(16, file->attributes_count) - 1;

	if (file->ntfs.type == 'ELIF')
	{
		file_info->flags = file->flags;
		file_info->has_data = false;

		for (int i = 0; i < attributes_number; i++)
		{
			if (attribute->type < 0 || attribute->type>0x100)
				break;

			StandardInformationAttribute *standard_info = nullptr;
			FileNameAttribute *name_attribute = nullptr;
			ResidentAttribute *resident_attribute = nullptr;
			NonResidentAttribute *non_resident_attribute = nullptr;

			switch (attribute->type)
			{
			case kStandardInformation:
				standard_info = (StandardInformationAttribute*)attribute;

				file_info->read_time = standard_info->read_time;
				file_info->write_time = standard_info->write_time;
				file_info->creation_time = standard_info->creation_time;
				file_info->mft_changed_time = standard_info->mft_changed_time;
				file_info->dos_file_permission = standard_info->dos_file_permission;
				break;
			case kFileName:
				resident_attribute = (ResidentAttribute *)attribute;
				attribute_pointer = (unsigned char *)attribute + resident_attribute->value_offset;

				name_attribute = (FileNameAttribute *)attribute_pointer;

				if (name_attribute->name_type & kWIN32Name || name_attribute->name_type == 0)
				{
					file_info->parent_id.QuadPart = name_attribute->parent_reference;
					file_info->parent_id.HighPart &= 0x0000ffff;

					auto temp_pointer = new wchar_t[name_attribute->characters_count + 1];
					memcpy((void *)temp_pointer, name_attribute->name, name_attribute->characters_count * sizeof(wchar_t));
					temp_pointer[name_attribute->characters_count] = L'\0';
					
					file_info->file_name = temp_pointer;
					file_info->file_name_lenght = name_attribute->characters_count;
					hasFilename = true;

					if (file->base_file_record.LowPart != 0)
						AddToFixList(disk->files_size, file->base_file_record.LowPart);
				}
				break;
			case kData:

				if (attribute->name_length == 0)
				{
					if (attribute->non_resident)
					{
						non_resident_attribute = (NonResidentAttribute *)attribute;
						file_info->file_size.QuadPart = non_resident_attribute->data_size;
						file_info->allocated_file_size.QuadPart = non_resident_attribute->allocated_size;
					}
					else
					{
						resident_attribute = (ResidentAttribute *)attribute;
						file_info->file_size.QuadPart = resident_attribute->value_length;
						file_info->allocated_file_size.QuadPart = ((ResidentAttribute *)(attribute))->value_length;
					}

					file_info->has_data = true;
				}
				break;
			default:
				break;
			};

			if (attribute->length > 0 && attribute->length < file->bytes_in_use)
				attribute = (Attribute *)((unsigned char *)(attribute)+attribute->length);
			else if (attribute->non_resident == TRUE)
				attribute = (Attribute *)((unsigned char *)(attribute)+sizeof(NonResidentAttribute));
		}
	}
	else if (file->ntfs.type == 'DAAB')
	{
		file_info->parent_id.QuadPart = SpecialFiles::kRoot;
		file_info->file_name = L"{BAAD}";
	}
	else if (file->ntfs.type == 'XDNI')
	{
		file_info->parent_id.QuadPart = SpecialFiles::kRoot;
		file_info->file_name = L"{INDEX}";
	}
	else if (file->ntfs.type == 'ELOH')
	{
		file_info->parent_id.QuadPart = SpecialFiles::kRoot;
		file_info->file_name = L"{HOLE}";
	}
	else if (file->ntfs.type == 'DKHC')
	{
		file_info->parent_id.QuadPart = SpecialFiles::kRoot;
		file_info->file_name = L"{CHECKED}";
	}
	else if (file->ntfs.type == 0)
	{
		file_info->parent_id.QuadPart = SpecialFiles::kRoot;
		file_info->file_name = L"{FREE}";
	}
	else
	{
		file_info->parent_id.QuadPart = SpecialFiles::kMFT;
		file_info->file_name = L"{NOT A FILE}";
		return 0;
	}

	if (!hasFilename)
		file_info->file_name = L"{NO_NAME}";

	file_info->flags = file->flags;
	return sizeof(FileInfo);
}
