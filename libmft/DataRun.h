#ifndef __DATARUN_H__
#define __DATARUN_H__
#include "Attributes.h"

//Reference: https://flatcap.org/linux-ntfs/ntfs/concepts/data_runs.html
unsigned long long GetDataRunLenght(unsigned char * run);
unsigned long DataRunLength(unsigned char * run);
long long GetDataRunOffset(unsigned char * run);
bool FindRun(NonResidentAttribute * attr, unsigned long long vcn, unsigned long long* lcn, unsigned long long* count);
#endif // !__DATARUN_H__