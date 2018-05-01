/* 
 * File:   ix_internal.h
 * Author: wwt
 *
 * Created on May 20, 2013, 11:51 PM
 */
/// it's for constants and defines 
#ifndef IX_INTERNAL_H
#define	IX_INTERNAL_H

#include <cstdlib>
#include <cstring>
#include <ostream>
#include <iostream>
#include <assert.h>
#include <math.h>       /* floor */
#include <assert.h>

#include "ix.h"


#define IX_PAGE_LIST_END  -1       // end of list of free pages
#define IX_PAGE_FULL      -2       // all slots in the page are full

//
// Constants and defines
//
const int IX_HEADER_PAGE_NUM = 0;

//
// IX_PageHdr: Header structure for pages
//
//struct IX_PageHdr {
//   PageNum nextPage;
//};

enum PageType {Leaf, NonLeaf};

struct _IX_PageHdr {
   PageNum LeftPage;//for internal node: this points to the son node; for the leaf node, this points the left brother
   PageNum RightPage;//for internal node: this points to its parent; for the leaf node, this points the right brother
                     //for the root node: this points to the first leaf node
   int entryNumbre;
   int pageType;//Leaf,NonLeaf
};

#endif
 

  

