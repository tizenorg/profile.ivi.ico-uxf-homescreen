/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

//==========================================================================
/**
 *  @file   CicoSCMessageRes.h
 *
 *  @brief  This file is definition of CicoSCMessage class
 */
//==========================================================================
#ifndef __CICO_SC_MESSAGE_RES_H__
#define __CICO_SC_MESSAGE_RES_H__

#include <string>

#include <json-glib/json-glib.h>
#include "CicoSCMessage.h"

//==========================================================================
//  forward declaration
//==========================================================================

//==========================================================================
/**
 *  @brief  This class hold communication message
 */
//==========================================================================
class CicoSCMessageRes :public CicoSCMessage
{
public:
    // default constructor
    CicoSCMessageRes();

    // destructor
    ~CicoSCMessageRes();

    // add argument object for char pointer type
    void addWinObject(const char * key, const char * value);

    // add argument object for string type
    void addWinObject(const char * key, const std::string & value);

    // add argument object for integer type
    void addWinObject(const char * key, int value);

    // add argument object for char pointer type
    void addSndObject(const char * key, const char * value);

    // add argument object for string type
    void addSndObject(const char * key, const std::string & value);

    // add argument object for integer type
    void addSndObject(const char * key, int value);

    // add argument object for char pointer type
    void addInpObject(const char * key, const char * value);

    // add argument object for string type
    void addInpObject(const char * key, const std::string & value);

    // add argument object for integer type
    void addInpObject(const char * key, int value);

    // get message data
    virtual const char* getData(void);

private:

private:
    JsonObject          *m_winObj;      //!< json window objects
    JsonObject          *m_sndObj;      //!< json sound objects
    JsonObject          *m_inpObj;      //!< json input objects
    int                 m_winCnt;
    int                 m_sndCnt;
    int                 m_inpCnt;
};
#endif  // __CICO_SC_MESSAGE_RES_H__
// vim:set expandtab ts=4 sw=4:
