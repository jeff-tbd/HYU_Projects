#
# Makefile for TINY
# Gnu C Version
# K. Louden 2/3/98
#

CC = gcc

CFLAGS = -Wall -g 

OBJS = y.tab.o lex.yy.o main.o util.o symtab.o analyze.o code.o

# OBJC_FLEX = lex.yy.o

cminus: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o cminus -lfl
#by flex
#cminus_flex: $(OBJS_FLEX)
#	$(CC) $(CFLAGS) main.o util.o lex.yy.o -o cminus_flex -lfl

y.tab.o: cminus.y globals.h
	bison -d cminus.y --yacc
	$(CC) $(CFLAGS) -c y.tab.c

main.o: main.c globals.h util.h scan.h
	$(CC) $(CFLAGS) -c main.c

util.o: util.c util.h globals.h
	$(CC) $(CFLAGS) -c util.c

symtab.o: symtab.c symtab.h
	$(CC) $(CFLAGS) -c symtab.c

analyze.o: analyze.c globals.h symtab.h analyze.h
	$(CC) $(CFLAGS) -c analyze.c

code.o: code.c code.h globals.h
	$(CC) $(CFLAGS) -c code.c


lex.yy.o: cminus.l scan.h util.h globals.h
	flex -o lex.yy.c cminus.l
	$(CC) $(CFLAGS) -c lex.yy.c


clean:
	-rm cminus
	-rm $(OBJS)
	-rm y.*
	-rm lex.yy.*

all: cminus

