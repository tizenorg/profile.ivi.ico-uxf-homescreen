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
 *  @file   CicoSCMessage.h
 *
 *  @brief  This file is definition of CicoSCMessage class
 */
//==========================================================================
#ifndef __CICO_SC_MESSAGE_H__
#define __CICO_SC_MESSAGE_H__

#include <string>

#include <json-glib/json-glib.h>

//==========================================================================
//  forward declaration
//==========================================================================
class CicoSCUwsHandle;

//==========================================================================
/**
 *  @brief  This class hold communication message
 */
//==========================================================================
class CicoSCMessage
{
public:
    // default constructor
    CicoSCMessage();

    // destructor
    ~CicoSCMessage();

    // get message id
    unsigned int getId(void);

    // add root object for char pointer type
    void addRootObject(const char * key, const char * value);

    // add root object for string type
    void addRootObject(const char * key, const std::string & value);

    // add root object for integer type
    void addRootObject(const char * key, int value);

    // add argument object for char pointer type
    void addArgObject(const char * key, const char * value);

    // add argument object for string type
    void addArgObject(const char * key, const std::string & value);

    // add argument object for integer type
    void addArgObject(const char * key, int value);

    // add argument object for array type
    void addArgObject(const char * key);

    // add element to array for char pointer type
    void addElmArray(const char * value);

    // add element to array for string type
    void addElmArray(const std::string & value);

    // get destination handle 
    CicoSCUwsHandle* getHandle(void);

    // set destination handle 
    void setHandle(CicoSCUwsHandle* handle);

    // get message data
    const char* getData(void);

    // set application id of destination
    void setSendToAppid(const std::string & appid);

    // get application id of destination
    const std::string & getSendToAppid(void);

private:
    // assignment operator
    CicoSCMessage& operator=(const CicoSCMessage &object);

    // copy constructor
    CicoSCMessage(const CicoSCMessage &object);

private:
    static unsigned int ms_counter;     //!< message id counter
    int                 m_id;           //!< message id
    CicoSCUwsHandle     *m_uwsHandle;   //!< communication handler
    JsonGenerator       *m_generator;   //!< json generator
    JsonNode            *m_root;        //!< json root node
    JsonObject          *m_rootObj;     //!< json root objects
    JsonObject          *m_argObj;      //!< json arg objects
    JsonArray           *m_array;       //!< json array
    std::string         m_toAppid;      //!< send to appid
    std::string         m_data;         //!< json format data
};
#endif  // __CICO_SC_MESSAGE_H__
// vim:set expandtab ts=4 sw=4:
