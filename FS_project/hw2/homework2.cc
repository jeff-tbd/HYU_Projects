#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pf.h"
#include "pf_internal.h"
#include "pf_buffermgr.h"
#include "pf_hashtable.h"

//record struct, offset define
#define MAX_LENGTH 32
typedef struct record{
	char name[MAX_LENGTH];
	char addr[MAX_LENGTH*2];
	char telnum[MAX_LENGTH];
	char email[MAX_LENGTH];
}RecordT;

//function prototype define 
int menuDisplay();
// void setOption(int*, int*, int*);
int insertContact(PF_FileHandle &fh);
int deleteContact(PF_FileHandle &fh);
int updateContact(PF_FileHandle &fh);
int searchContact(PF_FileHandle &fh);
int showAllContact(PF_FileHandle &fh);
int insert40Contacts(PF_FileHandle &fh);
void showBuffer(PF_Manager &pfm);
int findAllPerson(PF_FileHandle &fh);

//user-defined funtion
void recordTo_pData(RecordT record, char* pData)
{
    //insert record to pData
    //sprintf input string into array
    //which is indicated by pData
    sprintf(pData, "%s,%s,%s,%s,",
            record.name, record.addr, record.telnum, record.email);
}

RecordT pDataToRecord(char* pData)
{
    //insert pData to record
    RecordT person;
    char* temp = (char *)malloc(4096);
    strncpy(temp, pData, 4096);
    strcpy(person.name, strtok(temp, ","));
    strcpy(person.addr, strtok(NULL, ","));
    strcpy(person.telnum, strtok(NULL, ","));
    strcpy(person.email, strtok(NULL, ","));
    free(temp);
    return person;
}

int findPage(char* name, PF_FileHandle &fh, PF_PageHandle &ph, char* &pData)
{
    PageNum pageNum;
    RC rc;
    RecordT person;
    char* old;

    if(rc = fh.GetFirstPage(ph)){
        return (rc);
    }
    
    do{
        if( (rc = ph.GetData(old)) ||
            (rc = ph.GetPageNum(pageNum))){
            return (rc);
        }
        person = pDataToRecord(old);
        if(strcmp(person.name, name) == 0){
            pData = old;
            return pageNum;
        }
        if(rc = fh.UnpinPage(pageNum)){
            return (rc);
        }
    } while (!(rc = fh.GetNextPage(pageNum, ph)));

    return -1;
}

int main(){
    PF_Manager pfm; //I/O function, FileHdr
    PF_FileHandle fh; //get,allocate,dispose page by using ph
    PF_PageHandle ph; //provide access to the contents of a page
    RC rc; //type int
	int select;

    //if rc is zero, creating file is succeed
    //initialize fileHdr
    //write numBytes in file hdrbuf
    //O_EXCL makes open function return -1,
    //when there is file already.
	if(rc = pfm.CreateFile("Contactlist.dat")){
		printf("\nDataFile is already exist!\n");
	}

    //check if file is opened correctly
    //if not, return -1
    if((rc = pfm.OpenFile("Contactlist.dat", fh))){
        printf("DataFile open error!\n");
        return (rc); 
    }
	
	system("clear");
	while((select = menuDisplay()) != 6){
		switch(select){
			case 1 :
				insertContact(fh);
				break;
			case 2 :
				deleteContact(fh);
				break;
			case 3 :
				updateContact(fh);
				break;
			case 4 :
				searchContact(fh);
				break;
			case 5 :
				showAllContact(fh);
				break;
			default :
				break;
		}
	}

    //flush all buffers for this file and write out the header
    //close the file
    //reset
	pfm.CloseFile(fh);

    return 0;
}

// Print the menu screen
int menuDisplay(){
	int select;

	system("clear");
	printf("\n  CONTACT MANAGER\n");
	printf("=====================\n");
	printf("    1. Insert\n");
	printf("    2. Delete\n");
	printf("    3. Update\n");
	printf("    4. Search\n");
	printf("    5. Show All\n");
	printf("    6. Exit\n");
	printf("=====================\n");
	printf("Press Menu Number>>");
	
	select = getchar() - 48;
	system("clear");

	return select;
}


