//
// File:        rm_filescan.cc
//

#include "file_scan.h"
#include "rm.h"


using namespace std;

// if cond.rhsValue.data is null rest of the cond is not checked
FileScan::FileScan(SM_Manager& smm,
                   RM_Manager& rmm,
                   const char* relName_//,
//                   RC& status,
//                   const Condition* cond,
//                   int nOutFilters,
//                   const Condition outFilters[]
  ):psmm(&smm),
    prmm(&rmm),
    relName(relName_)
  //,nOFilters(nOutFilters)
{
//  rfs = new RM_FileScan;
//  rmh = new RM_FileHandle;

//  memset(relName, '\0', sizeof(relName_));
//  strncpy(relName, relName_, MAXNAME);
    nOFilters = 0;
    oFilters = NULL;
    attrCount = 0;
    attrs = NULL;
}

//destructor
FileScan::~FileScan()
{
  rfs.CloseScan();
  prmm->CloseFile(rmh);

  nOFilters = 0;

  if(attrs!=NULL)
    delete [] attrs;

  if(oFilters!=NULL)
    delete [] oFilters;
}

RC FileScan::CreateScan(
    const Condition* cond,
    int nOutFilters,
    const Condition outFilters[])
{
//    for (int i = 0; i < nOutFilters; i++)
//        cout << "   outFilters[" << i << "]:" << outFilters[i] << "\n";

    RC rc;
    char* condAttr = NULL;
    RM_Record rec;

    if((rc = psmm->GetFromTable(relName, attrCount, attrs)))
    {
        return rc;
    }

    //which means no conditions
    if((nOutFilters==0)&&(outFilters==NULL)&&(cond==NULL))
    {
        //it seems that the first attributes is the primairy key in the table
        if((rc = psmm->GetAttributeInfo(relName, attrs[0].attrName, rec, condAttr)))
        {
           return rc;
        }

        if((rc = prmm->OpenFile(relName, rmh)))
        {
          return rc;
        }
        //nocondition scan
        if((rc = rfs.OpenScan(rmh,
                          ((SM_AttrcatRec *)condAttr)->attrType,
                          ((SM_AttrcatRec *)condAttr)->attrLength,
                          ((SM_AttrcatRec *)condAttr)->offset,
                          NO_OP,
                          NULL,
                          NO_HINT)))
        {
            return rc;
        }
        return 0;
    }
    else if((cond!=NULL)&&(cond->bRhsIsAttr==FALSE)&&(cond->rhsValue.data != NULL))
    {
         if((rc = psmm->GetAttributeInfo(relName, cond->lhsAttr.attrName, rec, condAttr)))
         {
            return rc;
         }
         if((rc = prmm->OpenFile(relName, rmh)))
         {
           return rc;
         }

         if((rc = rfs.OpenScan(rmh,
                           ((SM_AttrcatRec *)condAttr)->attrType,
                           ((SM_AttrcatRec *)condAttr)->attrLength,
                           ((SM_AttrcatRec *)condAttr)->offset,
                           cond->op,
                           cond->rhsValue.data,
                           NO_HINT)))
         {
             return rc;
         }

    }
    else{
        rc = QL_BADATTR;
        return rc;
    }

    nOFilters = nOutFilters;
    oFilters = new Condition[nOFilters];
    for(int i = 0; i < nOFilters; i++) {
      oFilters[i] = outFilters[i]; // shallow copy
    }
    return 0;
}

string FileScan::Explain()  
{
  return indent + explain.str();
}

RC FileScan::IsValid()
{
  return (attrCount != -1 && attrs != NULL) ? 0 : SM_BADTABLE;
}



// iterator interface
// acts as a (re)open after OpenScan has been called.
RC FileScan::Open()
{
  if(bIterOpen)
    return RM_SCANOPEN;

  bIterOpen = true;
  return 0;
}

// iterator interface
RC FileScan::Close()
{
  if(!bIterOpen)
    return RM_CLOSEDSCAN;

  bIterOpen = false;
  return 0;
}

// iterator interface
RC FileScan::GetNext(Tuple &t)
{
  if(!bIterOpen)
    return RM_CLOSEDSCAN;
  
  RM_Record rec;
  RID recrid;
  bool found = false;
  RC rc;

  while(!found) {
    rc = rfs.GetNextRec(rec);
    if (rc != 0) return rc;

    char * buf;
    rec.GetData(buf);
    rec.GetRid(recrid);

    bool recordIn = true;
    for (int i = 0; i < nOFilters; i++) {
      Condition cond = oFilters[i];
      DataAttrInfo* condAttr;
      RID r;  
      //rc = psmm->GetAttrFromCat(relName, cond.lhsAttr.attrName, condAttr, r);
      RM_Record rec;
      rc = psmm->GetAttributeInfo(relName, cond.lhsAttr.attrName, rec, (char *&)condAttr);
      if (rc != 0) return rc;
      rec.GetRid(r);


      char * rhs = (char*)cond.rhsValue.data;
      if(cond.bRhsIsAttr == TRUE) {
        DataAttrInfo rhsAttr;
        RID r;  
        //rc = psmm->GetAttrFromCat(relName, cond.lhsAttr.attrName, rhsAttr, r);
        RM_Record rec;
        rc = psmm->GetAttributeInfo(relName, cond.lhsAttr.attrName, rec, (char *&)condAttr);
        if (rc != 0) return rc;
        rec.GetRid(r);

        rhs = (buf + rhsAttr.offset);
      }       
   
    } // for

    if(recordIn) {
      t.Set(buf);
      t.SetRid(recrid);
      found = true;
    }
  } // while

  return rc;
}

