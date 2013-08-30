#ifndef __HOMESCREENSOUNDCONFIG_H__
#define __HOMESCREENSOUNDCONFIG_H__
/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   sound an operation of homescreen
 *
 * @date    Apr-04-2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "CicoHomeScreenConfig.h"
#include "CicoHomeScreenResourceConfig.h"

/*============================================================================*/
/* Class Declaration (CicoHomeScreenSoundConfig)                              */
/*============================================================================*/
class CicoHomeScreenSoundConfig
{
  public:
    CicoHomeScreenSoundConfig(){};
    void SetFile(char *file_tmp);
    char *GetFile(void);
    void SetType(int type_tmp);
    int GetType(void);
    void SetNext(CicoHomeScreenSoundConfig *next_tmp);
    CicoHomeScreenSoundConfig *GetNext(void);
  private:
    CicoHomeScreenSoundConfig *next;
    int type;                               /* operation type                 */
    char *file;                             /* sound file name                */
  protected:
    CicoHomeScreenSoundConfig operator=(const CicoHomeScreenSoundConfig&);
    CicoHomeScreenSoundConfig(const CicoHomeScreenSoundConfig&); 
};

#endif
