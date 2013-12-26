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
 *  @file   CicoHSMessage.cpp
 *
 *  @brief  This file implementation of CicoHSMessage class
 */
//==========================================================================

#include <string>
#include <limits.h>

#include "CicoHSMessage.h"

//==========================================================================    
//  private static variable
//==========================================================================    
unsigned int CicoHSMessage::ms_counter = 0;

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoHSMessage::CicoHSMessage()
{
    if (ms_counter == UINT_MAX) {
        ms_counter = 0;
    }
    m_id = ++ms_counter;
    m_generator = json_generator_new();
    m_root      = json_node_new(JSON_NODE_OBJECT);
    m_rootObj   = json_object_new();
    m_argObj    = json_object_new();
    m_array     = json_array_new();
    m_data.clear();
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoHSMessage::~CicoHSMessage()
{
    //ICO_TRA("CicoHSMessage::~CicoHSMessage Enter");
    json_array_unref(m_array);
    json_object_unref(m_rootObj);
    json_object_unref(m_argObj);
    json_node_free(m_root);
    g_object_unref(m_generator);
    //ICO_TRA("CicoHSMessage::~CicoHSMessage Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  get message id
 *
 *  @return id of message
 */
//--------------------------------------------------------------------------
unsigned int
CicoHSMessage::getId(void)
{
    return m_id;
}

//--------------------------------------------------------------------------
/**
 *  @brief  add root object for char pointer type
 *
 *  @param [in] key     key name of object
 *  @param [in] value   value of object for char pointer type
 */
//--------------------------------------------------------------------------
void
CicoHSMessage::addRootObject(const char * key, const char * value)
{
//    ICO_DBG("json_object_set_string_member(%s, %s) called.", key, value);
    json_object_set_string_member(m_rootObj, key, value);
}

//--------------------------------------------------------------------------
/**
 *  @brief  add root object for string type
 *
 *  @param [in] key     keyname of object
 *  @param [in] value   value of object for string type
 */
//--------------------------------------------------------------------------
void
CicoHSMessage::addRootObject(const char * key, const std::string & value)
{
//    ICO_DBG("json_object_set_string_member(%s, %s) called.", key, value.c_str());
    json_object_set_string_member(m_rootObj, key, value.c_str());
}

//--------------------------------------------------------------------------
/**
 *  @brief  add root object for integer type
 *
 *  @param [in] key     key name of object
 *  @param [in] value   value of object for integer type
 */
//--------------------------------------------------------------------------
void
CicoHSMessage::addRootObject(const char * key, int value)
{
//    ICO_DBG("json_object_set_int_member(%s, %d) called.", key, value);
    json_object_set_int_member(m_rootObj, key, value);
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
CicoHSMessage::addArgObject(const char * key, const char * value)
{
//    ICO_DBG("json_object_set_string_member(%s, %s) called.", key, value);
    json_object_set_string_member(m_argObj, key, value);
}

//--------------------------------------------------------------------------
/**
 *  @brief  add argument object for string type
 *
 *  @param [in] key     key name of object
 *  @param [in] value   value of object for string type
 */
//--------------------------------------------------------------------------
void
CicoHSMessage::addArgObject(const char * key, const std::string & value)
{
//    ICO_DBG("json_object_set_string_member(%s, %s) called.", key, value.c_str());
    json_object_set_string_member(m_argObj, key, value.c_str());
}

//--------------------------------------------------------------------------
/**
 *  @brief  add argument object for integer type
 *
 *  @param [in] key     key name of object
 *  @param [in] value   value of object for integer type
 */
//--------------------------------------------------------------------------
void
CicoHSMessage::addArgObject(const char * key, int value)
{
//    ICO_DBG("json_object_set_int_member(%s, %d) called.", key, value);
    json_object_set_int_member(m_argObj, key, value);
}

//--------------------------------------------------------------------------
/**
 *  @brief  add argument object for array type
 *
 *  @param [in] key     key name of array
 */
//--------------------------------------------------------------------------
void
CicoHSMessage::addArgObject(const char * key)
{
    // count up reference
    json_array_ref(m_array);

//    ICO_DBG("json_object_set_array_member(%s) called.", key);
    json_object_set_array_member(m_argObj, key, m_array);
}

//--------------------------------------------------------------------------
/**
 *  @brief  add array element for char pointer type
 *
 *  @param [in] value   value of object for char pointer type
 */
//--------------------------------------------------------------------------
void
CicoHSMessage::addElmArray(const char * value)
{
//    ICO_DBG("json_array_add_string_element(%s) called.", value);
    json_array_add_string_element(m_array, value);
}

//--------------------------------------------------------------------------
/**
 *  @brief  add array element for string type
 *
 *  @param [in] value   value of object for string type
 */
//--------------------------------------------------------------------------
void
CicoHSMessage::addElmArray(const std::string & value)
{
//    ICO_DBG("json_array_add_string_element(%s) called.", value.c_str());
    json_array_add_string_element(m_array, value.c_str());
}

//--------------------------------------------------------------------------
/**
 *  @brief  get destination handle 
 *
 *  @return pointer of destination handle
 */
//--------------------------------------------------------------------------
CicoHSUwsHandle*
CicoHSMessage::getHandle(void)
{
    return m_uwsHandle;
}

//--------------------------------------------------------------------------
/**
 *  @brief  set destination handle 
 *
 *  @param [in]  handle destination handle
 */
//--------------------------------------------------------------------------
void
CicoHSMessage::setHandle(CicoHSUwsHandle* handle)
{
    m_uwsHandle = handle;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get message data
 *
 *  @return pointer of message data
 */
//--------------------------------------------------------------------------
const char*
CicoHSMessage::getData(void)
{
    // count up reference
    json_object_ref(m_rootObj);
    json_object_ref(m_argObj);

    json_object_set_object_member(m_rootObj, "arg", m_argObj);
    json_node_take_object(m_root, m_rootObj);
    json_generator_set_root(m_generator, m_root);
    gsize len = 0;
    gchar* data = json_generator_to_data(m_generator, &len);
    m_data = data;
    g_free(data);
    return m_data.c_str();
}

//--------------------------------------------------------------------------
/**
 *  @brief  get application id of destination
 *
 *  @return application id
 */
//--------------------------------------------------------------------------
const std::string &
CicoHSMessage::getSendToAppid(void)
{
    return m_toAppid;
}

//--------------------------------------------------------------------------
/**
 *  @brief  set application id of destination
 */
//--------------------------------------------------------------------------
void
CicoHSMessage::setSendToAppid(const std::string & appid)
{
    m_toAppid = appid;
}
// vim:set expandtab ts=4 sw=4:
