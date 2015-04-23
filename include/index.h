// index.h
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The header file of index definitions.

#ifndef __INDEX_H__
#define __INDEX_H__

#include "generic.h"
#include "storage.h"
#include "entity.h"

struct IndexHeader
{
	uint32_t type;
	uint32_t next;		// available when it's root 
	uint32_t is_leaf;
	uint32_t parent;
	uint32_t num_keys;

	uint32_t key_id;
	uint32_t key_type;
	uint32_t key_bytes;
};

struct Record
{
	Record * next;
	int row;
	void * data;
};

void delete_record_structure(Record * record);

class Index
{
public:
	Index(Storage &stor);
	~Index();

    int create_table(int n_property, Property * properties, int key_id);
    int create_index(int table_id, int key_id);
    void drop_table(int table_id);
    void drop_index(int table_id, int index_id);

    void open_table(int table_id);
    void close_table(int table_id);

    int count();
    void insert_record(void * data);
    void delete_records(int key_id, void * key);
    Record * select_records(int operand, int key_id, void * key);
    void modify_record(Records * record, void * data);
private:
	Storage &stor;
	Entity entity;

	bool opened_flag;
	int opened_table_id;

	
};

#endif