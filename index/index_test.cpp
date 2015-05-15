// index.cpp
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The implemetation file of index. testdrive

// hashtable_test.cpp
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The test drive btnode class.

#include "index.h"
#include "storage.h"
#include "gtest/gtest.h"
#include <stdexcept>
#include <cstring>
#include <iostream>
using namespace std;

TEST(index, test3)
{    
    EntityProperty pro;
    pro.type = 1;
    pro.length = 4;

    Storage stor;
    stor.create_file("test.db");
    stor.open_file("test.db");

    Index idx(stor);
    int table = idx.create_table(1, &pro);
    int btree = idx.create_index(table, 1, 4);
    EXPECT_EQ(table, 0);
    EXPECT_EQ(btree, 1);

    idx.open_table(table, btree);
    for (int i = 1; i < 4000; i++)
    	idx.insert_record(&i, &i);
    for (int i = 1; i < 4000; i++)
    	idx.delete_record(&i);    
    idx.close_table(table, btree);

    idx.drop_table(table);
    idx.drop_index(btree);
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
