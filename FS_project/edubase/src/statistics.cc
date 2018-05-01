
// statistics.cc
//

// This file holds the implementation for the StatisticsMgr class.
//
// The class is designed to dynamically track statistics for the client.
// You can add any statistic that you would like to track via a call to
// StatisticsMgr::Register.

// There is no need to setup in advance which statistics that you want to
// track.  The call to Register is sufficient.

// This is essentially a (poor-man's) simplified version of gprof.

// Andre Bergholz, who was the TA for the 2000 offering has written
// some (or maybe all) of this code.

#include <algorithm>
#include <iostream>
#include "statistics.h"
#include <cstring>
#include <string>
#include <list>

using namespace std;

//
// Here are Statistics Keys utilized by the PF layer of the Redbase
// project.
//
const char *PF_GETPAGE = "GETPAGE";
const char *PF_PAGEFOUND = "PAGEFOUND";
const char *PF_PAGENOTFOUND = "PAGENOTFOUND";
const char *PF_READPAGE = "READPAGE";           // IO
const char *PF_WRITEPAGE = "WRITEPAGE";         // IO
const char *PF_FLUSHPAGES = "FLUSHPAGES";

//
// Statistic class
//
// This class will track a single statistic

//
// Default Constructor utilized by the templates
//
Statistic::Statistic():iValue(0)
{
}


//
// Constructor utilized by the StatisticMgr class
//
// We are assured by the StatisticMgr that psKey_ is not a NULL pointer.
//
Statistic::Statistic(const std::string& sKey_)
{
   sName = sKey_;
   iValue = 0;
}

//
// Destructor
//
Statistic::~Statistic()
{
}

Boolean Statistic::operator==(const std::string& psKey_) const
{
   return 0 == this->sName.compare(psKey_);
}

const std::string & Statistic::getKey() const
{
   return this->sName;
}

int Statistic::getValue() const
{
   return this->iValue;
}

void Statistic::setValue(int iValue)
{
   this->iValue = iValue;
}

// --------------------------------------------------------------

//
// StatisticMgr class
//
// This class will track a dynamic list of statistics.
//

//
// Register
//
// Register a change to a statistic.  The psKey is the char* name of
// the statistic to be tracked.  This method will look for the statistic
// name withing its list of statistics and perform the operation over the
// stored value.  The piValue is utilized for some of the operations.
//
// Note: if the statistic isn't found (as it will not be the very first
// time) then it will be initialized to 0 - the default value.
//
RC StatisticsMgr::Register (const char *psKey, const Stat_Operation op,
      const int *const piValue)
{
   if (psKey==NULL || (op != STAT_ADDONE && piValue == NULL))
      return STAT_INVALID_ARGS;
   
   std::list<Statistic>::iterator pStat = llStats.begin();
   for (;pStat != llStats.end(); ++pStat) {
      if ( 0 == pStat->getKey().compare(psKey)) {
         break;
      }
   }
   
   if (pStat == llStats.end()) {
      llStats.push_back(Statistic(psKey));
      pStat = --llStats.end();
   }

   // Now perform the operation over the statistic
   if ( STAT_ADDONE == op) {
      pStat->setValue(pStat->getValue() + 1 );
   }
   return 0;
}

//
// Print
//
// Print out the information pertaining to a specific statistic
RC StatisticsMgr::Print(const char *psKey)
{
   if (psKey==NULL)
      return STAT_INVALID_ARGS;

   cout << psKey << "::" << Get(psKey) << endl;

   return 0;
}


//
// Get
//
// The Get method will return a pointer to the integer value associated
// with a particular statistic.  If it cannot find the statistic then it
// will return NULL.  The caller must remember to delete the memory
// returned when done.
//
int StatisticsMgr::Get(const char *psKey)
{
   for (std::list<Statistic>::iterator pStat = llStats.begin(); pStat != llStats.end(); ++pStat) {
      if ( 0 == pStat->getKey().compare(psKey)) {
         return pStat->getValue();
      }
   }
   return 0;
}

//
// Print
//
// Print out all the statistics tracked
//
void StatisticsMgr::Print()
{
   for (std::list<Statistic>::iterator pStat = llStats.begin(); pStat != llStats.end(); ++pStat) {
      cout << pStat->getKey() << "::" << pStat->getValue() << endl;
   }
}

//
// Reset
//
// Reset a specific statistic.  The easiest way to do this is to remove it
// completely from the list
//
RC StatisticsMgr::Reset(const char *psKey)
{
   if (psKey==NULL)
      return STAT_INVALID_ARGS;

   llStats.remove_if([psKey](Statistic &stat) -> bool {return (0 == stat.getKey().compare(psKey));});

   return 0;
}

//
// Reset
//
// Reset all of the statistics.  The easiest way is to tell the linklist of
// elements to Erase itself.
//
void StatisticsMgr::Reset()
{
   llStats.clear();
}
