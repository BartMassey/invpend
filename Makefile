# Copyright © 2013 Bart Massey
# [This program is licensed under the "MIT License"]
# Please see the file COPYING in the source
# distribution of this software for license terms.

# Makefile for invpend

CC = gcc -std=c99 -Wall
CFLAGS = -O4
XCFLAGS = `pkg-config --cflags cairo xcb-util xcb`
LIBS = -lm
XLIBS = `pkg-config --libs cairo xcb-util xcb`

invpend: invpend.o
	$(CC) $(CFLAGS) -o invpend invpend.o $(LIBS)

xinvpend: xinvpend.o graphics.o
	$(CC) $(CFLAGS) -o xinvpend xinvpend.o graphics.o $(XLIBS) $(LIBS)

xinvpend.o: invpend.c
	$(CC) $(CFLAGS) $(XCFLAGS) -DX -o xinvpend.o -c invpend.c

clean:
	-rm -f invpend.o xinvpend.o graphics.o invpend xinvpend
