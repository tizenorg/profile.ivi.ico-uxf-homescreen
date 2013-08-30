/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

/**
 * @brief   Application Framework debug log function
 *
 * @date    Feb-28-2013
 */

#include <iostream>
#include <sstream>
#include <exception>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
using namespace boost;
using namespace boost::property_tree;

#include "CicoSCCommandParser.h"
#include "CicoLog.h"
#include "ico_syc_error.h"
#include "ico_syc_msg_cmd_def.h"

CicoSCCommandParser::CicoSCCommandParser()
{
}

CicoSCCommandParser::~CicoSCCommandParser()
{
}

int
CicoSCCommandParser::parse(const char *cmdMessage, CicoSCCommand & cmd)
{
    ICO_DBG("CicoSCCommandParser::parse : Enter(%s)", cmdMessage);
    try {
        stringstream jsonString(cmdMessage);
        ptree root;
        read_json(jsonString, root);

        cmd.appid = getStrValue(root, "appid");
        cmd.cmdid = getIntValue(root, "command");
        cmd.pid   = getIntValue(root, "pid");

        switch ((cmd.cmdid & MSG_CMD_TYPE_MASK)) {
        case MSG_CMD_TYPE_WINCTRL:
            parseWinCtrlOpt(root, cmd);
            break;
        case MSG_CMD_TYPE_INPUTCTRL:
            parseInputDevCtrlOpt(root, cmd);
            break;
        case MSG_CMD_TYPE_USERMGR:
            parseUserMgrOpt(root, cmd);
            break;
        case MSG_CMD_TYPE_RESOURCEMGR:
            parseResCtrlOpt(root, cmd);
            break;
        case MSG_CMD_TYPE_INPUTDEVSETTING:
            parseInputDevSettingOpt(root, cmd);
            break;
        default:
            break;
        }
    }
    catch (std::exception const& e)
    {
        ICO_WRN("catch exception %s", e.what());
        return ICO_SYC_EINVAL;
    }

    ICO_DBG("CicoSCCommandParser::parse : Leave");
    return ICO_SYC_EOK;
}

int
CicoSCCommandParser::getIntValue(const ptree & root, const char* key)
{
    int intValue = -1;
    try {
        intValue = root.get<int>(key);
    }
    catch (std::exception const& e)
    {
        ICO_WRN("catch exception %s", e.what());
        return intValue;
    }
    return intValue;
}

string
CicoSCCommandParser::getStrValue(const ptree & root, const char* key)
{
    string strValue = "";
    try {
        strValue = root.get<string>(key);
    }
    catch (std::exception const& e)
    {
        ICO_WRN("catch exception %s", e.what());
        return strValue;
    }

    if (strValue == "null") {
        strValue = "";
    }

    return strValue;
}

bool
CicoSCCommandParser::isExistObject(const ptree & root, const char *key)
{
    string strValue = "";
    try {
        (void)root.get_child(key);
    }
    catch (std::exception const& e)
    {
        ICO_WRN("catch exception %s", e.what());
        return false;
    }
    return true;

}

void
CicoSCCommandParser::parseWinCtrlOpt(const ptree & root,
                                     CicoSCCommand & cmd)
{
    ICO_DBG("CicoSCCommandParser::parseWinCtrlOpt: Enter");

    CicoSCCmdWinCtrlOpt* opt = new CicoSCCmdWinCtrlOpt();
    cmd.opt = opt;

    opt->surfaceid     = getIntValue(root, "arg.surface");
    opt->layerid       = getIntValue(root, "arg.layer");
    opt->nodeid        = getIntValue(root, "arg.node");
    opt->zone          = getStrValue(root, "arg.zone");
    opt->animation     = getStrValue(root, "arg.anim_name");
    opt->animationTime = getIntValue(root, "arg.anim_time");
    opt->layerAttr     = getIntValue(root, "arg.attr");
    opt->x             = getIntValue(root, "arg.pos_x");
    opt->y             = getIntValue(root, "arg.pos_y");
    opt->width         = getIntValue(root, "arg.width");
    opt->height        = getIntValue(root, "arg.height");
    opt->raise         = getIntValue(root, "arg.raise");
    opt->visible       = getIntValue(root, "arg.visible");
    opt->active        = getIntValue(root, "arg.active");
    opt->stride        = getIntValue(root, "arg.stride");
    opt->format        = getIntValue(root, "arg.format");
    opt->framerate     = getIntValue(root, "arg.framerate");

    opt->dumpOpt();
    ICO_DBG("CicoSCCommandParser::parseWinCtrlOpt: Leave");
}

