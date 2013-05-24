/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   user experiance library public header file(system limit vaule)
 *
 * @date    Feb-28-2013
 */

#ifndef _ICO_UXF_SYSDEF_H_
#define _ICO_UXF_SYSDEF_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Default values                           */
#define ICO_UXF_PROC_DEFAULT_HOMESCREEN "org.tizen.ico.homescreen"
                                            /* Default application ID of HomeScreen */
#define ICO_UXF_ENV_APPINF  "ICO_UXF_ENV_APPINF"
                                            /* The name of the environment variable that*/
                                            /* an application manager hands to application*/
/* Maximum values on system wide            */
#define ICO_UXF_HOST_MAX            16      /* Maximum number of ECUs               */
#define ICO_UXF_DISPLAY_LAYER_MAX   16      /* Maximum number of Layers             */
#define ICO_UXF_CATEGORY_MAX        20      /* Maximum number of application categorys*/
#define ICO_UXF_DISPLAY_ZONE_MAX    30      /* Maximum zone on display in car       */
#define ICO_UXF_SOUND_ZONE_MAX      10      /* Maximum zone on sound in car         */
#define ICO_UXF_INPUT_SW_MAX        50      /* Maximum input sw in car              */
#define ICO_UXF_REGULATION_VIC_MAX  20      /* Maxumum vehicle info for regulation  */

/* Maximum length of string (with out last NULL)    */
#define ICO_UXF_MAX_PROCESS_NAME    63      /* Maximum length of application name   */
#define ICO_UXF_MAX_WIN_NAME        23      /* Maximum length of window name        */
#define ICO_UXF_MAX_DISP_NAME       23      /* Maximum length of display name       */
#define ICO_UXF_MAX_SOUND_NAME      23      /* Maximum length of sound device name  */
#define ICO_UXF_MAX_DEVICE_NAME     63      /* Maximum length of resource device name*/
#define ICO_UXF_MAX_STREAM_NAME     63      /* Maximum length of sound stream name  */

/* Maximum values on each application       */
#define ICO_UXF_APPDISPLAY_MAX      5       /* Maximum display zones for a application*/
#define ICO_UXF_APPSOUND_MAX        5       /* Maximum sound zones for a application*/
#define ICO_UXF_APPINPUT_MAX        20      /* Maximum input switch for a application*/

/* Number to manage the processing internall block  */
#define ICO_UXF_MNG_BLOCK_ALLOCS    20      /* Number to assign dynamic management block*/

/* Priority of resource                             */
#define ICO_UXF_PRIO_INTSCREEN   0x00000080 /* interrupt screen on basic screen     */
#define ICO_UXF_PRIO_CATEGORY    0x00000100 /* application category                 */
#define ICO_UXF_PRIO_ACTIVEAPP   0x0fff0000 /* active application count             */
#define ICO_UXF_PRIO_ACTIVECOUNT 0x00010000 /* active application                   */
#define ICO_UXF_PRIO_ONSCREEN    0x10000000 /* interrupt screen/sound               */
#define ICO_UXF_PRIO_REGULATION  0x40000000 /* no regulation controlled             */

/* maximum value        */
#define ICO_UXF_MAX_COORDINATE      16384   /* maximum coordinate position + 1      */

/* timer                */
#define ICO_UXF_SUSP_INITTIME   (10*1000)   /* initiale no suspend time (ms)        */
#define ICO_UXF_SUSP_DELAY      (300)       /* suspend delay time (ms)              */

#ifdef __cplusplus
}
#endif
#endif  /* _ICO_UXF_SYSDEF_H_ */

