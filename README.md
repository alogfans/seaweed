# Seaweed

Copyright 2015 Feng Ren(Nickname: Alogfans), All Rights Reserved.

## Introduction

Seaweed is a simple example that demonstrates the implementation of database systems. 
This project is inspired by REDBASE project, designed by Jennifer Widom, professor of 
Stanford University. However the implementation may be similar to SQLite, a perfect 
desktop and embedded database.

In this first released version, it just support a set of programming interfaces. Any 
C++ program can embedded it easily.

## Compile and Test

After clone it by git command, you can compile it by yourself. No third-party library
will be used, and the lastest compilation toolchian is the only prerequisite.

If you use Ubuntu as experiment platform, you may need to update compile tools by entering
the following command in terminal. (root permission may be required)
	
	sudo apt-get install build-essential bison flex

Note that in the later updates, I may support SQL command interpret with bison and flex.

Now everything is ready. Please open an new terminal and change current directory to the 
root of seaweed project, and then execute

	make

is OK. To clean middle files including executives, run
	
	make clean

The interfaces of this project will be described in the detailed documentation in latter 
updates. You can study code in `include/seaweed.h` to understand what you can do. And this
file should be included in your programs.

The `main.cpp` file in the root directory is a demostrate of how to use these APIs.

WARNING: This database is only for education purpose, that means that its effecient may
be extremely low in some conditions, and the fault tolerence may be incompleted. If you
want to do something in serious applications, please study the sqlite guide, which may
be more suitable for you.

Technical details can be learnt from `doc/paper.pdf`, but now I haven't completed it yet ^-^
