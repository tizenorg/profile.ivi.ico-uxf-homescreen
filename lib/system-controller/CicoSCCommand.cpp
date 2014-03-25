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
 *  @file   CicoSCCommand.cpp
 *
 *  @brief  This file is implementation of CicoSCCommand class
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

#include "CicoSCCommand.h"
#include "CicoLog.h"
#include "ico_syc_error.h"
#include "ico_syc_msg_cmd_def.h"
#include "ico_syc_mrp_resource_private.h"

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSCCommand::CicoSCCommand()
    : cmdid(0), appid(""), pid(-1), opt(NULL)
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSCCommand::~CicoSCCommand()
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
CicoSCCommand::parseMessage(const char *message)
{
//    ICO_TRA("CicoSCCommand::parseMessage Enter(%s)", message);
    try {
        stringstream jsonString(message);
        ptree root;
        read_json(jsonString, root);

        this->appid = getStrValue(root, MSG_PRMKEY_APPID);
        this->cmdid = getIntValue(root, MSG_PRMKEY_CMD);
        this->pid   = getIntValue(root, MSG_PRMKEY_PID);

        switch ((this->cmdid & MSG_CMD_TYPE_MASK)) {
        case MSG_CMD_TYPE_WINCTRL:
            parseWinCtrlOpt(root);
            break;
        case MSG_CMD_TYPE_INPUTCTRL:
            parseInputDevCtrlOpt(root);
            break;
        case MSG_CMD_TYPE_USERMGR:
            parseUserMgrOpt(root);
            break;
        case MSG_CMD_TYPE_RESOURCEMGR:
            parseResCtrlOpt(root);
            break;
        case MSG_CMD_TYPE_INPUTDEVSETTING:
            parseInputDevSettingOpt(root);
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
//        ICO_TRA("CicoSCCommand::parseMessage Leave(EINVAL)");
        return ICO_SYC_EINVAL;
    }

//    ICO_TRA("CicoSCCommand::parseMessage Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  dump log this class member variables
 */
//--------------------------------------------------------------------------
void
CicoSCCommand::dump(void)
{
    ICO_INF("Command: cmdid=0x%08X appid=%s pid=%d", cmdid, appid.c_str(), pid);
    if (NULL != this->opt) {
        this->opt->dump();
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
CicoSCCommand::getIntValue(const ptree & root, const char* key)
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
CicoSCCommand::getStrValue(const ptree & root, const char* key)
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
CicoSCCommand::isExistObject(const ptree & root, const char *key)
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

//--------------------------------------------------------------------------
/**
 *  @brief  parse from message, create window control options
 *
 *  @param [in] root    root object of json object
 *
 *  @note
 *  message format of window control(json format)
 *  <pre>
 *  {
 *    "arg": {
 *      "surface":   (int)    surface id
 *      "layer":     (int)    layer id
 *      "node":      (int)    node id
 *      "zone":      (string) display zone name
 *      "anim_name": (string) animation name
 *      "anim_time": (int)    animation time
 *      "pos_x":     (int)    window x position
 *      "pos_y":     (int)    window y position
 *      "width":     (int)    window width
 *      "height":    (int)    window height
 *      "raise":     (int)    raise flag
 *      "visible":   (int)    visible flag
 *      "active":    (int)    active flag
 *      "stride":    (int)    stride
 *      "format":    (int)    format
 *      "framerate": (int)    frame rate
 *    }
 *  }
 *  </pre>
 */
//--------------------------------------------------------------------------
void
CicoSCCommand::parseWinCtrlOpt(const ptree & root)
{
    CicoSCCmdWinCtrlOpt* options = new CicoSCCmdWinCtrlOpt();
    this->opt = options;

    options->surfaceid     = getIntValue(root, "arg.surface");
    options->layerid       = getIntValue(root, "arg.layer");
    options->nodeid        = getIntValue(root, "arg.node");
    options->zone          = getStrValue(root, "arg.zone");
    options->animation     = getStrValue(root, "arg.anim_name");
    options->animationTime = getIntValue(root, "arg.anim_time");
    if (-1 == options->animationTime) {
        options->animationTime = 0;
    }
    options->x             = getIntValue(root, "arg.pos_x");
    options->y             = getIntValue(root, "arg.pos_y");
    options->width         = getIntValue(root, "arg.width");
    options->height        = getIntValue(root, "arg.height");
    options->raise         = getIntValue(root, "arg.raise");
    options->visible       = getIntValue(root, "arg.visible");
    options->active        = getIntValue(root, "arg.active");
    options->stride        = getIntValue(root, "arg.stride");
    options->format        = getIntValue(root, "arg.format");
    options->framerate     = getIntValue(root, "arg.framerate");
}

//--------------------------------------------------------------------------
/**
 *  @brief  parse from message, create input device control options
 *
 *  @param [in] root    root object of json object
 *
 *  @note
 *  message format of input device control(json format)
 *  <pre>
 *  {
 *    "arg": {
 *      "device":     (string) device name
 *      "input_num":  (int) input number
 *      "surface":    (int) surface id
 *      "deviceno":   (int) device number
 *      "alloc_type": (int) allocation type
 *      "keycode":    (int) key code
 *      "ev_type":    (int) event type
 *      "ev_time":    (int) event time
 *      "ev_code":    (int) event cord
 *      "ev_value":   (int) event value
 *    }
 *  }
 *  </pre>
 */
//--------------------------------------------------------------------------
void
CicoSCCommand::parseInputDevCtrlOpt(const ptree & root)
{
    CicoSCCmdInputDevCtrlOpt* options = new CicoSCCmdInputDevCtrlOpt();
    this->opt = options;

    options->device    = getStrValue(root, "arg.device");
    options->input     = getIntValue(root, "arg.input_num");
    options->surfaceid = getIntValue(root, "arg.surface");
    options->deviceno  = getIntValue(root, "arg.deviceno");
    options->fix       = getIntValue(root, "arg.alloc_type");
    options->keycode   = getIntValue(root, "arg.keycode");
    options->evtype    = getIntValue(root, "arg.ev_type");
    options->evtime    = getIntValue(root, "arg.ev_time");
    options->evcode    = getIntValue(root, "arg.ev_code");
    options->evvalue   = getIntValue(root, "arg.ev_value");
}

//--------------------------------------------------------------------------
/**
 *  @brief  parse from message, create user manager options
 *
 *  @param [in] root    root object of json object
 *
 *  @note
 *  message format of user manager options(json format)
 *  <pre>
 *  {
 *    "arg": {
 *      "user":     (string) user name
 *      "password": (string) user password
 *      "lastinof": (string) information of application last state
 *    }
 *  }
 *  </pre>
 */
//--------------------------------------------------------------------------
void
CicoSCCommand::parseUserMgrOpt(const ptree & root)
{
    CicoSCCmdUserMgrOpt* options = new CicoSCCmdUserMgrOpt();
    this->opt = options;

    options->user     = getStrValue(root, "arg.user");
    options->pass     = getStrValue(root, "arg.password");
    options->lastinfo = getStrValue(root, "arg.lastinfo");
}

//--------------------------------------------------------------------------
/**
 *  @brief  parse from message, create resource manage options
 *
 *  @param [in] root    root object of json object
 *
 *  @note
 *  message format of resource manage options(json format)
 *  <pre>
 *  {
 *    "res": {
 *      "window": {
 *        "zone":    (string) window zone,
 *        "name":    (string) window's surface name,
 *        "id":      (string) window id
 *        "ECU":
 *        "display":
 *        "layer":
 *        "layout":
 *        "area":
 *        "dispatchApp":
 *        "role":
 *        "resourceId":
 *      },
 *      "sound": {
 *        "zone":    (string) sound zone,
 *        "name":    (string) sound's stream name,
 *        "id":      (string) sound id,
 *        "adjust":  (int)    sound adjust type
 *      },
 *      "input": {
 *        "name":    (string) input device name,
 *        "event":   (int)    input event id
 *      }
 *      "type":      (int)    basic or interruption
 *    }
 *  }
 *  </pre>
 */
//--------------------------------------------------------------------------
void
CicoSCCommand::parseResCtrlOpt(const ptree & root)
{
    CicoSCCmdResCtrlOpt* options = new CicoSCCmdResCtrlOpt();
    this->opt = options;

    if (true == isExistObject(root, "res.window")) {
        options->dispres   = true;
        options->dispzone  = getStrValue(root, "res.window.zone");
        options->winname   = getStrValue(root, "res.window.name");
        options->surfaceid = getIntValue(root, "res.window.id");
        bool bEx = true;
        options->ECU       = getStrValue(root, "res.window.ECU");
        if (true == options->ECU.empty()) {
            bEx = false;
        }
        options->display   = getStrValue(root, "res.window.display");
        if (true == options->display.empty()) {
            bEx = false;
        }
        options->layer     = getStrValue(root, "res.window.layer");
        if (true == options->layer.empty()) {
            bEx = false;
        }
        options->layout    = getStrValue(root, "res.window.layout");
        if (true == options->layout.empty()) {
            bEx = false;
        }
        options->area      = getStrValue(root, "res.window.area");
        if (true == options->area.empty()) {
            bEx = false;
        }
        options->dispatchApp= getStrValue(root, "res.window.dispatchApp");
        options->role      = getStrValue(root, "res.window.role");
        options->resourceID= getIntValue(root, "res.window.resourceId");
        options->dispresEx = bEx;
    }

    if (true == isExistObject(root, "res.sound")) {
        options->soundres  = true;
        options->soundzone = getStrValue(root, "res.sound.zone");
        options->soundname = getStrValue(root, "res.sound.name");
        options->soundid   = getIntValue(root, "res.sound.id");
        options->adjust    = getIntValue(root, "res.sound.adjust");
    }

    if (true == isExistObject(root, "res.input")) {
        options->inputres = true;
        options->device   = getStrValue(root, "res.input.name");
        options->input    = getIntValue(root, "res.input.event");
    }

    options->type = getIntValue(root, "res.type");
}

//--------------------------------------------------------------------------
/**
 *  @brief  parse from message, create input device setting options
 *
 *  @param [in] root    root object of json object
 *
 *  @note
 *  message format of input device setting(json format)
 *  <pre>
 *  {
 *    "region": {
 *      "winname":      (string) window name,
 *      "pos_x":        (int) position x,
 *      "pos_y":        (int) position y,
 *      "width":        (int) width,
 *      "height":       (int) height,
 *      "hotspot_x":    (int) cursor hotspot x,
 *      "hotspot_y":    (int) cursor hotspot y,
 *      "cursor_x":     (int) cursor position x,
 *      "cursor_y":     (int) cursor position y,
 *      "cursor_width": (int) cursor width,
 *      "cursor_height":(int) cursor height,
 *      "attr":         (int) attribute
 *    }
 *  }
 *  </pre>
 */
//--------------------------------------------------------------------------
void
CicoSCCommand::parseInputDevSettingOpt(const ptree & root)
{
    CicoSCCmdInputDevSettingOpt* options = new CicoSCCmdInputDevSettingOpt();
    this->opt = options;

    if (true == isExistObject(root, "region")) {
        options->winname    = getStrValue(root, "region.winname");
        options->x          = getIntValue(root, "region.pos_x");
        options->y          = getIntValue(root, "region.pos_y");
        options->width      = getIntValue(root, "region.width");
        options->height     = getIntValue(root, "region.height");
        options->hotspot_x  = getIntValue(root, "region.hotspot_x");
        options->hotspot_y  = getIntValue(root, "region.hotspot_y");
        options->cursor_x   = getIntValue(root, "region.cursor_x");
        options->cursor_y   = getIntValue(root, "region.cursor_y");
        options->cursor_width  = getIntValue(root, "region.cursor_width");
        options->cursor_height = getIntValue(root, "region.cursor_height");
        options->attr       = getIntValue(root, "region.attr");
    }
}
// vim:set expandtab ts=4 sw=4:
