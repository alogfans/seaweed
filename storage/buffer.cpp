// buffer.cpp
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The implementation of buffer which is storage subsystem.

#include "buffer.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <cstdlib>
using namespace std;

Buffer::Buffer() : hash_table(SizeOfHashTable)
{
	for (int i = 0; i < SizeOfHashTable; i++)
	{
		buf_table[i].prev = i - 1;		
		buf_table[i].next = i + 1;
		buf_table[i].dirty = false;
		buf_table[i].mapping = new byte[SizeOfPage];
		memset(buf_table[i].mapping, 0, SizeOfPage);		
	}
	buf_table[0].prev = InvalidSlot;
	buf_table[SizeOfHashTable - 1].next = InvalidSlot;
	first = last = InvalidSlot;
	free_first = 0;

	read_counter = write_counter = 0;
}

Buffer::~Buffer()
{
	for (int i = 0; i < SizeOfHashTable; i++)
		delete[] buf_table[i].mapping;
}

byte * Buffer::fetch_page(int fd, int page_num, bool allow_multiple_pin)
{
	int slot = hash_table.find(fd, page_num);
	if (slot < 0)
	{
        slot = alloc_slot();

        try
        {
        	buf_table[slot].fd = fd;
        	buf_table[slot].page_num = page_num;
        	buf_table[slot].dirty = false;
        	buf_table[slot].pin_count = 1;

            read_page(slot);
            hash_table.insert(fd, page_num, slot);
        }
        catch (logic_error)
        {
            unlink_slot(slot);
            insert_free(slot);
            throw ;
        }
	}
	else
	{
		if (buf_table[slot].pin_count == 1 && allow_multiple_pin == false)
			throw invalid_argument("multiple pins");
		buf_table[slot].pin_count++;
		unlink_slot(slot);
		push_front(slot);
	}
	return buf_table[slot].mapping;
}

byte * Buffer::acquire_page(int fd, int page_num)
{
	int slot = hash_table.find(fd, page_num);
	if (slot >= 0)
		throw invalid_argument("page existed");

    slot = alloc_slot();

    try
    {
    	buf_table[slot].fd = fd;
    	buf_table[slot].page_num = page_num;
    	buf_table[slot].dirty = false;
    	buf_table[slot].pin_count = 1;

        hash_table.insert(fd, page_num, slot);
    }
    catch (logic_error)
    {
        unlink_slot(slot);
        insert_free(slot);
        throw ;
    }

	return buf_table[slot].mapping;
}

void Buffer::unpin_page(int fd, int page_num)
{
	int slot = hash_table.find(fd, page_num);
	if (slot < 0)
		throw logic_error("page out of buffer");
	if (buf_table[slot].pin_count == 0)
		throw logic_error("page not pinned");

	buf_table[slot].pin_count--;
	if (buf_table[slot].pin_count == 0)
	{		
		unlink_slot(slot);
		push_front(slot);
	}
}

void Buffer::mark_dirty(int fd, int page_num)
{
	int slot = hash_table.find(fd, page_num);
	if (slot < 0)
		throw logic_error("page out of buffer");
	if (buf_table[slot].pin_count == 0)
		throw logic_error("page not pinned");
	buf_table[slot].dirty = true;

	unlink_slot(slot);
	push_front(slot);
}

void Buffer::show()
{
	int slot = first;
	while (slot != InvalidSlot)
	{
		int next = buf_table[slot].next;
		cerr << "slot = " << slot << "\tfd = " << buf_table[slot].fd << 
			"\tpage num = " << buf_table[slot].page_num << endl;
		slot = next;
	}
	cerr << "--- the end ---\n";
}

void Buffer::flush(int fd)
{
	int slot = first;
	while (slot != InvalidSlot)
	{
		int next = buf_table[slot].next;
		if (buf_table[slot].fd == fd)
		{
			if (buf_table[slot].pin_count != 0)
				throw logic_error("page pinned.");

			if (buf_table[slot].dirty == true)
			{
				write_page(slot);
				buf_table[slot].dirty = false;
			}
			hash_table.remove(fd, buf_table[slot].page_num);
			unlink_slot(slot);
			insert_free(slot);
		}
		slot = next;
	}
}

void Buffer::clear()
{
	int slot = first;
	while (slot != InvalidSlot)
	{
		int next = buf_table[slot].next;
		if (buf_table[slot].pin_count == 0)
		{
			hash_table.remove(buf_table[slot].fd, buf_table[slot].page_num);
			unlink_slot(slot);
			insert_free(slot);
		}
		slot = next;
	}
}


void Buffer::apply(int fd, int page_num)
{
	int slot = first;
	while (slot != InvalidSlot)
	{
		if (buf_table[slot].fd == fd && buf_table[slot].dirty == true)
		{
			if (page_num == AllPages || page_num == buf_table[slot].page_num)
			{
				write_page(slot);
				buf_table[slot].dirty = false;
			}
		}
		slot = buf_table[slot].next;
	}
}

void Buffer::read_page(int slot)
{
	int page_num = buf_table[slot].page_num;
	int fd = buf_table[slot].fd;
	uint32_t location = SizeOfStorageHeader + SizeOfBitmap + page_num * SizeOfPage;

	lseek(fd, location, SEEK_SET);

	if (read(fd, buf_table[slot].mapping, SizeOfPage) < SizeOfPage)
		throw logic_error("unable to read file");

	read_counter++;
}

void Buffer::write_page(int slot)
{
	int page_num = buf_table[slot].page_num;
	int fd = buf_table[slot].fd;
	uint32_t location = SizeOfStorageHeader + SizeOfBitmap + page_num * SizeOfPage;

	lseek(fd, location, SEEK_SET);
	
	if (write(fd, buf_table[slot].mapping, SizeOfPage) < SizeOfPage)
		throw logic_error("unable to write file");

	write_counter++;
}

void Buffer::unlink_slot(int slot)
{
	if (first == slot)
		first = buf_table[slot].next;

	if (last == slot)
        last = buf_table[slot].prev;

    if (buf_table[slot].next != InvalidSlot)
        buf_table[ buf_table[slot].next ].prev = buf_table[slot].prev;

    if (buf_table[slot].prev != InvalidSlot)
        buf_table[ buf_table[slot].prev ].next = buf_table[slot].next;

    buf_table[slot].prev = buf_table[slot].next = InvalidSlot;
}

void Buffer::push_front(int slot)
{
    buf_table[slot].next = first;
    buf_table[slot].prev = InvalidSlot;

    if (first != InvalidSlot)
    	buf_table[first].prev = slot;
    first = slot;

    if (last == InvalidSlot)
    	last = first;
}

void Buffer::insert_free(int slot)
{
	buf_table[slot].next = free_first;
	free_first = slot;
}

int Buffer::alloc_slot()
{
	int slot;
	if (free_first != InvalidSlot)
	{
		slot = free_first;
		free_first = buf_table[slot].next;
		push_front(slot);
		return slot;
	}

	for (slot = last; slot != InvalidSlot; slot = buf_table[slot].prev)
		if (buf_table[slot].pin_count == 0)
			break;

	if (slot == InvalidSlot)
		throw logic_error("alloc failed");

 	if (buf_table[slot].dirty)
 	{
		write_page(slot);
        buf_table[slot].dirty = false;
	}

	hash_table.remove(buf_table[slot].fd, buf_table[slot].page_num);
    unlink_slot(slot);
    push_front(slot);
    return slot;
}

int Buffer::get_read_counter()
{
	return read_counter;
}

int Buffer::get_write_counter()
{
	return write_counter;
}
