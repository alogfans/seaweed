// hashtable_test.cpp
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The test drive btnode class.

#include "btree.h"
#include "storage.h"
#include "gtest/gtest.h"
#include <stdexcept>
#include <vector>
#include <cstring>
#include <iostream>
using namespace std;

TEST(btree, test3)
{    
    Storage stor;
    stor.create_file("test.db");
    stor.open_file("test.db");

    BTree * btree = new BTree(stor, IntegerType, 4);
    for (int i = 0; i < 4000; i++)
    {
        RID rid(1, 10000 - i);
        // cout << rid.page_num << " " << rid.slot_num << endl;
        btree->insert(&i, rid);
    }
    for (int i = 0; i < 4000; i++)
    {
        RID rid = btree->search(&i);
        EXPECT_EQ(rid.page_num, 1);
        EXPECT_EQ(rid.slot_num, 10000 - i);
    } 
    btree->destroy();
    int k = 3980;
    vector<RID> rid_list = btree->selects(OP_GE, &k);
    for (int i = 0; i < rid_list.size(); i++)
        cout << rid_list[i].page_num << "  " << rid_list[i].slot_num << endl;
    delete btree;
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
