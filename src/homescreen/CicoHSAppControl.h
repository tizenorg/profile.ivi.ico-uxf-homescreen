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
 *  @file   CicoHSAppControl.h
 *
 *  @brief  This file is definition of CicoHSAppControl class
 */
//==========================================================================
#ifndef __CICO_HS_APP_CONTROL_H__
#define __CICO_HS_APP_CONTROL_H__
#include <string>
#include <vector>
#include "CicoHSCommandArg.h"

//==========================================================================
//  Forward declaration
//==========================================================================
class CicoHSCommand;

//==========================================================================
/*
 *  @brief  This class manages control of user and access to user information
 */
//==========================================================================
class CicoHSAppControl
{
public:
    // get CicoHSAppControl instance
    static CicoHSAppControl* getInstance(void);

    // handle command
    void handleCommand(const CicoHSCommand * cmd);

    // change application window
    void WinChangeApp(const std::string& appid, const std::string& zone, int visible);
    
    // move application
    void MoveApp(const std::string& appid, const std::string& zone);

    // show application
    void ShowApp(const std::string& appid);

    // hide application
    void HideApp(const std::string& appid);

    // execute application
    void ExecuteApp(const std::string& appid);

    // teminate application
    void TerminateApp(const std::string& appid);

    // get application id
    int GetAppId(int pid, std::string& appid);

private:
    // default constructor
    CicoHSAppControl();

    // destructor
    ~CicoHSAppControl();

    // assignment operator
    CicoHSAppControl& operator=(const CicoHSAppControl &object);

    bool WinChgControl(std::vector<CicoHSCommandArg>& arg);
private:
    static CicoHSAppControl*   ms_myInstance;  ///< CicoHSAppControl Object

};

#endif  // __CICO_HS_APP_CONTROL_H__
// vim:set expandtab ts=4 sw=4:
