// hashtable_test.cpp
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The test drive btnode class.

#include "btree.h"
#include "gtest/gtest.h"
#include <stdexcept>
#include <iostream>
using namespace std;

byte memory[4096 * 16];

TEST(btree_test, btnode)
{    
    BTNode node;
    node.create_block(true, IntegerType, 4);
    cout << "ORDER: " << node.order << endl;
    for (int i = 0; i < node.order; i++)
    {
        node.pointers[i] = i * 100;
        node.slots[i] = i * 100;
    }
    node.marshall_to(memory);
    node.unmarshall_from(memory);
    for (int i = 0; i < node.order; i++)
    {
        EXPECT_EQ(node.pointers[i], i * 100);
        EXPECT_EQ(node.slots[i], i * 100);
    }
    node.dispose();

    node.create_block(false, IntegerType, 4);
    cout << "ORDER: " << node.order << endl;
    for (int i = 0; i < node.order; i++)
    {
        node.pointers[i] = i * 100;
    }
    node.marshall_to(memory);
    node.unmarshall_from(memory);
    for (int i = 0; i < node.order; i++)
    {
        EXPECT_EQ(node.pointers[i], i * 100);
    }
    node.dispose(); 
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
