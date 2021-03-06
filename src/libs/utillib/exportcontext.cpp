/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 2017 by Great Hill Corporation.
 * All Rights Reserved
 *
 * The LICENSE at the root of this repo details your rights (if any)
 *------------------------------------------------------------------------*/
#include "basetypes.h"

#include "exportcontext.h"
#include "dates.h"

namespace qblocks {

    //-------------------------------------------------------------
    CExportContext::CExportContext(void) : tCh('\t') {
        nTabs = 0;
    }

    //-------------------------------------------------------------
    CExportContext& CExportContext::operator<<(bool b) {
#if 1
        ostringstream os;
        os << b;
        Output(os.str().c_str());
#else
        char val[64];
        sprintf(val, "%c", b ? '1' : '0');
        Output(val);
#endif
        return *this;
    }

    //-------------------------------------------------------------
    CExportContext& CExportContext::operator<<(char c) {
#if 1
        ostringstream os;
        os << c;
        Output(os.str().c_str());
#else
        char val[64];
        sprintf(val, "%c", c);
        Output(val);
#endif
        return *this;
    }

    //-------------------------------------------------------------
    CExportContext& CExportContext::operator<<(int64_t dw) {
        ostringstream os;
        os << dw;
        Output(os.str().c_str());
        return *this;
    }

    //-------------------------------------------------------------
    CExportContext& CExportContext::operator<<(unsigned int ui) {
#if 1
        ostringstream os;
        os << ui;
        Output(os.str().c_str());
#else
        char val[64];
        sprintf(val, (fmt.empty() ? "%d" : (const char*)fmt), ui);
        Output(val);
#endif
        return *this;
    }

    //-------------------------------------------------------------
    CExportContext& CExportContext::operator<<(uint64_t ul) {
        ostringstream os;
        os << ul;
        Output(os.str().c_str());
        return *this;
    }

    //-------------------------------------------------------------
    CExportContext& CExportContext::operator<<(int i) {
#if 1
        ostringstream os;
        os << i;
        Output(os.str().c_str());
#else
        char val[64];
        sprintf(val, (fmt.empty() ? "%d" : (const char*)fmt), i);
        Output(val);
#endif
        return *this;
    }

    //-------------------------------------------------------------
    CExportContext& CExportContext::operator<<(float f) {
        SFString val = formatFloat(f);
        Output(val);
        return *this;
    }

    //-------------------------------------------------------------
    CExportContext& CExportContext::operator<<(double f) {
        SFString val = formatFloat(f);
        Output(val);
        return *this;
    }

    //-------------------------------------------------------------
    CExportContext& CExportContext::operator<<(const char *str) {
        Output(str);
        return *this;
    }

    //-------------------------------------------------------------
    CExportContext& CExportContext::operator<<(const SFString& str) {
        if (str.length() >= 3 && str.Left(2) == "`%" && str.at(2) != '%')
            fmt = str.substr(1);
        else if (str == "%")
            fmt = EMPTY;
        else
            Output(str);
        return *this;
    }

    //-------------------------------------------------------------
    CExportContext& CExportContext::operator<<(const SFTime& tm) {
        Output(tm.Format(FMT_JSON));
        return *this;
    }

    //-------------------------------------------------------------
    void CFileExportContext::setOutput(void *output) {
        Close();  // just in case
        m_output = output == NULL ? stdout : reinterpret_cast<FILE*>(output);
    }

    //-------------------------------------------------------------
    void CFileExportContext::Output(const SFString& sss) {
        ASSERT(m_output);
        fprintf(m_output, "%s", (const char *)sss);
    }

    //-------------------------------------------------------------
    void CStringExportContext::setOutput(void *output) {
        // should not happen
        ASSERT(0);
    }

    void CStringExportContext::Output(const SFString& sss) {
        str += sss;
    }
}  // namespace qblocks
