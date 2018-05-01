//
// File:        IX_IndexScan.cc
// Description: IX_IndexScan class implementation
// Author:      Shishi LIU
//

#include "ix.h"
#include "ix_internal.h"

//
// IX_FileScan
//
// Desc: Default Constructor
//

IX_IndexScan::IX_IndexScan() {
   // Initialize member variables
   bScanOpen = FALSE;
   curPageNum = -1;
   pIndexHandle = NULL;
   attrType = INT;
   attrLength = sizeof (int);
   compOp = NO_OP;
   value = NULL;
   pinHint = NO_HINT;

   scanCount = 0;
   currNode = NULL;
   currPos = -1;
   currKey = NULL;
   currRid = RID(-1, -1);
}
//
// RM_FileScan
//
// Desc: Destructor
//

IX_IndexScan::~IX_IndexScan() {
   if (pIndexHandle != NULL && pIndexHandle->GetHeight() > 1) {
      if (currNode != NULL)
         delete currNode;
   }
}

//
// OpenScan
//
// Desc: Open a file scan with the given indexHandle an scan condition
// In:   indexHandle  - IX_IndexHandle object (must be open)
//       _attrType   - INT|FLOAT|STRING
//       _attrLength - 4 for INT|FLOAT, 1~MAXSTRING for STRING
//       _attrOffset - indicates the location of the attribute
//       _compOp     - EQ_OP|LT_OP|GT_OP|LE_OP|GE_OP|NE_OP|NO_OP
//       _value      - points to the value which will be compared with
//                     the given attribute
//       _pinHint    - not implemented yet
// Ret:  IX_SCANOPEN, IX_VALUENULL, IX_INVALIDATTR, IX_CLOSEDFILE
//

RC IX_IndexScan::OpenScan(const IX_IndexHandle &indexHandle,
        CompOp _compOp,
        void * _value,
        ClientHint pinHint) {
   // Sanity Check: 'this' should not be open yet
   if (bScanOpen)
      // Test: opened RM_FileScan
      return (IX_SCANOPEN);

   // Sanity Check: indexHandle must be open
   if (indexHandle.fileHdr.numPages == 0) // a little tricky here
      // Test: unopened indexHandle
      return (IX_CLOSEDFILE);

   // Sanity Check: compOp
   switch (_compOp) {
      case EQ_OP:
      case LT_OP:
      case GT_OP:
      case LE_OP:
      case GE_OP:
      case NE_OP:
         break;
      case NO_OP:
      default:
         return (IX_INVALIDCOMPOP);
   }

   if (compOp != NO_OP) {
      // Sanity Check: value must not be NULL
      if (value == NULL)
         // Test: null _value
         return (IX_NULLPOINTER);

      // Sanity Check: attrType, attrLength
      switch (attrType) {
         case INT:
         case FLOAT:
            if (attrLength != 4)
               // Test: wrong _attrLength
               return (IX_INVALIDATTR);
         case STRING:
            if (attrLength < 1 || attrLength > MAXSTRINGLEN)
               // Test: wrong _attrLength
               return (IX_INVALIDATTR);
            break;

         default:
            // Test: wrong _attrType
            return (IX_INVALIDATTR);
      }
   }
   // Copy parameters to local variable
   pIndexHandle = (IX_IndexHandle *) & indexHandle;
   attrType = indexHandle.fileHdr.attrType;
   attrLength = indexHandle.fileHdr.attrLength;
   compOp = _compOp;
   value = _value;
   //pinHint     = _pinHint;

   // Set local state variables
   bScanOpen = TRUE;
   curPageNum = indexHandle.fileHdr.firstFreePage; //get the root page
   //curSlotNum = pIndexHandle->fileHdr.numPages;
   // Return ok

   return (0);
}

//
// GetNextEntry
//
// Desc: Retrieve a copy of the next record that satisfies the scan condition
// Out:  rid - the RID of the next record in the index scan
//             IX_EOF no index entries left satisfying the scan condition
// Ret:  IX or PF return code
//

RC IX_IndexScan::GetNextEntry(RID &rid) {
   RC rc;
   //   PF_PageHandle pageHandle;
   //   char *pData;
   //   RID *curRid;

   // Sanity Check: 'this' must be open
   if (!bScanOpen)
      return (IX_CLOSEDSCAN);   

   // first time in
   if (currNode == NULL && currPos == -1 && scanCount == 0) {
      currKey = value;
      //refresh the currNode
      //the currPos is the mostright position in the node for the value
      rc = FindLeaf(currKey, currRid, currPos);
      //std::cerr<<"current rid page:"<<currRid.Page()<<std::endl;
      //std::cerr<<"current position:"<<currPos<<std::endl;
      //std::cerr<<"current node:"<<*currNode<<std::endl;     
      scanCount++;
   } else {

      scanCount++;
   }

   if (rc != IX_EOF) {
      //found the entry,flag is true
      //else flag is false
      bool flag = false;

      switch (compOp) {
         case EQ_OP:
            //           std::cerr<<"equal "<<std::endl;
            rc = FindDupKey(currKey, currRid, currPos, flag);
            //           std::cerr<<"flag:"<<flag<<std::endl;
            //           std::cerr<<"rc:"<<rc<<std::endl;
            //           std::cerr<<"current position:"<<currPos<<std::endl;
            //           std::cerr<<"current node:"<<currNode->GetNodeRID().Page()<<std::endl;
            if ((flag != true) || (rc != 0)) {
               rc = IX_EOF;
            }
            break;
         case LT_OP:
            rc = FindLeftKey(currKey, currRid, currPos, flag);
            //std::cerr<<"current position:"<<currPos<<std::endl;
            //std::cerr<<"current node:"<<currNode->GetNodeRID().Page()<<std::endl;
            if ((flag != true) || (rc != 0)) {
               rc = IX_EOF;
            }
            break;
         case GT_OP:
            rc = FindRightKey(currKey, currRid, currPos, flag);
            if ((flag != true) || (rc != 0)) {
               rc = IX_EOF;
            }
            break;
         case LE_OP:
            rc = FindDupLeftKey(currKey, currRid, currPos, flag);
            if ((flag != true) || (rc != 0)) {
               rc = IX_EOF;
            }
            break;
         case GE_OP:
            rc = FindDupRightKey(currKey, currRid, currPos, flag);
            if ((flag != true) || (rc != 0)) {
               rc = IX_EOF;
            }
            break;
         case NE_OP:
            //           rc = FindRightKey(currKey, currRid, currPos, currNode, flag);
            //           if(flag == false)
            //                rc = FindLeftKey(currKey, currRid, currPos, currNode, flag);
            break;
         case NO_OP:
            //scan all
            break;
         default:
            return (IX_INVALIDCOMPOP);
      }
   }

   rid = currRid;
   return rc;
}

//
// CloseScan
//
// Desc: Close a file scan with the given indexHandle and scan condition
// Ret:  RM_CLOSEDSCAN
//

RC IX_IndexScan::CloseScan() {
   // Sanity Check: 'this' must be open
   if (!bScanOpen)
      // Test: closed RM_FileScan
      return (IX_CLOSEDSCAN);

   // Reset member variables
   bScanOpen = FALSE;
   curPageNum = -1;
   pIndexHandle = NULL;
   attrType = INT;
   attrLength = sizeof (int);
   compOp = NO_OP;
   value = NULL;
   pinHint = NO_HINT;

   scanCount = 0;
   currNode = NULL;
   currPos = -1;
   currKey = NULL;
   currRid = RID(-1, -1);
   return (0);
}

RC IX_IndexScan::FindLeaf(const void *pData, RID& r, int& pos) {
   RC rc = OK_RC;
   IX_BTNode* node = pIndexHandle->GetRoot();
   int i = 0;

   for (i = 1; i < pIndexHandle->fileHdr.height; i++) {
      r = node->FindAddrBrut(pData);      
      pos = node->FindKeyBrut(pData);
      if (r.Page() == -1) {
         const void * p = node->LargestKey();
         r = node->FindAddrExact((const void*&) (p));
      }
      if((pos == node->GetKeysNum()-1)&&(node->GetRight()!=-1))
      {
          IX_BTNode* bnode = pIndexHandle->FetchNode(node->GetRight());
          char* key = NULL;
          rc = bnode->GetKey(0,(void*&) key);//compare the pData with the smallest key in the right brother
          if (rc != 0)
          {
              return rc;
          }
          if (bnode->CompareKey(pData, key) >= 0)
          {
              r = bnode->FindAddrExact(pData);
              pos = bnode->FindKeyExact(pData);
              node = bnode;
          }
      }
      node = pIndexHandle->FetchNode(r.Page());
   }
   //the node is the leaf
   if (NULL != node) {
      r = node->FindAddrExact(pData);
      pos = node->FindKeyExact(pData);
      if((pos == node->GetKeysNum()-1)&&(node->GetRight()!=-1))
      {
          IX_BTNode* bnode = pIndexHandle->FetchNode(node->GetRight());
          char* key = NULL;
          rc = bnode->GetKey(0,(void*&) key);//compare the pData with the smallest key in the right brother
          if (rc != 0)
          {
              return rc;
          }
          if (bnode->CompareKey(pData, key) >= 0)
          {
              r = bnode->FindAddrExact(pData);
              pos = bnode->FindKeyExact(pData);
              node = bnode;
          }
      }
      currNode = node;
   }
   else
   {
      rc = IX_EOF;
   }
   return rc;
}

RC IX_IndexScan::FindDupKey(const void *pData, RID& rid, int& pos, bool& flag) {
    RC rc;
    char *key = NULL;

    //the first time: the entry itself
    if (scanCount == 1)
    {
        key = NULL;
        rc = currNode->GetKey(pos, (void*&) key);
        if (rc != 0)
        {
            return rc;
        }
        if (currNode->CompareKey(pData, key) == 0)
        {
            flag = true;
            return (0);
          }
    }

    IX_BTNode* j = currNode;

    key = NULL;

   //the position of the next node
   int pos_t = pos - 1;
   for (j = currNode;
           j != NULL;
           ) {
      if (pos_t < 0) {
         if (currNode->GetLeft() == -1) {
            flag = false;
            break;
         }
         delete currNode;
         currNode = pIndexHandle->FetchNode(currNode->GetLeft());
         pos_t = currNode->GetKeysNum() - 1;
      }
      pos = pos_t;
      key = NULL;
      //search key in the next node
      rc = currNode->GetKey(pos, (void*&) key);
      if (rc != 0) {
         return rc;
      }
      //std::cerr<<"find key value:"<<*(int*)key<<std::endl;
      //find the key
      if (currNode->CompareKey(pData, key) == 0) {
         RID r = currNode->GetAddr(pos);
         //find the key with the same value
         if (!(r == rid)) {
            flag = true;
            rid = r;
            break;
         } else {
            flag = false;
            break;
         }
      } else {
         flag = false;
         break;
      }
   }
   return (0);
}

RC IX_IndexScan::FindLeftKey(const void *pData, RID& rid, int& pos, bool& flag) {
   //the position of the next node
   int pos_t = pos - 1;

   for (IX_BTNode* j = currNode;
           j != NULL;
           ) {
      if (pos_t < 0) {
         if (currNode->GetLeft() == -1) {
            flag = false;
            break;
         }
         delete currNode;
         currNode = pIndexHandle->FetchNode(currNode->GetLeft());
         pos_t = currNode->GetKeysNum() - 1;
      }
      pos = pos_t;

      char* key = NULL;

      //search key in the next node
      RC rc = currNode->GetKey(pos, (void*&) key);
      if (rc != 0) {
         return rc;
      }
      //std::cerr<<"find key value:"<<*(int*)key<<std::endl;
      //find the key
      if (currNode->CompareKey(key, pData) < 0) {
         rid = currNode->GetAddr(pos);
         flag = true;
         break;
      } else {
         flag = false;
         break;
      }
   }
   return (0);
}

