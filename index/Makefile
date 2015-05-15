# Generic Makefile for Seaweed Project
# Copyright Alogfans (Feng Ren), 2015. All rights reserved.

CC := gcc
CFLAGS := -g -Wall -Werror -Wextra -std=gnu99

CXX := g++
CXXFLAGS := -g -Wall -Werror -Wextra

INCLUDE := ../include
LIBS := ../lib

EXEC := index_test
DEPS := index.cpp btree.cpp btnode.cpp ../storage/storage.cpp ../storage/hashtable.cpp ../storage/buffer.cpp ../entity/entity.cpp ../entity/entity_bitmap.cpp ../entity/record.cpp



$(EXEC):
	$(CXX) $(EXEC).cpp $(DEPS) -o $(EXEC) -I$(INCLUDE) -L$(LIBS) -lgtest -lpthread
	./$(EXEC)
	$(RM) $(EXEC)

clean:
	$(RM) *.o $(EXEC) *.db
