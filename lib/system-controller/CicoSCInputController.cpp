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
 *  @brief  
 */
//==========================================================================

#include "CicoSCInputController.h"
#include "CicoSCWayland.h"
#include "CicoLog.h"
#include "CicoSCSwitch.h"
#include "CicoSCInputDev.h"
#include "ico_syc_error.h"
#include "ico_syc_msg_cmd_def.h"
#include "CicoSCSystemConfig.h"
#include "CicoSCConf.h"
#include "CicoSCCommandParser.h"

CicoSCInputController::CicoSCInputController()
{
    CicoSCWayland* wayland = CicoSCWayland::getInstance();
    wayland->addWaylandIF(ICO_WL_INPUT_MGR_CTRL_IF, this);
    wayland->addWaylandIF(ICO_WL_EXINPUT_IF, this);
    wayland->addWaylandIF(ICO_WL_INPUT_MGR_DEV_IF, this);

}

CicoSCInputController::~CicoSCInputController()
{
}

#if 0
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

void
CicoSCInputController::handleCommand(const CicoSCCommand *cmd)
{
    ICO_DBG("CicoSCInputController::handleCommand Enter"
            "(cmdid=%08X)", cmd->cmdid);

    CicoSCCmdInputDevCtrlOpt *opt;
    CicoSCCmdInputDevSettingOpt *set_opt; 

    switch (cmd->cmdid) {
    case MSG_CMD_ADD_INPUT:
        opt = static_cast<CicoSCCmdInputDevCtrlOpt*>(cmd->opt);
        addInputApp(cmd->appid, opt->device, opt->inputNum, opt->fix,
                    opt->keyCode);
        break;
    case MSG_CMD_DEL_INPUT:
        opt = static_cast<CicoSCCmdInputDevCtrlOpt*>(cmd->opt);
        delInputApp(cmd->appid, opt->device, opt->inputNum);
        break;
    case MSG_CMD_SEND_INPUT:
        opt = static_cast<CicoSCCmdInputDevCtrlOpt*>(cmd->opt);
        sendInputEvent(cmd->appid, opt->surfaceid, opt->eventType,
                       opt->deviceno, opt->eventCode, opt->eventValue);
        break;
    case MSG_CMD_SET_REGION:
        set_opt = static_cast<CicoSCCmdInputDevSettingOpt*>(cmd->opt);
        setInputRegion(set_opt->surfaceid, set_opt->x, set_opt->y,
                       set_opt->width, set_opt->height, set_opt->attr);
        break;
    case MSG_CMD_UNSET_REGION:
        set_opt = static_cast<CicoSCCmdInputDevSettingOpt*>(cmd->opt);
        resetInputRegion(set_opt->surfaceid, set_opt->x, set_opt->y,
                         set_opt->width, set_opt->height);
        break;
    default:
        ICO_WRN("Unknown Command(0x%08x)", cmd->cmdid);
        break;
    }

    ICO_DBG("CicoSCInputController::handleCommand Leave");
}

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

    ICO_DBG("ico_input_mgr_control_del_input_app(%s,%s,%d,%d,%d) called.",
            appid.c_str(), device.c_str(), input, fix, keycode);
    ico_input_mgr_control_add_input_app(m_inputmgr, appid.c_str(),
                                        device.c_str(), input, fix, keycode);

    ICO_DBG("CicoSCInputController::addInputApp Leave");
    return ICO_SYC_EOK;
}

int
CicoSCInputController::delInputApp(const string &appid,
                                   const string &device,
                                   int          input)
{
    ICO_DBG("CicoSCInputController::delInputApp Enter"
            "(appid=%s device=%s input=%d)",
            appid.c_str(), device.c_str(), input);

    ICO_DBG("ico_input_mgr_control_del_input_app(%s,%s,%d) called.",
            appid.c_str(), device.c_str(), input);
    ico_input_mgr_control_del_input_app(m_inputmgr, appid.c_str(),
                                        device.c_str(), input);

    ICO_DBG("CicoSCInputController::delInputApp Leave");
    return ICO_SYC_EOK;
}

int
CicoSCInputController::sendInputEvent(const string &appid,
                                      int          surfaceid,
                                      int          type,
                                      int          deviceno,
                                      int          code,
                                      int          value)
{
    ICO_DBG("CicoSCInputController::sendInputEvent Enter"
            "(appid=%s surfaceid=%08X type=%d dev_no=%d code=%d value=%d)",
            appid.c_str(), surfaceid, type, deviceno, code, value);

    ICO_DBG("ico_input_mgr_control_send_input_event"
            "(%s,%08X,%d,%d,%d,%d) called.",
            appid.c_str(), surfaceid, type, deviceno, code, value);
    ico_input_mgr_control_send_input_event(m_inputmgr, appid.c_str(), surfaceid,
                                           type, deviceno, code, value);
    ICO_DBG("CicoSCInputController::sendInputEvent Leave");
    return ICO_SYC_EOK;
}

int
CicoSCInputController::setInputRegion(int surfaceid,
                                      int x,
                                      int y,
                                      int width,
                                      int height,
                                      int attr)
{
    ICO_DBG("CicoSCInputController::setInputRegion Enter"
            "(surfaceid=%08X x=%d y=%d width=%d height=%d attr=%d)",
            surfaceid, x, y, width, height, attr);

    ICO_DBG("ico_exinput_set_input_region(%08X,%d,%d,%d,%d,%d) called.",
            surfaceid, x, y, width, height, attr);
    ico_exinput_set_input_region(m_exinput, surfaceid, x, y,
                                 width, height, attr);
    ICO_DBG("CicoSCInputController::setInputRegion Leave");
    return ICO_SYC_EOK;
}

int
CicoSCInputController::resetInputRegion(int surfaceid,
                                        int x,
                                        int y,
                                        int width,
                                        int height)
{
    ICO_DBG("CicoSCInputController::resetInputRegion Enter"
            "(surfaceid=%08X x=%d y=%d width=%d height=%d",
            surfaceid, x, y, width, height);
    ICO_DBG("ico_exinput_reset_input_region(%08X,%d,%d,%d,%d) called.",
            surfaceid, x, y, width, height);
    ico_exinput_reset_input_region(m_exinput, surfaceid, x, y, width, height);
    ICO_DBG("CicoSCInputController::resetInputRegion Leave");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback to application for input switch information
 *
 *  @param [IN] data        user data
 *  @param [IN] ico_exinput wayland ico_exinput interface
 *  @param [IN] device      input device name
 *  @param [IN] type        input device type (as enum type)
 *  @param [IN] swname      input switch name
 *  @param [IN] input       input switch number
 *  @param [IN] codename    input code name
 *  @param [IN] code        input code number
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
 *  @param [IN] data        user data
 *  @param [IN] ico_exinput wayland ico_exinput interface
 *  @param [IN] device      input device name
 *  @param [IN] input       input switch number
 *  @param [IN] codename    input code name
 *  @param [IN] code        input code number
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
            "(device=%s input=%d codename=%s code=%d",
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
 *  @param [IN] data        user data
 *  @param [IN] ico_exinput wayland ico_exinput interface
 *  @param [IN] time        input time of miri-sec
 *  @param [IN] device      input device name
 *  @param [IN] input       input switch number
 *  @param [IN] code        input switch code
 *  @param [IN] state       Of/Off status
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
    message.addElement("commnd", ICO_SYC_EV_WIN_CREATE);
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
 *  @param [IN] data                user data
 *  @param [IN] ico_input_mgr_dev   wayland ico_exinput interface
 *  @param [IN] region              input regions
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
