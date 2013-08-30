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

#include "CicoHomeScreenSoundConfig.h"

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreenSoundConfig::SetFile
 *          set sound file name
 *
 * @param[in]   file_tmp   sound file name
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreenSoundConfig::SetFile(char *file_tmp)
{
    file = file_tmp;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreenSoundConfig::GetFile
 *          get sound file name
 *
 * @param[in]   none
 * @return      sound file name
 */
/*--------------------------------------------------------------------------*/
char*
CicoHomeScreenSoundConfig::GetFile(void)
{
    return file;
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreenSoundConfig::SetType
 *          set sound type
 *
 * @param[in]   type_tmp    the value to set type
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreenSoundConfig::SetType(int type_tmp)
{
    type = type_tmp;
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreenSoundConfig::GetType
 *          get sound type
 *
 * @param[in]   none
 * @return      type
 */
/*--------------------------------------------------------------------------*/
int
CicoHomeScreenSoundConfig::GetType(void)
{
    return type;
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreenSoundConfig::SetNext
 *          set next instance
 *
 * @param[in]   next instance
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreenSoundConfig::SetNext(CicoHomeScreenSoundConfig *next_tmp)
{
    next = next_tmp;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreenSoundConfig::GetNext
 *          get next instance
 *
 * @param[in]   none
 * @return      next instance
 */
/*--------------------------------------------------------------------------*/
CicoHomeScreenSoundConfig*
CicoHomeScreenSoundConfig::GetNext(void)
{
    return next;
}

