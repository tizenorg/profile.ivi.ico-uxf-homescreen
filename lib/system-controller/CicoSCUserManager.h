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
 *  @file   CicoSCUserManager.h
 *
 *  @brief  This file is definition of CicoSCUserManager class
 */
//==========================================================================
#ifndef __CICO_SC_USER_MANAGER_H__
#define __CICO_SC_USER_MANAGER_H__

#include <string.h>
#include <vector>

//==========================================================================
//  Forward declaration
//==========================================================================
class CicoSCCommand;
class CicoSCUser;
class CicoSCUserConf;

typedef std::pair<int, std::string> pairPidAppid;

//==========================================================================
/*
 *  @brief  This class manages control of user and access to user information
 */
//==========================================================================
class CicoSCUserManager
{
public:
    // get CicoSCUserManager instance
    static CicoSCUserManager* getInstance(void);

    // handle command
    void handleCommand(const CicoSCCommand * cmd);

    // load configuration file
    int load(const std::string & confFile);
    // initialize
    int initialize(void);

    // get information
    const CicoSCUser* getLoginUser(void);
    const std::vector<CicoSCUser*>& getUserList(void);
    const std::vector<std::string>& getHomeScreenList(void);

    // change user
    void changeUser(const std::string & name, const std::string & passwd);
    void cloaseUser();

    // dump log
    void dumpUserList(void);
    void dumpHomeScreenList(void);

    // application daed watcher
    bool appDeadHandler(int pid);

    bool isStoppingNow();

    // working user directory make
    void getWorkingDir(const std::string& usr, std::string& dir);
    const std::string& getHomeDir() const;
private:
    // default constructor
    CicoSCUserManager();

    // destructor
    ~CicoSCUserManager();

    // assignment operator
    CicoSCUserManager& operator=(const CicoSCUserManager &object);

    // copy constructor
    CicoSCUserManager(const CicoSCUserManager &object);

    // callback
    void userlistCB(const std::string & appid);
    void lastinfoCB(const std::string & appid);

    // save last user name to file
    void saveLastUser(void);
    // load last user name from file
    void loadLastUser(void);
    // load last information from file
    void loadLastInfo(void);

    // set login user info from config file
    void setLoginUser(const boost::property_tree::ptree & root);
    // create list from config file
    void createUserList(const boost::property_tree::ptree & root);
    void createHomeScreenList(const boost::property_tree::ptree & root);

    // set application's last information
    void setLastInfo(const std::string & appid, const std::string & info);

    // find user config by user name
    const CicoSCUser* findUserConfbyName(const std::string & name);

    // Imprinting to file, that file is application's running information 
    bool impritingLastApps(const std::string& ofnm);

    // Killing running application and homeScreen
    bool killingAppsAndHS(const std::string& usrnm);

    // launch homescreen request
    bool launchHomescreenReq(const std::string& usr,
                             const std::string& appid_hs);

    // flag file control
    void flagFileOn(const char* text = NULL);
    void flagFileOff();

private:
    static CicoSCUserManager*   ms_myInstance;  ///< CicoSCUserManager Object

    std::string                 m_login;           ///< Login User Name
    std::vector<CicoSCUser*>    m_userList;        ///< User List
    std::vector<std::string>    m_homescreenList;  ///< HomeScreen List
    std::string                 m_parentDir;       ///  login-user use directory
    const CicoSCUserConf*       m_uConfig;
    std::string                 m_flagPath;        ///  history save control flag file
    std::vector<pairPidAppid>   m_vppa;            ///  app dead watcher
    bool                        m_wait;            ///  app dead wait flag
    std::string                 m_waitName;        ///  wait backup login name
    std::string                 m_waitHS;          ///  wait backup homescreen appid
};

/**
 * @brief get Homescreen Stopping Status
 * @ret bool
 * @retval true: stopping now
 * @retval false: no stop
 *
 */
inline bool CicoSCUserManager::isStoppingNow(){
    return !m_vppa.empty();
}

/**
 * @brief close User
 */
inline void CicoSCUserManager::cloaseUser()
{
    flagFileOn();
    killingAppsAndHS(m_login);
}

/**
 *
 */
inline const std::string& CicoSCUserManager::getHomeDir() const
{
    return m_parentDir;
}

#endif  // __CICO_SC_USER_MANAGER_H__
// vim:set expandtab ts=4 sw=4:
