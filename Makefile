# Copyright © 2013 Bart Massey
# [This program is licensed under the "MIT License"]
# Please see the file COPYING in the source
# distribution of this software for license terms.

# Makefile for invpend

CC = gcc -std=c99 -Wall
CFLAGS = -O4 `pkg-config --cflags xcb-util xcb-present xcb`
LIBS = -lcairo `pkg-config --libs xcb-util xcb-present xcb` -lm

all: invpend xinvpend

invpend: invpend.o
	$(CC) $(CFLAGS) -o invpend invpend.o $(LIBS)

xinvpend: xinvpend.o graphics.o
	$(CC) $(CFLAGS) -o xinvpend xinvpend.o graphics.o $(LIBS)

xinvpend.o: invpend.c
	$(CC) $(CFLAGS) -DX -o xinvpend.o -c invpend.c
