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
 *  @file   CicoSCInputController.cpp
 *
 *  @brief  This file is implementation of CicoSCInputController class
 */
//==========================================================================

#include <string>
#include <vector>
using namespace std;

#include "CicoSCInputController.h"
#include "CicoSCWayland.h"
#include "CicoLog.h"
#include "CicoSCSwitch.h"
#include "CicoSCInputDev.h"
#include "ico_syc_error.h"
#include "ico_syc_msg_cmd_def.h"
#include "ico_syc_private.h"
#include "CicoSystemConfig.h"
#include "CicoConf.h"
#include "CicoSCCommand.h"
#include "CicoSCServer.h"
#include "CicoSCMessage.h"

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSCInputController::CicoSCInputController()
{
    CicoSCWayland* wayland = CicoSCWayland::getInstance();
    wayland->addWaylandIF(ICO_WL_INPUT_MGR_CTRL_IF, this);
    wayland->addWaylandIF(ICO_WL_EXINPUT_IF, this);
    wayland->addWaylandIF(ICO_WL_INPUT_MGR_DEV_IF, this);

}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSCInputController::~CicoSCInputController()
{
}

#if 0 //TODO
int
initDB(void)
{
    const vector<CicoSCInputDevConf*>& inputDevConfList =
        CicoSystemConfig::getInstance()->getInputDevConfList();
    vector<CicoSCInputDevConf*>::const_iterator itr;
    itr = inputDevConfList.begin();
    for (; itr != inputDevConfList.end(); ++itr) {
        const CicoSCInputDevConf *iconf = const_cast<CicoSCInputDevConf*>(*itr);
        CicoSCInputDev *device = new CicoSCInputDev();

        device->devcie = iconf->name;

        vector<CicoSCSwitchConf*>::const_iterator itr2;
        itr2 = iconf->switchConfList.begin();
        for (; itr2 != dconf->switchConfList.end(); ++itr2) {
            const CicoSCSwitchConf *sconf = const_cast<CicoSCSwitchConf*>(*itr2);

            CicoSCSwitch *sw = new CicoSCSwitch();
            sw->swname = sconf->name;
            sw->input  = sconf->id;
            devcie->switchList.push_back(sw);
        }




    }

}
#endif

//--------------------------------------------------------------------------
/**
 *  @brief executes an input control process corresponding to the command
 *
 *  @param  cmd     command
 */
//--------------------------------------------------------------------------
void
CicoSCInputController::handleCommand(const CicoSCCommand *cmd)
{
    ICO_TRA("CicoSCInputController::handleCommand Enter"
            "(cmdid=0x%08x)", cmd->cmdid);

    CicoSCCmdInputDevCtrlOpt *opt;
    CicoSCCmdInputDevSettingOpt *set_opt;

    switch (cmd->cmdid) {
    case MSG_CMD_ADD_INPUT:
        opt = static_cast<CicoSCCmdInputDevCtrlOpt*>(cmd->opt);
        addInputApp(cmd->appid, opt->device, opt->input, opt->fix,
                    opt->keycode);
        break;
    case MSG_CMD_DEL_INPUT:
        opt = static_cast<CicoSCCmdInputDevCtrlOpt*>(cmd->opt);
        delInputApp(cmd->appid, opt->device, opt->input);
        break;
    case MSG_CMD_SEND_KEY_EVENT:
        opt = static_cast<CicoSCCmdInputDevCtrlOpt*>(cmd->opt);
        sendKeyEvent(cmd->appid, opt->evcode, opt->evvalue);
    case MSG_CMD_SEND_POINTER_EVENT:
        opt = static_cast<CicoSCCmdInputDevCtrlOpt*>(cmd->opt);
        sendPointerEvent(cmd->appid, opt->evcode, opt->evvalue);
        break;
    case MSG_CMD_SET_REGION:
        set_opt = static_cast<CicoSCCmdInputDevSettingOpt*>(cmd->opt);
        setInputRegion(cmd->appid, set_opt->winname, set_opt->x, set_opt->y,
                       set_opt->width, set_opt->height,
                       set_opt->hotspot_x, set_opt->hotspot_y,
                       set_opt->cursor_x, set_opt->cursor_y,
                       set_opt->cursor_width, set_opt->cursor_height, set_opt->attr);
        break;
    case MSG_CMD_UNSET_REGION:
        set_opt = static_cast<CicoSCCmdInputDevSettingOpt*>(cmd->opt);
        unsetInputRegion(cmd->appid, set_opt->winname, set_opt->x, set_opt->y,
                         set_opt->width, set_opt->height);
        break;
    default:
        ICO_WRN("Unknown Command(0x%08x)", cmd->cmdid);
        break;
    }

    ICO_TRA("CicoSCInputController::handleCommand Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  register input device control application
 *
 *  @param [in] appid   application id
 *  @param [in] device  input device name
 *  @param [in] input   input number
 *  @parma [in] fix     fixed assign flag
 *  @parma [in] keycode assigned keycode value
 */
//--------------------------------------------------------------------------
int
CicoSCInputController::addInputApp(const string &appid,
                                   const string &device,
                                   int          input,
                                   int          fix,
                                   int          keycode)
{
    ICO_TRA("CicoSCInputController::addInputApp Enter"
            "(appid=%s device=%s input=%d fix=%d keycode=%d)",
            appid.c_str(), device.c_str(), input, fix, keycode);

    CicoSCWlInputMgrIF::addInputApp(appid.c_str(), device.c_str(),
                                    input, fix, keycode);

    ICO_TRA("CicoSCInputController::addInputApp Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  unregister input device control application
 *
 *  @param [in] appid   application id
 *  @param [in] device  input device name
 *  @param [in] input   input number
 */
//--------------------------------------------------------------------------
int
CicoSCInputController::delInputApp(const string &appid,
                                   const string &device,
                                   int          input)
{
    ICO_TRA("CicoSCInputController::delInputApp Enter"
            "(appid=%s device=%s input=%d)",
            appid.c_str(), device.c_str(), input);

    CicoSCWlInputMgrIF::delInputApp(appid.c_str(), device.c_str(), input);

    ICO_TRA("CicoSCInputController::delInputApp Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  send keyboard input device event
 *
 *  @param [in] winname   windowname([name][@appid])
 *  @param [in] code      event code
 *  @param [in] value     event value
 */
//--------------------------------------------------------------------------
int
CicoSCInputController::sendKeyEvent(const string &winname,
                                    int          code,
                                    int          value)
{
    ICO_TRA("CicoSCInputController::sendKeyEvent Enter(winname=<%s> code=%d value=%d)",
            winname.c_str(), code, value);

    CicoSCWlInputMgrIF::sendKeyEvent(winname.c_str(), code, value);

    ICO_TRA("CicoSCInputController::sendKeyEvent Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  send pointer input device event
 *
 *  @param [in] winname   windowname([name][@appid])
 *  @param [in] code      event code
 *  @param [in] value     event value
 */
//--------------------------------------------------------------------------
int
CicoSCInputController::sendPointerEvent(const string &winname,
                                        int          code,
                                        int          value)
{
    ICO_TRA("CicoSCInputController::sendPointerEvent Enter(winname=<%s> code=%d value=%d)",
            winname.c_str(), code, value);

    CicoSCWlInputMgrIF::sendPointerEvent(winname.c_str(), code, value);

    ICO_TRA("CicoSCInputController::sendPointerEvent Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  set input region information
 *
 *  @param [in] appid     application id
 *  @param [in] winname   window name
 *  @param [in] x         region x positon
 *  @param [in] y         region y positon
 *  @param [in] width     region width
 *  @param [in] height    region height
 *  @param [in] hotspot_x hotspot x position
 *  @param [in] hotspot_y hotspot y position
 *  @param [in] cursor_x  cursor x position
 *  @param [in] cursor_y  cursor y position
 *  @param [in] cursor_width  cursor width
 *  @param [in] cursor_height cursor height
 *  @param [in] attr      region attribute
 */
//--------------------------------------------------------------------------
int
CicoSCInputController::setInputRegion(const string &appid,
                                      const string &winname,
                                      int x,
                                      int y,
                                      int width,
                                      int height,
                                      int hotspot_x,
                                      int hotspot_y,
                                      int cursor_x,
                                      int cursor_y,
                                      int cursor_width,
                                      int cursor_height,
                                      int attr)
{
    char    target[ICO_SYC_MAX_LEN];

    snprintf(target, ICO_SYC_MAX_LEN-1, "%s@%s", winname.c_str(), appid.c_str());

    ICO_TRA("CicoSCInputController::setInputRegion Enter"
            "(target=%s x=%d y=%d width=%d height=%d "
            "hotspot=%d/%d cursor=%d/%d-%d/%d attr=%d)",
            target, x, y, width, height, hotspot_x, hotspot_y,
            cursor_x, cursor_y, cursor_width, cursor_height, attr);

    CicoSCWlInputMgrIF::setInputRegion(target, x, y, width, height,
                                       hotspot_x, hotspot_y, cursor_x, cursor_y,
                                       cursor_width, cursor_height, attr);

    ICO_TRA("CicoSCInputController::setInputRegion Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  unset input region information
 *
 *  @param [in] appid     application id
 *  @param [in] winname   window name
 *  @param [in] x         region x positon
 *  @param [in] y         region y positon
 *  @param [in] width     region width
 *  @param [in] height    region height
 */
//--------------------------------------------------------------------------
int
CicoSCInputController::unsetInputRegion(const string &appid,
                                        const string &winname,
                                        int x,
                                        int y,
                                        int width,
                                        int height)
{
    char    target[ICO_SYC_MAX_LEN];

    snprintf(target, ICO_SYC_MAX_LEN-1, "%s@%s", winname.c_str(), appid.c_str());

    ICO_TRA("CicoSCInputController::unsetInputRegion Enter"
            "(target=%s x=%d y=%d width=%d height=%d", target, x, y, width, height);

    CicoSCWlInputMgrIF::unsetInputRegion(target, x, y, width, height);

    ICO_TRA("CicoSCInputController::unsetInputRegion Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback to application for input switch information
 *
 *  @param [in] data        user data
 *  @param [in] ico_exinput wayland ico_exinput interface
 *  @param [in] device      input device name
 *  @param [in] type        input device type (as enum type)
 *  @param [in] swname      input switch name
 *  @param [in] input       input switch number
 *  @param [in] codename    input code name
 *  @param [in] code        input code number
 */
//--------------------------------------------------------------------------
void
CicoSCInputController::capabilitiesCB(void               *data,
                                      struct ico_exinput *ico_exinput,
                                      const char         *device,
                                      int32_t            type,
                                      const char         *swname,
                                      int32_t            input,
                                      const char         *codename,
                                      int32_t            code)
{
    ICO_TRA("CicoSCInputController::capabilitiesCB Enter",
            "device=%s type=%s swname=%s input=%s codename=%s code=%d",
            device, type, swname, input, codename, code);

    // create CicoSCInputDev object
    CicoSCInputDev *inputdev = findInputDev(device);
    if (NULL == inputdev) {
        inputdev = new CicoSCInputDev();
        inputdev->device = device;
        inputdev->type   = type;
    }

    CicoSCSwitch *sw = findInputSwitch(device, input);
    if (NULL == sw) {
        sw = new CicoSCSwitch();
        sw->input   = input;
        sw->code.push_back(code);
        sw->codename.push_back(codename);
    }
    else {
        sw->input   = input;
    }

    CicoSystemConfig *sysConf = CicoSystemConfig::getInstance();
    const CicoSCSwitchConf *swconf = NULL;
    swconf = sysConf->findSwitchConfbyName(device, swname);
    if (NULL != swconf) {
        if (false == swconf->appid.empty()) {
            addInputApp(swconf->appid, inputdev->device,
                        input,CicoSCSwitch::SWITCH_FIX, swconf->keycode);
            sw->fix = true;
        }
    }

    ICO_TRA("CicoSCInputController::capabilitiesCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback to application for input code information
 *
 *  @param [in] data        user data
 *  @param [in] ico_exinput wayland ico_exinput interface
 *  @param [in] device      input device name
 *  @param [in] input       input switch number
 *  @param [in] codename    input code name
 *  @param [in] code        input code number
 */
//--------------------------------------------------------------------------
void
CicoSCInputController::codeCB(void               *data,
                              struct ico_exinput *ico_exinput,
                              const char         *device,
                              int32_t            input,
                              const char         *codename,
                              int32_t            code)
{
    ICO_TRA("CicoSCInputController::codeCB Enter"
            "(device=%s input=%d codename=%s code=%d)",
            device, input, codename, code);

    CicoSCSwitch *sw = findInputSwitch(device, input);
    if (NULL != sw) {
        ICO_ERR("codeCB: device=%s input=%d not found.");
        return;
    }

    if (sw->code.size() >= CicoSCSwitch::SWITCH_CODE_MAX) {
        ICO_ERR("codeCB: device=%s input=%d number of codes overflow.");
        return;
    }

    for (unsigned int i = 0; i < sw->code.size(); ++i) {
        if (sw->code[i] == code) {
            ICO_WRN("codeCB: device=%s input=%d code=%d already exist.");
           return;
        }
    }

    sw->code.push_back(code);
    sw->codename.push_back(codename);

    ICO_TRA("CicoSCInputController::codeCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback to application for switch input
 *
 *  @param [in] data        user data
 *  @param [in] ico_exinput wayland ico_exinput interface
 *  @param [in] time        input time of millisecond
 *  @param [in] device      input device name
 *  @param [in] input       input switch number
 *  @param [in] code        input switch code
 *  @param [in] state       Of/Off status
 */
//--------------------------------------------------------------------------
void
CicoSCInputController::inputCB(void               *data,
                               struct ico_exinput *ico_exinput,
                               uint32_t           time,
                               const char         *device,
                               int32_t            input,
                               int32_t            code,
                               int32_t            state)
{
    ICO_TRA("CicoSCInputController::inputCB Enter"
            "time=%d device=%s input=%d code=%d state=%d",
            time, device, input, code, state);

    // send message
    CicoSCMessage *message = new CicoSCMessage();
    message->addRootObject("command", ICO_SYC_EV_WIN_CREATE);
    message->addArgObject("device", device);
    message->addArgObject("time", time);
    message->addArgObject("input", input);
    message->addArgObject("code", code);
    message->addArgObject("state", state);
    CicoSCServer::getInstance()->sendMessageToHomeScreen(message);

    ICO_TRA("CicoSCInputController::inputCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback to application for change input region
 *
 *  @param [in] data                user data
 *  @param [in] ico_input_mgr_dev   wayland ico_exinput interface
 *  @param [in] region              input regions
 */
//--------------------------------------------------------------------------
void
CicoSCInputController::regionCB(void                        *data,
                                struct ico_input_mgr_device *ico_input_mgr_dev,
                                struct wl_array             *region)
{
    ICO_TRA("CicoSCInputController::regionCB Enter");
    ICO_TRA("CicoSCInputController::regionCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  find input device information
 *
 *  @param [in] device  input device name
 *
 *  @return CicoSCInputDev instance on found, NULL on not found
 */
//--------------------------------------------------------------------------
CicoSCInputDev*
CicoSCInputController::findInputDev(const char *device)
{
    vector<CicoSCInputDev*>::iterator itr;
    itr = m_inputDevList.begin();
    for (; itr != m_inputDevList.end(); ++itr) {
        CicoSCInputDev* inputdev = const_cast<CicoSCInputDev*>(*itr);
        if (inputdev->device == device) {
            return inputdev;
        }
    }
    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief  find input device switch information
 *
 *  @param [in] device  input device name
 *  @param [in] input   input number
 *
 *  @return CicoSCSwitch instance on found, NULL on not found
 */
//--------------------------------------------------------------------------
CicoSCSwitch*
CicoSCInputController::findInputSwitch(const char *device, int input)
{
    const CicoSCInputDev *inputdev = findInputDev(device);
    if (NULL == inputdev) {
        return NULL;
    }

    vector<CicoSCSwitch*>::const_iterator itr;
    itr = inputdev->switchList.begin();
    for (; itr != inputdev->switchList.end(); ++itr) {
        CicoSCSwitch* sw = const_cast<CicoSCSwitch*>(*itr);
        if (sw->input == input) {
            return sw;
        }
    }
    return NULL;
}
// vim:set expandtab ts=4 sw=4:
