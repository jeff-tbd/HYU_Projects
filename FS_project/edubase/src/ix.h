//
// ix.h
//
//   Index Manager Component Interface
//

#ifndef IX_H
#define IX_H

// Please do not include any other files than the ones below in this file.
#include "redbase.h"  // Please don't change these lines
#include "rm_rid.h"  // Please don't change these lines
#include "pf.h"
#include "ix.fwd.h"
#include <iosfwd>

////////////////////////////////////////////////////////////////////
//
// IX_BTNode: IX B+ Tree interface
//
// The leaf node and internal node used the same structure beacause the  internal node do not use 
// the slot id part of the RID 

class IX_BTNode {
   friend class IX_IndexHandle;
   friend class IX_IndexScan;
public:
   IX_BTNode(AttrType attributeType, int iAttrLength, PF_PageHandle& aPh, bool isNewPage);
   ~IX_BTNode();

   // Return the data corresponding to the record.  Sets *pData to the
   // record contents.

   PageNum GetLeft();
   PageNum GetLeftFromPage();
   void SetLeft(PageNum aPageId);

   PageNum GetRight();
   PageNum GetRightFromPage();
   void SetRight(PageNum aPageId);

   int GetKeysNumFromPage();
   int GetKeysNum();
   void SetKeysNumToPage(int aNumKeys);

   int GetOrder();
   int GetMaxKeys() const;
   //get the node page rid
   RID GetNodeRID();

   //get the key value given by the iPos
   RC GetKey(int iPos, void * & aKey) const;
   //set the key value in the iPos
   RC SetKey(int iPos, const void* aKey);
   int CopyKey(int pos, void* toKey) const;

   // return position if key already exists at position
   // if there are dups - returns rightmost position unless an RID is
   // specified.
   // return -1 if there was an error or if key does not exist
   int FindKeyBrut(const void* &akey) const;
   // if optional RID is specified, will only return a position if both
   // key and RID match.
   int FindKeyExact(const void* &aKeyconst, const RID& r = RID(-1, -1)) const;

   RID FindAddrBrut(const void* &key) const;
   RID FindAddrExact(const void* &key) const;

   RID GetAddr(const int pos) const;

   RC InsertNode(const void* aKey, const RID & aRid);
   RC DeleteNode(const void* aKey, int kpos);
   RC MergeNode(IX_BTNode* aNode);
   RC SplitNode(IX_BTNode* aNewNode);

   int CompareKey(const void * aKey, const void * bKey) const;
   bool TestSorted() const;

   AttrType GetType();
   int GetAttrLength();

   //friend std::ostream &operator<<(std::ostream &os,IX_BTNode &a);
   void Print();
   void* LargestKey() const;
   int Destroy();
   //
   RC IsValid() const;
   friend std::ostream &operator<<(std::ostream &os, IX_BTNode &a);

private:
   char* keys; //the table of key value according to the attribute
   RID* rids; //the rids values table
   RID nodeRid; //this is the rid of the page
   AttrType attributeType;
   int attributeLength;
   int order; //the order of the node
   int keysNum; //the number of keys
   PageNum left; //to the left neighbour:pageID
   PageNum right; //to the right neighbour:pageID
};

struct IX_FileHdr {
   PageNum firstFreePage; //addr of root page; first free page in th lined list
   PageNum firstLeafPage; //addr of root page; first free page in th lined list
   int numPages; // # of pages in the file
   int pageSize; // size per index node - usually PF_PAGE_SIZE
   int pairSize; // size of each (key, RID) pair in index
   int order; // order of btree
   int height; // height of btree
   AttrType attrType;
   int attrLength;
   //    unsigned pageHeaderSize;    // page header size
   //    unsigned numNodes;        // # of pages in the file
};
////////////////////////////////////////////////////////////////////
//
// IX_IndexHandle: IX Index File interface
//

class IX_IndexHandle {
   friend class IX_Manager;
   friend class IX_IndexScan;
public:
   IX_IndexHandle();
   ~IX_IndexHandle();

   // Insert a new index entry
   RC InsertEntry(void *pData, const RID &rid);
   RC InsertEntry(void *pData, const RID &rid, int detail);

   // Delete a new index entry
   RC DeleteEntry(void *pData, const RID &rid);
   RC DeleteEntry(void *pData, const RID &rid, int detail);

   // Force index files to disk
   RC ForcePages();

