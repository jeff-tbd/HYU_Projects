#include "IX_BTNode.h"
#include "redbase.h"
#include "pf.h"

#include <iostream>
#include <string.h>

using namespace std;



RC TestInsert(PF_PageHandle &pageHandler) {
    bool aborted = false;
    const int key = 1;
    RID rid(1, 1);
    IX_BTNode node(INT, sizeof (int), pageHandler, true);
    node.InsertNode(&key, rid);
    void *pResultKey = 0;
    node.GetKey(0, pResultKey);
    aborted = *static_cast<int*> (pResultKey) != key;
    if (aborted) {
        cerr << "Test failed" << endl;
    }
    return !aborted ? OK_RC : 1;
}

RC TestFind(PF_PageHandle &pageHandler) {
    int key1 = 1;
    RID rid1(1, 1);
    int key2 = 2;
    RID rid2(2, 2);
    int key3 = 3;
    RID rid3(3, 3);
    IX_BTNode node(INT, sizeof (int), pageHandler, true);
    node.InsertNode(&key1, rid1);
    node.InsertNode(&key2, rid2);
    node.InsertNode(&key3, rid3);
    const void * test = (void *) (&key2);
    int position = node.FindKey(test);
    bool result = (position == 1);
    if (!result) {
        cerr << "Test failed" << endl;
    }
    return result ? OK_RC : 1;
}

RC TestDelete(PF_PageHandle &pageHandler) {
   int  key1 = 1;
   RID rid1(1, 1);
    int key2 = 2;
   RID rid2(2, 2);
   int key3 = 3;
   RID rid3(3, 3);
   IX_BTNode node(INT, sizeof(int), pageHandler, true);
    node.InsertNode(&key1, rid1);
   node.InsertNode(&key2, rid2);
   node.InsertNode(&key3, rid3);
   node.DeleteNode(&key1);
   void *pResultKey = 0;
   node.GetKey(0, pResultKey);
   bool aborted = *static_cast<int*>(pResultKey) != key2;
    if (aborted) {
      cerr << "Test failed" << endl;
   }
   return !aborted? OK_RC: 1;
}
RC TestMerge(PF_PageHandle &pageHandler1,PF_PageHandle &pageHandler2) {
           IX_BTNode node1(INT, sizeof(int), pageHandler1, true);
           IX_BTNode node2(INT, sizeof(int), pageHandler2, true);
     int  key1 = 1;
     RID rid1(1, 1);
     int key2 = 3;
     RID rid2(2, 2);
     int key3 = 5;
     RID rid3(3, 3);
   node1.InsertNode(&key1, rid1);
   node1.InsertNode(&key2, rid2);
   node1.InsertNode(&key3, rid3);
   cout<<node1<<endl;
   PageNum id1 = node1.GetNodeRID().GetPage();
   PageNum id2 = node2.GetNodeRID().GetPage();
   node1.SetRight(id2);
   node2.SetLeft(id1);
    int  key4 = 2;
     RID rid4(1, 1);
     int key5 = 4;
     RID rid5(2, 2);
     int key6 = 6;
     RID rid6(3, 3);
     int key7 = 7;
     RID rid7(4, 4);
   node2.InsertNode(&key4, rid4);
   node2.InsertNode(&key5, rid5);
   node2.InsertNode(&key6, rid6);
   node2.InsertNode(&key7, rid7); 
   cout<<node2<<endl;
   node1.MergeNode(&node2);
   cout<<node1<<endl;
   cout<<node2<<endl;
   return OK_RC;
}
RC TestSplit(PF_PageHandle &pageHandler1,PF_PageHandle &pageHandler2){
     IX_BTNode node1(INT, sizeof(int), pageHandler1, true);
     IX_BTNode node2(INT, sizeof(int), pageHandler2, true);
     node1.SetRight(node2.GetNodeRID().GetPage());
     node2.SetLeft(node1.GetNodeRID().GetPage());
     int  key1 = 1;
     RID rid1(1, 1);
     int key2 = 3;
     RID rid2(2, 2);
     int key3 = 5;
     RID rid3(3, 3);
     int  key4 = 2;
     RID rid4(1, 1);
     int key5 = 4;
     RID rid5(2, 2);
     int key6 = 6;
     RID rid6(3, 3);
     int key7 = 7;
     RID rid7(4, 4);
   node1.InsertNode(&key1, rid1);
   node1.InsertNode(&key2, rid2);
   node1.InsertNode(&key3, rid3);
   node1.InsertNode(&key4, rid4);
   node1.InsertNode(&key5, rid5);
   node1.InsertNode(&key6, rid6);
   node1.InsertNode(&key7, rid7); 
   cout<<node1<<endl;
   cout<<node2<<endl;
   cout<<endl;
   node1.SplitNode(&node2);
   cout<<node1<<endl;
   cout<<node2<<endl;
   return OK_RC;
}

RC TestSort() {
    return OK_RC;
}

int main()
{
   static const char cstTEST_FILE[] = "TEST_FILE";
   bool aborted = false;
   RC rc = OK_RC;
   PF_Manager pfm;
   PF_FileHandle fh1;
   PF_PageHandle ph1,ph2;
   PageNum pageNum1;
   PageNum pageNum2;
   aborted = (OK_RC != (rc = pfm.CreateFile(cstTEST_FILE)));
   aborted = aborted || (OK_RC != (rc = pfm.OpenFile(cstTEST_FILE, fh1)));
   aborted = aborted || (OK_RC != (rc = fh1.AllocatePage(ph1)));
   aborted = aborted || (OK_RC != (rc = fh1.AllocatePage(ph2)));
   aborted = aborted || (OK_RC != (rc = TestSplit(ph1,ph2)));
   aborted = aborted || (OK_RC != (rc = ph1.GetPageNum(pageNum1)));
   aborted = aborted || (OK_RC != (rc = fh1.MarkDirty(pageNum1)));
   aborted = aborted || (OK_RC != (rc = fh1.UnpinPage(pageNum1)));
   aborted = aborted || (OK_RC != (rc = ph2.GetPageNum(pageNum2)));
   aborted = aborted || (OK_RC != (rc = fh1.MarkDirty(pageNum2)));
   aborted = aborted || (OK_RC != (rc = fh1.UnpinPage(pageNum2)));
   aborted = aborted || (OK_RC != (rc = pfm.CloseFile(fh1)));
   aborted = aborted || (OK_RC != (rc = pfm.DestroyFile(cstTEST_FILE)));
   if (aborted) {
       cout << "Some problem occurred" << endl;
   }
   else {
        cout << "TEST SUCCESS" << endl;   
   }
   return rc;
}
