// record.h
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The header file of record definitions.

#ifndef __RECORD_H__
#define __RECORD_H__

#include "generic.h"
#include "entity.h"
#include "btree.h"

class Record
{
public:
	Record(Storage &stor, int page_num = -1);
	~Record();

	void init_entity(int count_property, EntityProperty * properties);
	void * attain_record(RID &loc);
	RID insert_record(void * data);
	void remove_record(RID &loc);
	void replace_record(RID &loc, void * data);
	int get_first_page();

	void destroy();
		
private:
	int try_insert_record(int page_num, void * data, int &next_page);
	void destroy_recursive(int page_num);
	
	Storage &stor;
	int first_page_num;
};

#endif