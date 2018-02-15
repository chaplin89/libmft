#include "Path.h"
#include "Attributes.h"

const wchar_t* GetFileName(FileRecordHeader *file)
{
	if (file == nullptr)
		return nullptr;

	ResidentAttribute * attribute = (ResidentAttribute*)FindFirstAttribute(file, kFileName);

	if (attribute == nullptr)
		return nullptr;

	unsigned char *attribute_pointer = (unsigned char*)attribute + attribute->value_offset;
	FileNameAttribute *name_attribute = (FileNameAttribute*)attribute_pointer;

	return name_attribute->name;
}

wchar_t* GetPath(std::shared_ptr<DiskHandle> disk, int id)
{
	static wchar_t path[0xffff];

	int current_id = id;
	unsigned long pointer;
	unsigned char * ptr = (unsigned char *)disk->short_info;
	unsigned long path_stack[64];
	int path_stack_position = 0;
	int current_position = 0;

	path_stack_position = 0;
	for (int i = 0; i < 64; i++)
	{
		path_stack[path_stack_position++] = current_id;
		pointer = current_id * disk->is_long;
		current_id = ((SearchFileInfo *)(ptr + pointer))->parent_id.LowPart;

		if (current_id == 0 || current_id == 5)
			break;
	}

	if (disk->dos_device != 0)
	{
		path[0] = disk->dos_device;
		path[1] = L':';
		current_position = 2;
	}
	else
		path[0] = L'\0';

	for (int i = path_stack_position - 1; i > 0; i--)
	{
		pointer = path_stack[i] * disk->is_long;
		auto file_info = (SearchFileInfo *)(ptr + pointer);

		path[current_position++] = L'\\';
		memcpy(&path[current_position], file_info->file_name, file_info->file_name_lenght * 2);
		current_position += file_info->file_name_lenght;
	}

	path[current_position] = L'\\';
	path[current_position + 1] = L'\0';
	return path;
}

wchar_t* GetCompletePath(std::shared_ptr<DiskHandle> disk, int id)
{
	int current_id = id;
	unsigned long pt;
	unsigned char * ptr = (unsigned char *)disk->short_info;
	unsigned long path_stack[64];
	int path_stack_position = 0;
	static wchar_t gl_path[0xffff];
	int current_position = 0;

	path_stack_position = 0;
	for (int i = 0; i < 64; i++)
	{
		path_stack[path_stack_position++] = current_id;
		pt = current_id * disk->is_long;
		current_id = ((SearchFileInfo *)(ptr + pt))->parent_id.LowPart;

		if (current_id == SpecialDirectory::MFT || current_id == SpecialDirectory::Root)
			break;
	}

	if (disk->dos_device != 0)
	{
		gl_path[0] = disk->dos_device;
		gl_path[1] = L':';
		current_position = 2;
	}
	else
		gl_path[0] = L'\0';

	for (int i = path_stack_position - 1; i >= 0; i--)
	{
		pt = path_stack[i] * disk->is_long;
		gl_path[current_position++] = L'\\';
		memcpy(&gl_path[current_position], ((SearchFileInfo *)(ptr + pt))->file_name, ((SearchFileInfo *)(ptr + pt))->file_name_lenght * 2);
		current_position += ((SearchFileInfo *)(ptr + pt))->file_name_lenght;
	}
	gl_path[current_position] = L'\0';
	return gl_path;
}