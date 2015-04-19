// entity_bitmap.cpp
//
// Part of seaweed project. 
// Copyright <C> Alogfans 2015, All rights reserved.
// 
// The implementation file of entity bitmap definitions.

#include "entity.h"
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <iostream>

using namespace std;

EntityBitmap::EntityBitmap(int nslots) : nslots(nslots)
{
	//cout << nslots << endl;
	nbytes = nslots / 8 + ((nslots % 8 == 0) ? 0 : 1);
	//cout << nbytes << endl;
	bitmap = new byte[nbytes];
}

EntityBitmap::~EntityBitmap()
{
	delete bitmap;
}

void EntityBitmap::load(byte * buffer)
{
	memcpy(bitmap, buffer, nbytes);
}

void EntityBitmap::store(byte * buffer)
{
	memcpy(buffer, bitmap, nbytes);
}

bool EntityBitmap::test(int bit)
{
	if (bit < 0 || bit >= nslots)
		throw logic_error("bit overflow");

	int byte_idx = bit / 8;
	int bit_idx = bit % 8;

	return (bitmap[byte_idx] >> bit_idx) & 0x1;
}

void EntityBitmap::set(int bit)
{
	if (bit < 0 || bit >= nslots)
		throw logic_error("bit overflow");

	int byte_idx = bit / 8;
	int bit_idx = bit % 8;

	bitmap[byte_idx] |= 0x1 << bit_idx;
}

void EntityBitmap::clear(int bit)
{
	if (bit < 0 || bit >= nslots)
		throw logic_error("bit overflow");

	int byte_idx = bit / 8;
	int bit_idx = bit % 8;

	bitmap[byte_idx] &= ~(0x1 << bit_idx);
}

void EntityBitmap::clear()
{
	memset(bitmap, 0, nbytes);
}

int EntityBitmap::get_bytes()
{
	return nbytes;
}