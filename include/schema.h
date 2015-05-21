// schema.h
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The header file of schema definitions.

#ifndef __SCHEMA_H__
#define __SCHEMA_H__

#include "generic.h"
#include "storage.h"
#include "index.h"
#include <vector>

struct TableInfo
{
    char title[32];
    int root_page;
    char sql_statement[255];
};

struct Column
{
    char title[32];
    unsigned int type;
    unsigned int length;
    int index_root;
};

class Schema
{
public:
    Schema(Storage &stor);
    ~Schema();

    void initize_schema();
    void create_table(const char * tb_name, int n_columns, Column * columns);
    void create_index(const char * tb_name, const char * column_title);
    void drop_table(const char * tb_name);
    void drop_index(const char * tb_name, const char * column_title);

    void open_table(const char * tb_name);
    void close_table();

    void insert_record(void * data);
    void delete_record(void * data);
    std::vector<void *> select_record(int operand, const char * column_title, void * key);
    void replace_record(void * origin_data, void * modified_data);
    int count();

    void construct(const char * tb_name);
    void deconstruct();

//private:
    Storage &stor;
    Index * index;

    bool opened;
    int table_root;
    int record_root;
    int n_columns;
    Column * columns;
    byte * buffer;
};

#endif