// Insert the item in a record unit
int insertContact(PF_FileHandle &fh){
    RC rc;
    char* pData; 
    PageNum pageNum;
    PF_PageHandle ph;
    RecordT person;

    printf("name : ");
    scanf("%s", person.name);
    printf("addr : ");
    scanf("%s", person.addr);
    printf("telnum : ");
    scanf("%s",person.telnum);
    printf("email : ");
    scanf("%s", person.email);
    
    //insert pagenum into pData
    if( (rc = fh.AllocatePage(ph)) ||
        (rc = ph.GetData(pData)) ||
        (rc = ph.GetPageNum(pageNum))){
        return (rc);
    }
    recordTo_pData(person, pData);

    fh.MarkDirty(pageNum);
    fh.UnpinPage(pageNum);

	///////////////////////
	// Your Assignment ! //
	///////////////////////

	printf("\nPress Enter key to go back menu screen\n");
	getchar();
	getchar();

    return 0;
}

// Delete the item in a record unit by the appropriate field
int deleteContact(PF_FileHandle &fh){
    RC rc;
    PF_PageHandle ph;
    char person_name[32];
    RecordT person;
    char* pData;
    PageNum pageNum;

    printf("Input a name that you want to delete: ");
    scanf("%s", person_name);

    pageNum = findPage(person_name, fh, ph, pData);

    if(pageNum == -1){
        printf("\nYour Information doesn't exist\n");
        return -1;
    }

    if(rc = fh.UnpinPage(pageNum)){
        return (rc);
    }

    if( rc = fh.DisposePage(pageNum)){
        return (rc);
    }

	///////////////////////
	// Your Assignment ! //
	///////////////////////

	printf("\nPress Enter key to go back menu screen\n");
	getchar();
	getchar();
}

// Update the item in a record unit by the appropriate field
int updateContact(PF_FileHandle &fh){
    PF_PageHandle ph;
    char person_name[32];
    RecordT person;
    char* pData;
    PageNum pageNum;

    printf("Input a name that you want to update: ");
    scanf("%s", person_name);

    pageNum = findPage(person_name, fh, ph, pData);

    if(pageNum == -1){
        printf("\nYour Information doesn't exist\n");
        return -1;
    }

    printf("name : ");
    scanf("%s", person.name);
    printf("addr : ");
    scanf("%s", person.addr);
    printf("telnum : ");
    scanf("%s",person.telnum);
    printf("email : ");
    scanf("%s", person.email);
    
    //insert pagenum into pData
    recordTo_pData(person, pData);

    fh.MarkDirty(pageNum);
    fh.UnpinPage(pageNum);

	///////////////////////
	// Your Assignment ! //
	///////////////////////

	printf("\nPress Enter key to go back menu screen\n");
	getchar();
	getchar();

}

// Search the item in a record unit by the appropriate field
int searchContact(PF_FileHandle &fh){
    PF_PageHandle ph;
    char person_name[32];
    RecordT person;
    char* pData;
    PageNum pageNum;

    printf("Input a name that you want to search: ");
    scanf("%s", person_name);

    pageNum = findPage(person_name, fh, ph, pData);

    if(pageNum == -1){
        printf("\nYour Information doesn't exist\n");
        getchar();
        getchar();
        return -1;
    }

    person = pDataToRecord(pData);

    printf("\n      The result you want\n");
    printf("==============================\n");
    printf("name    : %s\n", person.name);
    printf("address : %s\n", person.addr);
    printf("contact : %s\n", person.telnum);
    printf("mail    : %s\n", person.email);
    printf("=============================\n");

	///////////////////////
	// Your Assignment ! //
	///////////////////////

	printf("\nPress Enter key to go back menu screen\n");
	getchar();
	getchar();

    return 0;
}

// Print all Items existing in file
int showAllContact(PF_FileHandle &fh){
    RC rc;
    PF_PageHandle ph;
    PageNum pageNum;
    RecordT person;
    char* pData;
    if(rc = fh.GetFirstPage(ph)){
        return (rc);
    }
   
    printf("==============================\n");
    do{
        if((rc = ph.GetData(pData)) ||
           (rc = ph.GetPageNum(pageNum)))
            return (rc);
        person = pDataToRecord(pData);
        printf("name    : %s\n", person.name);
        printf("address : %s\n", person.addr);
        printf("contact : %s\n", person.telnum);
        printf("mail    : %s\n", person.email);
        printf("=============================\n");
        if(rc = fh.UnpinPage(pageNum)){
            return (rc);
        }
    } while(!(rc = fh.GetNextPage(pageNum, ph))); 

	///////////////////////
	// Your Assignment ! //
	///////////////////////

	printf("\nPress Enter key to go back menu screen\n");
	getchar();
	getchar();

    return 0;
}



