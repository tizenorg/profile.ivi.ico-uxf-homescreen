/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#include <memory>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>
#include <sstream>
#include <utility>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>

#include "CicoLog.h"
#include "CicoSCCommand.h"
#include "CicoSCSysResourceController.h"
#include "CicoSCAppResourceController.h"
#include "CicoSCLifeCycleController.h"
#include "CicoSCUserManager.h"
#include "CicoConf.h"
#include "CicoSystemConfig.h"
#include "CicoLibString.h"
#include <Ecore.h>
//#include <Ecore_Evas.h>

/**
 * @brief constructor
 */
CicoSCAppResourceController::CicoSCAppResourceController()
{
    ICO_TRA("start");
    m_bDoItApp = false;
    m_filepath.clear();
    ICO_TRA("end");
}

/**
 * @brief destructor
 */
CicoSCAppResourceController::~CicoSCAppResourceController()
{
    ICO_TRA("start");
    ICO_TRA("end");
}

/**
 * @brief startAppResource
 */
bool CicoSCAppResourceController::initAppResource()
{
    ICO_TRA("start");
    if (true == m_bDoIt) {
        ICO_TRA("end");
        return false;
    }
    if (0 == m_rConf) {
        ICO_TRA("end");
        return false;
    }
    m_bDoItApp = m_rConf->m_bDoItApp;
    if (false == m_bDoItApp) {
        ICO_TRA("end");
        return false;
    }
    m_filepath.clear();
    ICO_TRA("end");
    return true;
}

/**
 * @brief startAppResource
 */
bool CicoSCAppResourceController::startAppResource(const string& un)
{
    ICO_TRA("start %s", un.c_str());
    if (false == m_bDoItApp) {
        ICO_TRA("end");
        return false;
    }

    string fn;
    getAppResourceFilePath(un, fn);
    if (false == orderApps(fn)) {
        ICO_TRA("end");
        return false;
    }
    m_filepath = fn;
    ICO_TRA("end");
    return true;
}

/**
 * @brief get application resouce file path
 * @param un user name
 * @param fi file path application resource file
 */
void CicoSCAppResourceController::getAppResourceFilePath(const string& un,
                                                         string& fp) const
{
    CicoSCUserManager *cscum = CicoSCUserManager::getInstance();
    cscum->getWorkingDir(un, fp);
    fp += D_STRappResource;
}

/**
 * @brief oder application resource
 */
bool CicoSCAppResourceController::orderApps(const string& filepath)
{
    ICO_TRA("start");
    if (false == m_bDoItApp) {
        ICO_TRA("end");
        return false;
    }
    clearApps();

    if (false == readApps(filepath)) {
        ICO_TRA("end fail:read");
        return false;
    }

    // read finish
    if (0 == m_vAppid.size()) {
        ICO_TRA("end fail read2");
        return false;
    }
    init_cgroup_app_dir();
    init_cgroup_app_cpushare();
    return false;
}

/**
 * @brief read app
 * @param filepath file name (full path)
 */
