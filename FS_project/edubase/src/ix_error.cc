//
// File:        ix_error.cc
// Description: IX_PrintError function
// Author:      Hyunjung Park (hyunjung@stanford.edu)
//

#include <cerrno>
#include <cstdio>
#include <iostream>
#include "ix_internal.h"

using namespace std;

//
// Error table
//

static char *IX_WarnMsg[] = {
  (char*)"",
  (char*)"invalid attribute",
  (char*)"null point",
  (char*)"the entry already exists",
  (char*)"the index file is already closed"
};

static char *IX_ErrorMsg[] = {
  (char*)"key size too big",//1
  (char*)"PF operations error",//2
  (char*)"bad index page",//3
  (char*)"undefined:IX_FCREATEFAIL",//4
  (char*)"the exactly same index is already open",//5
  (char*)"the index is already open",//6
  (char*)"undefined:IX_FNOTOPEN",//7
  (char*)"bad rid",//8
  (char*)"bad key",//9
  (char*)"no such entry",//10
  (char*)"key not found",//11
  (char*)"btnode's size(order) is invalid",//12
  (char*)"btnode dosen't have enough keys",//13
  (char*)"the scan is already open",//14
  (char*)"invalid operation",//15
  (char*)"the scan is already closed",//16
  (char*)"negative index number",//17
  (char*)"no file or directory is found for the index",//18
  (char*)"close file which dosen't exist"//19
};
//
// IX_PrintError
//
// Desc: Send a message corresponding to a IX return code to cerr
// In:   rc - return code for which a message is desired
//
void IX_PrintError(RC rc)
{
  // Check the return code is within proper limits
  if (rc >= START_IX_WARN && rc <= IX_LASTWARN)
    // Print warning
    cerr << "IX warning: " << IX_WarnMsg[rc - START_IX_WARN] << "\n";
  else if (-rc >= -START_IX_ERR && -rc <= -IX_LASTERROR)
    // Print error
    cerr << "IX error: " << IX_ErrorMsg[-rc + START_IX_ERR] << "\n";
  else if (rc == 0)
    cerr << "IX_PrintError called with return code of 0\n";
  else
    cerr << "IX error: " << rc << " is out of bounds\n";
}
