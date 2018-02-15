#ifndef  __COMMON_H__
#define  __COMMON_H__

enum class FetchOption
{
	kStandardInfo,
	kSearchInfo
};

enum SpecialDirectory
{
	MFT = 0,
	MFTMirror = 1,
	LogFile = 2,
	Volume = 3,
	AttributeDefinition = 4,
	Root = 5,
	Bitmap = 6,
	Boot = 7,
	BadClusters = 8,
	Quota = 9,
	Secure = 10,
	UpperCase = 11,
	Extend = 12,
	// 12-23 unused/empty

	// > 24, ordinary file/dir
	OrinaryFileMark = 24
};

const static int kClusterPerRead = 1024;
#endif // ! __COMMON_H__
