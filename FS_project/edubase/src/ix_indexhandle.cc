/*
todo: (1) for the test of 200000
        enumerate all pages pinned and unpinned
        to findout which step missing the operation of unpin
      (4) in order to implement the ix_scan, we can add a function ro refresh the root
        for the root, it's left/right neighbour could be the page of the first the leaf node

      (2) check the delete of 200000, to verify the merge and the redistribution
      (3) for the ix_scan
        implementation
    (5) rewrite the insert/delete
 */

//
// File:        ix_indexhandle.cc
// Description: IX_IndexHandle class implementation
// Author:      LIU Shishi (shishi.liu@enst.fr)
//

#include "ix_internal.h"
#include <stdio.h>

IX_IndexHandle::IX_IndexHandle() {
    // Initialize member variables
    bHdrChanged = FALSE;
    memset(&fileHdr, 0, sizeof (fileHdr));
    fileHdr.firstFreePage = IX_PAGE_LIST_END;
    fileHdr.firstLeafPage = IX_PAGE_LIST_END;

    fileHdr.height = 0;
    pfFileHandle = NULL;
    bFileOpen = false;
    root = NULL;
    path = NULL;
    pathP = NULL;
    treeLargest = NULL;
}

IX_IndexHandle::~IX_IndexHandle() {
    if (pfFileHandle != NULL) {
        delete pfFileHandle;
        pfFileHandle = NULL;
    }
}

RC IX_IndexHandle::Open(PF_FileHandle &fileHandle) {
    if (bFileOpen || (pfFileHandle) != NULL) {
        return IX_HANDLEOPEN;
    }

    RC rc;
    bFileOpen = true;
    pfFileHandle = new PF_FileHandle;
    *pfFileHandle = fileHandle;
    bool newPage = true;
    //if no root,allocates a root page
    //index header will be modified
    if (fileHdr.firstFreePage == IX_PAGE_LIST_END) {
        PageNum pageNum;
        //RC rc = GetNewPage(pageNum);

        if ((rc = GetNewPage(pageNum)))
            return rc;
        fileHdr.firstFreePage = pageNum;
        fileHdr.firstLeafPage = pageNum;
        SetHeight(1);
    }//if root already exists
    else {
        newPage = false;
        SetHeight(fileHdr.height); // do all other init        
    }
    PF_PageHandle rootPageHandle;
    // pin root page - should always be valid
    if ((rc = pfFileHandle->GetThisPage(fileHdr.firstFreePage, rootPageHandle)))
        return rc;

    root = new IX_BTNode(fileHdr.attrType, fileHdr.attrLength, rootPageHandle, newPage);

    path[0] = root;

    fileHdr.order = root->GetOrder();

    RC invalid = IsValid();
    if (invalid) return invalid;

    treeLargest = (void*) new char[fileHdr.attrLength];

    if (!newPage) {
        IX_BTNode * node = FindLargestLeaf();
        // set treeLargest
        if (node->GetKeysNum() > 0) {
            node->CopyKey(node->GetKeysNum() - 1, treeLargest);
        }
    }
    bHdrChanged = true;
    return 0;
}

RC IX_IndexHandle::Close() {
    if(bFileOpen != true)
     {
       return IX_CLOSEFILENONEXIST;
     }
     else
     {
       bFileOpen = false;
     }

    if (pathP != NULL) {
        delete [] pathP;
        pathP = NULL;
    }
    if (path != NULL) {
        // path[0] is root
        for (int i = 1; i < fileHdr.height; i++)
            if (path[i] != NULL) {
                if (pfFileHandle != NULL)
                    pfFileHandle->UnpinPage(path[i]->GetNodeRID().Page());
                delete path[i]; //- better leak than crash
            }
        delete [] path;
        path = NULL;
    }
    if (treeLargest != NULL) {
        //delete treeLargest;
        delete [] (char*) treeLargest;
        treeLargest = NULL;
    }
    if (root != NULL) {
        delete root;
        root = NULL;
    }
    if ((pfFileHandle) != NULL) {
        // unpin old root page
        pfFileHandle->UnpinPage(fileHdr.firstFreePage);
    }
    return 0;
}

RC IX_IndexHandle::WriteHdr() {
    //refresh the first leaf page
    fileHdr.firstLeafPage = this->FindSmallestLeaf()->GetNodeRID().Page();
    return 0;
}

//=================================================================================

RC IX_IndexHandle::GetNewPage(PageNum& pageNum) {
    RC invalid = IsValid();
    if (invalid) return invalid;
    PF_PageHandle ph;
    RC rc;
    if ((rc = pfFileHandle->AllocatePage(ph))) {
        return (rc);
    }
    if ((rc = ph.GetPageNum(pageNum))) {
        return (rc);
    }

    // the default behavior of the buffer pool is to pin pages
    // let us make sure that we unpin explicitly after setting
    // things up
    if (
            (rc = pfFileHandle->MarkDirty(pageNum)) ||
            (rc = pfFileHandle->UnpinPage(pageNum)))
        return rc;
    // std::cerr << "GetNewPage called to get page " << pageNum << std::endl;
    fileHdr.numPages++;
    assert(fileHdr.numPages > 1); // page 0 is this page in worst case
    bHdrChanged = true;
    return 0; // pageNum is set correctly
}
//=================================================================================

//
// Desc: Access the page number.  The page handle object must refer to
//       a pinned page.
// Out:  populates the path member variable with the path
// Ret:  NULL if there is no root
//       otherwise return a pointer to the leaf node that is rightmost AND
//       largest in value

// Users will call - RC invalid = IsValid(); if(invalid) return invalid;

RC IX_IndexHandle::IsValid() const {
    bool ret = true;
    ret = ret && ((pfFileHandle) != NULL);
    if (fileHdr.height > 0) {
        ret = ret && (fileHdr.firstFreePage > 0); // 0 is for file header
        ret = ret && (fileHdr.numPages >= fileHdr.height + 1);
        ret = ret && (root != NULL);
        ret = ret && (path != NULL);
    }
    return ret ? 0 : IX_BADIXPAGE;
}

IX_BTNode* IX_IndexHandle::FindLargestLeaf() {
    RC invalid = IsValid();
    if (invalid) return NULL;
    if (root == NULL) return NULL;

    if (fileHdr.height == 1) {
        path[0] = root;
        return root;
    }
    //path[0] is the root
    for (int i = 1; i < fileHdr.height; i++) {
        //get the mostright (key,rid), which is the largest key in the node
        RID r = path[i - 1]->GetAddr(path[i - 1]->GetKeysNum() - 1);
        if (r.Page() == -1) {
            // no such position or other error
            // no entries in node ?
            assert("should not run into empty node");
            return NULL;
        }
        // start with a fresh path
        if (path[i] != NULL) {
            RC rc = pfFileHandle->UnpinPage(path[i]->GetNodeRID().Page());
            if (rc < 0) return NULL;
            delete path[i];
            path[i] = NULL;
        }
        //put the largest key node into the path
        path[i] = FetchNode(r);
        PF_PageHandle dummy;
        // pin path pages
        RC rc = pfFileHandle->GetThisPage(path[i]->GetNodeRID().Page(), dummy);
        if (rc != 0) return NULL;
        pathP[i - 1] = 0; // dummy
    }
    return path[fileHdr.height - 1];
}
// return NULL if there is no root
// otherwise return a pointer to the leaf node that is leftmost OR
// smallest in value
// also populates the path member variable with the path

IX_BTNode* IX_IndexHandle::FindSmallestLeaf() {
    RC invalid = IsValid();
    if (invalid) return NULL;
    if (root == NULL) return NULL;

    if (fileHdr.height == 1) {
        path[0] = root;
        return root;
    }

    for (int i = 1; i < fileHdr.height; i++) {
        //get the mostleft (key,rid), which is the smallest key in the node
        RID r = path[i - 1]->GetAddr(0);
        if (r.Page() == -1) {
            assert("should not run into empty node");
            return NULL;
        }
        // start with a fresh path
        if (path[i] != NULL) {
            RC rc = pfFileHandle->UnpinPage(path[i]->GetNodeRID().Page());
            if (rc < 0) {
                assert("error");
            }
            if (rc < 0) return NULL;
            delete path[i];
            path[i] = NULL;
        }
        path[i] = FetchNode(r);
        PF_PageHandle dummy;
        // pin path pages
        RC rc = pfFileHandle->GetThisPage(path[i]->GetNodeRID().Page(), dummy);
        if (rc != 0) return NULL;
        pathP[i - 1] = 0; // dummy
    }
    return path[fileHdr.height - 1];
}
//
// Desc: Creat a BTNode at the PageNumber specified
//
// Ret:  NULL if the PageNumber < 0
//       otherwise return a pointer to the a BTNode

