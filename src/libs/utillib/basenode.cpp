/*-------------------------------------------------------------------------
 * This source code is confidential proprietary information which is
 * Copyright (c) 2017 by Great Hill Corporation.
 * All Rights Reserved
 *
 * The LICENSE at the root of this repo details your rights (if any)
 *------------------------------------------------------------------------*/
#include "basetypes.h"
#include "biglib.h"
#include "basenode.h"
#include "sfarchive.h"
#include "exportcontext.h"

namespace qblocks {

    //--------------------------------------------------------------------------------
    CRuntimeClass CBaseNode::classCBaseNode;
    static CBuiltIn _biBaseNode(&CBaseNode::classCBaseNode, "CBaseNode", sizeof(CBaseNode), NULL, NULL, NO_SCHEMA);

    //--------------------------------------------------------------------------------
    CBaseNode::CBaseNode(void) {
        Init();
    }

    //--------------------------------------------------------------------------------
    CBaseNode::~CBaseNode(void) {
    }

    //--------------------------------------------------------------------------------
    void CBaseNode::Init(void) {
        m_deleted  = false;
        m_schema = NO_SCHEMA;
        m_showing = true;
        pParent = NULL;
    }

    //--------------------------------------------------------------------------------
    void CBaseNode::Copy(const CBaseNode& bn) {
        m_deleted  = bn.m_deleted;
        m_schema = bn.m_schema;
        m_showing = bn.m_showing;
        pParent = NULL;
    }

    //--------------------------------------------------------------------------------
    bool CBaseNode::isDeleted(void) const {
        return (m_deleted);
    }

    //--------------------------------------------------------------------------------
    void CBaseNode::setDeleted(bool del) {
        m_deleted = del;
    }

    //--------------------------------------------------------------------------------
    bool CBaseNode::isKindOf(const CRuntimeClass* pClass) const {
        CRuntimeClass* pClassThis = getRuntimeClass();
        return pClassThis->IsDerivedFrom(pClass);
    }

    //--------------------------------------------------------------------------------
    CRuntimeClass* CBaseNode::getRuntimeClass() const {
        return &CBaseNode::classCBaseNode;
    }

    //--------------------------------------------------------------------------------
    char *CBaseNode::parseCSV(char *s, uint32_t& nFields, const SFString *fields) {
        nFields = 0;

        typedef enum { OUTSIDE = 0, INSIDE } parseState;
        parseState state = OUTSIDE;

        char *fieldVal = NULL;
        while (*s) {
            switch (state) {
                case OUTSIDE:
                    if (*s == '\"') {
                        state = INSIDE;
                        fieldVal = s+1;

                    } else if (*s == '\n') {
                        finishParse();
                        return (s+1);
                    }
                    s++;
                    break;

                case INSIDE:
                    if (*s == '\"') {
                        *s = '\0';
                        if (!this->setValueByName(fields[nFields++], fieldVal)) {
//                          fprintf(stderr, "Bad field name %s. Quitting...", (const char*)fields[nFields-1]);
//                          return NULL;
                        }
                        fieldVal = NULL;
                        state = OUTSIDE;

                    }
                    s++;
                    break;
            }
        }
        finishParse();
        return NULL;
    }

    //--------------------------------------------------------------------------------
    char *CBaseNode::parseText(char *s, uint32_t& nFields, const SFString *fields) {
        nFields = 0;
        char *fieldVal = s;
        while (s && *s) {
            switch (*s) {
                case '\r':
                    break;
                case '\t':
                    *s = '\0';
                    this->setValueByName(fields[nFields++], fieldVal);
                    fieldVal = s+1;
                    break;
                case '\n':
                    *s = '\0';
                    this->setValueByName(fields[nFields++], fieldVal);
                    finishParse();
                    return s+1;
            }
            s++;
        }
        this->setValueByName(fields[nFields++], fieldVal);
        finishParse();
        return NULL;
    }

    //--------------------------------------------------------------------------------
    char *CBaseNode::parseJson(char *s) {
        uint32_t nFields = 0;
        return parseJson(s, nFields);
    }

    // #define DEBUG_PARSER
#ifdef DEBUG_PARSER
    SFString tbs;
#endif

    //--------------------------------------------------------------------------------
    char *CBaseNode::parseJson(char *s, uint32_t& nFields) {
#ifdef DEBUG_PARSER
        tbs+="\t";
        printf("--------------------------\n%s\n-----------------------------\n", s);
        fflush(stdout);
        printf("--------------------------\n%s\n-----------------------------\n",
               (const char*)SFString(s).substr(SFString(s).find("{"), 300));
        printf("--------------------------\n%s\n-----------------------------\n",
               (const char*)SFString(s).Right(300));
#endif
        typedef enum { OUTSIDE = 0, IN_NAME, IN_VALUE } parseState;
        parseState state = OUTSIDE;

        char *fieldName = NULL;
        char *fieldVal = NULL;
        while (s && *s) {
            switch (state) {
                case OUTSIDE:
                    if (*s == '{')
                        state = IN_NAME;
                    s++;
                    break;

                case IN_NAME:
                    if (!fieldName) {
                        fieldName = s;

                    } else if (*s == ':') {
                        state = IN_VALUE;
                        *s = '\0';
#ifdef DEBUG_PARSER
//                      printf("fn: %-10.10s fv: %-40.40s ---> %-60.60s\n" , fieldName, fieldVal, (s+1));
#endif
                    }
                    s++;
                    break;

                case IN_VALUE:
                    fieldVal = s;
                    if (*s == '[') {  // array skip to end of array
                        fieldVal++;
                        s++;
                        int lev = 1;
                        bool done = false;
                        while (s && *s && !done) {
                            if (*s == '[') {
                                lev++;

                            } else if (lev == 1 && *s == ']') {
                                done = true;
                                if (*s == ']')
                                    lev--;
                                s--;  // remove the closing bracket

                            } else if (*s == ']') {
                                lev--;
                            }
                            s++;
                        }
                    } else {
                        int lev = 1;
                        bool done = false;
                        while (s && *s && !done) {
                            if (*s == '{') {
                                lev++;

                            } else if (lev == 1 && (*s == ',' || *s == '}')) {
                                done = true;
                                if (*s == '}')
                                    lev--;
                                s--;

                            } else if (*s == '}') {
                                lev--;
                            }
                            s++;
                        }
                    }
                    *s = '\0';
#ifdef DEBUG_PARSER
                    printf("%sfn: %-20.20s fv: %-60.60s ---> %-40.40s\n",
                           (const char*)tbs, fieldName, fieldVal, (s+1));
                    fflush(stdout);
#endif
                    nFields += this->setValueByName(fieldName, fieldVal);
                    fieldName = NULL;
                    fieldVal = NULL;
                    state = IN_NAME;
                    s++;
                    if (*s && *s == ',')
                        s++;
                    if (s && *s && (*s == '{' || *s == '}' || *s == ']')) {
                        finishParse();
#ifdef DEBUG_PARSER
                        tbs.Replace("\t", "");
#endif
                        return s;
                    }
                    break;
            }
        }
#ifdef DEBUG_PARSER
        tbs.Replace("\t", "");
#endif
        finishParse();
        return NULL;
    }

    //--------------------------------------------------------------------------------
    char *cleanUpJson(char *s) {
        if (!s)
            return s;
        char *l = s, *start = s;
        while (*s) {
            if (!isWhiteSpace(*s) && *s != '\"') {  // zap all the white space and quotes
                *l = *s;
                l++;
            }
            s++;
        }
        *l = '\0';
        return start;
    }

    //---------------------------------------------------------------------------
    bool CBaseNode::preSerialize(SFArchive& archive) {
        if (!archive.isReading())
            return ((const CBaseNode*)this)->preSerializeC(archive);

        archive.pParent = this;  // sets this value for items stored in lists or arrays -- read only
        if (archive.isReading()) {
            archive >> m_deleted;
            archive >> m_schema;
            archive >> m_showing;
            SFString str;
            archive >> str;
            ASSERT(str == SFString(getRuntimeClass()->getClassNamePtr()));
        }
        return true;
    }

