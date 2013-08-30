/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

/**
 * @brief   CicoSCUserManager.h
 *
 * @date    Aug-21-2013
 */

#ifndef __CICO_SC_USER_MANAGER_H__
#define __CICO_SC_USER_MANAGER_H__

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

#include <string.h>
#include <vector>
using namespace std;
using namespace boost;
using namespace boost::property_tree;

#include    "CicoSCCommandParser.h"
#include    "CicoSCUser.h"

class CicoSCUserManager
{
public:
    // get CicoSCUserManager instance
    static CicoSCUserManager* getInstance(void);

    // handle command
    void handleCommand(const CicoSCCommand * cmd);

    // load configuration file
    int load(const string & confFile);
    // initialize
    int initialize(void);

    // get information
    const CicoSCUser* getDefaultUser(void);
    const CicoSCUser* getLastUser(void);
    const CicoSCUser* getLoginUser(void);
    const vector<CicoSCUser*>& getUserList(void);
    const vector<string>& getHomeScreenList(void);

    // change user
    void changeUser(const string & name, const string & passwd);

    // dump log
    void dumpUserList(void);
    void dumpHomeScreenList(void);

private:
    // default constructor
    CicoSCUserManager();

    // destructor
    ~CicoSCUserManager();

    // assignment operator
    CicoSCUserManager& operator=(const CicoSCUserManager &object);

    // copy constoructor
    CicoSCUserManager(const CicoSCUserManager &object);

    // callback
    void userlistCB(const string & appid);

    // set default user info from config file
    void setDefaultUser(const ptree & root);
    void setLastUser(const ptree & root);
    void setLoginUser(const CicoSCUser * user);
    // create list from config file
    void createUserList(const ptree & root);
    void createHomeScreenList(const ptree & root);

    // find user config by user name
    const CicoSCUser* findUserConfbyName(const string & name);

private:
    static CicoSCUserManager*   ms_myInstance;  //!< CicoSCUserManager Object

    CicoSCUser*                 m_defaultUser;      //!< Default User Info
    CicoSCUser*                 m_lastUser;         //!< Last User Info
    CicoSCUser*                 m_loginUser;        //!< Login User Info
    vector<CicoSCUser*>         m_userList;         //!< User List
    vector<string>              m_homescreenList;   //!< HomeScreen List
};
#endif  // __CICO_SC_USER_MANAGER_H__
// vim:set expandtab ts=4 sw=4:
