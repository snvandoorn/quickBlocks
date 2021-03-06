#pragma once
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 2017 by Great Hill Corporation.
 * All Rights Reserved
 *
 * The LICENSE at the root of this repo details your rights (if any)
 *------------------------------------------------------------------------*/
/*
 * This file was generated with makeClass. Edit only those parts of the code inside
 * of 'EXISTING_CODE' tags.
 */
#include "transaction.h"

//--------------------------------------------------------------------------
class QResetSpentToday;
typedef SFArrayBase<QResetSpentToday>         QResetSpentTodayArray;
typedef SFList<QResetSpentToday*>             QResetSpentTodayList;
typedef SFUniqueList<QResetSpentToday*>       QResetSpentTodayListU;

// EXISTING_CODE
// EXISTING_CODE

//--------------------------------------------------------------------------
class QResetSpentToday : public CTransaction {
public:

public:
    QResetSpentToday(void);
    QResetSpentToday(const QResetSpentToday& re);
    ~QResetSpentToday(void);
    QResetSpentToday& operator=(const QResetSpentToday& re);

    DECLARE_NODE(QResetSpentToday);

    // EXISTING_CODE
    // EXISTING_CODE

protected:
    void Clear(void);
    void Init(void);
    void Copy(const QResetSpentToday& re);
    bool readBackLevel(SFArchive& archive);

    // EXISTING_CODE
    // EXISTING_CODE
};

//--------------------------------------------------------------------------
inline QResetSpentToday::QResetSpentToday(void) {
    Init();
    // EXISTING_CODE
    // EXISTING_CODE
}

//--------------------------------------------------------------------------
inline QResetSpentToday::QResetSpentToday(const QResetSpentToday& re) {
    // EXISTING_CODE
    // EXISTING_CODE
    Copy(re);
}

// EXISTING_CODE
// EXISTING_CODE

//--------------------------------------------------------------------------
inline QResetSpentToday::~QResetSpentToday(void) {
    Clear();
    // EXISTING_CODE
    // EXISTING_CODE
}

//--------------------------------------------------------------------------
inline void QResetSpentToday::Clear(void) {
    // EXISTING_CODE
    // EXISTING_CODE
}

//--------------------------------------------------------------------------
inline void QResetSpentToday::Init(void) {
    CTransaction::Init();

    // EXISTING_CODE
    // EXISTING_CODE
}

//--------------------------------------------------------------------------
inline void QResetSpentToday::Copy(const QResetSpentToday& re) {
    Clear();
    CTransaction::Copy(re);

    // EXISTING_CODE
    // EXISTING_CODE
    finishParse();
}

//--------------------------------------------------------------------------
inline QResetSpentToday& QResetSpentToday::operator=(const QResetSpentToday& re) {
    Copy(re);
    // EXISTING_CODE
    // EXISTING_CODE
    return *this;
}

//---------------------------------------------------------------------------
inline SFString QResetSpentToday::getValueByName(const SFString& fieldName) const {
    // EXISTING_CODE
    // EXISTING_CODE
    return Format("[{"+toUpper(fieldName)+"}]");
}

//---------------------------------------------------------------------------
IMPLEMENT_ARCHIVE_ARRAY(QResetSpentTodayArray);
IMPLEMENT_ARCHIVE_ARRAY_C(QResetSpentTodayArray);
IMPLEMENT_ARCHIVE_LIST(QResetSpentTodayList);

//---------------------------------------------------------------------------
// EXISTING_CODE
// EXISTING_CODE

