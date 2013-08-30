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
 *  @brief  
 */
//==========================================================================
#ifndef __CICO_SC_INPUT_CONTROLLER_H__
#define __CICO_SC_INPUT_CONTROLLER_H__

#include <vector>
using namespace std;

#include "CicoSCWlInputMgrIF.h"

//==========================================================================
//  Forward declaration
//==========================================================================
class CicoSCCommand;
class CicoSCInputDev;
class CicoSCSwitch;

//--------------------------------------------------------------------------
/**
 *  @brief  input device controller
 */
//--------------------------------------------------------------------------
class CicoSCInputController : public CicoSCWlInputMgrIF
{
public:
    // default constructor
    CicoSCInputController();

    // destructor
    ~CicoSCInputController();

    int initDB(void);

    void handleCommand(const CicoSCCommand *cmd);

    int addInputApp(const string &appid,
                    const string &device,
                    int          input,
                    int          fix,
                    int          keycode);

    int delInputApp(const string &appid,
                    const string &device,
                    int          input);

    int sendInputEvent(const string &appid,
                       int          surfaceid,
                       int          type,
                       int          deviceno,
                       int          code,
                       int          value);

    int setInputRegion(int surfaceid,
                       int x,
                       int y,
                       int width,
                       int height,
                       int attr);

    int resetInputRegion(int surfaceid,
                         int x,
                         int y,
                         int width,
                         int height);

    void capabilitiesCB(void               *data,
                        struct ico_exinput *ico_exinput,
                        const char         *device,
                        int32_t            type,
                        const char         *swname,
                        int32_t            input,
                        const char          *codename,
                        int32_t            code);

    void codeCB(void               *data,
                struct ico_exinput *ico_exinput,
                const char         *device,
                int32_t            input,
                const char         *codename,
                int32_t            code);

    void inputCB(void               *data,
                 struct ico_exinput *ico_exinput,
                 uint32_t           time,
                 const char         *device,
                 int32_t            input,
                 int32_t            code,
                 int32_t            state);

    void regionCB(void                        *data,
                  struct ico_input_mgr_device *ico_input_mgr_device,
                  struct wl_array             *region);

protected:
    // assignment operator
    CicoSCInputController& operator=(const CicoSCInputController &object);

    // copy constructor
    CicoSCInputController(const CicoSCInputController &object);

private:
    CicoSCInputDev* findInputDev(const char *device);

    CicoSCSwitch* findInputSwitch(const char *device, int input);

private:
    //
    vector<CicoSCInputDev*> m_inputDevList;
};
#endif	// __CICO_SC_INPUT_CONTROLLER_H__
// vim:set expandtab ts=4 sw=4:
