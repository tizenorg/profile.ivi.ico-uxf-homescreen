#ifndef __CICOSTATUSBARTIME_H__
#define __CICOSTATUSBARTIME_H__
/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   statusbar application
 *
 * @date    Feb-15-2013
 */
#include <stdio.h>
#include <time.h>

#include <Ecore.h>
#include <Ecore_Wayland.h>
#include <Ecore_Evas.h>

#include <ico_uws.h>

/*============================================================================*/
/* Define data types                                                          */
/*============================================================================*/
#define ICO_SB_IMAGE_FILE_PATH_LENGTH (64)

/*============================================================================*/
/* Class Declaration (CicoStatusBarTime)                                      */
/*============================================================================*/
class CicoStatusBarTime
{
  public:
    CicoStatusBarTime(const char *name);
    void SetTimeImg(Evas_Object *time_img_tmp);
    Evas_Object * GetTimeImg(void);
    char *GetFileName(void);
  private:
    char fname[ICO_SB_IMAGE_FILE_PATH_LENGTH];
    Evas_Object *time_img;
  protected:
    CicoStatusBarTime operator = (const CicoStatusBarTime&);
    CicoStatusBarTime(const CicoStatusBarTime&);
};
#endif
// vim: set expandtab ts=4 sw=4:
