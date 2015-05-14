// entity.cpp
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The implementation file of entity definitions.

#include "entity.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>

using namespace std;

Entity::Entity()
{
	opened = false;
	dirty = false;
	max_records = -1;
	ptr_buffer = new byte[SizeOfPage];
	ptr_properties = NULL;
}
Entity::~Entity()
{
	// delete [] ptr_buffer;
	// do nothing is ok.
}

void Entity::open_entity(byte * buffer)
{
	if (opened == true)
		throw logic_error("handle open");
	opened = true;
	// memcpy(ptr_buffer, buffer, SizeOfPage);
	ptr_buffer = buffer;

	// snap important data structure
	memcpy(&entity_header, ptr_buffer, sizeof(EntityHeader));

	int n_property = entity_header.count_property;
	ptr_properties = new EntityProperty[n_property];

	memcpy(ptr_properties, ptr_buffer + sizeof(EntityHeader), 
		sizeof(EntityProperty) * n_property);

	max_records = 8 *(SizeOfPage - sizeof(EntityHeader) - 
		sizeof(EntityProperty) * n_property) / (8 * entity_header.record_bytes + 1);
	bitmap = new EntityBitmap(max_records);
	bitmap->load(ptr_buffer + sizeof(EntityHeader) + sizeof(EntityProperty) * n_property);

}

void Entity::init_entity(int count_property, EntityProperty * properties, byte * buffer)
{
	if (opened == true)
		throw logic_error("handle open");
	opened = true;

	ptr_buffer = buffer;
	
	ptr_properties = new EntityProperty[count_property];
	memcpy(ptr_properties, properties, sizeof(EntityProperty) * count_property);

	entity_header.count_property = count_property;	
	entity_header.record_bytes = 0;
	entity_header.next_entity = -1;

	for (int i = 0; i < count_property; i++)
		entity_header.record_bytes += properties[i].length;

	max_records = 8 * (SizeOfPage - sizeof(EntityHeader) - 
		sizeof(EntityProperty) * count_property) / (8 * entity_header.record_bytes + 1);
	//cout << max_records << endl;
	bitmap = new EntityBitmap(max_records);
	bitmap->clear();

	entity_header.record_ptr = sizeof(EntityHeader) + 
		sizeof(EntityProperty) * count_property + bitmap->get_bytes();
	//cout << max_records << endl;
	//cout << entity_header.record_ptr << endl;
	memcpy(ptr_buffer, &entity_header, sizeof(EntityHeader));
}

void Entity::close_entity()
{
	if (opened == false)
		throw logic_error("handle open");
	opened = false;
	
	memcpy(ptr_buffer, &entity_header, sizeof(entity_header));
	// byte * output = new byte[SizeOfPage];
	bitmap->store(ptr_buffer + sizeof(EntityHeader) + 
		sizeof(EntityProperty) * entity_header.count_property);
	// memcpy(output, ptr_buffer, SizeOfPage);
	// remember to release!
}

void * Entity::attain_record(int slot)
{
	void * output = new byte[entity_header.record_bytes];
	if (slot < 0 || slot >= max_records)
		throw logic_error("slot out of range");
	if (bitmap->test(slot) == false)
		throw logic_error("invalid slot");
	int loc = entity_header.record_ptr + entity_header.record_bytes * slot;
	memcpy(output, ptr_buffer + loc, entity_header.record_bytes);
	return output;
}

int Entity::insert_record(void * data)
{
	int slot;
	for (slot = 0; slot < max_records; slot++)
		if (bitmap->test(slot) == false)
			break;
	if (slot == max_records)
		return -1;

	int loc = entity_header.record_ptr + entity_header.record_bytes * slot;
	memcpy(ptr_buffer + loc, data, entity_header.record_bytes);
	bitmap->set(slot);
	return slot;
}

void Entity::remove_record(int slot)
{
	if (slot < 0 || slot >= max_records)
		throw logic_error("slot out of range");
	if (bitmap->test(slot) == false)
		throw logic_error("invalid slot");
	bitmap->clear(slot);
}

void Entity::replace_record(int slot, void * data)
{
	if (slot < 0 || slot >= max_records)
		throw logic_error("slot out of range");
	if (bitmap->test(slot) == false)
		throw logic_error("invalid slot");
	int loc = entity_header.record_ptr + entity_header.record_bytes * slot;
	memcpy(ptr_buffer + loc, data, entity_header.record_bytes);
}

void Entity::set_next_entity(int page_num)
{
	if (opened == false)
		throw logic_error("handle open");
	entity_header.next_entity = page_num;
}

int Entity::get_next_entity()
{
	if (opened == false)
		throw logic_error("handle open");
	return entity_header.next_entity;
}

void Entity::clear_bitmap()
{
	bitmap->clear();
}