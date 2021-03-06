#pragma once
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 2017 by Great Hill Corporation.
 * All Rights Reserved
 *
 * The LICENSE at the root of this repo details your rights (if any)
 *------------------------------------------------------------------------*/
#include "utillib.h"

class COptions : public COptionsBase {
public:
    SFString addr;
    SFString name;
    SFString source;
    bool all;
    bool count;
    bool matchCase;
    bool open;
    bool list;
    bool addrOnly;

    COptions(void);
    ~COptions(void) {}

    bool parseArguments(SFString& command);
    void Init(void);
};
