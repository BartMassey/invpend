# Copyright © 2013 Bart Massey
# [This program is licensed under the "MIT License"]
# Please see the file COPYING in the source
# distribution of this software for license terms.

# Makefile for invpend

CC = gcc -std=c99 -Wall
CFLAGS = -O4
LIBS = -lcairo -lxcb-util -lxcb -lm

invpend: invpend.o graphics.o
	$(CC) $(CFLAGS) -o invpend invpend.o graphics.o $(LIBS)

