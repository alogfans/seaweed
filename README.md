Seaweed
---

A simple relational database implementation. 

Copyright 2015 Alogfans. All rights reserved.

## Introduction

The `Seaweed` project is a simple implementation of relation database management system
with concurrent support. We constructed as C/S architecture, and allowing multiple clients
connecting in one server and do some transations. It also supports standard SQL statements
and other functions like views or triggers. 

The overall design is inspired by the `REDBASE` project and SQLITE project.

## Installation

This system supports UNIX currently. After changing the directory by `cd` commands, just 
enter
	
	make

is OK.