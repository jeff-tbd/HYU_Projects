//
// ql.h
//   Query Language Component Interface
//

// This file only gives the stub for the QL component

#ifndef QL_H
#define QL_H

#include <stdlib.h>
#include <string.h>
#include "redbase.h"
#include "parser.h"
#include "rm.h"
#include "ix.h"

#include "sm_internal.h"
#include "iterator.h"
// QL_Manager: query language (DML)
//
class QL_Manager {
public:
    QL_Manager (SM_Manager &smm, IX_Manager &ixm, RM_Manager &rmm);
    ~QL_Manager();                       // Destructor

    RC Select  (int nSelAttrs,           // # attrs in select clause
        const RelAttr selAttrs[],        // attrs in select clause
        int   nRelations,                // # relations in from clause
        const char * const relations[],  // relations in from clause
        int   nConditions,               // # conditions in where clause
        const Condition conditions[]);   // conditions in where clause

    RC Insert  (const char *relName,     // relation to insert into
        int   nValues,                   // # values
        const Value values[]);           // values to insert

    RC Delete  (const char *relName,     // relation to delete from
        int   nConditions,               // # conditions in where clause
        const Condition conditions[]);   // conditions in where clause

    RC Update  (const char *relName,     // relation to update
        const RelAttr &updAttr,          // attribute to update
        const int bIsValue,              // 1 if RHS is a value, 0 if attribute
        const RelAttr &rhsRelAttr,       // attr on RHS to set LHS equal to
        const Value &rhsValue,           // or value to set attr equal to
        int   nConditions,               // # conditions in where clause
        const Condition conditions[]);   // conditions in where clause

private:
    // Choose between filescan and indexscan for first operation - leaf level of
    // operator tree
    // to see if NLIJ is possible, join condition is passed down
    Iterator* GetLeafIterator(const char *relName,
                              int nConditions,
                              const Condition conditions[],
                              int nJoinConditions = 0,
                              const Condition jconditions[] = NULL,
                              int order = 0,
                              RelAttr* porderAttr = NULL);

    RC MakeRootIterator(Iterator*& newit,
                        int nSelAttrs, const RelAttr selAttrs[],
                        int nRelations, const char * const relations[]);

    RC MakeRootIterator(Iterator*& newit,
                        int nSelAttrs, const RelAttr selAttrs[],
                        int nRelations, const char * const relations[],
                        int order, RelAttr orderAttr,
                        bool group, RelAttr groupAttr);

    RC PrintIterator(Iterator* it) const;

    RM_Manager& rmm;
    IX_Manager& ixm;
    SM_Manager& smm;
};

//
// Print-error function
//
void QL_PrintError(RC rc);

#define QL_KEYNOTFOUND    (START_QL_WARN + 0)  // cannot find key
#define QL_INVALIDSIZE    (START_QL_WARN + 1)  // invalid number of attributes
#define QL_ENTRYEXISTS    (START_QL_WARN + 2)  // key,rid already
                                               // exists in index
#define QL_NOSUCHENTRY    (START_QL_WARN + 3)  // key,rid combination
                                               // does not exist in index
#define QL_MISSRELATION   (START_QL_WARN + 4)  // key,rid combination


#define QL_LASTWARN QL_MISSRELATION


#define QL_BADJOINKEY          (START_QL_ERR - 0)
#define QL_ALREADYOPEN         (START_QL_ERR - 1)
#define QL_BADATTR             (START_QL_ERR - 2)
#define QL_DUPREL              (START_QL_ERR - 3)
#define QL_RELMISSINGFROMFROM  (START_QL_ERR - 4)
#define QL_FNOTOPEN            (START_QL_ERR - 5)
#define QL_JOINKEYTYPEMISMATCH (START_QL_ERR - 6)
#define QL_BADOPEN             (START_QL_ERR - 7)
#define QL_EOF                 (START_QL_ERR - 8)

#define QL_LASTERROR QL_EOF
#endif
