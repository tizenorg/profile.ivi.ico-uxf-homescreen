#ifndef __CICOHOMESCREENSOUND_H__
#define __CICOHOMESCREENSOUND_H__
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

//#include "ico_uxf.h"
//#include "ico_uxf_conf_def.h"
//#include "home_screen.h"
#include "CicoHomeScreenCommon.h"
#include "CicoHomeScreenConfig.h"
#include "CicoHomeScreenResourceConfig.h"
#include "CicoHomeScreenSoundConfig.h"

/* define for sound configuration */
#define ICO_HS_SND_TYPE_DEFAULT      (1)

#define ICO_HS_SND_CNAME_DEFAULT     "sound_default"
#define ICO_HS_SND_FNAME_DEFAULT     "default.wav"


/*============================================================================*/
/* Class Declaration (CicoHomeScreenSound)                                    */
/*============================================================================*/
class CicoHomeScreenSound
{
  public:
    CicoHomeScreenSound(void);
    CicoHomeScreenSoundConfig *ReadSoundConfiguration(CicoHomeScreenConfig *homescreen_config,
                                                      int type,char *conf_name);
    int Initialize(CicoHomeScreenConfig *homescreen_config);
    int PlaySound(char *wav_file);
    char *GetSoundFileName(int type);
  private:
    int snd_is_initialize;
    CicoHomeScreenSoundConfig *sound_files;
    char soundpath[ICO_HS_TEMP_BUF_SIZE];
  protected:
    CicoHomeScreenSound operator = (const CicoHomeScreenSound&);
    CicoHomeScreenSound(const CicoHomeScreenSound&);
};

#endif
