#
# Makefile for TINY
# Gnu C Version
# K. Louden 2/3/98
#

CC = gcc

CFLAGS =  

OBJS = main.o util.o scan.o

OBJC_FLEX = lex.yy.o

cminus: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o cminus
#by flex
cminus_flex: $(OBJS_FLEX)
	$(CC) $(CFLAGS) main.o util.o lex.yy.o -o cminus_flex -lfl


main.o: main.c globals.h util.h scan.h
	$(CC) $(CFLAGS) -c main.c

util.o: util.c util.h globals.h
	$(CC) $(CFLAGS) -c util.c

scan.o: scan.c scan.h util.h globals.h
	$(CC) $(CFLAGS) -c scan.c

lex.yy.o: cminus.l scan.h util.h globals.h
	flex cminus.l
	$(CC) $(CFLAGS) -c lex.yy.c -lfl


clean:
	-rm cminus
	-rm $(OBJS)
	-rm lex.yy.*


all: cminus cminus_flex

