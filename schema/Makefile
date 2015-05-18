# Generic Makefile for Seaweed Project
# Copyright Alogfans (Feng Ren), 2015. All rights reserved.

CC := gcc
CFLAGS := -g -Wall -Werror -Wextra -std=gnu99

CXX := g++
CXXFLAGS := -g -Wall -Werror -Wextra

INCLUDE := ../include
LIBS := ../lib

EXEC := schema_test
DEPS := schema.cpp ../index/btnode.cpp ../index/btree.cpp ../index/index.cpp ../entity/record.cpp ../entity/entity.cpp ../entity/entity_bitmap.cpp ../storage/storage.cpp ../storage/hashtable.cpp ../storage/buffer.cpp 


$(EXEC):
	$(CXX) $(EXEC).cpp $(DEPS) -o $(EXEC) -I$(INCLUDE) -L$(LIBS) -lgtest -lpthread
	./$(EXEC)
	$(RM) $(EXEC)

clean:
	$(RM) *.o $(EXEC) test.db
