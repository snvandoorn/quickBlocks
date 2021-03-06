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
    CParams("-freshen",     "mode: freshen only -- default (do not display transactions from the cache)"),
    CParams("-showCache",   "mode: show transactions from the cache, and then freshen"),
    CParams("-cacheOnly",   "mode: display transactions from the cache only (do not freshen)"),
    CParams("-kBlock",      "start processing at block :k"),
    CParams("-logs",        "display smart contract events (logs)"),
    CParams("-trace",       "display smart contract internal traces"),
    CParams("-bloom",       "display bloom filter matching"),
    CParams("-parse",       "display parsed input data"),
    CParams("-accounting",  "display credits and debits per account and reconcile at each block"),
    CParams("-debug",       "enter debug mode (pause after each transaction)"),
    CParams("-single",      "if debugging is enable, single step through transactions"),
    CParams("-rebuild",     "clear cache and reprocess all transcations (may take a long time)"),
    CParams("",             "Index transactions for a given Ethereum address (or series of addresses).\r\n"),
};
uint32_t nParams = sizeof(params) / sizeof(CParams);

//---------------------------------------------------------------------------------------------------
bool COptions::parseArguments(SFString& command) {

    Init();
    while (!command.empty()) {
        SFString arg = nextTokenClear(command, ' ');
        if (arg == "-f" || arg == "--freshen") {
            // do nothing -- this is the default
            mode = "freshen|";  // last in wins

        } else if (arg.Contains("-k:") || arg.Contains("-kBlock:")) {

            arg = arg.Substitute("-k:","").Substitute("-kBlock:","");
            kBlock = toLongU(arg);
            if (!kBlock) {
                cerr << usageStr("You must specify a block number").Substitute("\n","\r\n");
                return false;
            }

        } else if (arg == "-c" || arg == "--cacheOnly") {
            mode = "showCache|";  // last in wins

        } else if (arg == "-s" || arg == "--showCache") {
            mode = "showCache|freshen|";  // last in wins

        } else if (arg == "-s" || arg == "--single" || arg == "--single_step") {
            single_on = true;

        } else if (arg == "-a" || arg == "--accounting") {
            accounting_on = true;

        } else if (arg == "-l" || arg == "--logs") {
            logs_on = true;

        } else if (arg == "-t" || arg == "--trace") {
            trace_on = true;

        } else if (arg == "-p" || arg == "--parse") {
            parse_on = true;

        } else if (arg == "-b" || arg == "--bloom") {
            bloom_on = true;

        } else if (arg == "-d" || arg == "--debug") {
            debugger_on = true;

        } else if (arg == "-r" || arg == "--rebuild") {
            cerr << cRed << "Warning: " << cOff
            << "Rebuilding the cache may take a very long time. Are you sure? Type 'y' or 'yes' to continue...\r\n"
            << "  > ";
            cerr.flush();
            char buffer[256];
            cin >> buffer;
            SFString res = buffer;

            if (res == "y" || res == "yes") {
                CFilename fn("./cache/");
                SFString path = fn.getFullPath();
                removeFolder(path);
                establishFolder(path);
                cerr << cYellow << "The cache was cleared. Quitting...\r\n" << cOff;
            } else {
                cerr << cYellow << "The cache was not removed. Quitting...\r\n" << cOff;
            }
            return false;

        } else if (arg.startsWith('-')) {
            if (!arg.Contains("-h") && !arg.Contains("-v") && !arg.Contains("-t")) {
                cerr << usageStr("Invalid option: " + arg).Substitute("\n","\r\n");
                return false;
            }
       }
    }

    if (mode.empty())
        mode = "freshen|";

    if (debugger_on && !accounting_on)
        return usage("If you want to use the debugger, you must use the --accounting option as well.");

    return true;
}

//---------------------------------------------------------------------------------------------------
void COptions::Init(void) {
    paramsPtr = params;
    nParamsRef = nParams;

    CTrace::registerClass();
    CTraceAction::registerClass();
    CTraceResult::registerClass();

    mode = "freshen|";
    single_on = false;
    accounting_on = false;
    debugger_on = false;
    logs_on = false;
    trace_on = false;
    parse_on = false;
    bloom_on = false;
    kBlock = 0;

    useVerbose = true;
    useTesting = false;
    minArgs = 0;
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
