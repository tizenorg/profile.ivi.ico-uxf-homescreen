/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

/*========================================================================*/    
/**
 *  @file   CicoSCUserManager.cpp
 *
 *  @brief  
 */
/*========================================================================*/    

#include "ico_syc_error.h"
#include "ico_syc_msg_cmd_def.h"

#include "CicoLog.h"

#include "CicoSCCommandParser.h"
#include "CicoSCLifeCycleController.h"
#include "CicoSCMessage.h"
#include "CicoSCServer.h"
#include "CicoSCUserManager.h"

//==========================================================================    
//
//  private static variable
//
//==========================================================================    
CicoSCUserManager* CicoSCUserManager::ms_myInstance = NULL;

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSCUserManager::CicoSCUserManager()
{
    m_defaultUser = NULL;
    m_lastUser = NULL;
    m_loginUser = NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSCUserManager::~CicoSCUserManager()
{
    ICO_DBG("CicoSCUserManager::~CicoSCUserManager() Enter");

    delete m_defaultUser;

    m_homescreenList.clear();

    vector<CicoSCUser*>::iterator itr;
    itr = m_userList.begin();
    for (; itr != m_userList.end(); ++itr) {
        delete *itr;
    }
    m_userList.clear();

    ICO_DBG("CicoSCUserManager::~CicoSCUserManager() Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Get instance of CicoSCUserManager
 *
 *  @return  pointer of CicoSCUserManager object
 */
//--------------------------------------------------------------------------
CicoSCUserManager*
CicoSCUserManager::getInstance(void)
{
    if (NULL == ms_myInstance) {
        ms_myInstance = new CicoSCUserManager();
    }
    return ms_myInstance;
}

//--------------------------------------------------------------------------
/**
 *  @brief  Handle command
 *
 *  @param  [IN] cmd            control command
 *  @return none
 */
//--------------------------------------------------------------------------
void
CicoSCUserManager::handleCommand(const CicoSCCommand * cmd)
{
    ICO_DBG("CicoSCUserManager::handleCommand Enter (%d)", cmd->cmdid);

    CicoSCCmdUserMgrOpt *opt = static_cast<CicoSCCmdUserMgrOpt*>(cmd->opt);

    switch (cmd->cmdid) {
    case MSG_CMD_CHANGE_USER:
        // change user
        changeUser(opt->user, opt->pass);
        break;
    case MSG_CMD_GET_USERLIST:
        // get userlist
        userlistCB(cmd->appid);
        break;
    default:
        ICO_WRN("Unknown Command(0x%08x)", cmd->cmdid);
        break;
    }

    ICO_DBG("CicoSCUserManager::handleCommand Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Load configuration file
 *
 *  @param  [IN] confFile       configuration file path
 *  @return 0 on success, other on error
 */
//--------------------------------------------------------------------------
int
CicoSCUserManager::load(const string & confFile)
{
    ICO_DBG("CicoSCUserManager::load Enter (%s)", confFile.c_str());

    ptree root;
    // read config file (xml)
    read_xml(confFile, root);

    // create lists
    createUserList(root);
    createHomeScreenList(root);
    // set default user
    setDefaultUser(root);

    ICO_DBG("CicoSCUserManager::load Leave");

    // always success
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  Initialize CicoSCUserManager
 *          This API must call after load() API
 *
 *  @return 0 on success, other on error
 */
//--------------------------------------------------------------------------
int
CicoSCUserManager::initialize(void)
{
// TODO
    ICO_DBG("CicoSCUserManager::initialize Enter");

    const CicoSCUser *user = NULL;

    if (NULL != m_lastUser) {
        user = m_lastUser;
    }
    else if (NULL != m_defaultUser) {
        user = m_defaultUser;
    }
    else {
        ICO_ERR("last user and default user are null");
        return ICO_SYC_ENXIO;
    }

    // update login user information
    setLoginUser(user);

    // launch homescreen
    ICO_DBG("launch homescreen (user=%s, appid=%s)",
            (user->name).c_str(), (user->homescreen).c_str());
    CicoSCLifeCycleController::getInstance()->launch((user->homescreen).c_str());

    ICO_DBG("CicoSCUserManager::initialize Leave");

    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  Get default user information
 *
 *  @return pointer of CicoSCUser
 */
//--------------------------------------------------------------------------
const CicoSCUser*
CicoSCUserManager::getDefaultUser(void)
{
    return m_defaultUser;
}

//--------------------------------------------------------------------------
/**
 *  @brief  Get last user information
 *
 *  @return pointer of CicoSCUser
 */
//--------------------------------------------------------------------------
const CicoSCUser*
CicoSCUserManager::getLastUser(void)
{
    return m_lastUser;
}

//--------------------------------------------------------------------------
/**
 *  @brief  Get login user information
 *
 *  @return pointer of CicoSCUser
 */
//--------------------------------------------------------------------------
const CicoSCUser*
CicoSCUserManager::getLoginUser(void)
{
    return m_loginUser;
}

//--------------------------------------------------------------------------
/**
 *  @brief  Get userlist
 *
 *  @return pointer of userlist
 */
//--------------------------------------------------------------------------
const vector<CicoSCUser*>&
CicoSCUserManager::getUserList(void)
{
    return m_userList;
}

//--------------------------------------------------------------------------
/**
 *  @brief  Get homescreen list
 *
 *  @return pointer of homescreen list
 */
//--------------------------------------------------------------------------
const vector<string>&
CicoSCUserManager::getHomeScreenList(void)
{
    return m_homescreenList;
}

//--------------------------------------------------------------------------
/**
 *  @brief  Change login user
 *
 *  @param  [IN] name           user name
 *  @param  [IN] passwd         password
 *  @return none
 */
//--------------------------------------------------------------------------
void
CicoSCUserManager::changeUser(const string & name, const string & passwd)
{
    ICO_DBG("CicoSCUserManager::changeUser Enter"
            "(user=%s pass=%s)", name.c_str(), passwd.c_str());

    const CicoSCUser *conf = NULL;

    // get user config
    conf = findUserConfbyName(name);
    if (NULL == conf) {
        ICO_WRN("user \"%s\" does not exist in the userlist", name.c_str());
        return;
    }

    // check password
    if (passwd != conf->passwd) {
        ICO_ERR("user \"%s\" invalid password", name.c_str());
        return;
    }

    // change homescreen application
    ICO_DBG("launch homescreen (user=%s, appid=%s)",
            (conf->name).c_str(), (conf->homescreen).c_str());
    CicoSCLifeCycleController::getInstance()->launch((conf->homescreen).c_str());

    // set lastUser
    m_lastUser = const_cast<CicoSCUser*>(conf);
    ICO_DBG("last user changed (user=%s)", name.c_str());

    ICO_DBG("CicoSCUserManager::changeUser Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  dump log m_userList member variables
 *
 *  @return none
 */
//--------------------------------------------------------------------------
void
CicoSCUserManager::dumpUserList(void)
{
    vector<CicoSCUser*>::iterator itr;
    itr = m_userList.begin();
    for (; itr != m_userList.end(); ++itr) {
        const CicoSCUser* conf = const_cast<CicoSCUser*>(*itr);
        ICO_DBG("user name=%s, pass=%s, homescreen=%s",
                (conf->name).c_str(), (conf->passwd).c_str(),
                (conf->homescreen).c_str());
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief  dump log m_homescreenList member variables
 *
 *  @return none
 */
//--------------------------------------------------------------------------
void
CicoSCUserManager::dumpHomeScreenList(void)
{
    vector<string>::iterator itr;
    itr = m_homescreenList.begin();
    for (; itr != m_homescreenList.end(); ++itr) {
        ICO_DBG("homescreen=%s", (*itr).c_str());
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief  Callback of "get userlist"
 *
 *  @param  [IN] appid          application id to send message
 *
 *  @return none
 */
//--------------------------------------------------------------------------
void
CicoSCUserManager::userlistCB(const string & appid)
{
    ICO_DBG("CicoSCUserManager::userlistCB Enter");

    // send message
    CicoSCMessage *message = new CicoSCMessage();
    message->addRootObject("command", MSG_CMD_GET_USERLIST);
    message->addRootObject("appid", appid);
    // set number of user
    message->addArgObject("user_num", m_userList.size());
    // set user list
    vector<CicoSCUser*>::iterator itr;
    itr = m_userList.begin();
    for (; itr != m_userList.end(); ++itr) {
        const CicoSCUser* conf = const_cast<CicoSCUser*>(*itr);
        message->addElmArray(conf->name);
    }
    message->addArgObject("user_list");

    // set login user name
    if (NULL != m_loginUser) {
        message->addArgObject("user_login", m_loginUser->name);
    }
    else {
        message->addArgObject("user_login", m_defaultUser->name);
    }

    CicoSCServer::getInstance()->sendMessage(appid, message);

    ICO_DBG("CicoSCUserManager::userlistCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Set default user information
 *
 *  @param  [IN] root           pointer of ptree object
 *  @return none
 */
//--------------------------------------------------------------------------
void
CicoSCUserManager::setDefaultUser(const ptree & root)
{
    ICO_DBG("CicoSCUserManager::setDefaultUser Enter");

    if (NULL == m_defaultUser) {
        m_defaultUser = new CicoSCUser();
    }

    ptree default_user = root.get_child("userconfig.default");

    BOOST_FOREACH (const ptree::value_type& child, default_user) {
        if (0 != strcmp(child.first.data(), "user")) {
            ICO_ERR("unknown element (%s)", child.first.data());
        }

        optional<string> name;
        name = child.second.get_optional<string>("name");
        if (false == name.is_initialized()) {
            ICO_ERR("user.name element not found");
            continue;
        }
        m_defaultUser->name = name.get();
    }

    /* get homescreen and password info from m_userList */
    int check_flag = 0;
    vector<CicoSCUser*>::iterator itr;
    itr = m_userList.begin();
    for (; itr != m_userList.end(); ++itr) {
        const CicoSCUser* conf = const_cast<CicoSCUser*>(*itr);
        if (conf->name == m_defaultUser->name) {
            m_defaultUser->passwd = conf->passwd;
            m_defaultUser->homescreen = conf->homescreen;
            check_flag++;
            break;
        }
    }

    /* default user name does not exist in the m_userList */
    if (check_flag == 0) {
        itr = m_userList.begin();
        const CicoSCUser* conf = const_cast<CicoSCUser*>(*itr);
        m_defaultUser->name = conf->name;
        m_defaultUser->passwd = conf->passwd;
        m_defaultUser->homescreen = conf->homescreen;
    }

    // dump data
    m_defaultUser->dump();

    ICO_DBG("CicoSCUserManager::setDefaultUser Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Set last user information
 *
 *  @param  [IN] root           pointer of ptree object
 *  @return none
 */
//--------------------------------------------------------------------------
void
CicoSCUserManager::setLastUser(const ptree & root)
{
// TODO
    ICO_DBG("CicoSCUserManager::setLastUser Enter");

    if (NULL == m_lastUser) {
        m_lastUser = m_defaultUser;
    }

    // dump data
    //m_lastUser->dump();

    ICO_DBG("CicoSCUserManager::setLastUser Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Set login user information
 *
 *  @param  [IN] user           pointer of CicoSCUser
 *  @return none
 */
//--------------------------------------------------------------------------
void
CicoSCUserManager::setLoginUser(const CicoSCUser * user)
{
// TODO
    ICO_DBG("CicoSCUserManager::setLoginUser Enter");

    if (NULL == user) {
        m_loginUser = m_defaultUser;
    }
    else {
        m_loginUser = const_cast<CicoSCUser*>(user);
    }
    // dump data
    m_loginUser->dump();

    ICO_DBG("CicoSCUserManager::setLoginUser Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Create userlist
 *
 *  @param  [IN] root           pointer of ptree object
 *  @return none
 */
//--------------------------------------------------------------------------
void
CicoSCUserManager::createUserList(const ptree & root)
{
    ICO_DBG("CicoSCUserManager::createUserList Enter");

    ptree homescreens = root.get_child("userconfig.users");

    BOOST_FOREACH (const ptree::value_type& child, homescreens) {
        if (0 != strcmp(child.first.data(), "user")) {
            ICO_ERR("unknown element (%s)", child.first.data());
        }

        // get value
        optional<string> name;
        optional<string> passwd;
        optional<string> homescreen;

        name = child.second.get_optional<string>("name");
        if (false == name.is_initialized()) {
            ICO_ERR("user.name element not found");
            continue;
        }
        passwd = child.second.get_optional<string>("passwd");
        if (false == passwd.is_initialized()) {
            ICO_ERR("user.passwd element not found");
            continue;
        }
        homescreen = child.second.get_optional<string>("hs");
        if (false == homescreen.is_initialized()) {
            ICO_ERR("user.hs element not found");
            continue;
        }

        CicoSCUser* userConf    = new CicoSCUser;
        userConf->name          = name.get();
        userConf->passwd        = passwd.get();
        userConf->homescreen    = homescreen.get();
        // dump data
        userConf->dump();

        // add object to the list
        m_userList.push_back(userConf);
    }

    ICO_DBG("CicoSCUserManager::createUserList Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Create homescreen list
 *
 *  @param  [IN] root           pointer of ptree object
 *  @return none
 */
//--------------------------------------------------------------------------
void
CicoSCUserManager::createHomeScreenList(const ptree & root)
{
    ICO_DBG("CicoSCUserManager::createHomeScreenList Enter");

    ptree homescreens = root.get_child("userconfig.homescreens");

    BOOST_FOREACH (const ptree::value_type& child, homescreens) {
        if (0 != strcmp(child.first.data(), "homescreen")) {
            ICO_ERR("unknown element (%s)", child.first.data());
        }
        // get value
        optional<string> name;
        name = child.second.get_optional<string>("<xmlattr>.name");
        if (false == name.is_initialized()) {
            ICO_ERR("homescreen.name attr not found");
            continue;
        }
        // add value to the list
        m_homescreenList.push_back(name.get());
    }
    // dump data
    dumpHomeScreenList();

    ICO_DBG("CicoSCUserManager::createHomeScreenList Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Find user configuration by user name
 *
 *  @param  [IN] name           user name
 *  @return pointer of CicoSCUser data on success, NULL on error
 */
//--------------------------------------------------------------------------
const CicoSCUser*
CicoSCUserManager::findUserConfbyName(const string & name)
{
    ICO_DBG("CicoSCUserManager::findUserConfbyName Enter (%s)", name.c_str());

    if (name == m_defaultUser->name) {
        ICO_DBG("CicoSCUserManager::findUserConfbyName Leave (defaultUser)");
        return m_defaultUser;
    }

    vector<CicoSCUser*>::iterator itr;
    itr = m_userList.begin();
    for (; itr != m_userList.end(); ++itr) {
        const CicoSCUser* conf = const_cast<CicoSCUser*>(*itr);
        if (name == conf->name) {
            ICO_DBG("CicoSCUserManager::findUserConfbyName Leave");
            return conf;
        }
    }

    ICO_ERR("CicoSCUserManager::findUserConfbyName Leave (NOT find)");
    return NULL;
}
// vim:set expandtab ts=4 sw=4:
