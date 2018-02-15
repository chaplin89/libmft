#ifndef  __COMMON_H__
#define  __COMMON_H__

enum class FetchOption
{
	kStandardInfo,
	kSearchInfo
};

enum SpecialFiles
{
	kMFT = 0,
	kMFTMirror = 1,
	kLogFile = 2,
	kVolume = 3,
	kAttributeDefinition = 4,
	kRoot = 5,
	kBitmapFile = 6,
	kBoot = 7,
	kBadClusters = 8,
	kQuota = 9,
	kSecure = 10,
	kUpperCase = 11,
	kExtend = 12,
	// 12-23 unused/empty

	// > 24, ordinary file/dir
	kOrinaryFileMark = 24
};

const static int kClusterPerRead = 1024;
#endif // ! __COMMON_H__
