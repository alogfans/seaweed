// seaweed.h
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The header file for seaweed console app, implemented a
// simple SQL-like interface


#ifndef __SEAWEED_H__
#define __SEAWEED_H__

#include "schema.h"
#include "storage.h"
#include "index.h"
#include <vector>
#include <cstring>

class TableProperty
{
public:
	TableProperty(const char * p_title, int type, bool is_index = false, int length = 4) : 
		type(type), length(length), is_index(is_index)
	{
		strncpy(title, p_title, 32);
	}

private:
	int type;
	int length;
	char title[32];
	bool is_index;
};

class Value
{
public:
	Value(int value) : type(IntegerType), length(4)
	{
		v = new byte[length];
		*(int *) v = value;
	}

	Value(float value) : type(RealType), length(4)
	{
		v = new byte[length];
		*(float *) v = value;		
	}

	Value(char * value, int length) : type(CharType), length(length)
	{
		v = new byte[length];
		memcpy(v, value, length);
	}

	~Value()
	{
		if (v != NULL)
		{
			//delete [] v;
			v = NULL;
		}
	}

//private:
	int type;
	int length;

	byte * v;
};

typedef std::vector<Value> Row;

class Condition
{
public:
	Condition(const char * property, int operand, Value compared_value) : 
		operand(operand), compared_value(compared_value)
	{
		property_name = new char[32];
		strncpy(property_name, property, 32);
	}

	~Condition()
	{
		delete [] property_name;
	}

//private:
	bool is_atomic;

	// if it's atomic, the following params useful
	char * property_name;
	int operand;
	Value compared_value;

	// cluase version is not implemented in this version.
};

class Seaweed
{
public:
	Seaweed(const char * db_name);
	~Seaweed();

	static void create_db(const char * db_name);
	static void unlink_db(const char * db_name);

	void create_table(const char * tb_name, std::vector<TableProperty> &properties);

	void drop_table(const char * tb_name);

    void insert_record(const char * tb_name, std::vector<Value> &values);

    int delete_record(const char * tb_name, Condition &condition);

    int select_record(const char * tb_name, Condition &condition, 
    	std::vector<Row> &rows);

    void get_table_schema(const char * tb_name, std::vector<TableProperty> &properties);

    void replace_record(const char * tb_name, std::vector<Value> &origin_values, 
    	std::vector<Value> &modified_values);

    byte * marshall(std::vector<Value> &values);

    void unmarshall(byte * packet, std::vector<Value> &values);

private:
	Storage stor;
	Schema * schema;
};

#endif