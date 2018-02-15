#ifndef  __COMMON_H__
#define  __COMMON_H__

enum class FetchOption
{
	kStandardInfo,
	kSearchInfo
};

enum SpecialFiles
{
	/// The MFT, plain and simple
	kMFT = 0,
	/// A mirror of the MFT
	kMFTMirror = 1,
	/// Transactional logging
	kLogFile = 2,
	/// General info about the volume
	kVolume = 3,
	/// All the attribute usable in a give volume
	kAttributeDefinition = 4,
	/// Disk root directory
	kRoot = 5,
	/// Volume cluster map (in use/free)
	kBitmapFile = 6,
	/// Boot code
	kBoot = 7,
	/// Not-usable clusters
	kBadClusters = 8,
	/// Quota related information
	kQuota = 9,
	/// Security descriptors
	kSecure = 10,
	/// Upper case characters used for collation
	kUpperCase = 11,
	/// A directory that contains: $ObjId, $Quota, $Reparse, $UsnJrnl
	kExtend = 12,

	/// 12-23: unused/empty

	/// Any - $ObjId: Unique Ids given to every file
	/// Any - $Reparse: reparse point information
	/// Any - $UsnJrnl: journalling of encryption

	/// Every inode > 24 is an ordinary file/directory
	kOrinaryFileMark = 24
};

const static int kClusterPerRead = 1024;
#endif // ! __COMMON_H__
