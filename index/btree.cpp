// btree.cpp
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The implementation file of btree node componment.

#include "btree.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <stdexcept>
using namespace std;

BTNode * BTree::find_leaf(void * key)
{
	BTNode * node = load_page(root_page);
	int next;
	while (node->is_leaf == false)
	{
		int slot;
		while (slot < node->num_keys && compare(key, get_key(node->keys, slot) >= 0)
			slot++;
		next = node->pointers[slot];
		node.dispose();
		node = load_page(next);
	}
	// the 'next page' -- which is the leaf
	// node.dispose();
	return node;
}

Record * BTree::find(void * key)
{
	if (root_page < 0)
		return NULL;
	BTNode * leaf = find_leaf(key);
	int slot;
	for (slot = 0; slot < leaf->num_keys; slot++)
		if (compare(get_key(leaf->keys, slot), key) == 0)
			break;

	if (slot == leaf->num_keys)
		return NULL;

	Record * result = make_record(leaf, slot);
	leaf.dispose();
	return result;
}

Record * BTree::make_record(BTNode * leaf, int slot)
{
	// leaf->pointers[slot], leaf->slots[slot]
}

void BTree::insert(void * key, void * value)
{
	if (find(key) != NULL)
		return ;

	// trival cond: no pages alloted
	if (root_page < 0)
		root_page = allot_root(key, value);
	else
	{
		BTNode * leaf = find_leaf(key);
		if (leaf->num_keys < leaf->order - 1)
			insert_leaf(leaf, key, value);
		else
			root_page = insert_leaf_spilt(root_page, leaf, key, value);
		leaf.apply();
	}
}

void BTree::insert_leaf(BTNode * leaf_ptr, void * key, void * ptr)
{
    int insert_point = 0;
    while (insert_point < leaf->num_keys && 
    	compare(get_key(leaf->keys, insert_point), key) < 0)
        insert_point++;

    for (int i = leaf->num_keys; i > insert_point; i--)
    {
        set_key(leaf->keys, i, get_key(leaf->keys, i - 1));
        leaf->pointers[i] = leaf->pointers[i - 1];
        leaf->slots[i] = leaf->slots[i - 1];
    }

    set_key(leaf->keys, insert_point, key);

    int page, slot;
    insert_record(key, ptr, &page, &slot);
    leaf->pointers[insert_point] = page;
    leaf->slots[insert_point] = slot;
    leaf->num_keys++;
}

int BTree::insert_leaf_spilt(int root_page, BTNode * leaf, void * key, void * value)
{
	// this function attains a previous root page and spilt the leaf with two. Remember
	// that the root may be modified too!
	BTNode * new_node = new BTNode();
	new_node.create_block(true, key_type, key_sizeof);

	int new_key = 0;
    byte * temp_keys = new byte[leaf->order * key_sizeof];
    uint32_t * temp_pointers = new uint32_t[leaf->order];
    uint32_t * temp_slots = new uint32_t[leaf->order];

    int insert_point = 0;
    while (insert_point < leaf->num_keys && compare(get_key(leaf, insert_point), key) < 0)
        insert_point++;

    int i, j;                   // j is used to skip the insert_point element.
    for (i = 0, j = 0; i < leaf->num_keys; i++, j++) 
    {
        if (j == insert_point) 
            j++;
        set_key(temp_keys, j, get_key(leaf->keys, i));
        temp_pointers[j] = leaf->pointers[i];
        temp_slots[j] = leaf->slots[i];
    }

    set_key(temp_keys, insert_point, key);
    temp_pointers[insert_point] = pointer;
    temp_slots[insert_point] = slot;

    leaf->num_keys = 0;

    int split = (leaf->order) / 2 + (leaf->order % 2);

    for (i = 0; i < split; i++) {
        leaf->pointers[i] = temp_pointers[i];
        leaf->slots[i] = temp_slots[i];
        set_key(leaf->keys, i, get_key(temp_keys, i));
        leaf->num_keys++;
    }

    for (i = split, j = 0; i < leaf->order; i++, j++) {
        new_leaf->pointers[j] = temp_pointers[i];
        new_leaf->slots[j] = temp_slots[i];
        set_key(new_leaf->keys, j, get_key(temp_keys, i));
        new_leaf->num_keys++;
    }

    // note: temp_ is no longer used.
    delete [] temp_pointers;
    delete [] temp_keys;
    delete [] temp_slots;

    new_leaf->pointers[leaf->order - 1] = leaf->pointers[leaf->order - 1];
    leaf->pointers[leaf->order - 1] = new_leaf;

    // clear null point. is not neccessary.
    for (i = leaf->num_keys; i < leaf->order - 1; i++)
    {
    	leaf->pointers[i] = Invalid;
    	leaf->slots[i] = Invalid;
    }
        
    for (i = new_leaf->num_keys; i < leaf->order - 1; i++)
    {
    	new_leaf->pointers[i] = Invalid;
    	new_leaf->slots[i] = Invalid;
    }

    new_leaf->parent = leaf->parent;
    new_key = new_leaf->keys[0];

    // return InsertIntoParent(root, leaf, new_key, new_leaf);
}

void BTree::delete(void * key)
{

}

void BTree::print_tree()
{

}

void BTree::destroy()
{

}

