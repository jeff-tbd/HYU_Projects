//
// ql_manager_stub.cc
//

// Note that for the SM component (HW3) the QL is actually a
// simple stub that will allow everything to compile.  Without
// a QL stub, we would need two parsers.

#include <cstdio>
#include <iostream>
#include <algorithm>
#include <sys/times.h>
#include <sys/types.h>
#include <cassert>
#include <unistd.h>
#include "redbase.h"
#include "sm.h"
#include "ix.h"
#include "rm.h"
//
#include "ql_internal.h"
#include "printer.h"
#include "iterator.h"
#include <map>
using namespace std;

namespace {
bool strlt(char* i, char* j) {
	return (strcmp(i, j) < 0);
}

bool streq(char* i, char* j) {
	return (strcmp(i, j) == 0);
}
}
//
//
// QL_Manager::QL_Manager(SM_Manager &smm, IX_Manager &ixm, RM_Manager &rmm)
//
// Constructor for the QL Manager
//
QL_Manager::QL_Manager(SM_Manager &smm, IX_Manager &ixm, RM_Manager &rmm) :
	smm(smm), ixm(ixm), rmm(rmm) {
	// Can't stand unused variable warnings!
	assert (&smm && &ixm && &rmm);
}

//
// QL_Manager::~QL_Manager()
//
// Destructor for the QL Manager
//
QL_Manager::~QL_Manager() {
}

//
// Handle the select clause
//
RC QL_Manager::Select(int nSelAttrs, const RelAttr selAttrs_[], int nRelations,
		const char * const relations_[], int nConditions,
		const Condition conditions_[]) {
	int i;

	cout << "Select\n";

	cout << "   nSelAttrs = " << nSelAttrs << "\n";
	for (i = 0; i < nSelAttrs; i++)
		cout << "   selAttrs[" << i << "]:" << selAttrs_[i] << "\n";

	cout << "   nRelations = " << nRelations << "\n";
	for (i = 0; i < nRelations; i++)
		cout << "   relations[" << i << "] " << relations_[i] << "\n";

	cout << "   nCondtions = " << nConditions << "\n";
	for (i = 0; i < nConditions; i++)
		cout << "   conditions[" << i << "]:" << conditions_[i] << "\n";
	///////////////////////////////////////////////////////////////////////////////////
	char** relations = new char*[nRelations];
	for (i = 0; i < nRelations; i++) {
		// strncpy(relations[i], relations_[i], MAXNAME);
		relations[i] = strdup(relations_[i]);
	}

	Condition* conditions = new Condition[nConditions];
	for (i = 0; i < nConditions; i++) {
		conditions[i] = conditions_[i];
	}

	sort(relations, relations + nRelations, strlt);

	char** dup = adjacent_find(relations, relations + nRelations, streq);
	if (dup != (relations + nRelations))
		return QL_DUPREL;

	// copies for rewrite
	RelAttr* selAttrs = new RelAttr[nSelAttrs];
	for (i = 0; i < nSelAttrs; i++) {
		selAttrs[i].relName = selAttrs_[i].relName;
		selAttrs[i].attrName = selAttrs_[i].attrName;
	}

	// rewrite select *
	bool SELECTSTAR = false;
	if (nSelAttrs == 1 && strcmp(selAttrs[0].attrName, "*") == 0) {

		cout<<"rewrite select *"<<endl;

		SELECTSTAR = true;
		nSelAttrs = 0;
		for (int i = 0; i < nRelations; i++) {
			int ac;
			DataAttrInfo * aa;
			RC rc = smm.GetFromTable(relations[i], ac, aa);
			if (rc != 0)
				return rc;
			nSelAttrs += ac;
			delete [] aa;
		}

		delete [] selAttrs;
		selAttrs = new RelAttr[nSelAttrs];

		int j = 0;
		for (int i = 0; i < nRelations; i++) {
			int ac;
			DataAttrInfo * aa;
			RC rc = smm.GetFromTable(relations[i], ac, aa);
			if (rc != 0)
				return rc;
			for (int k = 0; k < ac; k++) {
				selAttrs[j].attrName = strdup(aa[k].attrName);
				selAttrs[j].relName = relations[i];
				j++;
			}
			delete [] aa;
		}
	} // if rewrite select "*"

	cout<<"rewrite select *,nSelect = "<<nSelAttrs<<endl;
	for (int i = 0; i < nSelAttrs; i++) {
		cout<<"rewrite select *,attr i:"<<selAttrs[i].attrName<<endl;
	}

	///////////////////////////////////////////////////////////////////////////////////
	// ensure that all relations mentioned in conditions are in the from clause
	for (int i = 0; i < nConditions; i++) {
		bool lfound = false;

		for (int j = 0; j < nRelations; j++) {
			if (conditions[i].lhsAttr.relName == NULL) {
				return QL_MISSRELATION;
			}

			if (strcmp(conditions[i].lhsAttr.relName, relations[j]) == 0) {
				lfound = true;
				break;
			}
		}

		if (!lfound)
			return QL_RELMISSINGFROMFROM;

		if (conditions[i].bRhsIsAttr == TRUE) {
			bool rfound = false;
			for (int j = 0; j < nRelations; j++) {
				if (strcmp(conditions[i].rhsAttr.relName, relations[j]) == 0) {
					rfound = true;
					break;
				}
			}

			cout<<"3. hello!!!!"<<endl;

			if (!rfound)
				return QL_RELMISSINGFROMFROM;
		}
	}

	Iterator* it= NULL;

	if (nRelations == 1) {
		int order = 0;//useless here, just for the orderby in future
		bool group = false;
		RelAttr* orderAttr= NULL;//useless here
		RelAttr* groupAttr= NULL;

		cout<<"1.rel name="<<relations[0]<<endl;
		cout<<"1.nCondition="<<nConditions<<endl;
		cout<<"1.condition="<<conditions<<endl;

		it = GetLeafIterator(relations[0], nConditions, conditions, 0, NULL,
				order, orderAttr);

		cout<<"get leafiterator success...next step..."<<endl;

		if (it!=NULL) {

			//          RC QL_Manager::MakeRootIterator(Iterator*& newit,
			//                                          int nSelAttrs, const RelAttr selAttrs[],
			//                                          int nRelations, const char * const relations[],
			//                                          int order, RelAttr orderAttr,
			//                                          bool group, RelAttr groupAttr)

			RC rc = MakeRootIterator(it, nSelAttrs, selAttrs, nRelations,
					relations);
			//          RC rc = MakeRootIterator(it, nSelAttrs, selAttrs,
			//                                   nRelations, relations,
			//                                   order, *orderAttr,
			//                                   group, *groupAttr);
			if (rc != 0)
				return rc;

			rc = PrintIterator(it);
			if (rc != 0)
				return rc;
		}
	}

	if (it!=NULL)
		delete it;
	////////////////////////////////////////////////////////////////////
	return 0;
}

//
// Insert the values into relName
//
RC QL_Manager::Insert(const char *relName, int nValues, const Value values[]) {
	RC rc;
	char* data= NULL;
	int attrCount;
	DataAttrInfo* attr= NULL;
	int size = 0;
	int offset = 0;
	int i = 0;

	cout << "Insert\n";
	cout << "   relName = " << relName << "\n";
	cout << "   nValues = " << nValues << "\n";

	for (i = 0; i < nValues; i++)
		cout << "   values[" << i << "]:" << values[i] << "\n";

	//step1: get attributes information
	if ((rc = smm.GetFromTable(relName, attrCount, attr))) {
		rc = SM_BADTABLE;
		goto err_delteattributes;
	}

	if (nValues != attrCount) {
		rc = QL_INVALIDSIZE;
		goto err_delteattributes;
	}
	//step2: examin the insert attibute
	for (int i =0; i < nValues; i++) {
		if (values[i].type != attr[i].attrType) {
			delete [] attr;
			return QL_JOINKEYTYPEMISMATCH;
		}
		size += attr[i].attrLength;
	}
	//step3: Make record data
	data = new char[size];
	if (data == NULL) {
		rc = SM_NOMEM;
		goto err_delteattributes;
	}
	offset = 0;
	for (int i =0; i < attrCount; i++) {
		memcpy(data + offset, values[i].data, attr[i].attrLength);
		offset += attr[i].attrLength;
	}
	//step4:write into file and updata related files(index)
	if ((rc = smm.InsertRecord(relName, attrCount, attr, data))) {
		goto err_deletedata;
	}

	delete [] data;
	delete [] attr;
	return (0);
	err_deletedata: delete [] data;
	err_delteattributes: if (attr!=NULL)
		delete [] attr;
	return (rc);
}

//
// Delete from the relName all tuples that satisfy conditions
//
RC QL_Manager::Delete(const char *relName_, int nConditions,
		const Condition conditions_[]) {
	RC rc;
	int i;

	cout << "Delete\n";
	cout << "   relName = " << relName_ << "\n";
	cout << "   nCondtions = " << nConditions << "\n";
	for (i = 0; i < nConditions; i++)
		cout << "   conditions[" << i << "]:" << conditions_[i] << "\n";

	char relName[MAXNAME];
	strncpy(relName, relName_, MAXNAME);

	Condition* conditions = new Condition[nConditions];
	for (int i = 0; i < nConditions; i++) {
		conditions[i] = conditions_[i];
	}

	for (int i = 0; i < nConditions; i++) {
		if (conditions[i].lhsAttr.relName == NULL) {
			conditions[i].lhsAttr.relName = relName;
		}
		if (strcmp(conditions[i].lhsAttr.relName, relName) != 0) {
			delete [] conditions;
			return QL_BADATTR;
		}

		if (conditions[i].bRhsIsAttr == TRUE) {
			if (conditions[i].rhsAttr.relName == NULL) {
				conditions[i].rhsAttr.relName = relName;
			}
			if (strcmp(conditions[i].rhsAttr.relName, relName) != 0) {
				delete [] conditions;
				return QL_BADATTR;
			}
		}
	}

	Iterator* it = GetLeafIterator(relName, nConditions, conditions);

	//    if(bQueryPlans == TRUE)
	//      cout << "\n" << it->Explain() << "\n";

	Tuple t = it->GetTuple();
	if ((rc = it->Open()))
		return rc;

	RM_FileHandle fh;
	if ((rc = rmm.OpenFile(relName, fh)))
		return rc;

	int attrCount = -1;
	DataAttrInfo * attributes;
	rc = smm.GetFromTable(relName, attrCount, attributes);

	if (rc != 0)
		return rc;
	IX_IndexHandle * indexes = new IX_IndexHandle[attrCount];
	for (int i = 0; i < attrCount; i++) {
		if (attributes[i].indexNo != -1) {
			ixm.OpenIndex(relName, attributes[i].indexNo, indexes[i]);
		}
	}

	while (1) {
		rc = it->GetNext(t);
		if (rc == it->Eof())
			break;
		if (rc != 0)
			return rc;

		rc = fh.DeleteRec(t.GetRid());
		if (rc != 0)
			return rc;

		for (int i = 0; i < attrCount; i++) {
			if (attributes[i].indexNo != -1) {
				void * pKey;
				t.Get(attributes[i].offset, pKey);
				indexes[i].DeleteEntry(pKey, t.GetRid());
			}
		}
	}

	for (int i = 0; i < attrCount; i++) {
		if (attributes[i].indexNo != -1) {
			RC rc = ixm.CloseIndex(indexes[i]);
			if (rc != 0)
				return rc;
		}
	}
	delete [] indexes;
	delete [] attributes;

	rc = rmm.CloseFile(fh);
	if (rc != 0)
		return rc;

	delete [] conditions;
	rc = it->Close();
	if (rc != 0)
		return rc;

	delete it;
	cerr << "done with delete it" << endl;
	return 0;
}

//
// Update from the relName all tuples that satisfy conditions
//
RC QL_Manager::Update(const char *relName, const RelAttr &updAttr,
		const int bIsValue, const RelAttr &rhsRelAttr, const Value &rhsValue,
		int nConditions, const Condition conditions[]) {
	int i;

	cout << "Update\n";

	cout << "   relName = " << relName << "\n";
	cout << "   updAttr:" << updAttr << "\n";
	if (bIsValue)
		cout << "   rhs is value: " << rhsValue << "\n";
	else
		cout << "   rhs is attribute: " << rhsRelAttr << "\n";

	cout << "   nCondtions = " << nConditions << "\n";
	for (i = 0; i < nConditions; i++)
		cout << "   conditions[" << i << "]:" << conditions[i] << "\n";
	char mRelName[MAXNAME];
	strncpy(mRelName, relName, MAXNAME);
	Condition* mConditions = new Condition[nConditions];
	for (int i = 0; i<nConditions; i++) {
		mConditions[i] = conditions[i];
	}
	RelAttr updateAttr;
	updateAttr.relName = mRelName;
	updateAttr.attrName = updAttr.attrName;
	Condition cond;
	cond.lhsAttr = updateAttr;
	cond.bRhsIsAttr = (bIsValue== true ? false : true);
	cond.rhsAttr.attrName = rhsRelAttr.attrName;
	cond.rhsAttr.relName = mRelName;
	cond.op = EQ_OP;
	cond.rhsValue.type = rhsValue.type;
	cond.rhsValue.data = rhsValue.data;
	if (bIsValue != true)
{		updateAttr.attrName = rhsRelAttr.attrName;
	}
	for(int i =0;i<nConditions; i++) {
		if(mConditions[i].lhsAttr.relName==NULL) {
			mConditions[i].lhsAttr.relName = mRelName;
		}
		if(strcmp(mConditions[i].lhsAttr.relName,mRelName)!=0) {
			delete [] mConditions;
			return QL_BADATTR;
		}
		if(mConditions[i].bRhsIsAttr==true) {
			if(mConditions[i].rhsAttr.relName==NULL) {
				mConditions[i].rhsAttr.relName = mRelName;
			}
			if(strcmp(mConditions[i].rhsAttr.relName,mRelName)!=0) {
				delete [] mConditions;
				return QL_BADATTR;
			}
		}
	}
	//for evit halloween problem
	Iterator* it;
	if(smm.IsAttrIndexed(updateAttr.relName,updateAttr.attrName)) {
		///drop the index
		//RC rc = smm.DropIndexFromAttrCatAlone(updAttr.relName, updAttr.attrName);
	//	if (rc ) return rc;
		it = GetLeafIterator(mRelName,nConditions,mConditions);
		///Reset the index
		//rc = smm.ResetIndexFromAttrCatAlone(updAttr.relName, updAttr.attrName);
	    //if (rc ) return rc;
	}
	else {
		it = GetLeafIterator(mRelName,nConditions,mConditions);
	}
	Tuple t = it->GetTuple();
	RC rc = it->Open();
	if(rc) return rc;
	void * val = NULL;
	if(bIsValue == true) val = rhsValue.data;
	else t.Get(rhsRelAttr.attrName,val);
	DataAttrInfo * info = t.GetAttributes();
	int Count = t.GetAttrCount();
	Printer p(info,Count);
	p.PrintHeader(cout);
	RM_FileHandle fh;
	rc = rmm.OpenFile(mRelName,fh);
	if(rc) return rc;
	int attrCount = -1;
	int updAttrOffset = -1;
	DataAttrInfo * attributes;
	rc = smm.GetFromTable(mRelName, attrCount, attributes);
	if(rc ) return rc;
	IX_IndexHandle* indexes = new IX_IndexHandle[attrCount];
	for(int i = 0;i<attrCount;i++) {
		if(attributes[i].indexNo!=-1&&
				strcmp(attributes[i].attrName,updateAttr.attrName)==0) {
			ixm.OpenIndex(mRelName,attributes[i].indexNo,indexes[i]);
		}
		if(strcmp(attributes[i].attrName,updateAttr.attrName)==0) {
			updAttrOffset = attributes[i].offset;
		}
	}
	while(1) {
		rc = it->GetNext(t);
		if(rc == it->Eof()) break;
		if(rc!=0) return rc;
		RM_Record rec;
		for(int i = 0;i<attrCount; i++) {
			if(attributes[i].indexNo!= -1&&
					strcmp(attributes[i].attrName,updateAttr.attrName)==0) {
				void *pKey;
				t.Get(attributes[i].offset,pKey);
				rc = indexes[i].DeleteEntry(pKey,t.GetRid());
				if(rc ) return rc;
				rc = indexes[i].InsertEntry(val,t.GetRid());
				if(rc) return rc;
			}
		}
		t.Set(updAttrOffset,val);
		
		char * newbuf;
		t.GetData(newbuf);

		rec.Set(newbuf,it->TupleLength(),t.GetRid());
		rc = fh.UpdateRec(rec);
		if(rc) return rc;
		const char * pData;
		
		t.GetData(pData);
		
		p.Print(cout,pData);
	}
	p.PrintFooter(cout);
	rc = it->Close();
	if (rc != 0) return rc;
	
	for (int i = 0; i < attrCount; i++) {
		if(attributes[i].indexNo != -1 &&
				strcmp(attributes[i].attrName, updateAttr.attrName) == 0) {
			RC rc = ixm.CloseIndex(indexes[i]);
			if(rc ) return rc;
		}
	}

	delete [] indexes;
	delete [] attributes;

	rc = rmm.CloseFile(fh);
	
	if (rc != 0) return rc;

	delete it;
	delete [] mConditions;
	
	
	return 0;
}

//
// Choose between filescan and indexscan for first operation
//- leaf level of operator tree
// REturned iterator should be deleted by user after use.
Iterator* QL_Manager::GetLeafIterator(const char *relName, int nConditions,
		const Condition conditions[],

		int nJoinConditions, const Condition jconditions[], int order,
		RelAttr* porderAttr) {

	if (relName == NULL) {
		return NULL;
	}

	int attrCount = -1;
	DataAttrInfo * attributes;
	RC rc = smm.GetFromTable(relName, attrCount, attributes);
	if (rc != 0)
		return NULL;

	int nIndexes = 0;
	char* chosenIndex= NULL;

	Condition * filters= NULL;
	int nFilters = -1;

	const Condition * chosenCond= NULL;
	Condition* jBased= NULL;
	map<string, const Condition*> jkeys;
	for (int j = 0; j < nJoinConditions; j++) {
		if (strcmp(jconditions[j].lhsAttr.relName, relName) == 0) {
			jkeys[string(jconditions[j].lhsAttr.attrName)] = &jconditions[j];
		}

		if (jconditions[j].bRhsIsAttr == TRUE && strcmp(
				jconditions[j].rhsAttr.relName, relName) == 0) {
			jkeys[string(jconditions[j].rhsAttr.attrName)] = &jconditions[j];
		}
	}

	for (map<string, const Condition*>::iterator itJkeys = jkeys.begin(); itJkeys
			!= jkeys.end(); itJkeys++) {
		// Pick last numerical index or at least one non-numeric index
		for (int i = 0; i < attrCount; i++) {
			if (attributes[i].indexNo != -1 && strcmp(itJkeys->first.c_str(),
					attributes[i].attrName) == 0) {
				nIndexes++;
				//just creat index for int||float attributes
				if (chosenIndex == NULL || attributes[i].attrType == INT
						|| attributes[i].attrType == FLOAT) {
					chosenIndex = attributes[i].attrName;
					jBased = (Condition*&)itJkeys->second;
					jBased->lhsAttr.attrName = chosenIndex;
					jBased->bRhsIsAttr = FALSE;
					jBased->rhsValue.type = attributes[i].attrType;
					jBased->rhsValue.data = NULL;

					chosenCond = jBased;
					cout<< "chose index for iscan based on join condition "
							<< *(chosenCond) << endl;
				}
			}
		}
	}
	// if join conds resulted in chosenCond
	if (chosenCond != NULL) {
		nFilters = nConditions;
		filters = new Condition[nFilters];
		for (int j = 0; j < nConditions; j++) {
			if (chosenCond != &(conditions[j])) {
				filters[j] = conditions[j];
			}
		}
	}
	//else: no join conds
	else {
		// (chosenCond == NULL) // prefer join cond based index
		map<string, const Condition*> keys;

		for (int j = 0; j < nConditions; j++) {
			if (strcmp(conditions[j].lhsAttr.relName, relName) == 0) {
				keys[string(conditions[j].lhsAttr.attrName)] = &conditions[j];
			}

			if (conditions[j].bRhsIsAttr == TRUE && strcmp(
					conditions[j].rhsAttr.relName, relName) == 0) {
				keys[string(conditions[j].rhsAttr.attrName)] = &conditions[j];
			}
		}

		// Pick last numerical index or at least one non-numeric index
		for (map<string, const Condition*>::iterator it = keys.begin(); it
				!= keys.end(); it++) {
			for (int i = 0; i < attrCount; i++) {
				if (attributes[i].indexNo != -1 && strcmp(it->first.c_str(),
						attributes[i].attrName) == 0) {
					nIndexes++;
					if (chosenIndex == NULL || attributes[i].attrType == INT
							|| attributes[i].attrType == FLOAT) {
						chosenIndex = attributes[i].attrName;
						chosenCond = it->second;
					}
				}
			}
		}

		if (chosenCond == NULL) {
			nFilters = nConditions;
			filters = new Condition[nFilters];
			for (int j = 0; j < nConditions; j++) {
				if (chosenCond != &(conditions[j])) {
					filters[j] = conditions[j];
				}
			}
		} else {
			nFilters = nConditions - 1;
			filters = new Condition[nFilters];
			for (int j = 0, k = 0; j < nConditions; j++) {
				if (chosenCond != &(conditions[j])) {
					filters[k] = conditions[j];
					k++;
				}
			}
		}
	}

	Iterator* it= NULL;

	//use a file scan
	if (chosenCond == NULL && (nConditions == 0 || nIndexes == 0)) {
		cout<<"use a file scan"<<endl;
		Condition* cond= NULL;
		it = new FileScan(smm,rmm,relName);

		if (nConditions == 0) {
			rc = it->CreateScan(cond);
		} else {
			//cout<<"3.condition="<< *conditions<<endl;
			cond = new Condition();
			*cond= conditions[0];
			rc = it->CreateScan(cond, nConditions, conditions);
		}
		if (rc != 0) {
			QL_PrintError(rc);
			return NULL;
		}
		delete [] filters;
		delete [] attributes;
		return it;
	}

	// else:use an index scan
	cout<<"use an index scan"<<endl;
	rc = -1;
	it = NULL;

	bool desc = false;
	if (order != 0 && strcmp(porderAttr->relName, relName) == 0 && strcmp(
			porderAttr->attrName, chosenIndex) == 0)
		desc = (order == -1 ? true : false);

	if (chosenCond != NULL) {
		if (chosenCond->op == EQ_OP || chosenCond->op == GT_OP
				|| chosenCond->op == GE_OP)
			if (order == 0) // use only if there is no order-by
				desc = true; // more optimal

		it = new IndexScan(smm, rmm, ixm, relName, chosenIndex, rc,
				*chosenCond, nFilters, filters, desc);
	} else
		// non-conditional index scan
		it = new IndexScan(smm, rmm, ixm, relName, chosenIndex, rc,
				*chosenCond, nFilters, filters, desc);

	if (rc != 0) {
		QL_PrintError(rc);
		return NULL;
	}

	delete [] filters;
	delete [] attributes;
	return it;
}

