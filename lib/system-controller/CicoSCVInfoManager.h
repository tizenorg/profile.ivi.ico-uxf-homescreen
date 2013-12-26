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
 *  @file   CicoSCVInfoManager.h
 *
 *  @brief  This file is definition of CicoSCVInfoManager class
 */
//==========================================================================
#ifndef __CICO_SC_VINFO_MANAGER_H__
#define __CICO_SC_VINFO_MANAGER_H__

#include <string>
#include <map>

#include <ico_dbus_amb_efl.h>
#include "CicoCommonDef.h"

//==========================================================================
//  Forward declaration
//==========================================================================

//==========================================================================
/*
 *  @brief  This class manage vehicle information
 */
//==========================================================================
class CicoSCVInfoManager
{
public:
    // get CicoSCVInfoManager instance
    static CicoSCVInfoManager* getInstance(void);

    // startup communication of vehicle information
    bool startup(void);

    // teardown communication of vehicle information
    bool teardown(void);

    // get ecore event type
    int getEcoreEvType(void);

    // get vehicle information
    bool getVehicleInfo(int property);

private:
    // default constructor
    CicoSCVInfoManager();

    // destructor
    ~CicoSCVInfoManager();

    // assignment operator
    CicoSCVInfoManager& operator=(const CicoSCVInfoManager &object);

    // copy constructor
    CicoSCVInfoManager(const CicoSCVInfoManager &object);

    // start vehicle information monitoring
    void startMonitoring(void);

    // stop vehicle information monitoring
    void stopMonitoring(void);

    // callback find vehicle information
    static void dbusAMBFindCB(const char *objectname,
                              const char *property,
                              dbus_type  type,
                              void *user_data,
                              ico_dbus_error_t *error);

    // callback get vehicle information
    static void dbusAMBGetCB(const char *objectname,
                             const char *property,
                             dbus_type  type,
                             union dbus_value_variant value,
                             void *user_data);

    // callback notify vehicle information
    static void dbusAMBNotifyCB(const char *objectname,
                                const char *property,
                                dbus_type type,
                                union dbus_value_variant value,
                                int sequence,
                                struct timeval tv,
                                void *user_data);

    // callback function of Ecore_End_Cb
    static void evVInfoFree(void* user_data, void* func_data);

    // callback function of ecore timer
    static Eina_Bool ecoreRetryFindPropCB(void *data);

private:
    // CicoSCVInfoManager instannce
    static CicoSCVInfoManager* ms_myInstance;

    // ecore event type
    static int ms_ecoreEvType;

    // dbus_type table
    static dbus_type ms_dbusTypes[ICO_TYPE_MAX];

    static int ms_retryCnt;

    // initialized flag
    bool m_initialized;
};
#endif  // __CICO_SC_VINFO_MANAGER_H__
// vim:set expandtab ts=4 sw=4:
