# Copyright © 2013 Bart Massey
# [This program is licensed under the "MIT License"]
# Please see the file COPYING in the source
# distribution of this software for license terms.

# Makefile for invpend

CC = gcc -std=c99
CFLAGS = -g -Wall
LIBS = -lm

invpend: invpend.c
	$(CC) $(CFLAGS) -o invpend invpend.c $(LIBS)
