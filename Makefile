# Generic Makefile for Seaweed Project
# Copyright Alogfans (Feng Ren), 2015. All rights reserved.

CC := gcc
CFLAGS := -g -Wall -Werror -Wextra -std=gnu99

CXX := g++
CXXFLAGS := -g -Wall -Werror -Wextra

AR := ar
ARFLAGS := cr

SUB_DIRS := storage index entity schema console

INCLUDE := ./include
LIB_DIR := ./lib

LIBS := -lconsole -lschema -lentity -lindex -lstorage 

SRCS := main.cpp
TARGET := seaweed

all: $(TARGET)

$(TARGET):
	for dir in $(SUB_DIRS); \
	do make -C $$dir all || exit 1; \
	done
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) -I$(INCLUDE) -L$(LIB_DIR) $(LIBS)

clean:
	for dir in $(SUB_DIRS); \
	do make -C $$dir clean || exit 1; \
	done
	$(RM) $(OBJS) $(TARGET)

.PHONY: all clean