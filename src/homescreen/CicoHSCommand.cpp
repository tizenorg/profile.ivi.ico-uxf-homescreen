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
 *  @file   CicoHSCommand.cpp
 *
 *  @brief  This file is implementation of CicoHSCommand class
 */
//==========================================================================

#include <iostream>
#include <sstream>
#include <exception>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
using namespace std;
using namespace boost;
using namespace boost::property_tree;

#include "CicoHSCommand.h"
#include "CicoHSCommandArg.h"
#include <ico_log.h>
#include "ico_syc_error.h"
#include "ico_syc_msg_cmd_def.h"

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoHSCommand::CicoHSCommand()
    : cmdid(0), appid(""), pid(-1), seq_no(-1), opt(NULL)
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoHSCommand::~CicoHSCommand()
{
    delete opt;
}

//--------------------------------------------------------------------------
/**
 *  @brief  parse from message, create command
 *
 *  @param [in] message     message of interprocess communication
 *
 *  @note
 *  message format(json format)
 *  key name of "appid", "command", "pid" is common command propety.
 *  <pre>
 *  {
 *    "appid":   (string) application id,
 *    "command": (int)    command id,
 *    "pid":     (int)    process id
 *    ...
 *    }
 *  }
 *  </pre>
 */
//--------------------------------------------------------------------------
int
CicoHSCommand::parseMessage(const char *message)
{
//    ICO_TRA("CicoHSCommand::parseMessage Enter(%s)", message);
    try {
        stringstream jsonString(message);
        ptree root;
        read_json(jsonString, root);

        this->cmdid = getIntValue(root, MSG_PRMKEY_CMD);
        this->appid = getStrValue(root, "source.appid");
        this->pid   = getIntValue(root, "source.pid");
        this->seq_no = getIntValue(root, "source.seq_no");

        switch (this->cmdid) {
        case MSG_CMD_APP_START:
        case MSG_CMD_APP_STOP:
        case MSG_CMD_WIN_CHANGE:
            parseAppCtlOpt(root);
            break;
        default:
            break;
        }

        // dump log command variables
        dump();
    }
    catch (std::exception const& e)
    {
        ICO_WRN("catch exception %s", e.what());
//        ICO_TRA("CicoHSCommand::parseMessage Leave(EINVAL)");
        return ICO_SYC_EINVAL;
    }

//    ICO_TRA("CicoHSCommand::parseMessage Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  dump log this class member variables
 */
//--------------------------------------------------------------------------
void
CicoHSCommand::dump(void)
{
    ICO_INF("Command: cmdid=0x%08X appid=%s pid=%d seq_no=%d", cmdid, appid.c_str(), pid, seq_no);

    if (NULL != this->opt) {
//        this->opt->dump(); 
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief  get integer value by key
 *
 *  @param [in] root    root object of json object
 *  @param [in] key     Key to get the value
 *
 *  @return integer value on success, INT_MAX on error
 */
//--------------------------------------------------------------------------
int
CicoHSCommand::getIntValue(const ptree & root, const char* key)
{
    int intValue = -1;
    try {
        intValue = root.get<int>(key);
    }
    catch (std::exception const& e)
    {
        //ICO_WRN("catch exception %s", e.what());
        return intValue;
    }
    return intValue;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get string value by key
 *
 *  @param [in] root    root object of json object
 *  @param [in] key     Key to get the value
 *
 *  @return string value on success, empty string on error
 */
//--------------------------------------------------------------------------
string
CicoHSCommand::getStrValue(const ptree & root, const char* key)
{
    string strValue = "";
    try {
        strValue = root.get<string>(key);
    }
    catch (std::exception const& e)
    {
        //ICO_WRN("catch exception %s", e.what());
        return strValue;
    }

    if (strValue == "null") {
        strValue = "";
    }

    return strValue;
}

//--------------------------------------------------------------------------
/**
 *  @brief  query whether the object exists
 *
 *  @param [in] root    root object of json object
 *  @param [in] key     Key to get the value
 *
 *  @return true on exist, false on not exist
 */
//--------------------------------------------------------------------------
bool
CicoHSCommand::isExistObject(const ptree & root, const char *key)
{
    try {
        (void)root.get_child(key);
    }
    catch (std::exception const& e)
    {
        //ICO_WRN("catch exception %s", e.what());
        return false;
    }

    return true;
}

bool
CicoHSCommand::getArgArray(const ptree& pt, std::vector<CicoHSCommandArg>& vArg)
{

    BOOST_FOREACH (const ptree::value_type& child, pt.get_child("arg")) {
        bool chk = false;
        const ptree& info = child.second;
        string Pappid;
        string Pzone;
        const char* sAppid = NULL;
        const char* sZone = NULL;
        int iPid = -1;
        int iVisible = -1;
        try {
            // arg.appid
            if (boost::optional<std::string> appid = info.get_optional<std::string>("appid")) {
                Pappid = appid.get();
                sAppid = Pappid.c_str();
                ICO_DBG("appid : %s ", sAppid);
                chk = true;
            }
            // arg.pid
            if (boost::optional<int> pid = info.get_optional<int>("pid")) {
                iPid=pid.get();
                ICO_DBG("pid : %d ", iPid);
                chk = true;
            }
            // arg.zone
            if (boost::optional<std::string> zone = info.get_optional<std::string>("zone")) {
                Pzone = zone.get();
                sZone = Pzone.c_str();
                ICO_DBG("zone : %s ", sZone);
                chk = true;
            }
            // arg.visible
            if (boost::optional<int> vis = info.get_optional<int>("visible")) {
                iVisible = vis.get();
                ICO_DBG("visible : %d ", iVisible);
                chk = true;
            }
        }
        catch (std::exception const& e)
        {
            ICO_WRN("catch exception %s", e.what());
            return false;
        }

        if (true == chk) {
            vArg.push_back(CicoHSCommandArg(sAppid, sZone, iVisible, iPid));
        }
    }
    if (0 == vArg.size()){
        return false;
    }
    return true;
}

bool
CicoHSCommand::getArg(const ptree& pt, std::vector<CicoHSCommandArg>& vArg)
{
    string Pappid;
    string Pzone;
    const char* sAppid = NULL;
    const char* sZone = NULL;
    int iPid = -1;
    int iVisible = -1;

    try {
        // Data.str
        if (boost::optional<std::string> str1 = pt.get_optional<std::string>("arg.appid")) {
            Pappid = str1.get();
            sAppid = Pappid.c_str();
            ICO_DBG("appid : %s ", sAppid);
        }
        if (boost::optional<std::string> str2 = pt.get_optional<std::string>("arg.zone")) {
            Pzone = str2.get();
            sZone = Pzone.c_str();
            ICO_DBG("zone : %s ", sZone);
        }

        if (boost::optional<int> pid = pt.get_optional<int>("arg.pid")) {
            iPid = pid.get();
            ICO_DBG("pid : %d ", iPid);
        }
        if (boost::optional<int> visible = pt.get_optional<int>("arg.visible")) {
            iVisible = visible.get();
            ICO_DBG("visible : %d ", iVisible);
        }
    }
    catch (std::exception const& e)
    {
        ICO_WRN("catch exception %s", e.what());
        return false;
    }

//    if (((NULL != sAppid) || (0 != iPid)) && ((NULL != sZone) || (-1 != iVisible))) {
    if (((NULL != sAppid) || (-1 != iPid))) {
        vArg.push_back(CicoHSCommandArg(sAppid, sZone, iVisible, iPid));
        return true;
    }
    return false;
}

void
CicoHSCommand::parseAppCtlOpt(const ptree & root)
{
    CicoHSCmdAppCtlOpt* options = new CicoHSCmdAppCtlOpt();
    this->opt = options;

    switch (this->cmdid) {
    case MSG_CMD_APP_START:
    case MSG_CMD_APP_STOP:
        ICO_DBG("app_start stop : %d ", this->cmdid);
        getArg(root, options->arg);
        break;
    case MSG_CMD_WIN_CHANGE:
        ICO_DBG("win_chg : %d ", this->cmdid);
        if (false == getArg(root, options->arg)) {
            getArgArray(root, options->arg);
        }
        break;
    default:
        break;
    }
}

// vim:set expandtab ts=4 sw=4:
