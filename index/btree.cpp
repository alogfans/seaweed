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

int BTree::compare(void * lhs, void * rhs)
{
    switch (key_type)
    {
    case IntegerType:
        if (* ((int *) lhs) > * ((int *) rhs))
            return 1;
        if (* ((int *) lhs) < * ((int *) rhs))
            return -1;
        return 0;
    case RealType:
        if (* ((float *) lhs) > * ((float *) rhs))
            return 1;
        if (* ((float *) lhs) < * ((float *) rhs))
            return -1;
        return 0;
    case CharType:
        return strncmp((char *) lhs, (char *) rhs, key_sizeof);
    }
    return -2;
}

RID BTree::find(void * key)
{
	if (root < 0)
		return NULL;
	BTNode * leaf = find_leaf(key);

	int slot;
	for (slot = 0; slot < leaf->num_keys; slot++)
		if (compare(leaf->keys[slot], key) == 0)
			break;

	if (slot == leaf->num_keys)
		return RID();
    
    RID result;
    result.page_num = leaf.pointers[slot];
    result.slot_num = leaf.slots[slot];

    close_page(leaf);
    return result;
}

BTNode * BTree::find_leaf(void * key)
{
    BTNode * node = load_page(root);
    int next;
    while (node->is_leaf == false)
    {
        int slot;
        while (slot < node->num_keys && compare(key, node->keys[slot]) >= 0)
            slot++;
        next = node->pointers[slot];
        close_page(node);
        node = load_page(next);
    }
    return node;
}

void BTree::insert(void * key, RID &loc)
{
	if (find(key) != RID())
		return ;

	if (root < 0)
		initize_root(key, loc);
	else
	{
		BTNode * leaf = find_leaf(key);
		if (leaf->num_keys < leaf->order - 1)
			insert_leaf(leaf, key, loc);
		else
			insert_leaf_spilt(leaf, key, loc);
		close_page(leaf);
	}
}

void BTree::initize_root(void * key, RID &loc)
{
    // request for a valid page for storage layer
    BTNode node;
    int page_num = lease_page();
    node.page_num = page_num;
    node.create_block(true, key_type, key_sizeof);
    assign(node.keys[0], key);
    node.pointers[0] = loc.page_num;
    node.slots[0] = loc.slot_num;

    node.num_keys++;
    close_page(&node);
    root = page_num;
}

void BTree::insert_leaf(BTNode * leaf, void * key, RID &loc)
{
    int insert_point = 0;
    while (insert_point < leaf->num_keys && compare(leaf->keys[insert_point], key) < 0)
        insert_point++;

    for (int i = leaf->num_keys; i > insert_point; i--)
    {
        assign(leaf->keys[i], leaf->keys[i - 1]);
        leaf->pointers[i] = leaf->pointers[i - 1];
        leaf->slots[i] = leaf->slots[i - 1];
    }

    assign(leaf->keys[insert_point], key);

    leaf->pointers[insert_point] = loc.page_num;
    leaf->slots[insert_point] = loc.slot_num;
    leaf->num_keys++;
}

void BTree::insert_leaf_spilt(BTNode * leaf, void * key, RID &loc)
{
	// this function attains a previous root page and spilt the leaf with two. Remember
	// that the root may be modified too!
    BTNode * new_leaf = new BTNode();
    int page_num = lease_page();
    new_leaf->page_num = page_num;

	new_leaf->create_block(true, key_type, key_sizeof);

	byte * new_key = new byte[key_sizeof];
    // byte * temp_keys = new byte[leaf->order * key_sizeof];
    byte ** temp_keys = new (byte *)[leaf->order];
    for (int i = 0; i < leaf->order; i++)
        temp_keys[i] = new byte[key_sizeof];

    int * temp_pointers = new int[leaf->order];
    int * temp_slots = new int[leaf->order];

    int insert_point = 0;
    while (insert_point < leaf->num_keys && compare(leaf[insert_point], key) < 0)
        insert_point++;

    int i, j;                   // j is used to skip the insert_point element.
    for (i = 0, j = 0; i < leaf->num_keys; i++, j++) 
    {
        if (j == insert_point) 
            j++;
        assign(temp_keys[j], leaf->keys[i]);
        temp_pointers[j] = leaf->pointers[i];
        temp_slots[j] = leaf->slots[i];
    }

    assign(temp_keys[insert_point], key);
    temp_pointers[insert_point] = pointer;
    temp_slots[insert_point] = slot;

    leaf->num_keys = 0;

    int split = (leaf->order + 1) / 2;

    for (i = 0; i < split; i++) 
    {
        leaf->pointers[i] = temp_pointers[i];
        leaf->slots[i] = temp_slots[i];
        assign(leaf->keys[i], temp_keys[i]);
        leaf->num_keys++;
    }

    for (i = split, j = 0; i < leaf->order; i++, j++) 
    {
        new_leaf->pointers[j] = temp_pointers[i];
        new_leaf->slots[j] = temp_slots[i];
        assign(leaf->keys[j], temp_keys[i]);
        new_leaf->num_keys++;
    }

    // note: temp_ is no longer used.
    delete [] temp_pointers;
    delete [] temp_slots;
    for (int i = 0; i < leaf->order; i++)
        delete [] temp_keys[i];
    delete [] temp_keys;
    
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
    assign(new_key, new_leaf->keys[0]);

    insert_into_parent(leaf, new_leaf, new_key);

    delete [] new_key;
    close_page(new_leaf);
}

void BTree::insert_into_parent(BTNode * left, BTNode * right, void * key)
{
    if (left->parent == NULL)
        insert_into_new_root(left, right, key);
    else
    {
        BTNode * parent = load_page(left->parent)
        int left_index = get_child_indexof(parent, left);

        if (parent->num_keys < parent->order - 1)
            insert_node(parent, left_index, key, right);
        else
            insert_node_split(parent, left_index, key, right);

        close_page(parent);      
    }
}

void BTree::insert_into_new_root(BTNode * left, BTNode * right, void * key)
{
    // request for a valid page for storage layer
    BTNode node;
    root = lease_page();
    node.page_num = root;
    node.create_block(false, key_type, key_sizeof);
    node.keys[0] = key;
    node.pointers[0] = left;
    node.pointers[1] = right;
    node.next = next_root;
    node.num_keys++;
    left->parent = root;
    right->parent = root;
    close_page(&node);
}

void BTree::insert_node(BTNode * parent, int left_index, void * key, BTNode * right)
{
    for (int i = parent->num_keys; i > left_index; i--)
    {
        parent->pointers[i + 1] = parent->pointers[i];
        assign(parent->keys[i], parent->keys[i - 1]);
    }

    parent->pointers[left_index + 1] = right;
    assign(parent->keys[left_index], key);
    parent->num_keys++;
}

void BTree::insert_node_split(BTNode * old_node, int left_index, void * key, BTNode * right)
{
    BTNode * new_node = new BTNode();
    new_node->page_num = lease_page();
    new_node->create_block(false, key_type, key_sizeof);

    int i, j;

    byte ** temp_keys = new (byte *)[old_node->order];
    for (int i = 0; i < old_node->order; i++)
        temp_keys[i] = new byte[key_sizeof];

    int * temp_pointers = new int[old_node->order + 1];

    for (i = 0, j = 0; i < old_node->num_keys + 1; i++, j++) 
    {
        if (j == left_index + 1) 
            j++;
        temp_pointers[j] = old_node->pointers[i];
    }

    for (i = 0, j = 0; i < old_node->num_keys; i++, j++) 
    {
        if (j == left_index) 
            j++;
        assign(temp_keys[j], old_node->keys[i]);
    }

    temp_pointers[left_index + 1] = right->page_num;
    temp_keys[left_index] = key;
    
    int split = (old_node->order + 1) / 2;
    old_node->num_keys = 0;

    for (i = 0; i < split - 1; i++) 
    {
        old_node->pointers[i] = temp_pointers[i];
        assign(old_node->keys[i], temp_keys[i]);
        old_node->num_keys++;
    }

    old_node->pointers[i] = temp_pointers[i];

    byte * new_key = new byte[key_sizeof];
    assign(new_key, temp_keys[split - 1]);

    for (++i, j = 0; i < Order; i++, j++) 
    {
        new_node->pointers[j] = temp_pointers[i];
        assign(new_node->keys[j], temp_keys[i]);
        new_node->num_keys++;
    }

    new_node->pointers[j] = temp_pointers[i];
    new_node->parent = old_node->parent;

    delete [] temp_pointers;
    for (int i = 0; i < leaf->order; i++)
        delete [] temp_keys[i];
    delete [] temp_keys;

    for (i = 0; i <= new_node->num_keys; i++) 
    {
        BTNode * child = load_page(new_node->pointers[i]);
        child->parent = new_node->page_num;
        close_page(child);
    }

    insert_into_parent(old_node, new_node, k_prime);
    close_page(new_node);
}


int BTree::get_child_indexof(BTNode * parent, BTNode * child)
{
    int left_index = 0;
    while (left_index <= parent->num_keys && parent->pointers[left_index] != child.page_num)
        left_index++;
    return left_index;
}

void BTree::remove(void * key)
{
    BTNode * key_leaf = FindLeaf(key);
    if (key_leaf == NULL)
        return ;
    RID found = find(key);
    if (found == RID())
        return ;
    remove_entry(key_leaf, key);
}
// very simple: tombstone impl.
void BTree::remove_entry(BTNode * node, void * key)
{
    int left = 0;

    while (compare(node->keys[i], key) != 0)
        left++;

    for (int i = left + 1; i < node->num_keys; i++)
        assign(node->keys[i - 1], node->keys[i]);

    for (int i = left + 1; i < node->order - 1; i++)
    {
        node->pointers[i - 1] = node->pointers[i]; 
        node->slots[i - 1] = node->slots[i];      
    }

    node->num_keys--;
}
