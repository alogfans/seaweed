# Generic Makefile for Seaweed Project
# Copyright Alogfans (Feng Ren), 2015. All rights reserved.

CC := gcc
CFLAGS := -g -Wall -Werror -Wextra -std=gnu99

CXX := g++
CXXFLAGS := -g -Wall -Werror -Wextra

INCLUDE := ../include
LIBS := ../lib

EXEC := storage_test
DEPS := storage.cpp buffer.cpp hashtable.cpp


$(EXEC):
	$(CXX) $(EXEC).cpp $(DEPS) -o $(EXEC) -I$(INCLUDE) -L$(LIBS) -lgtest -lpthread
	./$(EXEC)
	$(RM) $(EXEC)

clean:
	$(RM) *.o $(EXEC) test.db
