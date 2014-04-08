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
 *  @brief  This file implementation of CicoSCUserManager class
 */
/*========================================================================*/

#include <sys/stat.h>
#include <dirent.h>

#include <fstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <cstdio>
using namespace boost;
using namespace boost::property_tree;

#include "ico_syc_error.h"
#include "ico_syc_msg_cmd_def.h"
#include "ico_syc_public.h"

#include "CicoLog.h"

#include "CicoSCCommand.h"
#include "CicoSCLastInfo.h"
#include "CicoAulItems.h"
#include "CicoSCLifeCycleController.h"
#include "CicoSCMessage.h"
#include "CicoSCServer.h"
#include "CicoSCUserManager.h"
#include "CicoSCUser.h"
#include "CicoConf.h"
#include "CicoSystemConfig.h"
#include "Cico_aul_listen_app.h"

using namespace std;

//==========================================================================
//
//  definition
//
//==========================================================================
#define ICO_SYC_DEFAULT_PATH    "/home/app/ico"
#define ICO_SYC_APP_INFO        "runningApp.info"
#define ICO_SYC_APP_INFO_DEF    "defaultApps.info"
#define ICO_SYC_LASTINFO_DIR    (char*)"/lastinfo/"
#define ICO_SYC_LASTUSER_FILE   (char*)"/home/app/ico/lastuser.txt"
#define ICO_SYC_CHGUSR_FLAG_DIR "/tmp/ico"
#define ICO_SYC_CHGUSR_FLAG_FIL "changeUser.flag"

void chkAndAddSlash(string& s);

/**
 * @brief applications die. callback function
 * @param pid
 * @param data user data
 * @return int
 * @retval -1: parameter error
 * @retval 0: handler execute
 */
