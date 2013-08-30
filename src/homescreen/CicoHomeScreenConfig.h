#ifndef __CICOHOMESCREENCONFIG_H__
#define __CICOHOMESCREENCONFIG_H__
/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   load configuratoin file
 *
 * @date    Feb-15-2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <glib.h>

#include "CicoHomeScreenCommon.h"
#include "CicoHomeScreenResourceConfig.h"

/*============================================================================*/
/* definition                                                                 */
/*============================================================================*/
#define ICO_HOMESCREEN_CONFIG_FILE "homescreen.conf"
#define ICO_ONSCREEN_CONFIG_FILE   "onscreen.conf"
#define ICO_STATUSBAR_CONFIG_FILE  "statusbar.conf"

/*============================================================================*/
/* Class Declaration (CicoHomeScreenConfig)                               */
/*============================================================================*/
class CicoHomeScreenConfig
{
  public:
    CicoHomeScreenConfig();
    int Initialize(const char *conf);
    int ConfigGetInteger(const char *group_name, 
                         const char *key, int default_value);
    const char* ConfigGetString(const char *group_name, const char *key,
                   const char *default_value);
  private:
    GKeyFile *config_key;
    gboolean is_open_config;

  protected:
    CicoHomeScreenConfig operator = (const CicoHomeScreenConfig&);
    CicoHomeScreenConfig(const CicoHomeScreenConfig&);
};

#endif