    //---------------------------------------------------------------------------
    bool CBaseNode::preSerializeC(SFArchive& archive) const {
        archive.pParent = this;  // sets this value for items stored in lists or arrays -- read only
        if (m_deleted && !archive.writeDeleted())
            return false;
        archive << m_deleted;
        archive << m_schema;
        archive << m_showing;
        archive << getRuntimeClass()->getClassNamePtr();
        return true;
    }

    //---------------------------------------------------------------------------
    static CExportOptions expC;
    CExportOptions& expContext(void) {
        return expC;
    }

    //---------------------------------------------------------------------------
    void incIndent(void) {
        expC.lev++;
    }

    //---------------------------------------------------------------------------
    void decIndent(void) {
        expC.lev--;
    }

    //---------------------------------------------------------------------------
    SFString indent(void) {
        return SFString(expC.tab, expC.spcs*expC.lev);
    }

    extern SFString decBigNum(const SFString& str);
    //--------------------------------------------------------------------------------
    SFString CBaseNode::toJson1(void) const {
        return toJson().Substitute("\t", " ").Substitute("\n", "").Substitute("  ", " ");
    }
    //--------------------------------------------------------------------------------
    SFString CBaseNode::toJson(void) const {
        const CFieldList *fieldList = getRuntimeClass()->GetFieldList();
        if (!fieldList) {
            cerr << "No fieldList in " << getRuntimeClass()->m_ClassName
                << ". Did you register the class? Quitting...\n";
            cerr.flush();
            exit(0);
        }

        // TODO(tjayrush): THIS PER DISPLAY LOOKUP IS SLOW - THIS SHOULD ONLY BE DONE ONCE
        // If a class is not a direct decendent of CBaseNode we want to include that class's fields in here as well
        CFieldList theList = *fieldList;

        CRuntimeClass *pThis = getRuntimeClass();
        CRuntimeClass *pPar  = pThis->m_BaseClass;
        CRuntimeClass *pBase = GETRUNTIME_CLASS(CBaseNode);
        if (pPar != pBase) {
            const CFieldList *fieldList = pPar->GetFieldList();
            if (fieldList) {
                LISTPOS lPos = fieldList->SFList<CFieldData *>::GetHeadPosition();
                while (lPos) {
                    CFieldData *fld = fieldList->GetNext(lPos);
                    if (!fld->isHidden())
                        theList.AddTail(fld);
                }
            }
        }
        return toJson(&theList);
    }

    //--------------------------------------------------------------------------------
    SFString CBaseNode::toJson(const SFString& fieldsIn) const {
        const CFieldList *fieldList = getRuntimeClass()->GetFieldList();
        if (!fieldList) {
            cerr << "No fieldList in " << getRuntimeClass()->m_ClassName
                << ". Did you register the class? Quitting...\n";
            cerr.flush();
            exit(0);
        }

        // TODO(tjayrush): THIS PER DISPLAY LOOKUP IS SLOW - SAVE THIS STRING FIELD LIST AND ONLY
        // LOAD IF DIFFERENT USE STATIC
        CFieldList theList;
        SFString fields = fieldsIn;
        while (!fields.empty()) {
            SFString field = nextTokenClear(fields, '|');
            const CFieldData *fld = fieldList->getFieldByName(field);
            if (!fld) {
                cerr << "Could not find field " << field << " in class "
                    << getRuntimeClass()->m_ClassName << ". Quitting...\n";
                cerr.flush();
                exit(0);
            }
            theList.AddTail((CFieldData*)(fld));  // NOLINT
        }
        return toJson(&theList);
    }

#define toHex2(a)  ("0x"+toLower(SFString(to_hex(str2BigUint(a)).c_str())))
    //--------------------------------------------------------------------------------
    SFString CBaseNode::toJsonFldList(const CFieldList *fieldList) const {
        SFString ret;
        bool first = true;
        if (!expContext().noFrst)
            ret += indent();
        expContext().noFrst = false;
        LISTPOS lPos = fieldList->SFList<CFieldData *>::GetHeadPosition();
        while (lPos) {
            incIndent();
            CFieldData *fld = fieldList->GetNext(lPos);
            SFString val = getValueByName(fld->m_fieldName);
            if (!fld->isHidden() && (!val.empty() || fld->isArray())) {
                if (!first) {
                    if (expContext().colored)
                        ret += "#";
                    ret += ",\n";
                } else {
                    ret += "\n";
                }
                first = false;
                ret += indent();
                ret += "\"" + fld->m_fieldName + "\"";
                ret += ": ";
                if (expContext().colored)
                    ret += "%";
                if (fld->isArray()) {
                    ret += "[";
                    incIndent();
                    val = getValueByName(fld->m_fieldName);
                    decIndent();
                    ret += (val.Contains("\n") ? "\n" + val + indent() : val);
                    ret += "]";

                } else if (fld->isObject()) {
                    ret += val;

                } else if (fld->m_fieldType == T_BLOOM) {
                    ret += "\"" + val + "\"";


                } else if (fld->m_fieldType == T_NUMBER) {
                    if (expContext().quoteNums) ret += "\"";
                    ret += (expContext().hexNums) ? toHex2(val) : decBigNum(val);
                    if (expContext().quoteNums) ret += "\"";

                } else if (val == "null") {
                    ret += val;

                } else {
                    ret += "\"" + val + "\"";
                }
            }
            decIndent();
        }

        return ret;
    }

