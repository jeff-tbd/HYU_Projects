//
// printer.h
//

// This file contains the interface for the Printer class and some
// functions that will be used by both the SM and QL components.

#ifndef _HELPER
#define _HELPER

#include <iostream>
#include <cstring>
#include <sstream>
#include "redbase.h"      // For definition of MAXNAME

#include <assert.h>


#define MAXPRINTSTRING  ((2*MAXNAME) + 5)
//
// DataAttrInfo
//
// This struct stores the information that is kept within in
// attribute catalog.  It identifies a relation name, attribute name
// and the location type and length of the attribute.
//
struct DataAttrInfo
{
    // Default constructor
    DataAttrInfo() {
       memset(relName, 0, MAXNAME + 1);
       memset(attrName, 0, MAXNAME + 1);
       offset = -1;
  //     func = NO_F;
    };

    // Copy constructor
    DataAttrInfo( const DataAttrInfo &d ) {
       strcpy (relName, d.relName);
       strcpy (attrName, d.attrName);
       offset = d.offset;
       attrType = d.attrType;
       attrLength = d.attrLength;
       indexNo = d.indexNo;
//       func = NO_F;
    };

    DataAttrInfo& operator=(const DataAttrInfo &d) {
       if (this != &d) {
          strcpy (relName, d.relName);
          strcpy (attrName, d.attrName);
          offset = d.offset;
          attrType = d.attrType;
          attrLength = d.attrLength;
          indexNo = d.indexNo;
       }
       return (*this);
    };
    static unsigned int size() { 
        return 2*(MAXNAME+1) + sizeof(AttrType) + 3*sizeof(int);
      }
    char     relName[MAXNAME+1];    // Relation name
    char     attrName[MAXNAME+1];   // Attribute name
    int      offset;                // Offset of attribute
    AttrType attrType;              // Type of attribute
    int      attrLength;            // Length of attribute
    int      indexNo;               // Index number of attribute
//    AggFun func; //Aggregate function on attributes
};

struct DataRelInfo
{
  // Default constructor
  DataRelInfo() {
    memset(relName, 0, MAXNAME + 1);
  }

  DataRelInfo( char * buf ) {
    memcpy(this, buf, DataRelInfo::size());
  }

  // Copy constructor
  DataRelInfo( const DataRelInfo &d ) {
    strcpy (relName, d.relName);
    attrCount = d.attrCount;
    tupleLength = d.tupleLength;
    indexCount = d.indexCount;
//    recordSize = d.recordSize;
//    numPages = d.numPages;
//    numRecords = d.numRecords;
  };

  DataRelInfo& operator=(const DataRelInfo &d) {
    if (this != &d) {
      strcpy (relName, d.relName);
      attrCount = d.attrCount;
      tupleLength = d.tupleLength;
      indexCount = d.indexCount;
//      recordSize = d.recordSize;
//      numPages = d.numPages;
//      numRecords = d.numRecords;
    }
    return (*this);
  }

  static unsigned int size() {
//    return (MAXNAME+1) + 6*sizeof(int);
    return (MAXNAME+1) + 3*sizeof(int);
  }

  static unsigned int members() {
//    return 7;
      return 4;
  }

  char     relName[MAXNAME+1];    // Relation name
  int      attrCount;             // # of attributes
  int      tupleLength;           //tuple length in bytes
  int      indexCount;            // # of indexed attributes
//
//  int      recordSize;            // Size per row
//  int      numPages;              // # of pages used by relation
//  int      numRecords;            // # of records in relation
};

// Print some number of spaces
void Spaces(int maxLength, int printedSoFar);

class Printer {
public:
    // Constructor.  Takes as arguments an array of attributes along with
    // the length of the array.
    Printer(const DataAttrInfo *attributes, const int attrCount);
    
    ~Printer();

    void PrintHeader(std::ostream &c) const;

    // Two flavors for the Print routine.  The first takes a char* to the
    // data and is useful when the data corresponds to a single record in
    // a table -- since in this situation you can just send in the
    // RecData.  The second will be useful in the QL layer.
    void Print(std::ostream &c, const char * const data);
    void Print(std::ostream &c, const void * const data[]);
    void PrintFooter(std::ostream &c) const;

private:
    DataAttrInfo *attributes;
    int attrCount;

    // An array of strings for the header information
    char **psHeader;
    // Number of spaces between each attribute
    int *spaces;

    // The number of tuples printed
    int iCount;
};


#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#endif
