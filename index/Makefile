# Generic Makefile for Seaweed Project
# Copyright Alogfans (Feng Ren), 2015. All rights reserved.

CC := gcc
CFLAGS := -g -Wall -Werror -Wextra -std=gnu99

CXX := g++
CXXFLAGS := -g -Wall -Werror -Wextra

AR := ar
ARFLAGS := cr

INCLUDE := ../include
LIB_DIR := ../lib

SRCS := btnode.cpp btree.cpp index.cpp
OBJS := $(SRCS:.cpp=.o)
LIB := $(LIB_DIR)/libindex.a

all: $(LIB)

$(LIB):
	$(CXX) -c $(SRCS) -I$(INCLUDE) -L$(LIB_DIR)
	$(AR) $(ARFLAGS) $@ $(OBJS)

clean:
	$(RM) $(OBJS) $(LIB)

.PHONY: all clean