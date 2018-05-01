//
// pf_statistics.cc
//

// This file contains the procedure to display all the statistics for the
// PF layer.

// Code written by Andre Bergholz, who was the TA for 2000

//
// This file only makes sense when the PF Statistics layer is defined
//
#ifdef PF_STATS

#include <iostream>
#include "pf.h"
#include "statistics.h"

using namespace std;

// This is defined within pf_buffermgr.cc
extern StatisticsMgr *pStatisticsMgr;

void PF_Statistics()
{
   // First get all the statistics, must remember to delete memory returned
   int iGP = pStatisticsMgr->Get(PF_GETPAGE);
   int iPF = pStatisticsMgr->Get(PF_PAGEFOUND);
   int iPNF = pStatisticsMgr->Get(PF_PAGENOTFOUND);
   int iRP = pStatisticsMgr->Get(PF_READPAGE);
   int iWP = pStatisticsMgr->Get(PF_WRITEPAGE);
   int iFP = pStatisticsMgr->Get(PF_FLUSHPAGES);

   cout << "PF Layer Statistics" << endl;
   cout << "-------------------" << endl;

   cout << "Total number of calls to GetPage Routine: ";
   cout << iGP << endl;
   cout << "  Number found: ";
   cout << iPF << endl;
   cout << "  Number not found: ";
   cout << iPNF << endl;
   cout << "-------------------" << endl;

   cout << "Number of read requests: ";
   cout << iRP << endl;
   cout << "\nNumber of write requests: ";
   cout << iWP << endl;
   cout << "-------------------" << endl;
   cout << "Number of flushes: ";
   cout << iFP << endl;
   cout << "-------------------" << endl;
}

#endif