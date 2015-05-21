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

SRCS := buffer.cpp hashtable.cpp storage.cpp
OBJS := $(SRCS:.cpp=.o)
LIB := $(LIB_DIR)/libstorage.a

all: $(LIB)

$(LIB):
	$(CXX) -c $(SRCS) -I$(INCLUDE) -L$(LIB_DIR)
	$(AR) $(ARFLAGS) $@ $(OBJS)

clean:
	$(RM) $(OBJS) $(LIB)

.PHONY: all clean