void
CicoSCCommandParser::parseInputDevCtrlOpt(const ptree & root,
                                          CicoSCCommand & cmd)
{
    ICO_DBG("CicoSCCommandParser::parseInputDevCtrlOpt: Enter");

    CicoSCCmdInputDevCtrlOpt* opt = new CicoSCCmdInputDevCtrlOpt();
    cmd.opt = opt;

    opt->device     = getStrValue(root, "arg.device");
    opt->inputNum   = getIntValue(root, "arg.input_num");
    opt->surfaceid  = getIntValue(root, "arg.surface");
    opt->deviceno   = getIntValue(root, "arg.deviceno");
    opt->fix        = getIntValue(root, "arg.alloc_type");
    opt->keyCode    = getIntValue(root, "arg.keycode");
    opt->eventType  = getIntValue(root, "arg.ev_type");
    opt->eventCode  = getIntValue(root, "arg.ev_code");
    opt->eventValue = getIntValue(root, "arg.ev_value");

    ICO_DBG("CicoSCCommandParser::parseInputDevCtrlOpt: Leave");
}

void
CicoSCCommandParser::parseUserMgrOpt(const ptree & root,
                                      CicoSCCommand & cmd)
{
    ICO_DBG("CicoSCCommandParser::parseUserMgrOpt: Enter");

    CicoSCCmdUserMgrOpt* opt = new CicoSCCmdUserMgrOpt();
    cmd.opt = opt;

    opt->user = getStrValue(root, "arg.user");
    opt->pass = getStrValue(root, "arg.password");

    ICO_DBG("CicoSCCommandParser::parseUserMgrOpt: Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief   TODO
 *
 *  @pre
 *{
 *  "appid":   (string) appid,
 *  "command": (int) command id,
 *  "res": {
 *    "window": {
 *      "zone":(string) window zone,
 *      "name":(string) window's surface name,
 *      "id":(string) window id
 *    },
 *    "sound": {
 *      "zone":(string) sound zone,
 *      "name":(string) sound's stream name,
 *      "id":(string) sound id,
 *      "adjust":(int) sound adjust type
 *    },
 *    "input": {
 *      "name":(string) input device name,
 *      "event":(int) input event id
 *    },
 *    "type":(int) basic or interruption
 *  },
 *  "region": {
 *    "surface": (int) surface id,
 *    "pos_x":   (int) position x,
 *    "pos_y":   (int) position y,
 *    "width":   (int) width,
 *    "height":  (int) height,
 *    "attr":    (int) attribute
 *  }
 *}
 */
//--------------------------------------------------------------------------
void
CicoSCCommandParser::parseResCtrlOpt(const ptree & root,
                                     CicoSCCommand & cmd)
{
    ICO_DBG("CicoSCCommandParser::parseResCtrlOpt: Enter");

    CicoSCCmdResCtrlOpt* opt = new CicoSCCmdResCtrlOpt();
    cmd.opt = opt;

    if (true == isExistObject(root, "res.window")) {
        opt->displayres  = true;
        opt->displayZone = getStrValue(root, "res.window.zone");
        opt->windowName  = getStrValue(root, "res.window.name");
        opt->surfaceid   = getIntValue(root, "res.window.id");
    }
        
    if (true == isExistObject(root, "res.sound")) {
        opt->soundres  = true;
        opt->soundZone = getStrValue(root, "res.sound.zone");
        opt->soundName = getStrValue(root, "res.sound.name");
        opt->soundid   = getIntValue(root, "res.sound.id");
        opt->adjust    = getIntValue(root, "res.sound.adjust");
    }

    if (true == isExistObject(root, "res.input")) {
        opt->inputres = true;
        opt->device  = getStrValue(root, "res.input.name");
        opt->input   = getIntValue(root, "res.input.event");
    }

    opt->type = getIntValue(root, "type");

    ICO_DBG("CicoSCCommandParser::parseResCtrlOpt: Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief   TODO
 *
 *  @pre
 *{
 *  "appid":   (string) appid,
 *  "command": (int) command id,
 *  "region": {
 *    "surface": (int) surface id,
 *    "pos_x":   (int) position x,
 *    "pos_y":   (int) position y,
 *    "width":   (int) width,
 *    "height":  (int) height,
 *    "attr":    (int) attribute
 *  }
 *}
 */
//--------------------------------------------------------------------------
void
CicoSCCommandParser::parseInputDevSettingOpt(const ptree & root,
                                             CicoSCCommand & cmd)
{
    ICO_DBG("CicoSCCommandParser::parseResCtrlOpt: Enter");

    CicoSCCmdInputDevSettingOpt* opt = new CicoSCCmdInputDevSettingOpt();
    cmd.opt = opt;

    if (true == isExistObject(root, "region")) {
        opt->surfaceid  = getIntValue(root, "region.surface");
        opt->x          = getIntValue(root, "region.pos_x");
        opt->y          = getIntValue(root, "region.pos_y");
        opt->width      = getIntValue(root, "region.width");
        opt->height     = getIntValue(root, "region.height");
        opt->attr       = getIntValue(root, "region.attr");
    }

    ICO_DBG("CicoSCCommandParser::parseResCtrlOpt: Leave");
}
// vim:set expandtab ts=4 sw=4:
