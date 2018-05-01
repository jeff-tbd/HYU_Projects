#include <iostream>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string>
#include "pf.h"
#include "pf_internal.h"
#include "pf_hashtable.h"

using namespace std;

#define MAX_LENGTH 32

typedef struct record {
	char name[MAX_LENGTH];
	char addr[MAX_LENGTH * 2];
	char telnum[MAX_LENGTH];
	char email[MAX_LENGTH];
}RecordT;

int menuDisplay();
int findPage(char * name, PF_FileHandle &fh, PF_PageHandle &ph, char* &pData);
int findAllPerson(PF_FileHandle &fh);
int insertContact(PF_FileHandle &fh);
int deleteContact(PF_FileHandle &fh);
int updateContact(PF_FileHandle &fh);
int searchContact(PF_FileHandle &fh);
int showAllContact(PF_FileHandle &fh);
int insert40Contacts(PF_FileHandle &fh);
void showBuffer(PF_Manager &pfm);

void recordTo_pData(RecordT person, char * pData);
RecordT pDataToRecord(char * pData);


int main() {
	PF_Manager pfm;
	PF_FileHandle fh;
	PF_PageHandle ph;
	RC rc;
	int select;
	if (rc = pfm.CreateFile("Contactlist.dat")) {
		printf("\nDataFile is already exist!");
	}
	if ((rc = pfm.OpenFile("Contactlist.dat", fh))) {
		printf("DataFile open error!\n");
		return (rc);
	}



	system("clear");
	while ((select = menuDisplay()) != 8) {
		switch (select) {
		case 1:
			insertContact(fh);
			break;
		case 2:
			deleteContact(fh);
			break;
		case 3:
			updateContact(fh);
			break;
		case 4:
			searchContact(fh);
			break;
		case 5:
			showAllContact(fh);
			break;
		case 6:
			insert40Contacts(fh);
			break;
		case 7:
			showBuffer(pfm);
			break;
		default:
			break;
		}
	}

	pfm.CloseFile(fh);


	return 0;
}

int menuDisplay() {
	int select;

	system("clear");
	printf("\n  CONTACT MANAGER\n");
	printf("=====================\n");
	printf("    1. Insert\n");
	printf("    2. Delete\n");
	printf("    3. Update\n");
	printf("    4. Search\n");
	printf("    5. Show All\n");
	printf("    6. Insert 40 Pages\n");
	printf("    7. Show Buffer\n");
	printf("    8. Exit\n");
	printf("=====================\n");
	printf("Press Menu Number>>");

	select = getchar() - 48;
	system("clear");

	return select;
}


int insertContact(PF_FileHandle &fh) {
	RC rc;
	char * pData;
	PageNum pageNum;
	PF_PageHandle ph;
	RecordT person;
	printf("name : ");
	scanf("%s", person.name);
	printf("addr : ");
	scanf("%s", person.addr);
	printf("telnum : ");
	scanf("%s", person.telnum);
	printf("email : ");
	scanf("%s", person.email);
	if ((rc = fh.AllocatePage(ph)) ||
		(rc = ph.GetData(pData)) ||
		(rc = ph.GetPageNum(pageNum)))
		return (rc);
	recordTo_pData(person, pData);
	fh.MarkDirty(pageNum);
	fh.UnpinPage(pageNum);

	printf("\nPress Enter key to go back menu screen\n");
	getchar();
	getchar();

	return 0;
}

int updateContact(PF_FileHandle &fh) {


	printf("\nPress Enter key to go back menu screen\n");
	getchar();
	getchar();

	return 0;
}


int deleteContact(PF_FileHandle &fh) {


	printf("\nPress Enter key to go back menu screen\n");
	getchar();
	getchar();

	return 0;
}


int searchContact(PF_FileHandle &fh) {


	printf("\nPress Enter key to go back menu screen\n");
	getchar();
	getchar();

	return 0;
}


int showAllContact(PF_FileHandle &fh) {


	printf("\nPress Enter key to go back menu screen\n");
	getchar();
	getchar();

	return 0;
}


int findPage(char * name, PF_FileHandle &fh, PF_PageHandle &ph, char* &pData) {
	return 0;
}


void recordTo_pData(RecordT record, char * pData) {
	sprintf(pData, "%s,%s,%s,%s,", record.name, record.addr, record.telnum, record.email);
}


RecordT pDataToRecord(char * pData) {
	RecordT person;
	char * temp = (char *)malloc(4096);
	strncpy(temp, pData, 4096);
	strcpy(person.name, strtok(temp, ","));
	strcpy(person.addr, strtok(NULL, ","));
	strcpy(person.telnum, strtok(NULL, ","));
	strcpy(person.email, strtok(NULL, ","));
	free(temp);
	return person;
}

int insert40Contacts(PF_FileHandle &fh) {
	RC rc;
	char * pData;
	PageNum pageNum;
	PF_PageHandle ph;
	RecordT person;

	if (fh.hdr.numPages >= 40) {
		system("clear");
		printf("already exist 40 Pages!!\n");
		printf("\nPress Enter key to go back menu screen\n");
		getchar();
		getchar();
		return 0;
	}

	for (int i = 0; i < 40; i++) {
		sprintf(person.name, "test%d", i);
		sprintf(person.addr, "test%d", i);
		sprintf(person.telnum, "test%d", i);
		sprintf(person.email, "test%d", i);

		if ((rc = fh.AllocatePage(ph)) ||
			(rc = ph.GetData(pData)) ||
			(rc = ph.GetPageNum(pageNum)))
			return (rc);
		recordTo_pData(person, pData);
		fh.MarkDirty(pageNum);
		fh.UnpinPage(pageNum);
	}

	system("clear");
	printf("insert 40 Pages!!\n");
	printf("\nPress Enter key to go back menu screen\n");
	getchar();
	getchar();

	return 0;
}

void showBuffer(PF_Manager &pfm) {
	system("clear");

	pfm.PrintBuffer();

	printf("\nPress Enter key to go back menu screen\n");
	getchar();
	getchar();
}


