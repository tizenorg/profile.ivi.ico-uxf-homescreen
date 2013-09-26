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
 *  @file   CicoHomeScreenConfig
 *
 *  @brief  This file is definition of CicoHomeScreenConfig class
 */
//==========================================================================
#ifndef __CICO_HOMESCREEN_CONFIG_H__
#define __CICO_HOMESCREEN_CONFIG_H__

#include <glib.h>

//==========================================================================
//  definition
//==========================================================================
#define ICO_HOMESCREEN_CONFIG_FILE "homescreen.conf"
#define ICO_ONSCREEN_CONFIG_FILE   "onscreen.conf"
#define ICO_STATUSBAR_CONFIG_FILE  "statusbar.conf"

//--------------------------------------------------------------------------
/**
 *  @brief  
 */
//--------------------------------------------------------------------------
class CicoHomeScreenConfig
{
public:
    // constructor
    CicoHomeScreenConfig();

    // destructor
    ~CicoHomeScreenConfig();

    // intialize
    int Initialize(const char *conf);

    // get integer value
    int ConfigGetInteger(const char *group_name, 
                         const char *key,
                         int        default_value);

    // get string value
    const char* ConfigGetString(const char *group_name,
                                const char *key,
                                const char *default_value);

private:
    // assignment operator
    CicoHomeScreenConfig operator=(const CicoHomeScreenConfig&);

    // copy constructor
    CicoHomeScreenConfig(const CicoHomeScreenConfig&);

private:
    GKeyFile *m_gKeyFile;
};
#endif  // __CICO_HOMESCREEN_CONFIG_H__
// vim: set expandtab ts=4 sw=4:
