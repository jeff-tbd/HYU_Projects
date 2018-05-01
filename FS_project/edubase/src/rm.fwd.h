/* 
 * File:   rm.fwd.h
 * Author: zliu
 *
 * Created on May 22, 2013, 12:43 AM
 */

#ifndef RM_FWD_H
#define	RM_FWD_H

#include "redbase.h"
#include "pf.fwd.h"

#define RM_INVIABLERID     (START_RM_WARN + 0) // inviable rid
#define RM_UNREADRECORD    (START_RM_WARN + 1) // unread record
#define RM_INVALIDRECSIZE  (START_RM_WARN + 2) // invalid record size
#define RM_INVALIDSLOTNUM  (START_RM_WARN + 3) // invalid slot number
#define RM_RECORDNOTFOUND  (START_RM_WARN + 4) // record not found
#define RM_INVALIDCOMPOP   (START_RM_WARN + 5) // invalid comparison operator
#define RM_INVALIDATTR     (START_RM_WARN + 6) // invalid attribute parameters
#define RM_NULLPOINTER     (START_RM_WARN + 7) // pointer is null
#define RM_SCANOPEN        (START_RM_WARN + 8) // scan is open
#define RM_CLOSEDSCAN      (START_RM_WARN + 9) // scan is closed
#define RM_CLOSEDFILE      (START_RM_WARN + 10)// file handle is closed
#define RM_LASTWARN        RM_CLOSEDFILE

#define RM_EOF             PF_EOF              // work-around for rm_test

struct RM_PageHdr;
class RM_Record;
struct RM_FileHdr;
class RM_FileHandle;
class RM_FileScan;
class RM_Manager;

#define RM_PAGE_LIST_END  -1       // end of list of free pages
#define RM_PAGE_FULL      -2       // all slots in the page are full

//
// Constants and defines
//
const int RM_HEADER_PAGE_NUM = 0;

#endif	/* RM_FWD_H */

