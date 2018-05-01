//
// File:        ix_manager.cc
// Description: IX_Manager class implementation
// Author:      Shishi LIU (shishi.liu@enst.fr)
//

#include "ix_internal.h"
#include "redbase.h"
#include "pf.h"
#include <stdio.h>
//
// IX_Manager
//
// Desc: Constructor
//
IX_Manager::IX_Manager(PF_Manager &pfm)
{
  // Set the associated PF_Manager object
  pPfm = &pfm;
}

//
// ~IX_Manager
//
// Desc: Destructor
//
IX_Manager::~IX_Manager()
{
}

//
// CreateFile
//
// Desc: Create a new IX file whose name is "fileName"
//       Allocate a file header page and fill out some information
// In:   fileName - name of file to create
//       recordSize - fixed size of records
// Ret:  IX_INVALIDRECSIZE or PF return code
//
// Create a new Index

/**
 * This method creates an index numbered indexNo on the data file named fileName.
 * You may assume that clients of this method will ensure that the indexNo parameter is unique
 * and nonnegative for each index created on a file.
 * Thus, indexNo can be used along with fileName to generate a unique file
 * name (e.g., "fileName.indexNo") that you can use for the PF component file storing the new index.
 * The type and length of the attribute being indexed are described by parameters attrType and attrLength,
 * respectively. As in the RM component, attrLength should be 4 for attribute types INT or FLOAT,
 * and it should be between 1 and MAXSTRINGLEN for attribute type STRING.
 * This method should establish an empty index by creating the PF component file and initializing it appropriately.
 */
RC IX_Manager::CreateIndex(const char *fileName, int indexNo, AttrType attrType, int attrLength)
{
   RC rc;
   PF_FileHandle pfFileHandle;
   PF_PageHandle pageHandle;
   char* pData;
   IX_FileHdr *fileHdr;
   // 1.create a file with name "fileName.indexNo"
   // Note that PF_Manager::CreateFile() will take care of fileName
   // Call PF_Manager::CreateFile()
   if(indexNo < 0)
   {
       rc = IX_NEGATIVEINDEXNUM;
       goto err_return;
   }//todo:specific the error
   if(fileName == NULL)
   {
       rc = IX_NONEXISTFILE;
       goto err_return;
   }
   //2.analyse attribute type:Sanity Check: attrType, attrLength
  switch (attrType)
  {
     case INT:
        if (attrLength != 4)
        {
           rc = IX_INVALIDATTR;
           goto err_return;
        }
        break;

     case FLOAT:
        if (attrLength != 4)
        {
           rc = IX_INVALIDATTR;
           goto err_return;
        }
        break;
     case STRING:
        if (attrLength < 1 || attrLength > MAXSTRINGLEN)
        {
           rc = IX_INVALIDATTR;
           goto err_return;
        }
        break;
     default:
        rc = IX_INVALIDATTR;
        goto err_return;
  }

   //creat the index
   char indexFileName[20];//at the beginning, the length of the index name is fixed;
   sprintf(indexFileName,"%s.%d",fileName,indexNo);
   // Test: existing fileName, wrong permission
   if ((rc = pPfm->CreateFile(indexFileName))) {
       goto err_return;
   }

   // Call PF_Manager::OpenFile()
   if ((rc = pPfm->OpenFile(indexFileName, pfFileHandle))) {
      // Should not happen
      goto err_destroy;
   }

   // Allocate the header page (pageNum must be 0)
   if ((rc = pfFileHandle.AllocatePage(pageHandle))) {
      // Should not happen
      goto err_close;
   }

   // Get a pointer where header information will be written
   if ((rc = pageHandle.GetData(pData))) {
      // Should not happen
      goto err_unpin;
   }

   // Write the file header (to the buffer pool)
   fileHdr = (IX_FileHdr *)pData;
   fileHdr-> firstFreePage = IX_PAGE_LIST_END;
   fileHdr-> firstLeafPage = IX_PAGE_LIST_END;
   fileHdr->numPages = 1; // header page
   fileHdr->pageSize = PF_PAGE_SIZE;
   fileHdr->pairSize = attrLength + sizeof(RID);
   fileHdr->order = -1;
   fileHdr->height = 0;
   fileHdr->attrType = attrType;
   fileHdr->attrLength = attrLength;

   // Mark the header page as dirty
   if ((rc = pfFileHandle.MarkDirty(IX_HEADER_PAGE_NUM))) {
      // Should not happen
      goto err_unpin;
   }

   // Unpin the header page
   if ((rc = pfFileHandle.UnpinPage(IX_HEADER_PAGE_NUM))) {
      // Should not happen
      goto err_close;
   }

   // Call PF_Manager::CloseFile()
   if ((rc = pPfm->CloseFile(pfFileHandle))) {
      // Should not happen
      goto err_destroy;
   }

   // Return ok
   return (0);

   // Recover from inconsistent state due to unexpected error
err_unpin:
   pfFileHandle.UnpinPage(IX_HEADER_PAGE_NUM);
err_close:
   pPfm->CloseFile(pfFileHandle);
err_destroy:
   pPfm->DestroyFile(fileName);
err_return:
   return (rc);
}


