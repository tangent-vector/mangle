# Makefile for mangle project
#
# Users of the application usually shouldn't need to touch this.

CC=cc
CFLAGS=

SOURCES= source/*.md README.md

default: mangle

bootstrap:
	$(CC) -o bootstrap mangle.c	

mangle.c: $(SOURCES) bootstrap
	./bootstrap $(SOURCES)

mangle: mangle.c
	$(CC) -o mangle mangle.c

clean:
	rm mangle