/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of Apprication Framework (Resource Control)
 *
 * @date    Feb-28-2013
 */

#include    "ico_uxf_sysdef.h"

#ifndef __ICO_APF_RESOURCE_CONTROL_H__
#define __ICO_APF_RESOURCE_CONTROL_H__

/*
 *  id of resource
 */
typedef enum _resid {
    ICO_APF_RESID_BASIC_SCREEN  = 1, /* basic screen */
    ICO_APF_RESID_INT_SCREEN    = 2, /* interrupt screen */
    ICO_APF_RESID_ON_SCREEN     = 3, /* onscreeen */
    ICO_APF_RESID_BASIC_SOUND   = 4, /* basic sound */
    ICO_APF_RESID_INT_SOUND     = 5, /* interrupt sound */
    ICO_APF_RESID_INPUT_DEV     = 6, /* input device */
} ico_apf_resid_e;
#define ICO_APF_RESOURCE_RESID_MIN      ((int)(ICO_APF_RESID_BASIC_SCREEN))
#define ICO_APF_RESOURCE_RESID_MAX      ((int)(ICO_APF_RESID_INPUT_DEV))

/*
 *  state of resource
 */
typedef enum _resource_state {
    ICO_APF_RESOURCE_STATE_ACQUIRED     = 1,    /* acquired         */
    ICO_APF_RESOURCE_STATE_DEPRIVED     = 2,    /* deprived         */
    ICO_APF_RESOURCE_STATE_WAITTING     = 3,    /* waitting         */
    ICO_APF_RESOURCE_STATE_RELEASED     = 4,    /* released         */

    ICO_APF_RESOURCE_COMMAND_GET        = 5,    /* get command      */
    ICO_APF_RESOURCE_COMMAND_RELEASE    = 6,    /* release command  */
    ICO_APF_RESOURCE_COMMAND_ADD        = 7,    /* add command      */
    ICO_APF_RESOURCE_COMMAND_CHANGE     = 8,    /* change command   */
    ICO_APF_RESOURCE_COMMAND_DELETE     = 9,    /* delete command   */

    ICO_APF_RESOURCE_REPLY_OK           = 10,   /* OK reply         */
    ICO_APF_RESOURCE_REPLY_NG           = 11,   /* NG reply         */
    ICO_APF_RESOURCE_STATE_CONNECTED    = 12,   /* connected        */
    ICO_APF_RESOURCE_STATE_DISCONNECTED = 13,   /* disconnected     */
} ico_apf_resource_state_e;
#define ICO_APF_RESOURCE_COMMAND_MIN    ((int)(ICO_APF_RESOURCE_STATE_ACQUIRED))
#define ICO_APF_RESOURCE_COMMAND_MAX    ((int)(ICO_APF_RESOURCE_STATE_DISCONNECTED))

/*
 *  sound control
 */
typedef enum _sound_resource {
    ICO_APF_SOUND_COMMAND_MUTEON        = 21,   /* mute on          */
    ICO_APF_SOUND_COMMAND_MUTEOFF       = 22,   /* mute off         */
    ICO_APF_SOUND_COMMAND_CANCEL        = 23,   /* cancel           */
    ICO_APF_SOUND_COMMAND_GETLIST       = 24,   /* get stream list  */
    ICO_APF_SOUND_COMMAND_FRESH         = 25,   /* fresh            */
    ICO_APF_SOUND_COMMAND_ALLMUTEON     = 26,   /* mute on all streams*/
    ICO_APF_SOUND_COMMAND_ALLMUTEOFF    = 27,   /* mute off all streams*/

    ICO_APF_SOUND_EVENT_NEW             = 28,   /* created new stream*/
    ICO_APF_SOUND_EVENT_FREE            = 29,   /* destoryed stream */

    ICO_APF_SOUND_REPLY_LIST            = 30    /* reply on stream list*/
} ico_apf_sound_state_e;
#define ICO_APF_SOUND_COMMAND_MIN       ((int)(ICO_APF_SOUND_COMMAND_MUTEON))
#define ICO_APF_SOUND_COMMAND_CMD       ((int)(ICO_APF_SOUND_COMMAND_ALLMUTEOFF))
#define ICO_APF_SOUND_COMMAND_MAX       ((int)(ICO_APF_SOUND_REPLY_LIST))

/*
 *  notify callback information
 */
