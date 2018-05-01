/****************************************************/
/* File: analyze.c                                  */
/* Semantic analyzer implementation                 */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "analyze.h"

char* funcName;
static int keepLastScope = FALSE;

/* counter for variable memory locations */
static int location = 0;

/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc 
 * in postorder to tree pointed to by t
 */
static void traverse( TreeNode * t,
               void (* preProc) (TreeNode *),
               void (* postProc) (TreeNode *) )
{ if (t != NULL)
  { preProc(t);
    { int i;
      for (i=0; i < MAXCHILDREN; i++)
        traverse(t->child[i],preProc,postProc);
    }
    postProc(t);
    traverse(t->sibling,preProc,postProc);
  }
}

//for built-in function
static void insertIOFunc(void)
{
  st_insert("input",Integer,0,addLocation());
  st_insert("output",Void,0,addLocation());
}


/* nullProc is a do-nothing procedure to 
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode * t)
{ if (t==NULL) return;
  else return;
}

static void symbolError(TreeNode *t, char *message)
{ fprintf(listing,"Symbol error at line %d: %s\n",t->lineno,message);
  Error = TRUE;
}

/* Procedure insertNode inserts 
 * identifiers stored in t into 
 * the symbol table 
 */
static void insertNode( TreeNode * t)
{ char* tmpname;
  switch (t->nodekind)
  { //1. stmtk
    case StmtK:
      switch (t->kind.stmt)
      { case CompK:
          if (keepLastScope) {
              keepLastScope = FALSE;
          } else {
              Scope sc = sc_create(funcName);
              sc_push(sc);
          }
          t->attr.scope = sc_top();
          break;
        default:
          break;
      }
      break;
    //2. expk
    case ExpK:
      switch (t->kind.exp)
      { case IdK:
        case ArrIdK:
        case CallK:
          if (st_lookup(t->attr.name) == NULL)
          /* not yet in table, so treat as error */
            symbolError(t, "undeclared symbol");
          else
          /* already in table, so ignore location, 
             add line number of use only */ 
            st_addLineno(t->attr.name,t->lineno);
          break;
        default:
          break;
      }
      break;
    //3. declk
    case DeclK:
      switch (t->kind.decl)
      { case FuncK:
          funcName = t->attr.name;
          if(st_lookup_excluding_parent(funcName) != NULL){
              symbolError(t,"function already declared");
              break;
          }
          switch(t->child[0]->attr.type)
          { case INT:
              t->type = Integer;
              break;
            case VOID:
            default:
              t->type = Void;
              break;
          }
          st_insert(funcName,t->type,t->lineno,addLocation());
          sc_push(sc_create(funcName));
          keepLastScope = TRUE;
          break;
        case VarK:
        case ArrVarK:
          if(t->child[0]->attr.type == VOID) {
              symbolError(t,"variable should have non-void type");
              break;
          }

          if(t->kind.decl == VarK) {
              tmpname = t->attr.name;
              t->type = Integer;
          } else {
              tmpname = t->attr.arr.name;
              t->type = IntegerArray;
          }

          if(st_lookup_excluding_parent(tmpname) == NULL)
              st_insert(tmpname,t->type,t->lineno,addLocation());
          else
              symbolError(t,"symbol already declared for current scope");
          break;
          default:
          break;
      }
      break;
    //4. paramk
    case ParamK:
      if(t->child[0]->attr.type == VOID)
          symbolError(t->child[0],"void type parameter is not allowed");
      if(st_lookup(t->attr.name) == NULL) {
          if(t->kind.param == NonArrParamK)
              t->type = Integer;
          else
              t->type = IntegerArray;
          st_insert(t->attr.name,t->type,t->lineno,addLocation());
      }
      else
          symbolError(t,"symbol already declared for current scope");
      break;
      default:
      break;
  }
}

static void afterInsertNode(TreeNode *t)
{ 
  switch(t->nodekind)
  { 
    case StmtK:
      switch(t->kind.stmt)
      { 
        case CompK:
          sc_pop();
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

/* Function buildSymtab constructs the symbol 
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode * syntaxTree)
{
  globalScope = sc_create("global");
  sc_push(globalScope);
  insertIOFunc();
  traverse(syntaxTree,insertNode,afterInsertNode);
  sc_pop();
  if (TraceAnalyze)
  { fprintf(listing,"\nSymbol table:\n\n");
    printSymTab(listing);
  }
}

static void typeError(TreeNode * t, char * message)
{ fprintf(listing,"Type error at line %d: %s\n",t->lineno,message);
  Error = TRUE;
}

static void beforeCheckNode(TreeNode *t)
{ 
  switch (t->nodekind)
  { case DeclK:
      switch(t->kind.decl)
      { case FuncK:
          funcName = t->attr.name;
          break;
        default:
          break;
      }
      break;
    case StmtK:
      switch(t->kind.stmt)
      { case CompK:
          sc_push(t->attr.scope);
          break;
        default:
          break;
      }
    default:
      break;
  }
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
static void checkNode(TreeNode * t)
{
  ExpType leftType,rightType;
  TokenType op;
  BucketList bucket;
  ExpType exptype;
  TreeNode *arg;
  
  switch (t->nodekind)
  { case ExpK:
      switch (t->kind.exp)
      { 
        case OpK:
      //fprintf(listing,"case op\n");
          leftType = t->child[0]->type;
          rightType = t->child[1]->type;
          op = t->attr.op;

          if ((t->child[0]->type != Integer) || (t->child[1]->type != Integer))
          {
            typeError(t,"Op applied to non-integer");
          }
          /*else if(leftType == IntegerArray && rightType == IntegerArray)
          {
            typeError(t,"non both of operands can be array");
          }
          else if(op == MINUS &&
              leftType == Integer &&
              rightType == IntegerArray)
          {
            typeError(t,"invalid operands to binary expression");
          }
          else if((op == TIMES || op == OVER) &&
              (leftType == IntegerArray ||
               rightType == IntegerArray))
          {
            typeError(t,"invalid operands to binary expression");
          }*/
            //try test
          else if ((t->attr.op == EQ) || (t->attr.op == LT) ||
              (t->attr.op == LE) || (t->attr.op == GT) ||
              (t->attr.op == GE) || (t->attr.op == NE)){
            t->type = Boolean;
          }
            //////
          else {
            t->type = Integer;
          }
          break;
        case ConstK:
          t->type = Integer;
          break;
        case IdK: 
        case ArrIdK:
          bucket = st_lookup(t->attr.name);
          if(bucket == NULL) break;
          if(t->kind.exp == ArrIdK) {
              if(t->child[0]->type != Integer)
                typeError(t,"Index expression should have integer type");
              else
                t->type = Integer;
          } else {
              t->type = bucket->type;
          }
          break;          
        case AssignK:
        //fprintf(listing,"AssignK check\n");
          if(t->child[0]->type == IntegerArray)
              typeError(t->child[0],"assignment to array variable");
          else if (t->child[1]->type == Void)
              typeError(t->child[0],"assignment of void value");
          else
              t->type = t->child[0]->type;
          break;
        case CallK:
          bucket = st_lookup(t->attr.name);

          if(bucket == NULL)
          {
              typeError(t,"there is no function");
              break;
          }
          exptype = bucket->type;

          arg = t->child[0];
          
          while(arg != NULL)
          { if (arg->type == Void)
              typeError(arg,"void value cannot be passed as an argument");
            else {
              arg = arg->sibling;
              continue;
            }
            break;
          }

          t->type = exptype;
          break;
          //////not yet complete
        default:
          break;
      }
      break;
    case StmtK:
      switch (t->kind.stmt)
      { case IfK:
          if (t->child[0]->type == Void)
            typeError(t->child[0],"if test has void value");
          break;
        case CompK:
          sc_pop();
          break;
        case RetK:
        //fprintf(listing,"retK check\n");
          exptype = st_lookup(funcName)->type;
          arg = t->child[0];
          if (exptype == Void &&
              (arg != NULL && arg->type != Void)) {
              typeError(t,"expected no return value");
          } else if (exptype == Integer &&
                    (arg == NULL || arg->type == Void)) {
              typeError(t,"expected return value");
          }
          break;
        case IterK:
        //fprintf(listing,"iterK check\n");
          if (t->child[0]->type == Void)
            typeError(t->child[0],"repeat test has void value");
          break;
        default:
          break;
      }
      break;
    default:
      break;

  }
}

/* Procedure typeCheck performs type checking 
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode * syntaxTree)
{
  sc_push(globalScope);
  fprintf(listing,"current scope : %s\n",sc_top()->name);
  traverse(syntaxTree,beforeCheckNode,checkNode);
  sc_pop();
}
