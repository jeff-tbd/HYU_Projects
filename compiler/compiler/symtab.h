/****************************************************/
/* File: symtab.h                                   */
/* Symbol table interface for the TINY compiler     */
/* (allows only one symbol table)                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#include "globals.h"

#define SIZE 211

/* the list of line numbers of the source
* code in which a variable is referenced
*/
typedef struct LineListRec
{
    int lineno;
    struct LineListRec* next;
} * LineList;

/* The record in the bucket lists for
 * each variable, including name,
 * expression type, assigned memory location,
 * and the list of the line numbers in which
 * it appears in the source code
 */
typedef struct BucketListRec
{
    char* name;
    ExpType type;
    LineList lines;
    int memloc;
    struct BucketListRec * next;
} * BucketList;


typedef struct ScopeRec
{
    char * name;
    int nestedLevel;
    BucketList hashTable[SIZE];
    struct ScopeListRec * parent;
} * Scope;

Scope globalScope;

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert( char * name, ExpType type, int lineno, int loc );

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
BucketList st_lookup ( char * name );
BucketList st_lookup_excluding_parent ( char * name );

/* for implement static scope */
void sc_pop(void);
void sc_push(Scope scope);
Scope sc_top(void);
int addLocation(void);
void st_addLineno(char* name, int lineno);

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing);

#endif
