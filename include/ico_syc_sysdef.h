/*
 * Copyright (c) 2013-2014, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of System Fixed Parameters
 *          for privilege and general applications
 *
 * @date    Feb-21-2014
 */

#ifndef _ICO_SYC_SYSDEF_H_
#define _ICO_SYC_SYSDEF_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Display number
 */
#define ICO_SYC_DISPLAY_CENTER      0       /* center display Id        */
#define ICO_SYC_DISPLAY_MATER       1       /* meter display Id         */

/*
 * Layer id
 */
#define ICO_SYC_LAYER_BACKGROUND    0       /* layer of BackGround      */
#define ICO_SYC_LAYER_APPLICATION   1       /* layer of Application     */
#define ICO_SYC_LAYER_HOMESCREEN    2       /* layer of HomeScreen menu */
#define ICO_SYC_LAYER_CONTROLBAR    3       /* layer of Control/Status bar */
#define ICO_SYC_LAYER_FULLSCREEN    4       /* layer of FullScreen Application */
#define ICO_SYC_LAYER_ONSCREEN      5       /* layer of OnScreen        */
#define ICO_SYC_LAYER_TOUCHPANEL    6       /* layer of TouchPanel input */
#define ICO_SYC_LAYER_CURSOR        7       /* layer of Cursor          */

#define ICO_SYC_LAYER_MATERAPP      3       /* layer of Meter application */

#ifdef __cplusplus
}
#endif
#endif /*_ICO_SYC_SYSDEF_H_*/
/* vim:set expandtab ts=4 sw=4: */
