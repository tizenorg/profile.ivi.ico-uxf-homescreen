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
 *  @file   CicoSCInputController.h
 *
 *  @brief  This file is definition of CicoSCInputController class
 */
//==========================================================================
#ifndef __CICO_SC_INPUT_CONTROLLER_H__
#define __CICO_SC_INPUT_CONTROLLER_H__

#include <string>
#include <vector>

#include "CicoSCWlInputMgrIF.h"

//==========================================================================
//  Forward declaration
//==========================================================================
class CicoSCCommand;
class CicoSCInputDev;
class CicoSCSwitch;

//--------------------------------------------------------------------------
/**
 *  @brief  This class is controller of input device
 */
//--------------------------------------------------------------------------
class CicoSCInputController : public CicoSCWlInputMgrIF
{
public:
    // default constructor
    CicoSCInputController();

    // destructor
    ~CicoSCInputController();

    // initialize input device information 
    int initDB(void);

    // executes an input control process corresponding to the command
    void handleCommand(const CicoSCCommand *cmd);

    // register input device control application
    int addInputApp(const std::string &appid,
                    const std::string &device,
                    int               input,
                    int               fix,
                    int               keycode);

    // unregister input device control application
    int delInputApp(const std::string &appid,
                    const std::string &device,
                    int               input);

    // send input device event
    int sendInputEvent(const std::string &appid,
                       int               surfaceid,
                       int               type,
                       int               deviceno,
                       int               time,
                       int               code,
                       int               value);

    // set input region information
    int setInputRegion(const std::string &appid,
                       const std::string &winname,
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
                       int attr);

    // unset input region information
    int unsetInputRegion(const std::string &appid,
                         const std::string &winname,
                         int x,
                         int y,
                         int width,
                         int height);

    // callback to application for input switch information
    void capabilitiesCB(void               *data,
                        struct ico_exinput *ico_exinput,
                        const char         *device,
                        int32_t            type,
                        const char         *swname,
                        int32_t            input,
                        const char          *codename,
                        int32_t            code);

    // callback to application for input code information
    void codeCB(void               *data,
                struct ico_exinput *ico_exinput,
                const char         *device,
                int32_t            input,
                const char         *codename,
                int32_t            code);

    // callback to application for switch input 
    void inputCB(void               *data,
                 struct ico_exinput *ico_exinput,
                 uint32_t           time,
                 const char         *device,
                 int32_t            input,
                 int32_t            code,
                 int32_t            state);

    // callback to application for change input region
    void regionCB(void                        *data,
                  struct ico_input_mgr_device *ico_input_mgr_device,
                  struct wl_array             *region);

protected:
    // assignment operator
    CicoSCInputController& operator=(const CicoSCInputController &object);

    // copy constructor
    CicoSCInputController(const CicoSCInputController &object);

private:
    // find input device information
    CicoSCInputDev* findInputDev(const char *device);

    // find input device switch information
    CicoSCSwitch* findInputSwitch(const char *device, int input);

private:
    /// input device information list
    std::vector<CicoSCInputDev*> m_inputDevList;
};
#endif  // __CICO_SC_INPUT_CONTROLLER_H__
// vim:set expandtab ts=4 sw=4:
