#include "pf.h"
#include "ix.fwd.h"
#include "ix_internal.h"
#include <cmath>
#include <iostream>
#include <assert.h>
#include "ix.h"

IX_BTNode::IX_BTNode(AttrType iAttrType, int iAttrLength, PF_PageHandle& xPageHandle, bool bNewPage)
: attributeType(iAttrType)
, attributeLength(iAttrLength)
, order(0)
, keysNum(0)
, left(0)
, right(0) {
   int pageSize = PF_PAGE_SIZE;
   order = floor(
           (pageSize + sizeof (int) + 2 * sizeof (PageNum)) /
           (sizeof (RID) + attributeLength));
   // n + 1 pointers + n keys + 1 keyspace used for numKeys
   while (((order) * (attributeLength + sizeof (RID)))
           > ((unsigned) pageSize - sizeof (int) - 2 * sizeof (PageNum)))
      order--;
   assert(((order) * (attributeLength + sizeof (RID)))
           <= (unsigned int) pageSize - sizeof (keysNum) - 2 * sizeof (PageNum));

   char * pData = NULL;
   RC rc = xPageHandle.GetData(pData);
   if (rc != 0) {
      return;
   }
   PageNum aPageId;
   xPageHandle.GetPageNum(aPageId);
   nodeRid = RID(aPageId, -1); //in the internal node we do not use the slot id

   keys = pData;
   rids = (RID*) (pData + attributeLength * order);
   if (!bNewPage) {
      //keysNum = 0; //needs init value >=0
      keysNum = GetKeysNumFromPage();
      left = GetLeftFromPage();
      right = GetRightFromPage();
   } else {
      keysNum = 0;
      PageNum invalid = -1;
      SetLeft(invalid);
      SetRight(invalid);
      SetKeysNumToPage(0);
   }
}

IX_BTNode::~IX_BTNode() {
}

PageNum IX_BTNode::GetLeft() {
   return left;
}

PageNum IX_BTNode::GetLeftFromPage() {
   void * loc = (char*) rids + sizeof (RID) * order + sizeof (int);
   return *((PageNum*) loc);
}

void IX_BTNode::SetLeft(PageNum aPageId) {
   left = aPageId;
   memcpy((char*) rids + sizeof (RID) * order + sizeof (int),
           &left,
           sizeof (PageNum));
}

PageNum IX_BTNode::GetRightFromPage() {
   void * loc = (char*) rids + sizeof (RID) * order + sizeof (int) + sizeof (PageNum);
   return *((PageNum*) loc);
}

PageNum IX_BTNode::GetRight() {
    return right;
}

void IX_BTNode::SetRight(PageNum aPageId) {
   right = aPageId;
   memcpy((char*) rids + sizeof (RID) * order + sizeof (int) + sizeof (PageNum),
           &right,
           sizeof (PageNum));
}

void IX_BTNode::SetKeysNumToPage(int aNumKeys) {
   keysNum = aNumKeys; // conv variable
   memcpy((char*) rids + sizeof (RID) * order,
           &keysNum,
           sizeof (int));
}

int IX_BTNode::GetKeysNumFromPage() {
   void * loc = (char*) rids + sizeof (RID) * order;
   int * pi = (int *) loc;
   return (*pi);
}

int IX_BTNode::GetKeysNum() {
   return keysNum;
}

RID IX_BTNode::GetNodeRID() {
   return nodeRid;
}

RC IX_BTNode::InsertNode(const void* aKey, const RID & aRid) {
   if (keysNum >= order) return -1; //need to add the state code

   int i = -1;

   void *currKey = NULL;
   for (i = keysNum - 1; i >= 0; i--)
   {
      GetKey(i, currKey);
      if (CompareKey(aKey, currKey) >= 0) {
         break; // go out and insert at i
      }
      rids[i + 1] = rids[i];
      SetKey(i + 1, currKey);
   }
   rids[i + 1] = aRid;
   SetKey(i + 1, aKey);

   if (!TestSorted()) {
      std::cout << "Test sorted" << std::endl;
      TestSorted();
   }
   SetKeysNumToPage(GetKeysNum() + 1);
   return 0;
}

// return 0 if remove was successful
// return -2 if key does not exist
// return -1 if key is the last one (lazy deletion) - underflow
// kpos is optional - will remove from that position if specified
// if kpos is specified newkey can be NUL

RC IX_BTNode::DeleteNode(const void* aKey, int kpos) {
   {
      int pos = -1;
      if (kpos != -1) {
         if (kpos < 0 || kpos >= keysNum)
            return -2;
         pos = kpos;
      } else {
         pos = FindKeyExact(aKey);
         if (pos < 0)
            return IX_KEYNOTFOUND;
         // shift all keys after this pos
      }
      for (int i = pos; i < keysNum - 1; i++) {
         void *p;
         GetKey(i + 1, p);
         SetKey(i, p);
         rids[i] = rids[i + 1];
      }
      SetKeysNumToPage(GetKeysNum() - 1);
      if (keysNum == 0) {
         return IX_KEYNOTENGOUGH;
      }
      return OK_RC;
   }
}

