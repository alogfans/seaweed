// entity.h
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The header file of entity definitions.

#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "generic.h"
#include "storage.h"

struct EntityHeader
{
	uint32_t record_bytes;
	uint32_t record_ptr;
	uint32_t count_property;
	// then follows data properties.
};

// type can be found in generic file.
struct EntityProperty
{
	uint32_t type : 4;
	uint32_t length : 12;
};

class EntityBitmap
{
public:
	EntityBitmap(int nslots);
	~EntityBitmap();

	void load(byte * buffer);
	void store(byte * buffer);

	bool test(int bit);
	void set(int bit);
	void clear(int bit);
	void clear();
	int get_bytes();
private:
	int nslots;
	int nbytes;
	byte * bitmap;
};

// Entity: manage data resources which can be insert in
// one single page, so that it can be helpful for upper
// b-tree structure.
class Entity
{
public:
	Entity();
	~Entity();

	void open_entity(byte * buffer);
	void init_entity(int count_property, EntityProperty * properties);
	byte * close_entity();

	void * attain_record(int slot);
	int insert_record(void * data);
	void remove_record(int slot);
	void replace_record(int slot, void * data);
private:
	bool opened;
	bool dirty;
	int max_records;
	byte * ptr_buffer;
	EntityHeader entity_header;
	EntityProperty * ptr_properties;
	EntityBitmap * bitmap;
};

#endif