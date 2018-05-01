/****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the TINY compiler*/
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

/* SIZE is the size of the hash table */
#define SIZE 211

/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4

#define MAX_SCOPE 1000

/* the hash function */
static int hash ( char * key )
{ int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  { temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

static Scope scopeStack[MAX_SCOPE];
static int nScopeStack = 0;
static Scope scopes[MAX_SCOPE];
static int nScope = 0;
static int location[MAX_SCOPE];

Scope sc_top(void)
{ return scopeStack[nScopeStack - 1];
}

void sc_pop(void)
{ --nScopeStack;
}

void sc_push(Scope scope)
{ 
    scopeStack[nScopeStack] = scope;
    location[nScopeStack++] = 0;
}

int addLocation(void)
{ return location[nScopeStack-1]++;
}

Scope sc_create(char* funcName)
{ 
  Scope newscope = (Scope)malloc(sizeof(struct ScopeRec));
  newscope->name = funcName;
  newscope->nestedLevel = nScopeStack;
  newscope->parent = sc_top();
  scopes[nScope++] = newscope;
  return newscope;
}
/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void st_insert( char * name, ExpType type, int lineno, int loc )
{
  Scope sc =sc_top();
  int h = hash(name);
  BucketList l =  sc->hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l == NULL) /* variable not yet in table */
  { l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->name = name;
    l->type = type;
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = lineno;
    l->memloc = loc;
    l->lines->next = NULL;
    l->next = sc->hashTable[h];
    sc->hashTable[h] = l; }
  else /* found in table, so just add line number */
  { /* error -> follow parent */
  }
} /* st_insert */

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
BucketList st_lookup ( char * name )
{ Scope sc=sc_top();
  int h = hash(name);
  while(sc)
  {
    BucketList l =  sc->hashTable[h];
      while ((l != NULL) && (strcmp(name,l->name) != 0))
        l = l->next;
      if (l != NULL) return l;
      sc = sc->parent;
  }
  return NULL;
}

BucketList st_lookup_excluding_parent ( char * name )
{ Scope sc=sc_top();
  int h = hash(name);
  BucketList l = sc->hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l == NULL) return NULL;
  else return l;
}

void st_addLineno(char* name, int lineno)
{
    BucketList l = st_lookup(name);
    LineList t = l->lines;
    while (t->next != NULL) t = t->next;
    t->next = (LineList) malloc(sizeof(struct LineListRec));
    t->next->lineno = lineno;
    t->next->next = NULL;
    //
}

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing)
{ int i,j;
  for (i=0;i<nScope;++i)
  { 
    Scope sc = scopes[i];

    fprintf(listing,"Scope  Variable Name   Type   Location   Line Numbers\n");
    fprintf(listing,"-----  -------------   ----   --------   ------------\n");

    for (j=0;j<SIZE;++j)
    { 
      if(sc->hashTable[j] != NULL)
      { 
        BucketList l = sc->hashTable[j];
        while (l != NULL)
        { LineList t = l->lines;
          fprintf(listing,"%s(%d)",sc->name,sc->nestedLevel);
          fprintf(listing,"%-14s ",l->name);
          switch(l->type)
          {
              case Void:
                fprintf(listing,"Void     ");
                break;
              case Integer:
                fprintf(listing,"Integer  ");
                break;
              case Boolean:
                fprintf(listing,"Boolean  ");
                break;
              case IntegerArray:
                fprintf(listing,"IntegerArray  ");
                break;
          }

          //fprintf(listing,"%-14s ",l->type);
          fprintf(listing,"%-8d  ",l->memloc);
          while (t != NULL)
          { fprintf(listing,"%4d ",t->lineno);
            t = t->next;
          }
          fprintf(listing,"\n");
          l = l->next;
        }
      }
    }
    //end scope[i]

  }
} /* printSymTab */