RC IX_IndexScan::FindRightKey(const void *pData, RID& rid, int& pos, bool& flag) {
   //the position of the next node
   int pos_t = pos + 1;

   for (IX_BTNode* j = currNode;
           j != NULL;
           ) {
      if (pos_t > currNode->GetKeysNum() - 1) {
         if (currNode->GetRight() == -1) {
            flag = false;
            break;
         }
         delete currNode;
         currNode = pIndexHandle->FetchNode(currNode->GetRight());
         pos_t = 0;
      }

      //search key in the node
      pos = pos_t;

      char* key = NULL;

      //search key in the next node
      RC rc = currNode->GetKey(pos, (void*&) key);
      if (rc != 0) {
         return rc;
      }
      //std::cerr<<"find key value:"<<*(int*)key<<std::endl;
      //find the key
      if (currNode->CompareKey(key, pData) > 0) {
         rid = currNode->GetAddr(pos);
         flag = true;
         break;
      } else {
         flag = false;
         break;
      }
   }
   return (0);
}

RC IX_IndexScan::FindDupLeftKey(const void *pData, RID& rid, int& pos, bool& flag) {
   //the first time: the entry itself
   if (scanCount == 1) {
      char *key;
      RC rc = currNode->GetKey(pos, (void*&) key);
      if (rc != 0) {
         return rc;
      }
      if (currNode->CompareKey(pData, key) == 0) {
         flag = true;
         return (0);
      }
   }

   //the position of the next node
   int pos_t = pos - 1;

   for (IX_BTNode* j = currNode;
           j != NULL;
           ) {
      if (pos_t < 0) {
         if (currNode->GetLeft() == -1) {
            flag = false;
            break;
         }
         delete currNode;
         currNode = pIndexHandle->FetchNode(currNode->GetLeft());
         pos_t = currNode->GetKeysNum() - 1;
      }
      pos = pos_t;
      char* key = NULL;
      //search key in the next node
      RC rc = currNode->GetKey(pos, (void*&) key);
      if (rc != 0) {
         return rc;
      }
      //std::cerr<<"find key value:"<<*(int*)key<<std::endl;
      //find the key
      if (currNode->CompareKey(pData, key) == 0) {
         RID r = currNode->GetAddr(pos);
         //find the key with the same value
         if (!(r == rid)) {
            flag = true;
            rid = r;
            break;
         } else {
            flag = false;
            break;
         }
      } else if (currNode->CompareKey(key, pData) < 0) {
         rid = currNode->GetAddr(pos);
         flag = true;
         break;
      } else {
         flag = false;
         break;
      }
   }
   return (0);
}

RC IX_IndexScan::FindDupRightKey(const void *pData, RID& rid, int& pos, bool& flag) {
   //the first time: the entry itself
   if (scanCount == 1) {
      char *key;
      RC rc = currNode->GetKey(pos, (void*&) key);
      if (rc != 0) {
         return rc;
      }
      if (currNode->CompareKey(pData, key) == 0) {
         flag = true;
         return (0);
      }
   }

   //the position of the next node
   int pos_t = pos + 1;

   for (IX_BTNode* j = currNode;
           j != NULL;
           ) {
      if (pos_t > currNode->GetKeysNum() - 1) {
         if (currNode->GetRight() == -1) {
            flag = false;
            break;
         }
         delete currNode;
         currNode = pIndexHandle->FetchNode(currNode->GetRight());
         pos_t = 0;
      }

      //search key in the node
      pos = pos_t;

      char* key = NULL;

      //search key in the next node
      RC rc = currNode->GetKey(pos, (void*&) key);
      if (rc != 0) {
         return rc;
      }

      if (currNode->CompareKey(pData, key) == 0) {
         RID r = currNode->GetAddr(pos);
         //find the key with the same value
         if (!(r == rid)) {
            flag = true;
            rid = r;
            break;
         } else {
            flag = false;
            break;
         }
      } else if (currNode->CompareKey(key, pData) > 0) {
         rid = currNode->GetAddr(pos);
         flag = true;
         break;
      } else {
         flag = false;
         break;
      }
   }
   return (0);
}

RC IX_IndexScan::FindNoEqualKey(const void *pData, RID& r, int& pos, bool& flag) {
   return 0;
}