static int CSCUMapp_dead_handler(int pid, void *data)
{
    CicoSCUserManager* x = (CicoSCUserManager*)data;
    if ((NULL == x) || (0 == x)) {
        return -1;
    }
    x->appDeadHandler(pid);
    return 0;
}

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
    : m_login("")
{
    ICO_TRA("start");
    aul_listen_app_dead_signal_add(CSCUMapp_dead_handler, (void*)this);
    m_vppa.clear();
    m_waitName.clear();
    m_waitHS.clear();
    m_wait = false;
    // login-user application information file
    m_uConfig = CicoSystemConfig::getInstance()->getUserConf();
    if ((NULL == m_uConfig) || (true == m_uConfig->m_parent_dir.empty())) {
        m_parentDir = ICO_SYC_DEFAULT_PATH;
    }
    else {
        m_parentDir = m_uConfig->m_parent_dir;
    }
    chkAndAddSlash(m_parentDir);

    // flag file
    m_flagPath = ICO_SYC_CHGUSR_FLAG_DIR;
    struct stat st;
    int ret = stat(m_flagPath.c_str(), &st);
    if (0 != ret) {
        mkdir(m_flagPath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
    }
    chkAndAddSlash(m_flagPath);
    m_flagPath += ICO_SYC_CHGUSR_FLAG_FIL;
    ICO_TRA("end");

    CicoSCUserManager::ms_myInstance = this;
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSCUserManager::~CicoSCUserManager()
{
    ICO_TRA("CicoSCUserManager::~CicoSCUserManager Enter");

    // clear homescreen list
    m_homescreenList.clear();

    // clear userlist
    vector<CicoSCUser*>::iterator itr;
    itr = m_userList.begin();
    for (; itr != m_userList.end(); ++itr) {
        CicoSCUser *user = const_cast<CicoSCUser*>(*itr);
        // clear list of last information
        vector<CicoSCLastInfo*>::iterator info_itr;
        info_itr = user->lastInfoList.begin();
        for (; info_itr != user->lastInfoList.end(); ++info_itr) {
            // free memory
            delete *info_itr;
        }
        // free memory
        delete *itr;
    }
    m_userList.clear();
    m_vppa.clear();

    CicoSCUserManager::ms_myInstance = NULL;

    ICO_TRA("CicoSCUserManager::~CicoSCUserManager Leave");
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
 *  @param  [in] cmd            control command
 *  @return none
 */
//--------------------------------------------------------------------------
void
CicoSCUserManager::handleCommand(const CicoSCCommand * cmd)
{
    ICO_TRA("CicoSCUserManager::handleCommand Enter(%d)", cmd->cmdid);

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
    case MSG_CMD_GET_LASTINFO:
        // get last information
        lastinfoCB(cmd->appid);
        break;
    case MSG_CMD_SET_LASTINFO:
        // set application's last information
        setLastInfo(cmd->appid, opt->lastinfo);
        break;
    default:
        ICO_WRN("Unknown Command(0x%08x)", cmd->cmdid);
        break;
    }

    ICO_TRA("CicoSCUserManager::handleCommand Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Load configuration file
 *
 *  @param  [in] confFile       configuration file path
 *  @return 0 on success, other on error
 */
//--------------------------------------------------------------------------
int
CicoSCUserManager::load(const string & confFile)
{
    ICO_TRA("CicoSCUserManager::load Enter (%s)", confFile.c_str());

    ptree root;
    // read config file (xml)
    read_xml(confFile, root);

    // create lists
    createUserList(root);
    createHomeScreenList(root);
    // set login user name
    setLoginUser(root);

    ICO_TRA("CicoSCUserManager::load Leave(EOK)");

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
    ICO_TRA("CicoSCUserManager::initialize Enter");

    // get login user information
    const CicoSCUser *user = findUserConfbyName(m_login);
    if (NULL == user) {
        ICO_ERR("user not found");
        ICO_TRA("CicoSCUserManager::initialize Leave(ENXIO)");
        return ICO_SYC_ENXIO;
    }

    // launch homescreen
    if (true == user->autolaunch) {
        launchHomescreenReq(user->name, user->homescreen);
    }
    else {
        ICO_DBG("launch homescreen skip");
    }

    // load last information
    loadLastInfo();

    // make root directory
    string root_dir = m_uConfig->m_parent_dir;
    struct stat st;
    int ret = stat(root_dir.c_str(), &st);
    if (0 != ret) {
        mkdir(root_dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
    }
    // make user's directory
    string user_dir = m_parentDir + m_login;
    chkAndAddSlash(user_dir);
    ret = stat(user_dir.c_str(), &st);
    if (0 != ret) {
        mkdir(user_dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
    }

    // save last user
    saveLastUser();

    ICO_TRA("CicoSCUserManager::initialize Leave(EOK)");

    return ICO_SYC_EOK;
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
    return findUserConfbyName(m_login);
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
 *  @param  [in] name           user name
 *  @param  [in] passwd         password
 *  @return none
 */
//--------------------------------------------------------------------------
void
CicoSCUserManager::changeUser(const string & name, const string & passwd)
{
    ICO_TRA("CicoSCUserManager::changeUser Enter"
            "(user=%s pass=%s)", name.c_str(), passwd.c_str());

    char tmpText[128];
    sprintf(tmpText, "CHG USER[%s]->[%s]", m_login.c_str(), name.c_str());

    string oldUsr = m_login; /* get before login user */
    const CicoSCUser *conf = NULL;
    // check all user logoff
    if ((name.empty()) || (name[0] == ' ')) {
        ICO_TRA("CicoSCUserManager::changeUser Leave(all user logoff)");
        flagFileOn();
        killingAppsAndHS(oldUsr);
        return;
    }

    // if changed user name is same last user name
    if (0 == name.compare(m_login)) {
        ICO_TRA("CicoSCUserManager::changeUser Leave(request is same user)");
        return;
    }

    // get user config
    conf = findUserConfbyName(name);
    if (NULL == conf) {
        ICO_TRA("CicoSCUserManager::changeUser Leave(ENXIO)");
        return;
    }

    // check password
    if (passwd != conf->passwd) {
        ICO_TRA("CicoSCUserManager::changeUser Leave(EINVAL)");
        return;
    }

    flagFileOn();

#if 0
    // Imprinting to file, that file is application's running information
    string usr_dir_old;
    getWorkingDir(oldUsr, usr_dir_old);
    string outfilename = usr_dir_old + ICO_SYC_APP_INFO;
    impritingLastApps(outfilename);
#endif

    // killing running application and homeScreen
    killingAppsAndHS(oldUsr);
    CicoSCLifeCycleController* csclcc = CicoSCLifeCycleController::getInstance();
    csclcc->startAppResource(name);

    // check wheather directory exists
    vector<string> mk_dir_info;
    mk_dir_info.push_back(m_uConfig->m_parent_dir);
    string usr_dir;
    getWorkingDir(conf->name, usr_dir);
    mk_dir_info.push_back(usr_dir);
    vector<string>::iterator it = mk_dir_info.begin();
    for (; it != mk_dir_info.end(); it++) {
        const char* dir = (*it).c_str();
        struct stat st;
        if (0 == stat(dir, &st)) {
            continue; // continue of for
        }
        mkdir(dir, S_IRWXU | S_IRWXG | S_IRWXO);
    }
    csclcc->createAppResourceFile(name);

    if (0 != m_vppa.size()) {
        // wait dead signal recieve
        m_waitName = conf->name;
        m_waitHS = conf->homescreen;
        m_wait = true;
        // run call launchHomescreenReq is appDeadHandler !
        ICO_TRA("CicoSCUserManager::changeUser Leave(WAIT:%s)", m_waitName.c_str());
        return;
    }
    // change homescreen application
    sleep(2);                   // wait 2 sec for dead all applications
    launchHomescreenReq(conf->name, conf->homescreen);
    // change login user
    m_login = conf->name;
    saveLastUser();
    ICO_DBG("login user changed (user=%s)", m_login.c_str());
    ICO_INF("%s", tmpText);
    flagFileOff();
    m_waitName.clear();
    m_waitHS.clear();
    m_wait = false;
    ICO_TRA("CicoSCUserManager::changeUser Leave(EOK)");
}

//--------------------------------------------------------------------------
/**
 *  @brief  imprinting to file, that file is application's running information
 *  @param  usrnam target user name
 *  @return bool
 *  @retval true success
 *  @retval false fail
 */
//--------------------------------------------------------------------------
bool CicoSCUserManager::impritingLastApps(const string& ofnm)
{
    ICO_TRA("CicoSCUserManager::impritingLastApps Enter");
    vector<string> vs;
    CicoSCLifeCycleController* oCSCLCC;
    oCSCLCC = CicoSCLifeCycleController::getInstance();
    if ((NULL == oCSCLCC) || (0 == oCSCLCC)) {
        ICO_ERR(" CicoSCLifeCycleController is null");
        ICO_TRA("CicoSCUserManager::impritingLastApps Leave(false)");
        return false;
    }
    const vector<CicoAulItems>& aulList = oCSCLCC->getAulList();
    for (int i=aulList.size() ; i != 1; i--) {
        const CicoAulItems* pO = aulList[i-1].p();
        if ((NULL == pO) || (0 == pO)) {
            continue;
        }
        if (pO->m_appid == "org.tizen.ico.login") {
            continue;
        }
        if (pO->m_appid == "org.tizen.ico.statusbar") {
            continue;
        }
        if (pO->m_appid == "org.tizen.ico.homescreen") {
            continue;
        }
        vs.push_back(pO->m_appid);
    }

    if (0 == vs.size()) {
        remove(ofnm.c_str());
        ICO_TRA("CicoSCUserManager::impritingLastApps Leave(app none)");
        return true;
    }
    ofstream ofs;
    ofs.open(ofnm.c_str(), ios::trunc);
    vector<string>::iterator it = vs.begin();
    for (; it != vs.end(); it++) {
        ofs << *it << endl;
    }
    ofs.close();
    ICO_TRA("CicoSCUserManager::impritingLastApps app = %d", vs.size());
    return false;
}

//--------------------------------------------------------------------------
/**
 *  @brief  killing running applications and homeScreen
 *  @param  usrnam target user name
 *  @return bool
 *  @retval true success
 *  @retval false fail
 */
//--------------------------------------------------------------------------
bool CicoSCUserManager::killingAppsAndHS(const string&)
{
    ICO_TRA("CicoSCUserManager::killingAppsAndHS Enter");

    m_vppa.clear();
    CicoSCLifeCycleController* oCSCLCC;
    oCSCLCC = CicoSCLifeCycleController::getInstance();
    if ((NULL == oCSCLCC) || (0 == oCSCLCC)) {
        ICO_TRA("CicoSCUserManager::killingAppsAndHS Leave(ENXIO)");
        return false;
    }
    bool r = false;
    const vector<CicoAulItems>& aulList = oCSCLCC->getAulList();
    vector<int> pids;
    for (int i=aulList.size() ; i != 0; i--) {
        const CicoAulItems* pObj = aulList[i-1].p();
        if ((NULL == pObj) || (0 == pObj)) {
            continue;
        }
        ICO_DBG("CicoSCUserManager::killingAppsAndHS Tgt:%d(%s)", pObj->m_pid,
                pObj->m_appid.c_str());
        pids.push_back(pObj->m_pid);
        m_vppa.push_back(pairPidAppid(pObj->m_pid, pObj->m_appid));
        r = true;
    }
    int sz = pids.size();
    for (int j=0; j < sz; j++) {
        oCSCLCC->terminate(pids[j]);
    }
    ICO_TRA("CicoSCUserManager::killingAppsAndHS Leave(ret=%s)",
            r? "true": "false");
    return r;
}

//--------------------------------------------------------------------------
/**
 *  @brief  homescreen launch request
 *  @param  usrnam target user name
 *  @return bool
 *  @retval true success
 *  @retval false fail
 */
//--------------------------------------------------------------------------
void CicoSCUserManager::getWorkingDir(const string& usr, string& dir)
{
    dir = m_parentDir;
    dir += usr;
    chkAndAddSlash(dir);
    return;
}
//--------------------------------------------------------------------------
/**
 *  @brief  homescreen launch request
 *  @param  usrnam target user name
 *  @return bool
 *  @retval true success
 *  @retval false fail
 */
//--------------------------------------------------------------------------
bool CicoSCUserManager::launchHomescreenReq(const string& usr,
                                            const string& appid_hs)
{
    string usr_dir;
    getWorkingDir(usr, usr_dir);
    // set login-user history file path
    string a_prmfn = usr_dir + ICO_SYC_APP_INFO;
    // set default history file path
    string d_prmfn = m_parentDir + string(ICO_SYC_APP_INFO_DEF);

    bundle *b;
    b = bundle_create();
    // bundle add login-user name
    bundle_add(b, ICO_SYC_APP_BUNDLE_KEY1, usr.c_str());
    // bundle add login-user history file path
    bundle_add(b, ICO_SYC_APP_BUNDLE_KEY2, a_prmfn.c_str());
    // bundle add default history file path
    bundle_add(b, ICO_SYC_APP_BUNDLE_KEY3, d_prmfn.c_str());
    // bundle add flag file path
    bundle_add(b, ICO_SYC_APP_BUNDLE_KEY4, m_flagPath.c_str());

    ICO_DBG("launch homescreen (user=%s, appid=%s, parm=%s, %s)",
            usr.c_str(), appid_hs.c_str(), a_prmfn.c_str(), d_prmfn.c_str());

    CicoSCLifeCycleController* oCSCLCC;
    oCSCLCC = CicoSCLifeCycleController::getInstance();
    int r = oCSCLCC->launch(appid_hs.c_str(), b);

    bundle_free(b);

    if (ICO_SYC_EOK != r) {
        ICO_DBG("CicoSCUserManager::launchHomescreenReq false(%d)", r);
        return false;
    }
    ICO_DBG("CicoSCUserManager::launchHomescreenReq true(ICO_SYC_EOK)");
    return true;
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
 *  @param  [in] appid          application id to send message
 *
 *  @return none
 */
//--------------------------------------------------------------------------
void
CicoSCUserManager::userlistCB(const string & appid)
{
    ICO_TRA("CicoSCUserManager::userlistCB Enter (%s)", appid.c_str());

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
    message->addArgObject("user_login", m_login);

    CicoSCServer::getInstance()->sendMessage(appid, message);

    ICO_TRA("CicoSCUserManager::userlistCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Callback of "get application's last information"
 *
 *  @param  [in] appid          application id to send message
 *
 *  @return none
 */
//--------------------------------------------------------------------------
void
CicoSCUserManager::lastinfoCB(const string & appid)
{
    ICO_TRA("CicoSCUserManager::lastinfoCB Enter(%s)", appid.c_str());

    // send message
    CicoSCMessage *message = new CicoSCMessage();
    message->addRootObject("command", MSG_CMD_GET_LASTINFO);
    message->addRootObject("appid", appid);

    CicoSCUser* user = const_cast<CicoSCUser*>(findUserConfbyName(m_login));
    // search application's last information
    vector<CicoSCLastInfo*>::iterator itr;
    itr = user->lastInfoList.begin();
    for (; itr != user->lastInfoList.end(); ++itr) {
        const CicoSCLastInfo *info = const_cast<CicoSCLastInfo*>(*itr);
        if (appid == info->appid) {
            // set last information
            message->addArgObject("lastinfo", info->lastinfo);
            break;
        }
    }

    CicoSCServer::getInstance()->sendMessage(appid, message);

    ICO_TRA("CicoSCUserManager::lastinfoCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Save last user name to last user file
 *
 *  @return none
 */
//--------------------------------------------------------------------------
void
CicoSCUserManager::saveLastUser(void)
{
    ICO_TRA("CicoSCUserManager::saveLastUser Enter(name=%s)", m_login.c_str());

    // output last user name to file
    std::ofstream stream;
    string file = ICO_SYC_LASTUSER_FILE;
    stream.open(file.c_str());
    stream << m_login << std::endl;
    stream.close();

    ICO_TRA("CicoSCUserManager::saveLastUser Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Load last user name from last user file
 *
 *  @return none
 */
//--------------------------------------------------------------------------
void
CicoSCUserManager::loadLastUser(void)
{
    ICO_TRA("CicoSCUserManager::loadLastUser Enter");

    // check weather file exists
    struct stat st;
    int ret = stat(ICO_SYC_LASTUSER_FILE, &st);
    if (0 != ret) {
        // last user file does not exist
        ICO_TRA("CicoSCUserManager::loadLastUser Leave");
        return;
    }

    // load last user name
    std::ifstream stream;
    stream.open(ICO_SYC_LASTUSER_FILE);
    stream >> m_login;
    stream.close();

    // check login name is valid
    if (false == m_login.empty()) {
        bool bValid = false;
        vector<CicoSCUser*>::iterator it = m_userList.begin();
        while (it != m_userList.end()) {
            if (0 == (*it)->name.compare(m_login)) {
                bValid = true;
                break;
            }
            ++it;
        }
        if (false == bValid) {
            ICO_WRN("last user NG!, user name \"%s\" clear", m_login.c_str());
            m_login.clear();
        }
    }

    ICO_TRA("CicoSCUserManager::loadLastUser Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Load application's last information
 *
 *  @return none
 */
//--------------------------------------------------------------------------
void
CicoSCUserManager::loadLastInfo()
{
    ICO_TRA("CicoSCUserManager::loadLastInfo Enter(name=%s)", m_login.c_str());

    if (m_login.empty()) {
        ICO_ERR("m_login is empty");
        ICO_TRA("CicoSCUserManager::loadLastInfo Leave(m_login is empty)");
        return;
    }

    // get login user object
    CicoSCUser* loginUser = NULL;
    loginUser = const_cast<CicoSCUser*>(findUserConfbyName(m_login));
    // check login user
    if (NULL == loginUser) {
        // login user does not exist in the user list
        ICO_ERR("login user not found");
        ICO_TRA("CicoSCUserManager::setLastInfo Leave(login user not found)");
        return;
    }

    // check weather file exists
    struct stat st;
    string dir = m_parentDir + m_login + ICO_SYC_LASTINFO_DIR;
    int ret = stat(dir.c_str(), &st);
    if (0 != ret) {
        // lastinfo directory does not exist
        ICO_WRN("lastinfo directory dose not exist)");
        ICO_TRA("CicoSCUserManager::loadLastInfo Leave(dir dose not exist)");
        return;
    }

    // get file list
    struct dirent **filelist;
    int filenum = scandir(dir.c_str(), &filelist, NULL, NULL);
    for (int i = 0; i < filenum; ++i) {
        string filename = filelist[i]->d_name;
        string::size_type index = filename.find(".txt");
        if (string::npos != index) {
            // load last information from file
            string infofile = dir + filename;
            string info;
            std::ifstream stream;
            stream.open(infofile.c_str());
            std::getline(stream, info);
            stream.close();

            // get appid (erase ".txt" from filename)
            filename.erase(index, filename.size());
            // create new object
            CicoSCLastInfo* lastInfo = new CicoSCLastInfo;
            // set application's information
            lastInfo->appid = filename;
            lastInfo->lastinfo = info;
            // add to list
            loginUser->lastInfoList.push_back(lastInfo);
        }
        free(filelist[i]);
    }
    free(filelist);

    ICO_TRA("CicoSCUserManager::loadLastInfo Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Set login user information
 *
 *  @param  [in] root           pointer of ptree object
 *  @return none
 */
//--------------------------------------------------------------------------
void
CicoSCUserManager::setLoginUser(const ptree & root)
{
    ICO_TRA("CicoSCUserManager::setLoginUser Enter");

    // load last user name
    loadLastUser();

    if (m_login.empty()) {
        // get user name from config file
        ptree default_user = root.get_child("userconfig.default");

        int check_flag = 0;
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
            m_login = name.get();
            check_flag++;
        }

        // default user name does not exist in the m_userList
        if (check_flag == 0) {
            vector<CicoSCUser*>::iterator itr;
            itr = m_userList.begin();
            const CicoSCUser* conf = const_cast<CicoSCUser*>(*itr);
            m_login = conf->name;
        }
    }

    // dump data
    ICO_DBG("login user name: %s", m_login.c_str());

    ICO_TRA("CicoSCUserManager::setLoginUser Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Create userlist
 *
 *  @param  [in] root           pointer of ptree object
 *  @return none
 */
//--------------------------------------------------------------------------
void
CicoSCUserManager::createUserList(const ptree & root)
{
    ICO_TRA("CicoSCUserManager::createUserList Enter");

    ptree homescreens = root.get_child("userconfig.users");

    BOOST_FOREACH (const ptree::value_type& child, homescreens) {
        if (0 != strcmp(child.first.data(), "user")) {
            ICO_ERR("unknown element (%s)", child.first.data());
        }

        // get value
        optional<string> name;
        optional<string> passwd;
        optional<string> homescreen;
        optional<string> autolaunch;

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
        autolaunch = child.second.get_optional<string>("hs.<xmlattr>.autolaunch");
        if (false == autolaunch.is_initialized()) {
            autolaunch = optional<string>("true");
        }
        homescreen = child.second.get_optional<string>("hs");
        if (false == homescreen.is_initialized()) {
            ICO_ERR("user.hs element not found");
            continue;
        }

        CicoSCUser* userConf = new CicoSCUser;
        userConf->name       = name.get();
        userConf->passwd     = passwd.get();
        userConf->homescreen = homescreen.get();
        userConf->autolaunch = (autolaunch.get().compare("false") == 0) ? false : true;
        // dump data
        userConf->dump();

        // add object to the list
        m_userList.push_back(userConf);
    }

    ICO_TRA("CicoSCUserManager::createUserList Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Create homescreen list
 *
 *  @param  [in] root           pointer of ptree object
 *  @return none
 */
//--------------------------------------------------------------------------
void
CicoSCUserManager::createHomeScreenList(const ptree & root)
{
    ICO_TRA("CicoSCUserManager::createHomeScreenList Enter");

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

    ICO_TRA("CicoSCUserManager::createHomeScreenList Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Set last user information
 *
 *  @param  [in] appid          application id
 *  @param  [in] info           application's last information
 *  @return none
 */
//--------------------------------------------------------------------------
void
CicoSCUserManager::setLastInfo(const string & appid, const string & info)
{
    ICO_TRA("CicoSCUserManager::setLastInfo Enter(appid: %s, info: %s)",
            appid.c_str(), info.c_str());

    CicoSCUser* loginUser = NULL;
    CicoSCLastInfo* lastInfo = NULL;

    // get login user object
    loginUser = const_cast<CicoSCUser*>(findUserConfbyName(m_login));
    // check login user
    if (NULL == loginUser) {
        // login user does not exist in the user list
        ICO_ERR("login user does not exist in the user list");
        ICO_TRA("CicoSCUserManager::setLastInfo Leave");
        return;
    }

    // check whether lastinfo object exists
    vector<CicoSCLastInfo*>::iterator info_itr;
    info_itr = loginUser->lastInfoList.begin();
    for (; info_itr != loginUser->lastInfoList.end(); ++info_itr) {
        CicoSCLastInfo *linfo = const_cast<CicoSCLastInfo*>(*info_itr);
        if (appid == linfo->appid) {
            lastInfo = linfo;
            break;
        }
    }

    if (NULL == lastInfo) {
        // create new object
        lastInfo = new CicoSCLastInfo;
        // set application's information
        lastInfo->appid = appid;
        lastInfo->lastinfo = info;
        // add to list
        loginUser->lastInfoList.push_back(lastInfo);
    }
    else {
        // update application's last information
        lastInfo->lastinfo = info;
    }

    // check wheather directory exists
    string info_dir =  m_parentDir + m_login + ICO_SYC_LASTINFO_DIR;
    struct stat st;
    int ret = stat(info_dir.c_str(), &st);
    if (0 != ret) {
        // make directory
        mkdir(info_dir.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
    }

    // output application's last information to file
    std::ofstream stream;
    string file = info_dir + lastInfo->appid + ".txt";
    stream.open(file.c_str());
    stream << lastInfo->lastinfo << std::endl;
    stream.close();

    // dump data
    ICO_DBG("login user=%s", (loginUser->name).c_str());
    lastInfo->dumpLastInfo();

    ICO_TRA("CicoSCUserManager::setLastInfo Leave(EOK)");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Find user configuration by user name
 *
 *  @param  [in] name           user name
 *  @return pointer of CicoSCUser data on success, NULL on error
 */
//--------------------------------------------------------------------------
const CicoSCUser*
CicoSCUserManager::findUserConfbyName(const string & name)
{
//    ICO_TRA("CicoSCUserManager::findUserConfbyName Enter (%s)", name.c_str());

    vector<CicoSCUser*>::iterator itr;
    itr = m_userList.begin();
    for (; itr != m_userList.end(); ++itr) {
        const CicoSCUser* conf = const_cast<CicoSCUser*>(*itr);
        if (name == conf->name) {
//            ICO_TRA("CicoSCUserManager::findUserConfbyName Leave(%s)",
//                    conf->name);
            return conf;
        }
    }

//    ICO_TRA("CicoSCUserManager::findUserConfbyName Leave(NULL)");
    return NULL;
}

/**
 * @brief flag file on(create)
 */
void CicoSCUserManager::flagFileOn(const char* text)
{
    if (true == m_flagPath.empty()) {
        ICO_DBG("FLAG(FILE) empty");
        return;
    }
    const char* f = m_flagPath.c_str();
    ofstream oFlagFile;
    oFlagFile.open(f, ios::trunc);
    if ((NULL != text) && (0 != text) && (0 != strlen(text))) {
        oFlagFile << text << endl;
    }
    oFlagFile.close();
    ICO_DBG("FILE(%s) create(FLAG ON)", f);
}

/**
 * @brief flag file off(remove)
 */
void CicoSCUserManager::flagFileOff()
{
    if (true == m_flagPath.empty()) {
        ICO_DBG("FLAG(FILE) empty");
        return;
    }
    const char* f = m_flagPath.c_str();
    struct stat st;
    int r = stat(f, &st);
    if (0 != r) {
        ICO_DBG("FILE(%s) is Nothing", f);
        return;
    }
    remove(f);
    ICO_DBG("FILE(%s) FLAG off(remove)", f);
    return;
}

/**
 * @brief app dead Handler
 * @param pid dead pid
 * @return bool
 * @retval true pid is target
 * @retval false pid is no target
 */
bool CicoSCUserManager::appDeadHandler(int pid)
{
    ICO_TRA("start (%d)", pid);
    if (false == m_wait) {
        ICO_TRA("end");
        return false;
    }
    ICO_DBG("dead app(%d)", pid);
    bool bBINGO = false;
    string appid;
    vector<pairPidAppid>::iterator it = m_vppa.begin(); // iterator set begin
    while(it != m_vppa.end()) {  // loop to the last data
        if ((*it).first == pid) {
            appid = (*it).second;
            m_vppa.erase(it);
            bBINGO = true;
            break;  // break of while it
        }
        ++it;   // next data
    }
    if (false == bBINGO) {
        ICO_TRA("end FAIL pid(%d)", pid);
        return false;
    }
#if 1
    int sz = m_vppa.size();
    if (0 != sz) {
        ICO_TRA("end contenue wait(%d)", sz);
        return true;
    }
#else
    if (0 != appid.compare(m_waitHS)) {
        ICO_TRA("end contenue wait(%s)", appid.c_str());
        return true;
    }
#endif
    char tmpText[128];
    sprintf(tmpText, "CHG USER[%s]->[%s]", m_login.c_str(), m_waitName.c_str());

    // change homescreen application
    sleep(2);                   // wait 2 sec for dead all applications
    launchHomescreenReq(m_waitName, m_waitHS);
    // change login user
    m_login = m_waitName;
    saveLastUser();
    ICO_DBG("login user changed (user=%s)", m_login.c_str());
    ICO_INF("%s", tmpText);
    flagFileOff();
    m_waitName.clear();
    m_waitHS.clear();
    m_wait = false;
    ICO_TRA("end homescreen start req.(%s)", appid.c_str());
    return true;
}
// vim:set expandtab ts=4 sw=4:
