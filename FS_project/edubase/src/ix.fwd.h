//
// ix.h
//
//   Index Manager Component Interface
//

#ifndef IX_FWD_H
#define IX_FWD_H

// Please do not include any other files than the ones below in this file.

#include "redbase.h"  // Please don't change these lines
#include "rm_rid.h"  // Please don't change these lines
#include "pf.fwd.h"

class IX_BTNode;

////////////////////////////////////////////////////////////////////
//
// IX_IndexHandle: IX Index File interface
//
class IX_IndexHandle;

//
// IX_IndexScan: condition-based scan of index entries
//
class IX_IndexScan;

//
// IX_Manager: provides IX index file management
//
class IX_Manager;

//
// Print-error function
//
void IX_PrintError(RC rc);

#define IX_INVALIDATTR     (START_IX_WARN + 1) // invalid attribute parameters
#define IX_NULLPOINTER     (START_IX_WARN + 2) // pointer is null
#define IX_ENTRYEXISTS     (START_IX_WARN + 3) //
#define IX_CLOSEDFILE      (START_IX_WARN + 4) // index handle is closed
#define IX_LASTWARN        IX_CLOSEDFILE
//#define START_IX_ERR  (-201) in redbase.h
#define IX_SIZETOOBIG      (START_IX_ERR - 0)  // key size too big
#define IX_PF              (START_IX_ERR - 1)  // error in PF operations
#define IX_BADIXPAGE       (START_IX_ERR - 2)  // bad page
#define IX_FCREATEFAIL     (START_IX_ERR - 3)  //
#define IX_HANDLEOPEN      (START_IX_ERR - 4)  // error handle open
#define IX_BADOPEN         (START_IX_ERR - 5)  // the index is alread open
#define IX_FNOTOPEN        (START_IX_ERR - 6)  //
#define IX_BADRID          (START_IX_ERR - 7)  // bad rid
#define IX_BADKEY          (START_IX_ERR - 8)  // bad key
#define IX_NOSUCHENTRY     (START_IX_ERR - 9)  //
#define IX_KEYNOTFOUND     (START_IX_ERR - 10) // key is not found
#define IX_INVALIDSIZE     (START_IX_ERR - 11) // btnode's size(order) is invalid
#define IX_KEYNOTENGOUGH   (START_IX_ERR - 12) // btnode has not enough keys
#define IX_SCANOPEN        (START_IX_ERR - 13) //
#define IX_INVALIDCOMPOP   (START_IX_ERR - 14) //
#define IX_CLOSEDSCAN      (START_IX_ERR - 15) //
#define IX_NEGATIVEINDEXNUM  (START_IX_ERR - 16) //
#define IX_NONEXISTFILE    (START_IX_ERR - 17)//
#define IX_CLOSEFILENONEXIST (START_IX_ERR - 18)//
#define IX_LASTERROR       IX_CLOSEFILENONEXIST  // end of file

#define IX_EOF             PF_EOF              // work-around for rm_test
#endif