RC IX_BTNode::MergeNode(IX_BTNode * aNode) {
   if (keysNum + aNode->GetKeysNum() > order) {
      return -1; // overflow of the node
   }
   for (int i = 0; i < aNode->GetKeysNum(); i++) {
      void *k;
      aNode->GetKey(i, k);
      RID rid = aNode->GetAddr(i);
      RC rc = InsertNode(k, rid);
      if (rc) {
         return rc;
      }
   }
   int moveCount = aNode->GetKeysNum();
   for (int i = 0; i < moveCount; i++) {
      void * keytodel;
      aNode->GetKey(0, keytodel);
      RC rc = aNode->DeleteNode(keytodel, -1);
      if (rc == -1) return rc;
   }
   PageNum page;
   page = this->GetNodeRID().Page();
   if (page == aNode->GetLeft()) {
      SetRight(aNode->GetRight());
   } else
      SetLeft(aNode->GetLeft());
   return OK_RC;
}

RC IX_BTNode::SplitNode(IX_BTNode * aNewNode) {
   if (keysNum > order) return -1; //no need to split;
   // else move a half to the new node 
   int movePos = (keysNum + 1) / 2;
   int moveCount = (keysNum - movePos);
   if (aNewNode->GetKeysNum() + moveCount > aNewNode->GetOrder()) return -1; //fails
   for (int i = movePos; i < keysNum; i++)
   {
      RID rid = *(rids + i);
      void * k;
      GetKey(i, k);
      RC rc = aNewNode->InsertNode(k, rid);
      if (rc != 0) return rc;
   }
   for (int i = 0; i < moveCount; i++) {
      void * keytodel;
      GetKey(GetKeysNum() - 1, keytodel);
      RC rc = DeleteNode(keytodel, -1);
      if (rc == -1) return rc;
   }

   aNewNode->SetRight(this->GetRight());
   this->SetRight(aNewNode->GetNodeRID().Page());
   aNewNode->SetLeft(this->GetNodeRID().Page());

   return OK_RC;
}

int IX_BTNode::CompareKey(const void* aKey, const void* bKey) const {
   switch (attributeType) {
      case STRING:
         return memcmp(aKey, bKey, attributeLength);
      case FLOAT:
         if (*(float*) aKey > *(float*) bKey) return 1;
         if (*(float*) aKey == *(float*) bKey) return 0;
         if (*(float*) aKey < *(float*) bKey) return -1;
      case INT:
         if (*(int*) aKey > *(int*) bKey) return 1;
         if (*(int*) aKey == *(int*) bKey) return 0;
         if (*(int*) aKey < *(int*) bKey) return -1;
   }
   return 0;
}
//get the key in the position iPos
//return OK_RC if success
//return -1 if fail

RC IX_BTNode::GetKey(int iPos, void*& aKey) const {
   if (iPos >= 0 && iPos < keysNum) {
      aKey = keys + attributeLength*iPos;
      return OK_RC;
   } else {
      return -1;
   }
}

RC IX_BTNode::SetKey(int iPos, const void* aKey) {
   if (iPos >= 0 && iPos < order) {
      memcpy(keys + attributeLength*iPos,
              aKey,
              attributeLength);
      return 0;
   } else {
      return -1;
   }
}

// copy key at location pos to the pointer provided
// must be already allocated

int IX_BTNode::CopyKey(int pos, void* toKey) const {
   if (toKey == NULL)
      return -1;
   if (pos >= 0 && pos < order) {
      memcpy(toKey,
              keys + attributeLength*pos,
              attributeLength);
      return 0;
   } else {
      return -1;
   }
}
// descrp:
//          inexact search the rid of the pData
// input:
//          void*& key
// return:
//         (1)rid
//            if there are dups - this will return rightmost rid
//         (2)(-1 -1)(not found or other error)

RID IX_BTNode::FindAddrBrut(const void* &key) const {
   assert(IsValid() == 0);
   int pos = FindKeyBrut(key);
   if (pos == -1 || pos >= keysNum) {
      return RID(-1, -1);
   }
   return rids[pos];
}
// descrp:
//          inexact search the position of the pData specified with the RID in a node
// input:
//          void* pData
// return:
//         (1)the position if both key and RID match (found)
//            if there are dups, the rightmost position
//         (2)-1 (not found)

int IX_BTNode::FindKeyBrut(const void*& aKey) const {
   assert(IsValid() == 0);
   for (int i = keysNum - 1; i >= 0; i--) {
      void* k;
      if (GetKey(i, k) != 0)
         return IX_KEYNOTFOUND;
      // added == condition so that FindLeaf can return exact match and not
      // the position to the right upon matches. this affects where inserts
      // will happen during dups.
      if (CompareKey(aKey, k) == 0)
         return i;
      if (CompareKey(aKey, k) > 0)
         return i + 1;
   }
   return 0; // key is smaller than anything currently
}

// exact
// find rid for given key
// return (-1, -1) if there was an error or pos was not found

RID IX_BTNode::FindAddrExact(const void* &key) const {
   assert(IsValid() == 0);
   int pos = FindKeyExact(key);
   if (pos == -1 || pos >= keysNum) {
      return RID(-1, -1);
   }
   return rids[pos];
}
// description:
//          exact search the position of the pData specified with the RID in a node
// input:
//          void* pData
//          RID&  rid
// return:
//         (1)the position if both key and RID match (found)
//            if there are dups, the rightmost position
//         (2)-1 (not found)

int IX_BTNode::FindKeyExact(const void* &aKey, const RID& aRid) const {
   for (int i = keysNum - 1; i >= 0; i--) {
      void* k;
      if (GetKey(i, k) != 0)
         return -1;
      if (CompareKey(aKey, k) == 0) //find the key who has the same value in the node
      {
         if (aRid == RID(-1, -1)) {
            return i;
         } else { // match RID as well
            if (rids[i] == aRid) {
               return i;
            }
         }
      }
   }
   return -1;
}
// exact
// get rid for given position
// return (-1, -1) if there was an error or pos was not found

RID IX_BTNode::GetAddr(const int pos) const {
   assert(IsValid() == 0);
   if (pos == -1 || pos >= keysNum) {
      return RID(-1, -1);
   }
   return rids[pos];
}

int IX_BTNode::GetOrder() {
   return order;
}

int IX_BTNode::GetMaxKeys() const {
   return order;
}

int IX_BTNode::GetAttrLength() {
   return attributeLength;
}

AttrType IX_BTNode::GetType() {
   return attributeType;
}

std::ostream& operator<<(std::ostream &os, IX_BTNode &a) {
  os<< a.GetLeft() << "<--" << a.GetNodeRID().Page() << "{";
   os << "page# " << a.GetNodeRID().Page() << ";order " << a.GetOrder()<< ";numKeys " <<a.GetKeysNum()<< "{";
   int pos;
   for (pos = 0; pos < a.GetKeysNum(); pos++) {
      void * k = NULL;
      a.GetKey(pos, k);
      os << "(";
      if (a.GetType() == INT)
         os << *((int*) k);
      if (a.GetType() == FLOAT)
         os << *((float*) k);
      if (a.GetType() == STRING) {
         for (int i = 0; i < a.GetAttrLength(); i++)
            os << ((char*) k)[i];
      }
      PageNum P;
      P = a.GetAddr(pos).Page();
      SlotNum S;
      S = a.GetAddr(pos).Slot();
      os << "," << P << " " << S << "), ";

   }
   os << "}" << "->" << a.GetRight() << "\n";
   return os;
}

void* IX_BTNode::LargestKey() const {
   void * key = NULL;
   if (keysNum > 0) {
      GetKey(keysNum - 1, key);
      return key;
   } else {
      return NULL;
   }
}

int IX_BTNode::Destroy() {
   if (keysNum != 0)
      return -1;

   keys = NULL;
   rids = NULL;
   return 0;
}

void IX_BTNode::Print() {
   std::cerr << GetLeft() << "<--" << nodeRid.Page() << "{";
   std::cerr << "page# " << nodeRid.Page() << ";order " << order << ";numKeys " << keysNum << "{";
   for (int pos = 0; pos < GetKeysNum(); pos++) {
      void * k = NULL;
      GetKey(pos, k);
      std::cerr << "(" << GetAddr(pos).Page() << "," << GetAddr(pos).Slot() << "), ";
   }
   if (keysNum > 0)
      std::cerr << "\b\b";
   std::cerr << "}" << "-->" << GetRight() << "\n";
   std::cerr << "}" << "\n";
}

bool IX_BTNode::TestSorted() const {
   for (int i = 0; i < keysNum - 2; i++) {
      void* k1;
      GetKey(i, k1);
      void* k2;
      GetKey(i + 1, k2);
      if (CompareKey(k1, k2) > 0)
         return false;
   }
   return true;
}

RC IX_BTNode::IsValid() const {
   if (order <= 0)
      return IX_INVALIDSIZE;

   bool ret = true;
   ret = ret && (keys != NULL);
   assert(ret);
   ret = ret && (rids != NULL);
   assert(ret);
   ret = ret && (keysNum >= 0);
   assert(ret);
   ret = ret && (keysNum <= order);
   if (!ret) {
      std::cerr << "order was " << order << " numkeys was  " << keysNum << std::endl;
   }
   return ret ? 0 : IX_BADIXPAGE;
}

