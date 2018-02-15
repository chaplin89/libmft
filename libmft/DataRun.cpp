
#include "DataRun.h"

unsigned long DataRunLength(unsigned char * run)
{
	return (*run & 0xf) + ((*run >> 4) & 0xf) + 1;
}

long long GetDataRunOffset(unsigned char * data_run)
{
	unsigned char lenght_bytes_count = *data_run & 0x0f;
	unsigned char offset_bytes_count = (*data_run >> 4) & 0xf;

	if (offset_bytes_count == 0)
		return 0;

	// Compose the offset, strarting from the end (little-endian)
	// TODO: offset_bytes_count should be <= 8
	long long logical_cluster_number = char(data_run[lenght_bytes_count + offset_bytes_count]);

	for (long i = lenght_bytes_count + offset_bytes_count - 1; i > lenght_bytes_count; i--)
		logical_cluster_number = (logical_cluster_number << 8) + data_run[i];

	return logical_cluster_number;
}

unsigned long long GetDataRunLenght(unsigned char * data_run)
{
	// left nibble: offset
	// right nibble: length
	unsigned char lenght_bytes_count = *data_run & 0xf;
	unsigned long long lenght = 0;

	for (unsigned long i = lenght_bytes_count; i > 0; i--)
		lenght = (lenght << 8) + data_run[i];

	return lenght;
}

bool FindRun(NonResidentAttribute * attribute, unsigned long long vcn, unsigned long long* lcn, unsigned long long* count)
{
	if (vcn < attribute->low_vcn || vcn > attribute->high_vcn)
		return false;

	unsigned long long base = attribute->low_vcn;

	*lcn = 0;
	unsigned char * begin_pointer = (unsigned char *)attribute + attribute->run_array_offset;

	for (unsigned char * run = begin_pointer; *run != 0; run += DataRunLength(run))
	{
		*lcn += GetDataRunOffset(run);
		*count = GetDataRunLenght(run);

		if (base <= vcn && vcn < base + *count)
		{
			if (GetDataRunOffset(run) == 0)
				*lcn = 0;
			else
				*lcn = *lcn + vcn - base;

			*count -= unsigned long(vcn - base);
			return true;
		}
		else
		{
			base += *count;
		}
	}
	return false;
}
