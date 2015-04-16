// storage.h
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The header file of storage componment.

#ifndef __STORAGE_H__
#define __STORAGE_H__

#include "generic.h"
#include "buffer.h"

// HACK FOR TEST
#define private public

// The following part defines the header of storage. Each file consists
// only one instance of it and locates in the first  bytes. Then follows
// PageBitmap, representing the allocate status of whole vaild pages. 
// As a limitation of this program, The page bitmap should be 1KB, which 
// means that the maximal file size is 4GiB.

struct StorageHeader 
{
	char magic[16];		// should be "Seaweed DB File\0"
	uint32_t used_pages;
	uint32_t allot_pages;
	uint32_t bitmap_checksum;
	char reserved[36];
};

// each bitmap item is 8-bit (1 byte), as the figure below shown
// *------------------------*-----*-----*-----*-----*
// *       RESERVED(4)      *  W  *  R  *  U  *  V  *
// *------------------------*-----*-----*-----*-----*

const int VaildBit = 0;		// If this page is allot in disk, set 1
const int UsedBit = 1;		// If this page is not free, set 1
const int ReadBit = 2;		// If this page allows to read, set 1 (for lock service)
const int WriteBit = 3;		// If this page allows to write, set 1 (for lock service)

inline bool get_slot_property(Slot slot, int property)
{
	return (slot >> property) & 0x1;
}

inline void set_slot_property(Slot &slot, int property)
{
	slot |= 0x1 << property;
}

inline void clear_slot_property(Slot &slot, int property)
{
	slot &= ~(0x1 << property);
}

class Storage
{
public:
	Storage();
	~Storage();

	static void create_file(const char * path);
	static void destory_file(const char * path);

	void open_file(const char * path);
	bool is_opening();

	void close_file();

	int acquire_page();
	void release_page(int page_num);
	byte * get_page_content(int page_num);
#ifdef CONFIG_USING_BUFFER
	void unpin_page(int page_num);
#endif
	void update_page_content(int page_num, byte * content);
private:
#ifdef CONFIG_USING_BUFFER
	static Buffer buffer;
#endif
	StorageHeader shadow;
	bool dirty;
	Slot bitmap[SizeOfBitmap];
	int fd;
};

#endif
