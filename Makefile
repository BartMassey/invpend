# Copyright © 2013 Bart Massey
# Makefile for invpend

CC = gcc -std=c99
CFLAGS = -g -Wall
LIBS = -lm

invpend: invpend.c
	$(CC) $(CFLAGS) -o invpend invpend.c $(LIBS)
