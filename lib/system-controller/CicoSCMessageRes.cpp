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
 *  @file   CicoSCMessageRes.cpp
 *
 *  @brief  This file implementation of CicoSCMessage class
 */
//==========================================================================

#include <string>
#include <limits.h>

#include "CicoSCMessage.h"
#include "CicoSCMessageRes.h"
#include "CicoLog.h"

//==========================================================================    
//  private static variable
//==========================================================================    

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSCMessageRes::CicoSCMessageRes()
{
    m_winObj = json_object_new();
    m_sndObj = json_object_new();
    m_inpObj = json_object_new();
    m_winCnt = 0;
    m_sndCnt = 0;
    m_inpCnt = 0;
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSCMessageRes::~CicoSCMessageRes()
{
    //ICO_TRA("Enter");
    json_object_unref(m_winObj);
    json_object_unref(m_sndObj);
    json_object_unref(m_inpObj);
    //ICO_TRA("Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  add argument object for char pointer type
 *
 *  @param [in] key     key name of object
 *  @param [in] value   value of object for char pointer type
 */
//--------------------------------------------------------------------------
void
CicoSCMessageRes::addWinObject(const char * key, const char * value)
{
    json_object_set_string_member(m_winObj, key, value);
    m_winCnt++;
}

void
CicoSCMessageRes::addSndObject(const char * key, const char * value)
{
    json_object_set_string_member(m_sndObj, key, value);
    m_sndCnt++;
}

void
CicoSCMessageRes::addInpObject(const char * key, const char * value)
{
    json_object_set_string_member(m_inpObj, key, value);
    m_inpCnt++;
}

void
CicoSCMessageRes::addWinObject(const char * key, const std::string & value)
{
    json_object_set_string_member(m_winObj, key, value.c_str());
    m_winCnt++;
}

void
CicoSCMessageRes::addSndObject(const char * key, const std::string & value)
{
    json_object_set_string_member(m_sndObj, key, value.c_str());
    m_sndCnt++;
}

void
CicoSCMessageRes::addInpObject(const char * key, const std::string & value)
{
    json_object_set_string_member(m_inpObj, key, value.c_str());
    m_inpCnt++;
}

void
CicoSCMessageRes::addWinObject(const char * key, int value)
{
    json_object_set_int_member(m_winObj, key, value);
    m_winCnt++;
}

void
CicoSCMessageRes::addSndObject(const char * key, int value)
{
    json_object_set_int_member(m_sndObj, key, value);
    m_sndCnt++;
}

void
CicoSCMessageRes::addInpObject(const char * key, int value)
{
    json_object_set_int_member(m_inpObj, key, value);
    m_inpCnt++;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get message data
 *
 *  @return pointer of message data
 */
//--------------------------------------------------------------------------
const char*
CicoSCMessageRes::getData(void)
{
    // count up reference
    json_object_ref(m_rootObj);
    json_object_ref(m_argObj);
    json_object_ref(m_winObj);
    json_object_ref(m_sndObj);
    json_object_ref(m_inpObj);

    if (0 != m_winCnt) {
        json_object_set_object_member(m_argObj, "window", m_winObj);
    }
    if (0 != m_sndCnt) {
        json_object_set_object_member(m_argObj, "sound",  m_sndObj);
    }
    if (0 != m_inpCnt) {
        json_object_set_object_member(m_argObj, "input",  m_inpObj);
    }

    json_object_set_object_member(m_rootObj, "res",   m_argObj);
    json_node_take_object(m_root, m_rootObj);
    json_generator_set_root(m_generator, m_root);
    gsize len = 0;
    gchar* data = json_generator_to_data(m_generator, &len);
    m_data = data;
    g_free(data);
    return m_data.c_str();
}

