// btnode.cpp
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The implementation file of btree node componment.

#include "btnode.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <stdexcept>
using namespace std;

BTNode::BTNode()
{
	keys = NULL;
	pointers = NULL;
}

BTNode::~BTNode()
{

}

void BTNode::create_block(bool is_leaf, uint32_t key_type, uint32_t key_sizeof)
{
	this->is_leaf = is_leaf;
	this->key_type = key_type;
	this->key_sizeof = key_sizeof;
	num_keys = 0;
	next = InvalidEntry;
	prev = InvalidEntry;
	parent = InvalidEntry;
	order = SizeOfPage - sizeof(BTNodeMarshall);
	order /= key_sizeof + sizeof(uint32_t);

	if (keys || pointers)
		throw logic_error("create: storage not closed");

	keys = new byte *[order - 1];
	for (int i = 0; i < order - 1; i++)
		keys[i] = new byte [key_sizeof];
	pointers = new uint32_t[order];
}

void BTNode::marshall_to(byte * buffer)
{
	BTNodeMarshall head_block;
	head_block.key_type = key_type;
	head_block.key_sizeof = key_sizeof;
	head_block.order = order;
    head_block.parent = parent;
    head_block.is_leaf = is_leaf;
    head_block.num_keys = num_keys;
    head_block.next = next;
    head_block.prev = prev;

	uint32_t offset_keys = sizeof(BTNodeMarshall);
	uint32_t offset_pointers = offset_keys + key_sizeof * (order - 1);

	memcpy(buffer, &head_block, sizeof(BTNodeMarshall));
	// memcpy(buffer + offset_keys, keys, key_sizeof * (order - 1));
	for (int i = 0; i < order - 1; i++)
	{
		memcpy(buffer + offset_keys + key_sizeof * i, keys[i], key_sizeof);
	}
	memcpy(buffer + offset_pointers, pointers, sizeof(uint32_t) * order);
	dispose();
}

void BTNode::unmarshall_from(byte * buffer)
{
	BTNodeMarshall head_block;
	memcpy(&head_block, buffer, sizeof(BTNodeMarshall));

	key_type = head_block.key_type;
	key_sizeof = head_block.key_sizeof;
	order = head_block.order;
    parent = head_block.parent;
    is_leaf = head_block.is_leaf;
    num_keys = head_block.num_keys;
    next = head_block.next;
    prev = head_block.prev;

	if (keys || pointers)
		throw logic_error("unmarshall: storage not closed");

	keys = new byte *[order - 1];
	for (int i = 0; i < order - 1; i++)
		keys[i] = new byte[key_sizeof];
	pointers = new uint32_t[order];

	uint32_t offset_keys = sizeof(BTNodeMarshall);
	uint32_t offset_pointers = offset_keys + key_sizeof * (order - 1);

	//memcpy(keys, buffer + offset_keys, key_sizeof * (order - 1));
	for (int i = 0; i < order - 1; i++)
	{
		memcpy(keys[i], buffer + offset_keys + key_sizeof * i, key_sizeof);
	}
	memcpy(pointers, buffer + offset_pointers, sizeof(uint32_t) * order);
}

void BTNode::dispose()
{	
	if (!keys || !pointers)
		throw logic_error("dispose: storage not used");

	for (int i = 0; i < order - 1; i++)
		delete [] keys[i];	
	delete [] keys;
	keys = NULL;
	delete [] pointers;
	pointers = NULL;
}