typedef struct _resmgr_notify_info {
    ico_apf_resid_e resid;
    int             id;
    int             bid;
    int             pid;
    char            appid[ICO_UXF_MAX_PROCESS_NAME+1];
    ico_apf_resource_state_e state;
    char            device[ICO_UXF_MAX_DEVICE_NAME+1];
} ico_apf_resource_notify_info_t;

typedef void (*ico_apf_resource_notify_cb_t)(ico_apf_resource_notify_info_t* info,
                                             void *user_data);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Initialize resouce manager component for client Application
 *
 *  @param[in]  uri         server URI
 *  @return result status
 *  @retval ICO_APF_RESOURCE_E_NONE                 success
 *  @retval ICO_APF_RESOURCE_E_INIT_COM_FAILD       can not create connection
 *  @retval ICO_APF_RESOURCE_E_INIT_COMMUNICATION   initialize communication error
 */
/*--------------------------------------------------------------------------*/
int ico_apf_resource_init_client(const char *uri);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Terminame resouce manager component
 *
 *  @param      none
 *  @return     none
 */
/*--------------------------------------------------------------------------*/
void ico_apf_resource_term_client(void);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Initialize resouce manager component for sever(AppsController)
 *
 *  @param[in]  uri         my URI
 *  @return result status
 *  @retval ICO_APF_RESOURCE_E_NONE                 success
 *  @retval ICO_APF_RESOURCE_E_INIT_COM_FAILD       can not create connection
 *  @retval ICO_APF_RESOURCE_E_INIT_COMMUNICATION   initialize communication error
 *  @retval ICO_APF_RESOURCE_E_INVALID_PARAMETER    appid is null
 */
/*--------------------------------------------------------------------------*/
int ico_apf_resource_init_server(const char *uri);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Terminame resouce manager component for server(AppsController)
 *
 *  @param      none
 *  @return     none
 */
/*--------------------------------------------------------------------------*/
void ico_apf_resource_term_server(void);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Set event callback function
 *
 *  @param[in]  callbacks   callback functions
 *  @param[in]  user_data   passed data on called callback function
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE     success
 *  @retval     ICO_APF_RESOURCE_E_INVAL    callbacks is null
 */
/*--------------------------------------------------------------------------*/
int ico_apf_resource_set_event_cb(ico_apf_resource_notify_cb_t callbacks,
                                  void *user_data);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Unset event callback function
 *
 *  @param      none
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE     success(At present, always give back this)
 */
/*--------------------------------------------------------------------------*/
int ico_apf_resource_unset_event_cb(void);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Get the rights of basic screen
 *
 *  @param[in]  disp_dev    display device uri
 *  @param[in]  src_id      id of basic screen
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
int ico_apf_resource_get_screen_mode(const char* disp_dev, int scr_id);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Release the rights of basic screen
 *
 *  @param[in]  disp_dev    display device uri
 *  @param[in]  src_id      id of basic screen
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
int ico_apf_resource_release_screen_mode(const char* disp_dev, int scr_id);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Reply the basic screen status change event
 *
 *  @param[in]  disp_dev    display device uri
 *  @param[in]  src_id      id of basic screen
 *  @param[in]  ok          OK(1) or NG(0)
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
int ico_apf_resource_reply_screen_mode(const char* disp_dev, int scr_id, const int ok);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Release the rights of interrupt screen
 *
 *  @param[in]  disp_dev    display device uri
 *  @param[in]  src_id      id of basic screen
 *  @param[in]  int_src_id  id of interrupt screen
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
int ico_apf_resource_get_int_screen_mode(const char* disp_dev,
                                         int scr_id, int int_scr_id);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Release the rights of interrupt screen on basic screen
 *
 *  @param[in]  disp_dev    display device uri
 *  @param[in]  src_id      id of basic screen
 *  @param[in]  int_scr_id  id of interrupt screen
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
int ico_apf_resource_release_int_screen_mode(const char* disp_dev,
                                             int scr_id, int int_scr_id);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Reply the rights of interrupt screen on basic screen
 *
 *  @param[in]  disp_dev    display device uri
 *  @param[in]  src_id      id of basic screen
 *  @param[in]  int_src_id  id of interrupt screen
 *  @param[in]  ok          OK(1) or NG(0)
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
int ico_apf_resource_reply_int_screen_mode(const char* disp_dev,
                                           int scr_id, int int_scr_id, const int ok);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Get the rights of interrupt screen on display
 *
 *  @param[in]  disp_dev    display device uri
 *  @param[in]  int_scr_id  id of interrupt screen
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
int ico_apf_resource_get_int_screen_mode_disp(const char* disp_dev,
                                              int int_scr_id);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Release the rights of interrupt screen on display
 *
 *  @param[in]  disp_dev    display device uri
 *  @param[in]  int_scr_id  id of interrupt screen
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
int ico_apf_resource_release_int_screen_mode_disp(const char* disp_dev,
                                                  int int_scr_id);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Reply the rights of interrupt screen on display
 *
 *  @param[in]  disp_dev    display device uri
 *  @param[in]  int_scr_id  id of interrupt screen
 *  @param[in]  ok          OK(1) or NG(0)
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
int ico_apf_resource_reply_int_screen_mode_disp(const char* disp_dev,
                                                int int_scr_id, const int ok);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Get the rights of basic sound on zone
 *
 *  @param[in]  zone        sound device uri
 *  @param[in]  snd_id      id of basic sound
 *  @param[in]  adjust      adjust acction
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
int ico_apf_resource_get_sound_mode(const char* zone, int snd_id, int adjust);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Release the rights of basic sound on zone
 *
 *  @param[in]  zone        sound device uri
 *  @param[in]  snd_id      id of basic sound
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
int ico_apf_resource_release_sound_mode(const char* zone, int snd_id);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Reply the rights of basic sound on zone
 *
 *  @param[in]  zone        sound device uri
 *  @param[in]  snd_id      id of basic sound
 *  @param[in]  ok          OK(1) or NG(0)
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
int ico_apf_resource_reply_sound_mode(const char* zone, int snd_id, const int ok);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Get the rights of interrupt sound on zone
 *
 *  @param[in]  zone        sound device uri
 *  @param[in]  int_snd_id  id of basic sound
 *  @param[in]  adjust      adjust action
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
int ico_apf_resource_get_int_sound_mode(const char* zone,
                                        int int_snd_id, int adjust);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Release the rights of interrupt sound on zone
 *
 *  @param[in]  zone        sound device uri
 *  @param[in]  int_snd_id  id of interrupt sound
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
int ico_apf_resource_release_int_sound_mode(const char* zone, int int_snd_id);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Reply the rights of interrupt sound on zone
 *
 *  @param[in]  zone        sound device uri
 *  @param[in]  int_snd_id  id of interrupt sound
 *  @param[in]  ok          OK(1) or NG(0)
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
int ico_apf_resource_reply_int_sound_mode(const char* zone,
                                          int int_snd_id, const int ok);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Add the input event notification from input device
 *
 *  @param[in]  input_dev   input device uri
 *  @param[in]  events      notify input events
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
int ico_apf_resource_add_input_event(const char* input_dev, int events);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Change the input event notification from input device
 *
 *  @param[in]  input_dev   input device uri
 *  @param[in]  events      notify input events
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
int ico_apf_resource_change_input_event(const char* input_dev, int events);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Delete the input event notification from input device
 *
 *  @param[in]  input_dev   input device uri
 *  @param[in]  events      notify input events
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
int ico_apf_resource_delete_input_event(const char* input_dev, int events);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Send responce form server(AppsController) to client application
 *
 *  @param[in]  appid       client application id
 *  @param[in]  event       event
 *  @param[in]  resource    target resource
 *  @param[in]  device      display device / sound zone
 *  @param[in]  id          application defined object id
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE             success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED  not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION    communication error
 *  @retval     ICO_APF_RESOURCE_E_INVAL            illegal appid or event code
 */
/*--------------------------------------------------------------------------*/
int ico_apf_resource_send_to_client(const char *appid, const int event,
                                    const int resource, const char *device, const int id);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Send request to Multi Input Manager
 *
 *  @param[in]  cmd         send command
 *  @param[in]  pid         target pid
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE             success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED  not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION    communication error
 */
/*--------------------------------------------------------------------------*/
int ico_apf_resource_send_to_soundctl(const ico_apf_sound_state_e cmd, const int pid);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Get aplication Id
 *
 *  @param[in]  pid         process id (if 0, current process id)
 *  @param[out] appid       client application id
 *  @return     result status
 *  @retval     ICO_APP_CTL_E_NONE              success
 *  @retval     ICO_APP_CTL_E_INVALID_PARAM     error(pid dose not exist)
 */
/*--------------------------------------------------------------------------*/
/* Applicaton Manager                           */
int ico_apf_get_app_id(const int pid, char *appid);

#ifdef __cplusplus
}
#endif

#endif  /*__ICO_APF_RESOURCE_CONTROL_H__*/