//
// DestroyFile
//
// Desc: Delete a RM file named fileName (fileName must exist and not be open)
// In:   fileName - name of file to delete
// Ret:  PF return code
//
RC IX_Manager::DestroyIndex(const char *fileName, int indexNo)
{
    RC rc;
    char indexFileName[20];//at the beginning, the length of the index name is fixed;
    sprintf(indexFileName,"%s.%d",fileName,indexNo);
    // Call PF_Manager::DestroyFile()
    if ((rc = pPfm->DestroyFile(indexFileName)))
    {
       // Test: non-existing fileName, wrong permission
       goto err_return;
    }
    // Return ok
    return (0);

err_return:
   // Return error
   return (rc);
}

//
// OpenFile
//
// Desc: Open the paged file whose name is "fileName.indexNo".
//       Copy the file header information into a private variable in
//       the file handle so that we can unpin the header page immediately
//       and later find out details without reading the header page
// In:   name of file to open:
//       fileName
//       indexNo
// Out:  indexHandle - refer to the open file
// Ret:  PF return code
//
RC IX_Manager::OpenIndex(const char *fileName, int indexNo, IX_IndexHandle& indexHandle)
{
   RC rc;
   PF_PageHandle pageHandle;
   PF_FileHandle fileHandle;
   char* pData;

   char indexFileName[20];//at the beginning, the length of the index name is fixed;
   sprintf(indexFileName,"%s.%d",fileName,indexNo);

   if ((rc = pPfm->OpenFile(indexFileName, fileHandle)))
   {
      // Test: non-existing fileName, opened fileHandle
      goto err_return;
   }

   // Get the header page
   if ((rc = fileHandle.GetFirstPage(pageHandle)))
   {
      // Test: invalid file
      goto err_close;
   }

   // Get a pointer where header information resides
   if ((rc = pageHandle.GetData(pData)))
      // Should not happen
      goto err_unpin;

   // Read the file header (from the buffer pool to IX_FileHandle)
   memcpy(&indexHandle.fileHdr, pData, sizeof(indexHandle.fileHdr));

   if((rc = indexHandle.Open(fileHandle)))
   {
       goto err_unpin;
   }

   // Unpin the header page
   if ((rc = indexHandle.pfFileHandle->UnpinPage(IX_HEADER_PAGE_NUM)))
   {
      goto err_close;
   }
   // Return ok
   return (0);

   // Recover from inconsistent state due to unexpected error
err_unpin:
   indexHandle.pfFileHandle->UnpinPage(IX_HEADER_PAGE_NUM);
err_close:
   pPfm->CloseFile(fileHandle);
err_return:
   // Return error
   return (rc);
}

//
// CloseFile
//
// Desc: Close file associated with fileHandle
//       Write back the file header (if there was any changes)
// In:   fileHandle - handle of file to close
// Out:  fileHandle - no longer refers to an open file
// Ret:  RM return code
//
RC IX_Manager::CloseIndex(IX_IndexHandle &indexHandle)
{
   RC rc;

   // Write back the file header if any changes made to the header
   // while the file is open
   if (indexHandle.bHdrChanged) {
      PF_PageHandle pageHandle;
      char* pData;

      // Get the header page
      if ((rc = indexHandle.pfFileHandle->GetFirstPage(pageHandle)))
         // Test: unopened(closed) indexHandle, invalid file
         goto err_return;

      // Get a pointer where header information will be written
      if ((rc = pageHandle.GetData(pData)))
         // Should not happen
         goto err_unpin;

      // Write the file header (to the buffer pool)
      memcpy(pData, &indexHandle.fileHdr, sizeof(indexHandle.fileHdr));

      // Mark the header page as dirty
      if ((rc = indexHandle.pfFileHandle->MarkDirty(IX_HEADER_PAGE_NUM)))
         // Should not happen
         goto err_unpin;

      // Unpin the header page
      if ((rc = indexHandle.pfFileHandle->UnpinPage(IX_HEADER_PAGE_NUM)))
         // Should not happen
         goto err_return;

      // Set file header to be not changed
      indexHandle.bHdrChanged = FALSE;
   }
   // Call PF_Manager::CloseFile()

   if ((rc = indexHandle.Close()))
   {
       goto err_return;
    }

   if ((rc = pPfm->CloseFile(*indexHandle.pfFileHandle)))
   {
       goto err_return;
   }

   // Reset member variables
   memset(&indexHandle.fileHdr, 0, sizeof(indexHandle.fileHdr));
   indexHandle.~IX_IndexHandle();

   // Return ok
   return (0);

   // Recover from inconsistent state due to unexpected error
err_unpin:
   indexHandle.pfFileHandle->UnpinPage(IX_HEADER_PAGE_NUM);
err_return:
   // Return error
   return (rc);
}

