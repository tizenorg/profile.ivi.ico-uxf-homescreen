/*
 * Copyright (c) 2013 TOYOTA MOTOR CORPORATION.
 *
 * Contact: 
 *
 * Licensed under the Apache License, Version 2.0 (the License){}
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. 
 */
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <ctype.h>
#include <string.h>

#include "CicoBlockParser.h"


/**
 * Perform a block classification defined string.
 * perform a json parse
 * @author m.kamoshida
 * @version 0.1
 */


/**
 * error message store
 */
std::string CicoBlockParser::m_err;

/**
 * @brief CicoBlockParser::CicoBlockParser
 * @      constructor
 * @      initial member variables
 */
CicoBlockParser::CicoBlockParser()
{
    m_void = (void*)0;
    m_jsonTmp = (char*)0;
}

/**
 * @brief CicoBlockParser::~CicoBlockParser
 * @      Destructor
 * @      release memory under the management
 */
CicoBlockParser::~CicoBlockParser()
{
    int sz = m_childs.size();
    for (int i = 0; i < sz; i++) {
        delete (m_childs[i]);
    }
    m_childs.clear();

    if ((char*)0 != m_jsonTmp) {
        delete[] m_jsonTmp;
        m_jsonTmp = (char*)0;
    }

}

/**
 * @brief CicoBlockParser::getStateMachineParts
 * @      get stateMachine block
 * @param str
 * @return true: get success/false:get fail
 */
bool CicoBlockParser::getStateMachineParts(const char* str)
{

    size_t tmp_sz = strlen(str);
    char  tmp_s[tmp_sz+1];
    tmp_s[0]='\0';
    size_t t2sz = spaceLess(tmp_s, str); // strip string set workspace

    char*  tmpstr = tmp_s;
    const char* tmpend = tmpstr+t2sz;

    const char* key0 = D_PTF1;          // "stateMachine"
    bool    dqF = false;                // flag double quotate
    char*   sttM = (char*)0;
    char*   ts = tmpstr;
    const char* errq = (char*)0;
    for (; *ts != '\0'; ts++) {
        if ((true == dqF) && ('\"' == *ts)) { // double quotate ?
            dqF = false;                // end string part
            errq = (char*)0;
            continue;                   // continue of for *ts != 0
        }

        if ((false == dqF) && ('\"' == *ts)) { // double quotate ?
            if (0 == strncmp(ts, key0, strlen(key0))) { // stateMachine ?
                sttM = ts;
                break;                  // break of for *ts != 0
            }
            else {
                dqF = true;             // start string part
                errq = ts;
                continue;               // continue of for *ts != 0
            }
        }
        if (tmpend == ts) {
            break;                      // break of for *ts != 0
        }
    }

    if ((char*)0 != sttM) {
        m_kind = STTMAC;
        if ((char*)0 != getBrackets(sttM, tmpend)) { 
            return true;
        }
    }
    else {
        std::string errhead;
        if (errq) {
            getErrorHead(errhead, errq);
        }
        else {
            getErrorHead(errhead, tmpstr);
        }
        std::ostringstream stream;
        stream << "define error(\"" << errhead << "\")";
        m_err = stream.str();
    }
    return false;
}

/**
 * @brief CicoBlockParser::getBrackets
 * @      get state/finalState/historyState/event block
 * @param tmpstr string top
 * @param tmpend string end
 * @return !=0:next string pointer ==0:get failure
 */
const char* CicoBlockParser::getBrackets(char* tmpstr, const char* tmpend)
{
    // block hea key word
    const char* key1 = D_PTS1;          // "\"state\""
    const char* key2 = D_PTS2;          // "\"finalState\""
    const char* key3 = D_PTS3;          // "\"historyState\""
    const char* key4 = D_PTS4;          // "\"event\""
    std::string errhead;
    getErrorHead(errhead, tmpstr);
    int bracketsCnt = 0;                // counter
    bool dqF = false;                   // flag double quotate
    char* ts = tmpstr;
    const char* te = tmpend;

    if ('\"' == *ts) {
        dqF = true;
    }
    ts++;

    while ('\0' != *ts) {               // Repeat until you find the stop code
        if ((true == dqF) && ('\"' == *ts)) { // Is it the end of the string ?
            dqF = false;                // end string part
            ts++;                       // next code
            continue;                   // continue of while
        }
        if ((false == dqF) && ('\"' == *ts)) { // Is it the start of the string ?
            const char* tr = (char*)0;
            if (0 == strncmp(ts, key1, strlen(key1))) { // Is it parts of state
                CicoBlockParser* smdp = new CicoBlockParser;
                m_childs.push_back(smdp);
                smdp->m_kind = STT;
                tr = smdp->getBrackets(ts, te);
            }
            else if (0 == strncmp(ts, key2, strlen(key2))) {
                CicoBlockParser* smdp = new CicoBlockParser;
                m_childs.push_back(smdp);
                smdp->m_kind = FSTT;
                tr = smdp->getBrackets(ts, te);
            }
            else if (0 == strncmp(ts, key3, strlen(key3))) {
                CicoBlockParser* smdp = new CicoBlockParser;
                m_childs.push_back(smdp);
                smdp->m_kind = HSTT;
                tr = smdp->getBrackets(ts, te);
            }
            else if (0 == strncmp(ts, key4, strlen(key4))) {
                CicoBlockParser* smdp = new CicoBlockParser;
                m_childs.push_back(smdp);
                smdp->m_kind = EV;
                tr = smdp->getBrackets(ts, te);
            }
            else {
                dqF = true;             // start of the string
                ts++;
                continue;               // continue of while
            }
            if ((char*)0 == tr) {
                return tr;
            }
            if (',' == *(ts-1)) {
                ts--;
            }
#if 0
            if ('\0'==*tr) {            // stop code
                ts = '\0';              // set stop code
            }
            else {
                strcpy(ts , tr);
            }
            te = ts + strlen(ts);
#else
            char* s = ts;
            for (const char* d = tr; d < te; s++, d++) {
                *s = *d;
            }
            *s = '\0';
            te = s;
#endif
        }

        if ((false == dqF) && ('{' == *ts)) {
            bracketsCnt++;
            ts++;
            continue;                   // continue of while
        }

        if ((false == dqF) && ('}' == *ts)) {
            bracketsCnt--;
            if (0 < bracketsCnt) {
                ts++;
                continue;                // continue of while
            }
            else if (0 == bracketsCnt) {
                size_t szt = ts - tmpstr;
                m_parts.assign(tmpstr, szt+1);
                ts++;
                return ts;
            }
            else {
                if (m_err.empty()) {
                    std::ostringstream stream;
                    stream<<"r-brackets mis.:"<<errhead;
                    m_err = stream.str();
                }
                return (char*)0;
            }
        }
        if (te == ts) {
            break;                      // break of while
        }
        ts++;
    }
    if (m_err.empty()) {
        std::ostringstream stream;
        stream<<"syntax:"<<errhead;
        m_err = stream.str();
    }
    return (char*)0;
}


