
//
// File:        redbase.cc
// Description: redbase command line utility
// Author:      Hyunjung Park (hyunjung@stanford.edu)
//

#include <iostream>
#include <cstring>
#include <unistd.h>
#include "redbase.h"
#include "parser.h"
#include "sm.h"
#include "ql.h"

using namespace std;

//
// main
//

int main(int argc, char *argv[])
{
   //
   // Global PF_Manager and RM_Manager variables
   //
   PF_Manager pfm;
   RM_Manager rmm(pfm);
   IX_Manager ixm(pfm);
   SM_Manager smm(ixm, rmm);
   QL_Manager qlm(smm, ixm, rmm);


   RC rc;
   char *dbname = "student";

   // Open DB
   if (OK_RC == (rc = smm.OpenDb(dbname))) {

      RelAttr attrs[] = {
         { "student", "no"},
         { "student", "name"},
         { "student", "dept"},
         { "student", "GPA"},
         { "student", "comment"}
      };
      RelAttr attrs2[]={{"student","GPA"}};

      char * relations[] = {"student"};
      Condition condition1 ;
      condition1.op=EQ_OP;
      condition1.lhsAttr = RelAttr();
      condition1.lhsAttr.attrName = "GPA";
      condition1.lhsAttr.relName = "student";
      condition1.bRhsIsAttr =false;
      condition1.rhsAttr =RelAttr();
      condition1.rhsAttr.attrName = "GPA";
      condition1.rhsAttr.relName = "student";
      float tmp = 4.0000;
      void * temp =(void *) &tmp;
      Value va1 ;
      va1.type =FLOAT;
      va1.data = temp;
      condition1.rhsValue= va1;
      Condition conditions_[] = {Condition()};
   //   qlm.Select(5, attrs, 1, relations, 0, conditions_);
      RelAttr rhsUAttr = RelAttr();
      rhsUAttr.relName = "student";
      rhsUAttr.attrName = "GPA";
      Value rhsUVal = Value();
      rhsUVal.data=temp;
      rhsUVal.type = FLOAT;
       qlm.Update(relations[0],attrs2[0],true,rhsUAttr,rhsUVal,0,conditions_);
      // Close DB
      if ((rc = smm.CloseDb())) {
         PrintError(rc);
         goto err_exit;
      }

      cout << "Bye.\n";
      return (0);

   } else {
      PrintError(rc);
      goto err_exit;
   }

   // Return error
err_exit:
   return (1);
}
