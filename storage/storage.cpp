// storage.cpp
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The implementation of storage subsystem.

#include "storage.h"
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

#ifdef CONFIG_USING_BUFFER
Buffer Storage::buffer;
#endif

Storage::Storage()
{
	fd = -1;
}

Storage::~Storage()
{
	if (fd > 0)
		close(fd);
}

void Storage::create_file(const char * path)
{
	if (access(path, F_OK) == 0)
		throw logic_error("file existed");
	int fd_new = open(path, O_CREAT | O_WRONLY, 0664);
	if (fd_new < 0)
		throw logic_error("unable to create file");

	// Write header info
	StorageHeader header;
	memset(&header, 0, SizeOfStorageHeader);
	memcpy(header.magic, MagicString, 16);
	
	if (write(fd_new, &header, SizeOfStorageHeader) < SizeOfStorageHeader)
		throw logic_error("unable to write file");

	// Write bitmap -- because the vaild bit is 0 so that filling zero is OK
	Slot * bitmap = new Slot[SizeOfBitmap];
	memset(bitmap, 0, SizeOfBitmap);
	
	if (write(fd_new, bitmap, SizeOfBitmap) < SizeOfBitmap)
		throw logic_error("unable to write file");
	delete[] bitmap;

	close(fd_new);
}

void Storage::destory_file(const char * path)
{
	if (unlink(path) < 0)
		throw logic_error("unable to remove file.");
}

void Storage::open_file(const char * path)
{
	if (is_opening())
		throw logic_error("file opened currently");
	fd = open(path, O_RDWR);
	if (fd < 0)
		throw logic_error("unable to open file");
	dirty = false;
	lseek(fd, 0, SEEK_SET);
	if (read(fd, &shadow, SizeOfStorageHeader) < SizeOfStorageHeader)
		throw logic_error("unable to read file");
	if (read(fd, bitmap, SizeOfBitmap) < SizeOfBitmap)
		throw logic_error("unable to read file");
}

bool Storage::is_opening()
{
	return (fd >= 0);
}

void Storage::close_file()
{
#ifdef CONFIG_USING_BUFFER
	buffer.flush(fd);
#endif
	if (dirty == true)
	{
		lseek(fd, 0, SEEK_SET);
		if (write(fd, &shadow, SizeOfStorageHeader) < SizeOfStorageHeader)
			throw logic_error("unable to write file");		
		if (write(fd, bitmap, SizeOfBitmap) < SizeOfBitmap)
			throw logic_error("unable to write file");	
	}
	close(fd);
	fd = -1;
}

#ifdef CONFIG_USING_BUFFER
void Storage::unpin_page(int page_num)
{
	if (dirty == true)
	{
		lseek(fd, 0, SEEK_SET);
		if (write(fd, &shadow, SizeOfStorageHeader) < SizeOfStorageHeader)
			throw logic_error("unable to write file");		
		if (write(fd, bitmap, SizeOfBitmap) < SizeOfBitmap)
			throw logic_error("unable to write file");	
		dirty = false;
	}
	
	if (!is_opening())
		throw logic_error("file not opened currently");
	if (page_num < 0 || page_num >= shadow.allot_pages)
		throw domain_error("page_num out of range");
	if (get_slot_property(bitmap[page_num], UsedBit) == false)
		throw logic_error("page is not used");
	buffer.unpin_page(fd, page_num);	
}
#endif

int Storage::acquire_page()
{

	if (!is_opening())
		throw logic_error("file not opened currently");
	// first, we scan for the vaild but not used page
	int i = 0;
	for (; i < shadow.allot_pages; i++)
		if (get_slot_property(bitmap[i], UsedBit) == false)
			break;

	if (i == shadow.allot_pages)
	{  
#ifndef CONFIG_USING_BUFFER
		// need to fresh out new page
		lseek(fd, 0, SEEK_END);
		byte new_page[SizeOfPage] = { 0 };
		if (write(fd, new_page, SizeOfPage) < SizeOfPage)
			throw logic_error("unable to write file");
#else
		byte * ptr = buffer.acquire_page(fd, i);
		memset(ptr, 0, SizeOfPage);
		buffer.mark_dirty(fd, i);
		buffer.unpin_page(fd, i);
#endif
		shadow.allot_pages++;
		set_slot_property(bitmap[i], VaildBit);
	}	
	dirty = true;
	set_slot_property(bitmap[i], UsedBit);
	shadow.used_pages++;
	return i;
}

void Storage::release_page(int page_num)
{
	if (!is_opening())
		throw logic_error("file not opened currently");
	if (page_num < 0 || page_num >= shadow.allot_pages)
		throw domain_error("page_num out of range");
	if (get_slot_property(bitmap[page_num], UsedBit) == false)
		throw logic_error("page is not used");
	dirty = true;	
	clear_slot_property(bitmap[page_num], UsedBit);
	shadow.used_pages--;
#ifdef CONFIG_USING_BUFFER
	//buffer.fetch_page(fd, page_num, false);
	//buffer.unpin_page(fd, page_num);
#endif
}

byte * Storage::get_page_content(int page_num)
{
	if (!is_opening())
		throw logic_error("file not opened currently");

	if (page_num < 0 || page_num >= shadow.allot_pages)
		throw domain_error("page_num out of range");
	if (get_slot_property(bitmap[page_num], UsedBit) == false)
		throw logic_error("page is not used");
#ifndef CONFIG_USING_BUFFER
	byte * p_buffer = new char[SizeOfPage];
	if (p_buffer == NULL)
		throw overflow_error("out of memory");

	uint32_t location = SizeOfStorageHeader + SizeOfBitmap + page_num * SizeOfPage;

	lseek(fd, location, SEEK_SET);
	if (read(fd, p_buffer, SizeOfPage) < SizeOfPage)
		throw logic_error("unable to read file");
	return p_buffer;
#else
	char * ptr = buffer.fetch_page(fd, page_num);
	buffer.mark_dirty(fd, page_num);
	return ptr;
#endif
}

void Storage::update_page_content(int page_num, byte * content)
{
	if (!is_opening())
		throw logic_error("file not opened currently");
	if (page_num < 0 || page_num > shadow.allot_pages)
		throw domain_error("page_num out of range");
	if (get_slot_property(bitmap[page_num], UsedBit) == false)
		throw logic_error("page is not used");
	
	if (dirty == true)
	{
		lseek(fd, 0, SEEK_SET);
		if (write(fd, &shadow, SizeOfStorageHeader) < SizeOfStorageHeader)
			throw logic_error("unable to write file");		
		if (write(fd, bitmap, SizeOfBitmap) < SizeOfBitmap)
			throw logic_error("unable to write file");	
		dirty = false;
	}

#ifndef CONFIG_USING_BUFFER

	uint32_t location = SizeOfStorageHeader + SizeOfBitmap + page_num * SizeOfPage;
	lseek(fd, location, SEEK_SET);
	if (write(fd, content, SizeOfPage) < SizeOfPage)
		throw logic_error("unable to write file");
#else
	buffer.apply(fd, page_num);

#endif
}