bool CicoSCAppResourceController::readApps(const string& filepath)
{
    ICO_TRA("start file:%s", filepath.c_str());
    if (false == m_bDoItApp) {
        ICO_TRA("end");
        return false;
    }
    bool bR = false;
    double d1 = 1024.0;
    double d2 = 100.0;
#if 0 // TODO
    // text file read
    ifstream ifs(filepath.c_str());
    char sBuff[256];
    while(ifs.getline(sBuff,sizeof(sBuff))) {
        string tagApp(sBuff);
        if (true == tagApp.empty()) {
            continue;
        }
        vector<string> vTmp;
        split(tagApp, ',', vTmp);
        if (2 == vTmp.size()) {
            string appid = trim(vTmp[0]);
            string share = trim(vTmp[1]);
            int iShare = atoi(share.c_str());
            if (0 != iShare) {
                if (100 < iShare) {
                    ICO_DBG("change %d -> 100", iShare);
                    iShare = 100;
                }
                iShare = (int)(d1 * ((double)iShare) / d2);
                ICO_DBG("TARGET_OK %s, %d", appid.c_str(), iShare);
                m_vAppid.push_back(appid);
                m_vShares.push_back(iShare);
                bR = true;
            }
            else {
                ICO_DBG("TARGET_NG %s, >> %s <<", appid.c_str(), share.c_str());
            }

        }
        else {
            ICO_DBG("TARGET_NG >> %s <<", sBuff);
        }
        vTmp.clear();
    }
    ifs.close();
#else
    struct stat buff;
    /* file check */
    memset(&buff, 0, sizeof(buff));
    if (0 != stat(filepath.c_str(), &buff)) {
        ICO_TRA("end");
        return false;
    }
    bool bStt=true;
    // json read
    try {
        ptree pt;
        read_json(filepath.c_str(), pt);
        // cpu_shares
        BOOST_FOREACH (const ptree::value_type& child,
                       pt.get_child(DSTRcpu_shares)) {
            const ptree& info = child.second;

            string appid;
            int iShareO;
            // cpu_shares.share
            if (boost::optional<int> iVal = info.get_optional<int>(DSTRshare)) {
                iShareO = iVal.get();
            }
            else {
                ICO_DBG("NG get share");
                bStt=false;
                break; // break of BOOST_FOREACH
            }

            // cpu_shares.appid
            if (boost::optional<std::string> sVal =
                    info.get_optional<std::string>(DSTRappid)) {
                appid = sVal.get();
            }
            else {
                ICO_DBG("NG get appid");
                bStt=false;
                break; // break of BOOST_FOREACH
            }
            // DATA get!
            int iShare = iShareO;
            if (100 < iShare) {
                iShare = 100;
            }
            iShare = (int)(d1 * ((double)iShare) / d2);
            ICO_DBG("TARGET_GET %d(%d), %s", iShare, iShareO, appid.c_str());
            m_vAppid.push_back(appid);
            m_vShares.push_back(iShare);
            bR = true;
        }
    }
    catch (...) {
        ICO_WRN("NG read(%s)", filepath.c_str());
        bStt = false;
    }
    if (false == bStt) {
        m_vAppid.clear();
        m_vShares.clear();
        ICO_TRA("end %d", (int)bR);
        return false;
    }
#endif
    ICO_DBG("read %d, %d, %d, %d", m_vAppid.size(), m_vShares.size(),
            m_RCCpuTasks.size(), m_RCCpuShares.size());
    ICO_TRA("end %d", (int)bR);
    return bR;
}

/**
 * @brief initialize cgroup application
 */
void CicoSCAppResourceController::init_cgroup_app_dir()
{
    ICO_TRA("start");
    if (false == m_bDoItApp) {
        ICO_TRA("end");
        return;
    }
    string sTasks(D_STRtasks);
    string sShares(D_STRcpushares);
    string t1(m_rConf->m_cpuCGRPPath);
    chkAndAddSlash(t1);

    string tDir1(m_rConf->m_cpuCGRPPath);
    vector<string> d;
    d.push_back(tDir1);
    chkAndAddSlash(tDir1);

    vector<string>::iterator it = m_vAppid.begin();
    for (; it != m_vAppid.end(); it++) {
        // make cgroup apps directroy full path
        string tDir2(tDir1);
        tDir2 += *it;
        d.push_back(tDir2);
        chkAndAddSlash(tDir2);
        // make cgroup tasks file full path
        string tDir3tasks(tDir2);
        tDir3tasks += sTasks; 
        m_RCCpuTasks.push_back(tDir3tasks);
        // make cgroup cpu.shares file full path
        string tDir4cpushare(tDir2);
        tDir4cpushare += sShares;
        m_RCCpuShares.push_back(tDir4cpushare);
    }
    make_directorys(d);
    ICO_TRA("end");
    return;
}

/**
 * @brief initialize cgroup application
 */
void CicoSCAppResourceController::init_cgroup_app_cpushare()
{
    ICO_TRA("start");
    if (false == m_bDoItApp) {
        ICO_TRA("end");
        return;
    }

    vector<string>::iterator itDir = m_RCCpuShares.begin();
    vector<int>::iterator itShare = m_vShares.begin();
    while (itDir != m_RCCpuShares.end()) {
        replaceCgroupFile(*itDir, *itShare);
        ++itDir;
        ++itShare;
    }
    ICO_TRA("end");
    return;
}

/**
 * @brief clear Apps
 */
bool CicoSCAppResourceController::clearApps()
{
    ICO_TRA("start");
    if (false == m_bDoItApp) {
        ICO_TRA("end");
        return false;
    }
    m_vAppid.clear();
    m_vShares.clear();
    m_RCCpuTasks.clear();
    m_RCCpuShares.clear();
    ICO_TRA("end fail:read");
    return true;
}

/**
 * @brief entry
 * @param appid 
 * @param pid
 */
bool CicoSCAppResourceController::entryApp(const string& appid, int pid)
{
    ICO_TRA("start appid(%s), pid(%d)", appid.c_str(), pid);
    if (false == m_bDoItApp) {
        ICO_TRA("end");
        return false;
    }
    vector<string>::iterator itTasks = m_RCCpuTasks.begin();
    vector<string>::iterator itAppid = m_vAppid.begin();
    string tasks;
    tasks.clear();
    while (itTasks != m_RCCpuTasks.end()) {
        if (0 == (*itAppid).compare(appid.c_str())) {
            tasks = *itTasks;
            break;
        }
        ++itTasks;
        ++itAppid;
    }
    if (true == tasks.empty()) {
        ICO_TRA("end no match appid");
        return false;
    }
    replaceCgroupFile(tasks, pid);
    ICO_TRA("end write cgroup %s, %d", tasks.c_str(), pid);
    return true;
}

/**
 * @brief entry application's
 * @param vAulItem
 */
bool CicoSCAppResourceController::entryApps(vector<CicoAulItems>& vAulItem)
{
    ICO_TRA("start size(%d)", vAulItem.size());
    if (false == m_bDoItApp) {
        ICO_TRA("end");
        return false;
    }
    bool bR = false;
    vector<CicoAulItems>::iterator it = vAulItem.begin();
    while (it != vAulItem.end()) {
        entryApp(it->m_appid, it->m_pid);
        bR = true;
        ++it;
    }
    ICO_TRA("end %s", bR? "true": "false");
    return bR;
}

/**
 * @brief create applications Resource file
 * @param file path create target
 */
void CicoSCAppResourceController::createAppResourceFile(const string& filepath)
{
    ICO_TRA("start %s", filepath.c_str());
#if 0
    ofstream ofs;
    ofs.open(filepath.c_str(), ios::trunc);
    const vector<CicoAilItems>& vAil =
        CicoSCLifeCycleController::getInstance()->getAilList();
    vector<CicoAilItems>::const_iterator it = vAil.begin();
    for (; it != vAil.end(); ++it) {
        ofs << (*it).m_appid << ",100" << endl;
    }
    ofs.close();
#else
    try{
        ptree pt;
        ptree child;

        const vector<CicoAilItems>& vAil =
            CicoSCLifeCycleController::getInstance()->getAilList();
        vector<CicoAilItems>::const_iterator it = vAil.begin();
        for (; it != vAil.end(); ++it) {
            {
                ptree info;
                info.put(DSTRshare, 100);
                info.put(DSTRappid, (*it).m_appid.c_str());
                child.push_back(std::make_pair("", info));
            }
        }
        pt.add_child(DSTRcpu_shares, child);
        write_json(filepath.c_str(), pt);
    }
    catch (std::exception const& e) {
        ICO_WRN("catch exception %s", e.what());
    }
    catch (...) {
        ICO_WRN("NG write(%s)", filepath.c_str());
    }

#endif
    ICO_TRA("end");
}
