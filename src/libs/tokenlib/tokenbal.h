#pragma once
/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 2017 by Great Hill Corporation.
 * All Rights Reserved
 *
 * The LICENSE at the root of this repo details your rights (if any)
 *------------------------------------------------------------------------*/
#include "etherlib.h"

//-------------------------------------------------------------------
namespace qblocks {

    extern SFUintBN getTokenBalance(const SFAddress& token, const SFAddress& holder, blknum_t blockNum);

}  // namespace qblocks
