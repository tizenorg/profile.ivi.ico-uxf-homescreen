/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   control time image for statusbar application
 *
 * @date    Feb-15-2013
 */
#include "CicoStatusBarTime.h"

/*============================================================================*/
/* Functions Declaration (CicoStatusBarTime)                          */
/*============================================================================*/

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoStatusBarTime::CicoStatusBarTime
 *          constractor 
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoStatusBarTime::CicoStatusBarTime(const char *name)
{
    strcpy(fname,name);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoStatusBarTime::SetTimeImg
 *          set time image
 *
 * @param[in]   time_img_tmp    setting value
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoStatusBarTime::SetTimeImg(Evas_Object *time_img_tmp)
{
    time_img = time_img_tmp;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoStatusBarTime::GetTimeImg
 *          get time image 
 *
 * @param[in]   none
 * @return      time image(Evas_Object)
 */
/*--------------------------------------------------------------------------*/
Evas_Object *
CicoStatusBarTime::GetTimeImg(void)
{
    return time_img;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoStatusBarTime::GetFileName
 *          get file name
 *
 * @param[in]   none
 * @return      filename
 */
/*--------------------------------------------------------------------------*/
char *
CicoStatusBarTime::GetFileName(void)
{
    return fname;
}
// vim: set expandtab ts=4 sw=4:
