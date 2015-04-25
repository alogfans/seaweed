// hashtable_test.cpp
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The test drive btnode class.

#include "btree.h"
#include "gtest/gtest.h"
#include <stdexcept>
#include <cstring>
#include <iostream>
using namespace std;

byte memory[4096 * 16];
bool used[16] = { 0 };

BTNode * BTree::load_page(int page_num)
{
    if (page_num < 0 || page_num >= 16 || used[page_num] == 0)
    {
        cout << "page num invalid: load_page";
        return NULL;
    }
    BTNode * node = new BTNode();

    node->unmarshall_from(memory + page_num * 4096);
    node->page_num = page_num;
    return node;
}

void BTree::close_page(BTNode * node)
{
    if (node != NULL)
    {
        node->marshall_to(memory + node->page_num * 4096);
        // cout << "write " << node->page_num << endl;
        delete node;
    }
}

int BTree::lease_page()
{
    for (int i = 0; i < 16; i++)
        if (used[i] == 0)
        {
            used[i] = 1;
            //cout << "requested: " << i << endl;
            return i;
        }    
}

TEST(btree, test1)
{    
    memset(memory, 0, sizeof(memory));
    memset(used, 0, sizeof(used));
    BTree * btree = new BTree(IntegerType, 4);
    for (int i = 0; i < 200; i++)
    {
        RID rid(1, 1000 - i);
        //cout << rid.page_num << " " << rid.slot_num << endl;
        btree->insert(&i, rid);
    }
    for (int i = 0; i < 200; i++)
    {
        RID rid = btree->search(&i);
        EXPECT_EQ(rid.page_num, 1);
        EXPECT_EQ(rid.slot_num, 1000 - i);
    }    
    delete btree;
}

TEST(btree, test2)
{    
    memset(memory, 0, sizeof(memory));
    memset(used, 0, sizeof(used));
    BTree * btree = new BTree(IntegerType, 4);
    for (int i = 0; i < 1000; i++)
    {
        RID rid(1, 1000 - i);
        // cout << rid.page_num << " " << rid.slot_num << endl;
        btree->insert(&i, rid);
    }
    for (int i = 0; i < 1000; i++)
    {
        RID rid = btree->search(&i);
        EXPECT_EQ(rid.page_num, 1);
        EXPECT_EQ(rid.slot_num, 1000 - i);
    }    
    delete btree;
}

TEST(btree, test3)
{    
    memset(memory, 0, sizeof(memory));
    memset(used, 0, sizeof(used));
    BTree * btree = new BTree(IntegerType, 4);
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
    delete btree;
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