/**
 * @brief CicoBlockParser::getErrorHead
 * @      error message parts edit
 * @param errhead output message
 * @param strtop message edit source
 */
void CicoBlockParser::getErrorHead(std::string& errhead, const char* strtop)
{
    int     edsz = 49;                  // get header string max size
    char    tmperr[edsz];               // temp. area
    const char* s = strtop; 
    char*   d = tmperr;
    int     dqCnt = 0;                  // double quotate counter
    for (int k = 0; k < (edsz-1); k++) {
        *d = *s++;                      // copy code;
        if ('\"' == *d) {               // double quotate ?
            dqCnt++;                    // count up
            if (6==dqCnt) {
                d++;                    // set position stop code
                break;                  // break of for k
            }
        }
        d++;                            // next store position
        if ('\0' == *s) {               // stop code
            break;                      // break of for k
        }
    }
    *d='\0';                            // stop code
    errhead.assign(tmperr);
}

/**
 * string less space code
 * @brief CicoBlockParser::spaceLess
 * @param s source
 * @param d store area
 * @return store string size
 */
int CicoBlockParser::spaceLess(char* d, const char* s) const
{
    size_t  dsz = 0;                    // return value
    char*   dd = d;                     // work pointer set destination
    bool    dqF = false;                // flag double quotate
    bool    cmtF = false;               // comment flag
    const char* ss = s;                 // work pointer source set
    for (; *ss != '\0'; ss++) {         // loop string size
        // "(double quotate) block check
        if ((false == cmtF) && (false == dqF) && ('\"' == *ss)) {
            dqF = true;                 // start string part
        }
        else if ((false == cmtF) && (true == dqF) && ('\"' == *ss)) {
            dqF = false;                // end string part
        }
        // comment(/* .. */) block check
        else if ((false == cmtF) && (false == dqF) &&
                 ('/' == *ss) && ('*' == *(ss+1))) {
            cmtF = true;                // start comment part
        }
        else if ((ss != s) && (true == cmtF) && (false == dqF) &&
                 ('/' == *ss) && ('*' == *(ss-1))) {
            cmtF = false;               // end comment part
            continue;
        }
        bool storeF = false;            // store flag off
        if (true == dqF) {              // string part ?
            storeF = true;              // store flag on
        }
        else if ((false == cmtF) && (0 == isspace(*ss))) { // is not space code
            storeF = true;              // store flag on
        }
        if (true == storeF) {           // is flag on ?
            *dd = *ss;                  // code copy
            dd++;                       // pointer up
            *dd='\0';                   // Stop code insert
            dsz++;                      // store string size count
        }
    }
    return dsz;
}

/**
 * @brief CicoBlockParser::jsonParse
 * @param objectname
 * @return true:parse success false:parse fail
 */
bool CicoBlockParser::jsonParse(const char* objectname)
{
    std::string tmpstr("{");
    tmpstr += m_parts;
    tmpstr += std::string("}");
    size_t sz = tmpstr.size();
    m_jsonTmp = new char[sz+1];
    strcpy(m_jsonTmp, tmpstr.c_str());
    char* start = m_jsonTmp;
    char* end = m_jsonTmp + sz;
    m_err = picojson::parse(m_v, start, end);
    if (!m_err.empty()) {
        if ((char*)0 != m_jsonTmp) {
            delete[] m_jsonTmp;
            m_jsonTmp = (char*)0;
        }
        return false;
    }
    if (false == m_v.get(objectname).is<picojson::object>()) {
        m_err = std::string("get error(");
        m_err += std::string(objectname);
        m_err += std::string(")");
        if ((char*)0 != m_jsonTmp) {
            delete[] m_jsonTmp;
            m_jsonTmp = (char*)0;
        }
        return false;
    }
    return true;
}
