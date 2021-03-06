/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 2017 by Great Hill Corporation.
 * All Rights Reserved
 *
 * The LICENSE at the root of this repo details your rights (if any)
 *------------------------------------------------------------------------*/
#include "options.h"

//---------------------------------------------------------------------------------------------------
CParams params[] = {
    CParams("~hash",  "transaction hash of the requested transaction"),
    CParams("-te(r)se", "display the traces in a terse fashion"),
    CParams("",       "Retrieve the full trace of a transaction.\n"),
};
uint32_t nParams = sizeof(params) / sizeof(CParams);

//---------------------------------------------------------------------------------------------------
bool COptions::parseArguments(SFString& command) {

    Init();
    while (!command.empty()) {
        SFString arg = nextTokenClear(command, ' ');
        if (arg == "-o" || arg == "--option1") {
            //option1 = true;

        } else if (arg == "-r" || arg == "--terse") {
            terse = true;

        } else if (arg.startsWith('-')) {
            if (!arg.Contains("-h") && !arg.Contains("-v") && !arg.Contains("-t"))
                return usage("Invalid option: " + arg);
        } else {
            hash = arg;
        }
    }

    if (hash.empty())
        return usage("You must supply at least one transaction hash.");

    return true;
}

//---------------------------------------------------------------------------------------------------
void COptions::Init(void) {
    paramsPtr = params;
    nParamsRef = nParams;

    // hash = "";
    terse = false;

    useVerbose = true;
    useTesting = false;
//    minArgs = 0;
}

//---------------------------------------------------------------------------------------------------
COptions::COptions(void) {
    Init();
    // header = "";
    // footer = "";
    // seeAlso = "";
}

//--------------------------------------------------------------------------------
COptions::~COptions(void) {
}
