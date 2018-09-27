
CC=g++
CFLAGS:=-g

LIBS:=-lpthread

all:snake

snake:node.cpp node.h
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

.PHONY:clean

clean:
	$(RM) snake
