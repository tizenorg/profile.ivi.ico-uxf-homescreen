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
#ifndef CICOBLOCKPARSER_H
#define CICOBLOCKPARSER_H
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "picojson.h"

/**
 * Part head string define
 */
#define D_PTF1      "\"stateMachine\""
#define D_PTS1      "\"state\""
#define D_PTS2      "\"finalState\""
#define D_PTS3      "\"historyState\""
#define D_PTS4      "\"event\""


/**
 * Perform a block classification defined string.
 * perform a json parse
 * @author m.kamoshida
 * @version 0.1
 */

/**
 * @brief The CicoBlockParser class
 */
class CicoBlockParser{
public:
    enum E_PARTSKIND{
        UNKNOWN = 0,                    // unknown
        STTMAC = 1,                     // state machine
        STT,                            // state
        HSTT,                           // history state
        FSTT,                           // final state
        EV                              // event
    };
                CicoBlockParser();
    virtual     ~CicoBlockParser();


    bool        getStateMachineParts(const char* tmpstr);
    const std::string& getError();
    int         spaceLess(char* d, const char* s) const;
    void*       getVoid() const;
    void        setVoid(void* v);
    bool        jsonParse(const char* objectname);
    bool        isJsonParse() const;
    void        getErrorHead(std::string& errhead, const char* strtop);
public:
    E_PARTSKIND m_kind;
    std::string m_parts;
    std::vector<CicoBlockParser*> m_childs;
    picojson::value m_v;

protected:
    const char* getBrackets(char* tmpstr, const char* tmpend);

private:
    static std::string m_err;
    void*       m_void;
    char*       m_jsonTmp;
};

/**
 * @brief get error message
 * @return error message
 */
inline const std::string& CicoBlockParser::getError()
{
    return m_err;
}

/**
 * @brief get void pointer
 * @return void pointer
 */
inline void* CicoBlockParser::getVoid() const
{
    return m_void;
}

/**
 * @brief set void pointer
 * @param v register void pointer
 */
inline void CicoBlockParser::setVoid(void* v)
{
    m_void = v;
}

/**
 * @brief json parse condhition
 * @return treu:parse / false:not parse
 */
inline bool CicoBlockParser::isJsonParse() const
{
    if ((char*)0 != m_jsonTmp) {
        return true;
    }
    return false;
}

#endif // CICOBLOCKPARSER_H