IX_BTNode* IX_IndexHandle::FetchNode(PageNum p) const {
    return FetchNode(RID(p, (SlotNum) - 1));
}
//
// Desc: Creat the BTNode at the RID specified
//
// Ret:  NULL if the PageNumber < 0
//       otherwise return a pointer to the a BTNode

IX_BTNode* IX_IndexHandle::FetchNode(RID r) const {
    RC invalid = IsValid();
    if (invalid) return NULL;
    if (r.Page() < 0) return NULL;
    PF_PageHandle ph;
    RC rc = GetThisPage(r.Page(), ph);
    if (rc != 0) {
        IX_PrintError(rc);
    }
    if (rc != 0) return NULL;

    return new IX_BTNode(fileHdr.attrType,
            fileHdr.attrLength,
            ph,
            false);
}

//
// Desc: Unpinning version that will unpin after every call correctly
//

RC IX_IndexHandle::GetThisPage(PageNum p, PF_PageHandle& ph) const {
    RC rc = pfFileHandle->GetThisPage(p, ph);
    if (rc != 0) return rc;
    // Needs to be called everytime GetThisPage is called.
    rc = pfFileHandle->MarkDirty(p);
    if (rc != 0) return rc;

    rc = pfFileHandle->UnpinPage(p);
    if (rc != 0) return rc;
    return 0;
}

//
// InsertEntry
//
// Desc: Insert a new entry
// In:   pData -
// Out:  rid -
// Ret:  RM return code
//

RC IX_IndexHandle::InsertEntry(void *pData, const RID &rid) {
    RC rc;
    int detail = 0;
    rc = InsertEntry(pData, rid, detail);
    if (rc == 0) {
        WriteHdr();
    }
    return rc;
}

RC IX_IndexHandle::InsertEntry(void *pData, const RID &rid, int detail) {
    RC invalid = IsValid();
    if (invalid) return invalid;

    if (pData == NULL) return IX_BADKEY;

    bool newLargest = false;
    void * prevKey = NULL;
    int level = fileHdr.height - 1;

    IX_BTNode* node = FindLeaf(pData);
    IX_BTNode* newNode = NULL;
    assert(node != NULL);

    int pos2 = node->FindKeyExact((const void*&) pData, rid);
    //same data + same rid
    if ((pos2 != -1))
        return IX_ENTRYEXISTS;
    //perhaps same data + different rid

    // largest key in tree is in every intermediate root from root onwards
    if (node->GetKeysNum() == 0 || node->CompareKey(pData, treeLargest) > 0) {
        newLargest = true;
        prevKey = treeLargest;
    }

    if (newLargest) {
        for (int i = 0; i < fileHdr.height - 1; i++) {
            int pos = path[i]->FindKeyExact((const void *&) prevKey);
            if (pos != -1) {
                path[i]->SetKey(pos, pData);
            } else {
                // assert(false); //largest key should be everywhere
                // return IX_BADKEY;
                std::cerr << "Fishy intermediate node ?" << std::endl;
            }
        }
        // copy from pData into new treeLargest
        memcpy(treeLargest, pData, fileHdr.attrLength);
        //std::cerr << "new treeLargest " << *(int*)treeLargest << std::endl;
    }

    int result = node->InsertNode(pData, rid);
    // no room in node - deal with overflow - non-root
    void * failedKey = pData;
    RID failedRid = rid;

    while (result == -1) {
        //std::cerr << "non root overflow" << std::endl;

        char * charPtr = new char[fileHdr.attrLength];
        void * oldLargest = charPtr;

        if (node->LargestKey() == NULL)
            oldLargest = NULL;
        else
            node->CopyKey(node->GetKeysNum() - 1, oldLargest);

        //std::cerr << "nro largestKey was " << *(int*)oldLargest  << std::endl;
        //std::cerr << "nro numKeys was " << node->GetNumKeys()  << std::endl;
        delete [] charPtr;

        // make new  node
        PF_PageHandle ph;
        PageNum p;
        RC rc = GetNewPage(p);
        if (rc != 0) return rc;
        rc = GetThisPage(p, ph);
        if (rc != 0) return rc;

        // rc = pfFileHandle->MarkDirty(p);
        // if(rc!=0) return NULL;

        newNode = new IX_BTNode(fileHdr.attrType, fileHdr.attrLength, ph, true);
        // split into new node
        rc = node->SplitNode(newNode);
        if (rc != 0) return IX_PF;
        // split adjustment
        IX_BTNode * currRight = FetchNode(newNode->GetRight());
        if (currRight != NULL) {
            currRight->SetLeft(newNode->GetNodeRID().Page());
            delete currRight;
        }

        // put the new entry into one of the 2 now.
        // In the comparison,
        // > takes care of normal cases
        // = is a hack for dups - this results in affinity for preserving
        // RID ordering for children - more balanced tree when all keys
        // are the same.
        if (node->CompareKey(pData, node->LargestKey()) >= 0) {
            newNode->InsertNode(failedKey, failedRid);
        } else { // <
            node->InsertNode(failedKey, failedRid);
        }

        // go up to parent level and repeat
        level--;
        if (level < 0) break; // root !
        // pos at which parent stores a pointer to this node
        int posAtParent = pathP[level];
        // std::cerr << "posAtParent was " << posAtParent << std::endl ;
        // std::cerr << "level was " << level << std::endl ;


        IX_BTNode * parent = path[level];
        // update old key - keep same addr
        parent->DeleteNode(NULL, posAtParent);

        result = parent->InsertNode((const void*) node->LargestKey(),
                node->GetNodeRID());
        // this result should always be good - we removed first before
        // inserting to prevent overflow.

        // insert new key
        result = parent->InsertNode(newNode->LargestKey(), newNode->GetNodeRID());

        // iterate for parent node and split if required
        pfFileHandle->UnpinPage(node->GetNodeRID().Page());
        node = parent;
        failedKey = newNode->LargestKey(); // failure cannot be in node -
        // something was removed first.
        failedRid = newNode->GetNodeRID();

        delete newNode;
        newNode = NULL;
    } // while

    if (level >= 0) {
        return 0;
    } else {
        std::cerr << "root split happened" << std::endl;

        // unpin old root page
        RC rc = pfFileHandle->UnpinPage(fileHdr.firstFreePage);
        if (rc < 0)
            return rc;

        // make new root node
        PF_PageHandle ph;
        PageNum p;
        rc = GetNewPage(p);
        if (rc != 0) return IX_PF;
        rc = GetThisPage(p, ph);
        if (rc != 0) return IX_PF;

        // rc = pfFileHandle->MarkDirty(p);
        // if(rc!=0) return NULL;

        root = new IX_BTNode(fileHdr.attrType, fileHdr.attrLength,
                ph, true);
        root->InsertNode(node->LargestKey(), node->GetNodeRID());
        root->InsertNode(newNode->LargestKey(), newNode->GetNodeRID());

        // pin root page - should always be valid
        fileHdr.firstFreePage = root->GetNodeRID().Page();

        PF_PageHandle rootph;
        rc = pfFileHandle->GetThisPage(fileHdr.firstFreePage, rootph);
        if (rc != 0) return rc;

        if (newNode != NULL) {
            delete newNode;
            newNode = NULL;
        }
        //refresh all information in the header
        SetHeight(fileHdr.height + 1);
        return 0;
    }
}
// description:
// return NULL if there is no root
// otherwise return a pointer to the leaf node where key might go
// also populates the path[] member variable with the path
// if there are dups (keys) along the path, the rightmost path will be
// chosen
// TODO - add a random parameter to this which will be used during
// inserts - this is prevent pure rightwards growth when inserting a
// dup value continuously.
//
// input:
//          *pData
// return:
//         (1)no root:NULL
//         (2)BTNode*-->pData; populates the path[]membre

IX_BTNode* IX_IndexHandle::FindLeaf(const void *pData) {
    RC invalid = IsValid();
    if (invalid) return NULL;
    if (root == NULL) return NULL;

    if (fileHdr.height == 1) {
        path[0] = root;
        return root;
    }

    RID r;
    //height > 1
    for (int i = 1; i < fileHdr.height; i++) {
        int pos = path[i - 1]->FindKeyBrut(pData);
        r = path[i - 1]->FindAddrBrut(pData);

        if (r.Page() == -1)
        {
            const void * p = path[i - 1]->LargestKey();
            r = path[i - 1]->FindAddrExact((const void*&) (p));
            pos = path[i - 1]->FindKeyExact((const void*&) (p));
        }

        // start with a fresh path
        if (path[i] != NULL) {
            RC rc = pfFileHandle->UnpinPage(path[i]->GetNodeRID().Page());
            if (rc != 0) return NULL;
            delete path[i];
            path[i] = NULL;
        }

        path[i] = FetchNode(r.Page());

        PF_PageHandle dummy;

        // pin path pages
        RC rc = pfFileHandle->GetThisPage(path[i]->GetNodeRID().Page(), dummy);
        if (rc != 0) return NULL;

        pathP[i - 1] = pos;
    }        


    IX_BTNode *node = path[fileHdr.height-1];
    //check the right sibling node to ensure the node is the rightmost node
    int pos = node->FindKeyBrut(pData);
    if((pos == node->GetKeysNum()-1)&&(node->GetRight()!=(-1)))
    {
        IX_BTNode* bnode = FetchNode(node->GetRight());
        char* key = NULL;
        RC rc = bnode->GetKey(0,(void*&) key);//compare the pData with the smallest key in the right brother
        if (rc != 0)
        {
            return NULL;
        }
        if (bnode->CompareKey(pData, key) >= 0)
        {
            pos = bnode->FindKeyExact(pData);
            node = bnode;
            // start with a fresh path
            if (path[fileHdr.height-1] != NULL) {
                RC rc = pfFileHandle->UnpinPage(path[fileHdr.height-1]->GetNodeRID().Page());
                if (rc != 0) return NULL;
                delete path[fileHdr.height-1];
                path[fileHdr.height-1] = NULL;
            }
            path[fileHdr.height-1] = bnode;
            PF_PageHandle dummy;
            // pin path pages
            RC rc = pfFileHandle->GetThisPage(path[fileHdr.height-1]->GetNodeRID().Page(), dummy);
            if (rc != 0) return NULL;
            pathP[fileHdr.height-2] = pos;
        }
    }

    return path[fileHdr.height - 1];
}

// get/set height

int IX_IndexHandle::GetHeight() const {
    return fileHdr.height;
}

void IX_IndexHandle::SetHeight(const int& h) {
    for (int i = 1; i < fileHdr.height; i++)
        if (path != NULL && path[i] != NULL) {
            delete path[i];
            path[i] = NULL;
        }
    if (path != NULL) delete [] path;
    if (pathP != NULL) delete [] pathP;

    fileHdr.height = h;

    path = new IX_BTNode* [fileHdr.height];

    for (int i = 1; i < fileHdr.height; i++)
        path[i] = NULL;
    path[0] = root;

    pathP = new int [fileHdr.height - 1]; // leaves don't point
    for (int i = 0; i < fileHdr.height - 1; i++)
        pathP[i] = -1;
}

IX_BTNode* IX_IndexHandle::GetRoot() const {
    return root;
}

void IX_IndexHandle::PrintTree() const {
    RID rid;
    this->Print(-1, rid);
}

void IX_IndexHandle::Print(int level, RID r) const {
    RC invalid = IsValid();
    if (invalid) assert(invalid);
    // level -1 signals first call to recursive function - root
    // std::cerr << "Print called with level " << level << endl;
    IX_BTNode * node = NULL;
    if (level == -1) {
        level = fileHdr.height;
        node = FetchNode(root->GetNodeRID());
        std::cout << "Print called with level " << level << std::endl;
    } else {
        if (level < 1) {
            return;
        } else {
            node = FetchNode(r);
        }
    }
    if (node != NULL) {
        std::cout << (*node);
        std::cout << " " << std::endl;
        std::cout << " " << std::endl;

        //node->Print();

        if (level >= 2) // non leaf
        {
            PF_PageHandle ph; 
            this->pfFileHandle->GetThisPage(node->GetNodeRID().Page(),ph);
            for (int i = 0; i < node->GetKeysNum(); i++) {
                RID newr = node->GetAddr(i);
                Print(level - 1, newr);
            }
            this->pfFileHandle->UnpinPage(node->GetNodeRID().Page());
        }
        // else level == 1 - recursion ends
        if (level == 1 && node->GetRight() == -1)
            std::cerr << std::endl; //blank after rightmost leaf
        if (node != NULL) delete node;
    }
}

void IX_IndexHandle::PrintHeader() const {

    if (bFileOpen) {
        std::cout << "State:Index is open." << std::endl;
    } else {
        std::cout << "State:Index is closed." << std::endl;
    }
    std::cout << "Index Root Page number:" << fileHdr.firstFreePage << std::endl;
    std::cout << "Index Smallest Page number:" << fileHdr.firstLeafPage << std::endl;
    std::cout << "Index #Pages:" << fileHdr.numPages << std::endl;
    std::cout << "Index BTNode order:" << fileHdr.order << std::endl;
    std::cout << "Index B+ Tree Height:" << fileHdr.height << std::endl;
}

//==================================================================
// Delete a index entry
// 0 indicates success
// Implements lazy deletion - underflow is defined as 0 keys for all
// non-root nodes.

RC IX_IndexHandle::DeleteEntry(void *pData, const RID& rid) {
    RC rc;
    int detail = 0;
    rc = DeleteEntry(pData, rid, detail);
    if (rc == 0) {
        WriteHdr();
    }
    return rc;
}

RC IX_IndexHandle::DeleteEntry(void *pData, const RID& rid, int detail) {
    if (pData == NULL)
        // bad input to method
        return IX_BADKEY;
    RC invalid = IsValid();
    if (invalid) return invalid;

    bool nodeLargest = false;

    IX_BTNode* node = FindLeaf(pData); //it's a leaf node
    assert(node != NULL);

    int pos = node->FindKeyExact((const void*&) pData, rid);
    //if there is no (key rid) pair exactly the same in the node
    if (pos == -1) {
        // make sure that there are no dups (keys) left of this node that might
        // have this rid.
        int p = node->FindKeyExact((const void*&) pData, RID(-1, -1));
        if (p != -1) {
            IX_BTNode * other = DupScanLeftFind(node, pData, rid);
            if (other != NULL) {
                int pos = other->FindKeyExact((const void*&) pData, rid);
                other->DeleteNode(pData, pos); // ignore result - not dealing with
                // underflow here
                return 0;
            }
        }
        // key/rid does not exist - error
        return IX_NOSUCHENTRY;
    } else if (pos == node->GetKeysNum() - 1)
        nodeLargest = true;

    // Handle special case of key being largest and rightmost in
    // node. Means it is in parent and potentially whole path (every
    // intermediate node)
    if (nodeLargest) {
        //std::cerr << "node largest" << std::endl;
        // replace this key with leftkey in every intermediate node
        // where it appears
        for (int i = fileHdr.height - 2; i >= 0; i--) {
            int posInPath = path[i]->FindKeyExact((const void *&) pData);
            if (posInPath != -1) {
                void * leftKey = NULL;
                leftKey = path[i + 1]->LargestKey();
                if (node->CompareKey(pData, leftKey) == 0) {
                    int posInLeaf = path[i + 1]->GetKeysNum() - 2;
                    if (posInLeaf < 0) {
                        continue; //underflow will happen
                    }
                    path[i + 1]->GetKey(path[i + 1]->GetKeysNum() - 2, leftKey);
                }
                // if level is lower than leaf-1 then make sure that this is
                // the largest key
                if ((i == fileHdr.height - 2) || // leaf's parent level
                        (posInPath == path[i]->GetKeysNum() - 1) // largest at
                        // intermediate node too
                        )
                    path[i]->SetKey(posInPath, leftKey);
            }
        }
    }

    int result = node->DeleteNode(pData, pos); // pos is the param that counts

    int level = fileHdr.height - 1; // leaf level

    while (result == IX_KEYNOTENGOUGH) {
        // go up to parent level and repeat
        level--;
        if (level < 0) break; // root !

        // pos at which parent stores a pointer to this node
        int posAtParent = pathP[level];
        // cerr << "posAtParent was " << posAtParent << endl ;
        // cerr << "level was " << level << endl ;

        IX_BTNode * parent = path[level];

        result = parent->DeleteNode(NULL, posAtParent);

        // root is considered underflow even it is left with a single key
        if (level == 0 && parent->GetKeysNum() == 1 && result == 0)
            result = -1;

        // adjust L/R pointers because a node is going to be destroyed
        IX_BTNode* left = FetchNode(node->GetLeft());
        IX_BTNode* right = FetchNode(node->GetRight());
        if (left != NULL) {
            if (right != NULL)
                left->SetRight(right->GetNodeRID().Page());
            else
                left->SetRight(-1);
        }
        if (right != NULL) {
            if (left != NULL)
                right->SetLeft(left->GetNodeRID().Page());
            else
                right->SetLeft(-1);
        }
        if (right != NULL)
            delete right;
        if (left != NULL)
            delete left;

        node->Destroy();
        fileHdr.numPages--;

        RC rc = DisposePage(node->GetNodeRID().Page());
        if (rc < 0)
            return IX_PF;

        node = parent;
    } // end of while result == -1


    if (level >= 0) {
        // deletion done
        return 0;
    } else {
        // cerr << "root underflow" << endl;

        if (fileHdr.height == 1) { // root is also leaf
            return 0; //leave empty leaf-root around.
        }

        // new root is only child
        root = FetchNode(root->GetAddr(0));
        // pin root page - should always be valid
        fileHdr.firstFreePage = root->GetNodeRID().Page();
        PF_PageHandle rootph;
        RC rc = pfFileHandle->GetThisPage(fileHdr.firstFreePage, rootph);
        if (rc != 0) return rc;

        node->Destroy();
        rc = DisposePage(node->GetNodeRID().Page());
        if (rc < 0)
            return IX_PF;

        SetHeight(fileHdr.height - 1); // do all other init
        return 0;
    }
}

// search other nodes(located in the leftside of the node) who contains the (key, rid)
// return NULL if (key, rid) is not found

IX_BTNode* IX_IndexHandle::DupScanLeftFind(IX_BTNode* right, void *pData, const RID& rid) {
    IX_BTNode* currNode = FetchNode(right->GetLeft());

    int currPos = -1;

    for (IX_BTNode* j = currNode;
            j != NULL;
            j = FetchNode(j->GetLeft())) {
        currNode = j;
        int i = currNode->GetKeysNum() - 1;

        for (; i >= 0; i--) {
            currPos = i; // save Node in object state for later.
            char* key = NULL;
            int ret = currNode->GetKey(i, (void*&) key);
            if (ret == -1)
                break; //error in get the key in the position i
            if (currNode->CompareKey(pData, key) < 0)
                return NULL;
            if (currNode->CompareKey(pData, key) > 0)
                return NULL;
            if (currNode->CompareKey(pData, key) == 0) {
                if (currNode->GetAddr(currPos) == rid)
                    return currNode;
            }

        }
    }
    return NULL;
}

RC IX_IndexHandle::DisposePage(const PageNum& pageNum) {
    RC invalid = IsValid();
    if (invalid) return invalid;

    RC rc;
    if ((rc = pfFileHandle->DisposePage(pageNum)))
        return (rc);

    fileHdr.numPages--;
    assert(fileHdr.numPages > 0); // page 0 is this page in worst case
    bHdrChanged = true;
    return 0; // pageNum is set correctly
}

// Force index files to disk

RC IX_IndexHandle::ForcePages() {
    return 0;
}

