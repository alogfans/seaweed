// record.cpp
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The header file of record implementations.

#include "record.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>

using namespace std;

Record::Record(Storage &stor, int page_num): stor(stor), first_page_num(first_page_num)
{

}

Record::~Record()
{

}

void * Record::attain_record(RID &loc)
{
	Entity entity;
	byte * buffer = stor.get_page_content(loc.page_num);
	entity.open_entity(buffer);
	void * callback = entity.attain_record(loc.slot_num);
	entity.close_entity();
	stor.unpin_page(loc.page_num);
	return callback;
}

RID Record::insert_record(void * data)
{
	int page_num, next_page = first_page_num, slot_num;
	bool success = false;
	while (!success && next_page >= 0)
	{
		page_num = next_page;

		slot_num = try_insert_record(page_num, data, next_page);
		if (slot_num >= 0)
			success = true;
		//cout << page_num << " " << slot_num << " " << next_page << endl;
	}

	if (success)
		return RID(page_num, slot_num);

	next_page = stor.acquire_page();
	Entity entity;
	byte * buffer_prev = stor.get_page_content(page_num);
	byte * buffer_curr = stor.get_page_content(next_page);
	memcpy(buffer_curr, buffer_prev, SizeOfPage);

	entity.open_entity(buffer_prev);
	entity.set_next_entity(next_page);
	entity.close_entity();
	stor.update_page_content(page_num, buffer_prev);
	stor.unpin_page(page_num);

	entity.open_entity(buffer_curr);
	entity.clear_bitmap();
	entity.close_entity();
	stor.update_page_content(next_page, buffer_curr);
	stor.unpin_page(next_page);
	// like fork implementation
	page_num = next_page;
	slot_num = try_insert_record(page_num, data, next_page);
	return RID(page_num, slot_num);
}

int Record::try_insert_record(int page_num, void * data, int &next_page)
{
	Entity entity;
	byte * buffer = stor.get_page_content(page_num);
	entity.open_entity(buffer);
	int slot_num = entity.insert_record(data);
	next_page = entity.get_next_entity();
	entity.close_entity();
	stor.update_page_content(page_num, buffer);
	stor.unpin_page(page_num);
	return slot_num;
}

void Record::remove_record(RID &loc)
{
	Entity entity;
	byte * buffer = stor.get_page_content(loc.page_num);

	entity.open_entity(buffer);
	entity.remove_record(loc.slot_num);
	entity.close_entity();

	stor.update_page_content(loc.page_num, buffer);
	stor.unpin_page(loc.page_num);
}

void Record::replace_record(RID &loc, void * data)
{
	Entity entity;
	byte * buffer = stor.get_page_content(loc.page_num);
	entity.open_entity(buffer);
	entity.replace_record(loc.slot_num, data);
	entity.close_entity();
	stor.update_page_content(loc.page_num, buffer);
	stor.unpin_page(loc.page_num);
}

void Record::init_entity(int count_property, EntityProperty * properties)
{
	first_page_num = stor.acquire_page();

	Entity entity;
	byte * buffer = stor.get_page_content(first_page_num);

	entity.init_entity(count_property, properties, buffer);

	stor.update_page_content(first_page_num, buffer);
	stor.unpin_page(first_page_num);
}

int Record::get_first_page()
{
	return first_page_num;
}