   RC Open(PF_FileHandle &fileHandle);
   RC Close();

   RC GetNewPage(PageNum& pageNum);
   //===============================================================
   RC IsValid() const;
   IX_BTNode* FindLargestLeaf();
   IX_BTNode* FindSmallestLeaf();
   IX_BTNode* FetchNode(PageNum p) const;
   IX_BTNode* FetchNode(RID r) const;
   RC GetThisPage(PageNum p, PF_PageHandle& ph) const;
   //===============================================================
   IX_BTNode* FindLeaf(const void *pData);
   int GetHeight() const;
   void SetHeight(const int& h);
   IX_BTNode* GetRoot() const;
   //===============================================================
   void PrintHeader() const;
   void PrintTree() const;
   void Print(int level, RID r) const;
   //===============================================================
   IX_BTNode* DupScanLeftFind(IX_BTNode* right, void *pData, const RID& rid);
   RC DisposePage(const PageNum& pageNum);
   //===============================================================
   // Insert a new index entry
   RC FindEntry(void *pData, const RID &rid);
   RC WriteHdr();

   PF_FileHandle* getPfFileHandle() {
      return pfFileHandle;
   }
private:
   // Copy constructor
   IX_IndexHandle(const IX_IndexHandle &fileHandle);
   // Overloaded =
   IX_IndexHandle& operator=(const IX_IndexHandle &fileHandle);
   PF_FileHandle* pfFileHandle;
   IX_FileHdr fileHdr; // file header
   int bHdrChanged; // dirty flag for file hdr
   //===============================================================
   bool bFileOpen;
   IX_BTNode* root; // root in turn points to the other nodes
   IX_BTNode** path;
   //IX_BTNode ** path; // list of nodes that is the path to leaf as a
   // result of a search.
   int* pathP; // the position in the parent in the path the points to
   // the child node.

   void * treeLargest; // largest key in the entire tree
};

class IX_IndexScan {
public:
   IX_IndexScan();
   ~IX_IndexScan();

   RC OpenScan(const IX_IndexHandle &indexHandle,
           CompOp compOp,
           void *value,
           ClientHint pinHint = NO_HINT
           ); // Initialize a file scan

   // Get the next matching entry return IX_EOF if no more matching
   // entries.
   RC GetNextEntry(RID &rid);

   // Close index scan
   RC CloseScan();

private:
   // Copy constructor
   IX_IndexScan(const IX_IndexScan &fileScan);
   // Overloaded =
   IX_IndexScan& operator=(const IX_IndexScan &fileScan);

   RC FindDupKey(const void *pData, RID& rid, int& pos, bool& flag);
   RC FindDupLeftKey(const void *pData, RID& rid, int& pos, bool& flag);
   RC FindDupRightKey(const void *pData, RID& rid, int& pos, bool& flag);

   RC FindLeaf(const void *pData, RID& r, int& pos);
   RC FindLeftKey(const void *pData, RID& r, int& pos, bool& flag);
   RC FindRightKey(const void *pData, RID& r, int& pos, bool& flag);

   RC FindNoEqualKey(const void *pData, RID& rid, int& pos, bool& flag);

   void FindNextRecInCurPage(char *pData);

   int bScanOpen;
   int scanCount;

   PageNum curPageNum;
   IX_BTNode* currNode;

   RID currRid; // saved to check for delete on scan
   int currPos;
   void* currKey; // saved to check for delete on scan

   bool eof; // early EOF set by btree analysis - set by OpOpt
   bool foundOne; // flag that is set by getNext if it is ever successful
   //BtreeNode* lastNode; // last node setup by OpOpt

   IX_IndexHandle *pIndexHandle;
   AttrType attrType;
   int attrLength;
   CompOp compOp;
   void *value;
   ClientHint pinHint;
};

//
// IX_Manager: provides IX index file management
//

class IX_Manager {
public:
   IX_Manager(PF_Manager &pfm);
   ~IX_Manager();

   // Create a new Index
   RC CreateIndex(const char *fileName, int indexNo, AttrType attrType, int attrLength);

   // Destroy and Index
   RC DestroyIndex(const char *fileName, int indexNo);

   // Open an Index
   RC OpenIndex(const char *fileName, int indexNo, IX_IndexHandle &indexHandle);

   // Close an Index
   RC CloseIndex(IX_IndexHandle &indexHandle);
private:
   PF_Manager *pPfm;
};

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