    //--------------------------------------------------------------------------------
    SFString CBaseNode::toJson(const CFieldList *fieldList) const {
        ASSERT(fieldList);
        SFString ret;
        ret += "{";
        ret += toJsonFldList(fieldList);
        ret += "\n";
        ret += indent();
        ret += "}";
        return ret;
    }

    //--------------------------------------------------------------------------------
    SFString decBigNum(const SFString& str) {
        SFString ret = str;
        size_t len = ret.length();
        if (len > 29) ret = ret.Left(1) + "." + StripTrailing(ret.substr(1), '0') + "e+29";
        else if (len >28) ret = ret.Left(1) + "." + StripTrailing(ret.substr(1), '0') + "e+28";
        else if (len > 27) ret = ret.Left(1) + "." + StripTrailing(ret.substr(1), '0') + "e+27";
        else if (len > 26) ret = ret.Left(1) + "." + StripTrailing(ret.substr(1), '0') + "e+26";
        else if (len > 25) ret = ret.Left(1) + "." + StripTrailing(ret.substr(1), '0') + "e+25";
        else if (len > 24) ret = ret.Left(1) + "." + StripTrailing(ret.substr(1), '0') + "e+24";
        else if (len > 23) ret = ret.Left(1) + "." + StripTrailing(ret.substr(1), '0') + "e+23";
        else if (len > 22) ret = ret.Left(1) + "." + StripTrailing(ret.substr(1), '0') + "e+22";
        else if (len > 21) ret = ret.Left(1) + "." + StripTrailing(ret.substr(1), '0') + "e+21";
        ret.Replace(".e+", "e+");
        return ret;
    }

    //--------------------------------------------------------------------------------
    SFString nextBasenodeChunk(const SFString& fieldIn, bool force, const CBaseNode *node) {
        if (node) {
            SFString className = node->getRuntimeClass()->getClassNamePtr();
            switch (tolower(fieldIn[0])) {
                case 'd':
                    if ( fieldIn % "deleted" ) return asString(node->isDeleted());
                    break;
                case 'n':
                    if ( fieldIn % "null" ) return "<x>";
                    break;
                case 'p':
                    if ( fieldIn % "parsed" ) {
                        const CFieldList *fieldList = node->getRuntimeClass()->GetFieldList();
                        if (!fieldList) {
                            cerr << "No fieldList in " << node->getRuntimeClass()->m_ClassName
                                << ". Did you register the class? Quitting...\n";
                            cerr.flush();
                            exit(0);
                        }
                        SFString ret;
                        ret += "{";
                        ret += node->toJsonFldList(fieldList);
                        ret += "}";
                        return ret.Substitute("\n", "");;
                    }
                    break;
                case 's':
                    if ( fieldIn % "schema" ) return asString(node->m_schema);
                    if ( fieldIn % "showing" ) return asString(node->m_showing);
                    break;
                default:
                    break;
            }
        }

        return EMPTY;
    }

    //---------------------------------------------------------------------------------------------
    SFString getNextChunk(SFString& fmtOut, NEXTCHUNKFUNC func, const void *data) {
        SFString chunk = fmtOut;
        if (!fmtOut.Contains("[")) {
            // There are no more tokens.  Return the last chunk and empty out the format
            fmtOut = EMPTY;
            return chunk;
        }

        if (!fmtOut.startsWith('[')) {
            // We've encountered plain text outside of a token. There is more to process so grab
            // the next chunk and then prepare the remaining chunk by prepending the token.
            // The next time through we will hit the token.
            chunk  = nextTokenClear(fmtOut, '[', false);
            fmtOut = "[" + fmtOut;
            return chunk;
        }

        // We've hit a token, toss the start token, look for a field and toss the last token
        // leaving the remainder of the format in fmtOut.  A field is found if we find a pair
        // of squigglies).  Save text inside the start token and outside the field in pre and post
        ASSERT(fmtOut.startsWith('['));

        SFString pre, fieldName, post;
        nextTokenClear(fmtOut, '[', false);  // toss the start token
        if (chunk.Contains("{")) {
            // we've encountered a field
            pre       = nextTokenClear(fmtOut, '{', false);
            fieldName = nextTokenClear(fmtOut, '}', false);
            post      = nextTokenClear(fmtOut, ']', false);
        } else {
            // we've encountered a token with no field inside of it.  Just pull off
            // the entire contents into post.  It will be returned just below.
            post = nextTokenClear(fmtOut, ']', false);
        }

        // Either no squigglies were found or an empty pair of squigglies were found.  In either
        // case return surrounding text (text inside the token and outside the squiggles)
        if (fieldName.empty())
            return pre + post;

        // We have a field so lets process it.
        ASSERT(fieldName.length() >= 2);
        bool isPrompt = false;
        uint32_t maxWidth = 0xdeadbeef;

        // The fieldname may contain p: or w:width: or both.  If it contains either it
        // must contain them at the beginning of the string (before the fieldName).  Anything
        // left after the last ':' is considered the fieldName
        SFString promptName = fieldName;
        if (fieldName.ContainsI("p:")) {
            isPrompt = true;
            fieldName.ReplaceI("p:", EMPTY);
            promptName = fieldName;
        }

        if (fieldName.ContainsI("w:")) {
            ASSERT(fieldName.Left(2) % "w:");  // must be first modifier in the string
            fieldName.ReplaceI("w:", EMPTY);   // get rid of the 'w:'
            maxWidth = toLong32u(fieldName);   // grab the width
            nextTokenClear(fieldName, ':');    // skip to the start of the fieldname
        }

        //--------------------------------------------------------------------
#define truncPad(str, size) (size == 0xdeadbeef ? str : padRight(str.Left(size), size))

        // Get the value of the field.  If the value of the field is empty we return empty for the entire token.
        bool forceShow = false;
        SFString fieldValue = (func)(fieldName, forceShow, data);
        forceShow = (isPrompt?true:forceShow);
        if (!forceShow && fieldValue.empty())
            return EMPTY;
        fieldValue = truncPad(fieldValue, maxWidth);  // pad or truncate

        // The field is not hidden, the value of the field is not empty, we are not working
        // on a prompt, so we toss back the token referencing the value of the field.
        if (!isPrompt)
            return pre + fieldValue + post;

        // We are working on a prompt.  Pick up customizations if any
        SFString prompt = promptName;
        prompt = truncPad(prompt, maxWidth);  // pad or truncate
        return pre + prompt + post;
    }

    //--------------------------------------------------------------------------------
    SFString fldNotFound(const SFString& str) {
        return "Field not found: " + str + "\n";
    }
}  // namespace qblocks
