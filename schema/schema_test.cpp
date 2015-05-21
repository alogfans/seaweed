// schema_test.cpp
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The test drive storage structure in /inc/storage.h.

#include "schema.h"
#include <iostream>
#include "gtest/gtest.h"
using namespace std;

struct DATA
{
    int index;
    char msg[16];
};

TEST(storage_test, simple)
{
    Storage stor;
    stor.create_file("test.db");
    stor.open_file("test.db");

    Schema schema(stor);
    schema.initize_schema();
    Column * columns = new Column[2];
    strcpy(columns[0].title, "index");
    columns[0].type = IntegerType;
    columns[0].length = 4;

    strcpy(columns[1].title, "msg");
    columns[1].type = CharType;
    columns[1].length = 16;

    schema.create_table("table_main", 2, columns);

    schema.create_index("table_main", "index");

    schema.open_table("table_main");
    for (int i = 0; i < 4000; i++)
    {
        DATA data;
        data.index = i;
        sprintf(data.msg, "Text of %d.", i);
        schema.insert_record(&data);
    }

    for (int i = 3000; i < 4000; i++)
    {
        DATA data;
        data.index = i;
        sprintf(data.msg, "Text of %d.", i);
        schema.delete_record(&data);
        //cout << "completed" << endl;
    }
    //cout << schema.count() << endl;
    schema.close_table();
}

TEST(storage_test, advance)
{
    Storage stor;
    stor.open_file("test.db");
    Schema schema(stor);

    schema.create_index("table_main", "msg");
    schema.open_table("table_main");
    vector<void *> vecs = schema.select_record(OP_AL, "msg", NULL);
    cout << vecs.size() << endl;
    schema.close_table();
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
