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
#include "CicoSCSystemConfig.h"
#include "CicoSCConf.h"
#include "CicoSCCommand.h"

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
        CicoSCSystemConfig::getInstance()->getInputDevConfList();
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
 *  @brief�@executes an input control process corresponding to the command
 *
 *  @param  cmd     command
 */
//--------------------------------------------------------------------------
void
CicoSCInputController::handleCommand(const CicoSCCommand *cmd)
{
    ICO_DBG("CicoSCInputController::handleCommand Enter"
            "(cmdid=0x%08X)", cmd->cmdid);

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
    case MSG_CMD_SEND_INPUT:
        opt = static_cast<CicoSCCmdInputDevCtrlOpt*>(cmd->opt);
        sendInputEvent(cmd->appid, opt->surfaceid, opt->evtype,
                       opt->deviceno, opt->evcode, opt->evvalue);
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
        ICO_WRN("Unknown Command(0x%08X)", cmd->cmdid);
        break;
    }

    ICO_DBG("CicoSCInputController::handleCommand Leave");
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
    ICO_DBG("CicoSCInputController::addInputApp Enter"
            "(appid=%s device=%s input=%d fix=%d keycode=%d)",
            appid.c_str(), device.c_str(), input, fix, keycode);

    CicoSCWlInputMgrIF::addInputApp(appid.c_str(), device.c_str(),
                                    input, fix, keycode);

    ICO_DBG("CicoSCInputController::addInputApp Leave(EOK)");
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
    ICO_DBG("CicoSCInputController::delInputApp Enter"
            "(appid=%s device=%s input=%d)",
            appid.c_str(), device.c_str(), input);

    CicoSCWlInputMgrIF::delInputApp(appid.c_str(), device.c_str(), input);

    ICO_DBG("CicoSCInputController::delInputApp Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  send input device event
 *
 *  @param [in] appid     application id
 *  @param [in] surfaceid surface id
 *  @param [in] type      //TODO
 *  @param [in] deviceno  input device number
 *  @param [in] code      //TODO
 *  @param [in] value     //TODO
 */
//--------------------------------------------------------------------------
int
CicoSCInputController::sendInputEvent(const string &appid,
                                      int          surfaceid,
                                      int          type,
                                      int          deviceno,
                                      int          code,
                                      int          value)
{
    ICO_DBG("CicoSCInputController::sendInputEvent Enter"
            "(appid=%s surfaceid=0x%08X type=%d dev_no=%d code=%d value=%d)",
            appid.c_str(), surfaceid, type, deviceno, code, value);

    CicoSCWlInputMgrIF::sendInputEvent(appid.c_str(), surfaceid,
                                       type, deviceno, code, value);

    ICO_DBG("CicoSCInputController::sendInputEvent Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  set input region informantion
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

    ICO_DBG("CicoSCInputController::setInputRegion Enter"
            "(target=%s x=%d y=%d width=%d height=%d "
            "hotspot=%d/%d cursor=%d/%d-%d/%d attr=%d)",
            target, x, y, width, height, hotspot_x, hotspot_y,
            cursor_x, cursor_y, cursor_width, cursor_height, attr);

    CicoSCWlInputMgrIF::setInputRegion(target, x, y, width, height,
                                       hotspot_x, hotspot_y, cursor_x, cursor_y,
                                       cursor_width, cursor_height, attr);

    ICO_DBG("CicoSCInputController::setInputRegion Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  unset input region informantion
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

    ICO_DBG("CicoSCInputController::unsetInputRegion Enter"
            "(target=%s x=%d y=%d width=%d height=%d", target, x, y, width, height);

    CicoSCWlInputMgrIF::unsetInputRegion(target, x, y, width, height);

    ICO_DBG("CicoSCInputController::unsetInputRegion Leave(EOK)");
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
    ICO_DBG("CicoSCInputController::capabilitiesCB Enter",
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

    CicoSCSystemConfig *sysConf = CicoSCSystemConfig::getInstance();
    const CicoSCSwitchConf *swconf = NULL;
    swconf = sysConf->findSwitchConfbyName(device, swname);
    if (NULL != swconf) {
        if (false == swconf->appid.empty()) {
            addInputApp(swconf->appid, inputdev->device,
                        input,CicoSCSwitch::SWITCH_FIX, swconf->keycode);
            sw->fix = true;
        }
    }

    ICO_DBG("CicoSCInputController::capabilitiesCB Leave");
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
    ICO_DBG("CicoSCInputController::codeCB Enter"
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

    ICO_DBG("CicoSCInputController::codeCB Leave");
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
    ICO_DBG("CicoSCInputController::inputCB Enter"
            "time=%d device=%s input=%d code=%d state=%d",
            time, device, input, code, state);

    // TODO send message
#if 0
    CicoSCMessage message;
    message.addElement("command", ICO_SYC_EV_WIN_CREATE);
    message.addElement("appid", TODO);
    message.addElement("arg.device", device);
    message.addElement("arg.input", input);
    message.addElement("arg.code", code);
    message.addElement("arg.state", state);
    CicoSCServer::getInstance()->sendMessage(message);
#endif

    ICO_DBG("CicoSCInputController::inputCB Leave");
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
    ICO_DBG("CicoSCInputController::regionCB Enter");
    ICO_DBG("CicoSCInputController::regionCB Leave");
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