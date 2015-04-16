// generic.h
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The header file of buffer componment, belonging to storage subsystems.

#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "generic.h"
#include "hashtable.h"

const uint32_t BufferSlots = 20;
const uint32_t SizeOfHashTable = 20;

const int InvalidSlot = -1;
const int AllPages = -1;

struct BufferSlot
{
	int prev;
	int next;
	int page_num;
	int pin_count;
	bool dirty;
	byte * mapping;
	int fd;
};

class Buffer
{
public:
	Buffer();
	~Buffer();

	byte * fetch_page(int fd, int page_num, bool allow_multiple_pin = true);
	byte * acquire_page(int fd, int page_num);
	void unpin_page(int fd, int page_num);
	void mark_dirty(int fd, int page_num);

	void clear(int fd);
	void apply(int fd, int page_num = AllPages);
private:

	void read_page(int slot);
	void write_page(int slot);
	void unlink_slot(int slot);
	void push_front(int slot);
	void insert_free(int slot);
	int alloc_slot();

	int first;
	int last;
	int free_first;
	BufferSlot buf_table[BufferSlots];
	HashTable hash_table;
};


#endif 