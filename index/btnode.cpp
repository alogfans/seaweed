// btnode.cpp
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The implementation file of btree node componment.

#include "btree.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <stdexcept>
using namespace std;

BTNode::BTNode()
{
	keys = NULL;
	pointers = NULL;
	slots = NULL;
}

BTNode::~BTNode()
{

}

void BTNode::create_block(bool is_leaf, uint32_t key_type, uint32_t key_sizeof)
{
	page_num = 0;						// wait for upper comp to update
	this->is_leaf = is_leaf;
	this->key_type = key_type;
	this->key_sizeof = key_sizeof;
	num_keys = 0;
	next = -1;
	parent = -1;
	order = (SizeOfPage - sizeof(BTNodeMarshall));
	if (is_leaf == true)
		order /= key_sizeof + 2 * sizeof(uint32_t);
	else
		order /= key_sizeof + sizeof(uint32_t);

	if (keys || pointers || slots)
		throw logic_error("create: storage not closed");

	keys = new byte[key_sizeof * (order - 1)];
	pointers = new uint32_t[order];

	if (is_leaf == true)
		slots = new uint32_t[order];
}

void BTNode::marshall_block(BTNodeMarshall * head_block)
{
	head_block->key_type = key_type;
	head_block->key_sizeof = key_sizeof;
	head_block->order = order;
    head_block->parent = parent;
    head_block->is_leaf = is_leaf;
    head_block->num_keys = num_keys;
    head_block->next = next;
}

void BTNode::unmarshall_block(BTNodeMarshall * head_block)
{
	key_type = head_block->key_type;
	key_sizeof = head_block->key_sizeof;
	order = head_block->order;
    parent = head_block->parent;
    is_leaf = head_block->is_leaf;
    num_keys = head_block->num_keys;
    next = head_block->next;
}

void BTNode::marshall_to(byte * buffer)
{
	BTNodeMarshall head_block;
	marshall_block(&head_block);
	uint32_t offset_keys = sizeof(BTNodeMarshall);
	uint32_t offset_pointers = offset_keys + key_sizeof * (order - 1);

	memcpy(buffer, &head_block, sizeof(BTNodeMarshall));
	memcpy(buffer + offset_keys, keys, key_sizeof * (order - 1));
	memcpy(buffer + offset_pointers, pointers, sizeof(uint32_t) * order);

	if (is_leaf == true)
	{
		uint32_t offset_slots = offset_pointers + sizeof(uint32_t) * order;
		memcpy(buffer + offset_slots, slots, sizeof(uint32_t) * order);
	}

	dispose();
}

void BTNode::unmarshall_from(byte * buffer)
{
	BTNodeMarshall head_block;
	memcpy(&head_block, buffer, sizeof(BTNodeMarshall));
	unmarshall_block(&head_block);

	if (keys || pointers || slots)
		throw logic_error("unmarshall: storage not closed");
	keys = new byte[key_sizeof * (order - 1)];
	pointers = new uint32_t[order];

	if (is_leaf == true)
		slots = new uint32_t[order];

	uint32_t offset_keys = sizeof(BTNodeMarshall);
	uint32_t offset_pointers = offset_keys + key_sizeof * (order - 1);

	memcpy(keys, buffer + offset_keys, key_sizeof * (order - 1));
	memcpy(pointers, buffer + offset_pointers, sizeof(uint32_t) * order);

	if (is_leaf == true)
	{
		uint32_t offset_slots = offset_pointers + sizeof(uint32_t) * order;
		memcpy(slots, buffer + offset_slots, sizeof(uint32_t) * order);
	}
}

void BTNode::dispose()
{	
	if (!keys || !pointers || (is_leaf && !slots))
		throw logic_error("dispose: storage not used");

	delete [] keys;
	keys = NULL;
	delete [] pointers;
	pointers = NULL;

	if (is_leaf == true)
	{
		delete [] slots;
		slots = NULL;		
	}
	//cout << "DISPOSED" << endl;
}