////////////////////////////////////////////////////////////////////////////
RC QL_Manager::MakeRootIterator(Iterator*& newit, int nSelAttrs,
		const RelAttr selAttrs_[], int nRelations,
		const char * const relations[]) {

	RelAttr* selAttrs = new RelAttr[nSelAttrs];
	for (int i = 0; i < nSelAttrs; i++) {
		if (selAttrs_[i].relName==0) {
			return QL_MISSRELATION;
		}
		selAttrs[i].relName = selAttrs_[i].relName;
		selAttrs[i].attrName = selAttrs_[i].attrName;
	}
	//    cout<<"nSelAttr:"<<nSelAttrs<<endl;
	//    cout << "newit info"<<newit->GetAttrCount()<<endl;

	newit = new Projection(newit,nSelAttrs,selAttrs);
	return 0;
}

RC QL_Manager::MakeRootIterator(Iterator*& newit, int nSelAttrs,
		const RelAttr selAttrs[], int nRelations,
		const char * const relations[], int order, RelAttr orderAttr,
		bool group, RelAttr groupAttr) {
	//RC rc ;
	//if((rc = Projection(newit, nSelAttrs, selAttrs)))
	//return rc;
	newit = new Projection(newit,nSelAttrs,selAttrs);
	return 0;
}

RC QL_Manager::PrintIterator(Iterator* it) const {
	//  if(bQueryPlans == TRUE)
	//    cout << "\n" << it->Explain() << "\n";

	Tuple t = it->GetTuple();
	RC rc = it->Open();
	if (rc != 0)
		return rc;

	int attrCount = t.GetAttrCount();
	DataAttrInfo* attrs = t.GetAttributes();

	Printer* p = new Printer(attrs,attrCount);
	p->PrintHeader(cout);
	while (1) {
		rc = it->GetNext(t);
		if (rc == it->Eof())
			break;
		if (rc != 0)
			return rc;
		//cout << "QL_Manager::PrintIterator tuple " << t << endl;
		const char* data= NULL;
		t.GetData(data);
		p->Print(cout, data);
	}
	p->PrintFooter(cout);

	rc = it->Close();

	if (p==NULL)
		delete p;
	if (rc != 0)
		return rc;
	return 0;
}

