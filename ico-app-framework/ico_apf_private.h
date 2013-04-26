/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of AppsController interface
 *
 * @date    Feb-28-2013
 */

#ifndef __ICO_APF_PRIVATEI_H__
#define __ICO_APF_PRIVATEI_H__

#include "ico_apf_log.h"
#include "ico_uxf_sysdef.h"
#include "ico_apf.h"
#include "ico_apf_apimacro.h"

#ifdef  __cplusplus
extern "C" {
#endif

/*
 * AppsControllerr Protocol(TEXT)
 *
 * TEXT ="cmd resource appid device id (base id)"
 *
 * command(Client->AppsController)
 *  "GET BSCR appid disp id"        ... Get basic screen
 *  "REL BSCR appid disp id"        ... Release basic screen
 *  "GET ISCR appid disp id bid"    ... Get interrupt screen
 *  "REL ISCR appid disp id bid"    ... Release interrupt screen
 *  "GET OSCR appid disp id"        ... Get on screen
 *  "REL OSCR appid disp id"        ... Release on screen
 *  "GET BSND appid zone id adjust" ... Get basic sound
 *  "REL BSND appid zone id"        ... Release basic sound
 *  "GET ISND appid zone id adjust" ... Get interrupt sound
 *  "REL ISND appid zone id"        ... Release interrupt sound
 *  "ADD INPT appid device event"   ... Add input switch
 *  "CHG INPT appid device event"   ... Change input switch
 *  "DEL INPT appid device event"   ... Delete input switch
 * event(AppsController->Client)
 *  "ACQ BSCR appid disp id"        ... Acquired basic screen(with surface visible)
 *  "DEP BSCR appid disp id"        ... Deprived basic screen(with surface invisible)
 *  "WAI BSCR appid disp id"        ... Waitting basic screen(with surface invisible)
 *  "DES BSCR appid disp id"        ... Destoryed basic screen(with surface destory)
 *   * ISCR(interrupt screen), BSND(basic sound), ISND(interrupt sound),
 *     INPT(inout switch) are the same, too.
 * reply(Client->AppsController)
 *  "ACK BSCR appid disp id"        ... OK Reply basic screen
 *  "NAK BSCR appid disp id"        ... NG Reply basic screen
 *   * ISCR(interrupt screen), BSND(basic sound), ISND(interrupt sound),
 *     INPT(inout switch) are the same, too.
 */
/* Multi Sound Manager Protocol(Text)
 *
 * TEXT ="#cmd# pid"
 *
 * command(Client->Multi Sound Manager)
 *  "#stream_mute_set# pid"         ... Mute control on(mute)
 *  "#stream_mute_reset# pid"       ... Mute control off((no mute)
 *  "#stream_cancel# pid"           ... Cancel
 *  "#get_stream_list#"             ... Get stream list
 *  "#fresh#"                       ... Fresh
 *  "#all_mute_set#"                ... Mute on(mute) all streams
 *  "#all_mute_reset#"              ... Mute off(no mute) all streams
 *                  pid      = process Id (decimal)
 * event(Multi Sound Manager->Client)
 *  "#stream_new# #pid# pid #stream_name# str-name #app_name# app-name #command_end#"
 *                                  ... Create new sound stream
 *                  pid      = process Id (decimal)
 *                  str-name = stream name
 *                  app-name = application name(appid)
 *  "#stream_free# #pid# pid #stream_name# str-name #app_name# app-name #command_end#"
 *                                  ... End sound stream
 * reply(Multi Sound Manager->Client)
 *  "#get_stream_list# #stream_state# state #pid# pid
 *          #stream_name# str-name #app_name# app-name #command_end#"
 *                                  ... reply of #get_stream_list# (multi message)
 *                  state    = "PA_SINK_INPUT_INIT"
 *                           = "PA_SINK_INPUT_DRAINED"
 *                           = "PA_SINK_INPUT_RUNNING"
 *                           = "PA_SINK_INPUT_CORKED"
 *                           = "PA_SINK_INPUT_UNLINKED"
 *                           = "INCORRECT_STATUS"
 *  "#get_stream_list# #no_stream# #command_end#"
 *                                  ... reply of #get_stream_list# (no stream)
 */

/* Protocol name on libwebsockets for Application Manager   */
#define ICO_PROTOCOL_APPSCONTROLLER     "ico_apps_controller"

/* Protocol name on libwebsockets for Pulse-Audio Plugin(Multi Sound Manager)*/
#define ICO_PROTOCOL_MULTISOUNDMANAGER  "ico_soundmgr-protocol"

/* AppsController resource control command and event    */
#define ICO_APF_SRESOURCE_STATE_ACQUIRED    "ACQ"
#define ICO_APF_SRESOURCE_STATE_DEPRIVED    "DEP"
#define ICO_APF_SRESOURCE_STATE_WAITTING    "WAI"
#define ICO_APF_SRESOURCE_STATE_RELEASED    "DES"

#define ICO_APF_SRESOURCE_COMMAND_GET       "GET"
#define ICO_APF_SRESOURCE_COMMAND_RELEASE   "REL"
#define ICO_APF_SRESOURCE_COMMAND_ADD       "ADD"
#define ICO_APF_SRESOURCE_COMMAND_CHANGE    "CHG"
#define ICO_APF_SRESOURCE_COMMAND_DELETE    "DEL"

#define ICO_APF_SRESOURCE_REPLY_OK          "ACK"
#define ICO_APF_SRESOURCE_REPLY_NG          "NAK"
#define ICO_APF_SRESOURCE_STATE_CONNECTED   "CON"
#define ICO_APF_SRESOURCE_STATE_DISCONNECTED "DCN"

/* AppsController resource control target object        */
#define ICO_APF_SRESID_BASIC_SCREEN         "BSCR"
#define ICO_APF_SRESID_INT_SCREEN           "ISCR"
#define ICO_APF_SRESID_ON_SCREEN            "OSCR"
#define ICO_APF_SRESID_BASIC_SOUND          "BSND"
#define ICO_APF_SRESID_INT_SOUND            "ISND"
#define ICO_APF_SRESID_INPUT_DEV            "INPT"

/* Multi Sound Manager sound control command            */
#define ICO_APF_SSOUND_COMMAND_MUTEON       "#stream_mute_set#"
#define ICO_APF_SSOUND_COMMAND_MUTEOFF      "#stream_mute_reset#"
#define ICO_APF_SSOUND_COMMAND_CANCEL       "#stream_cancel#"
#define ICO_APF_SSOUND_COMMAND_GETLIST      "#get_stream_list#"
#define ICO_APF_SSOUND_COMMAND_FRESH        "#fresh#"
#define ICO_APF_SSOUND_COMMAND_ALLMUTEON    "#all_mute_set#"
#define ICO_APF_SSOUND_COMMAND_ALLMUTEOFF   "#all_mute_reset#"

#define ICO_APF_SSOUND_EVENT_NEW            "#stream_new#"
#define ICO_APF_SSOUND_EVENT_FREE           "#stream_free#"

#define ICO_APF_SSOUND_REPLY_LIST           "#get_stream_list#"

/* Maximum number of send/receive items         */
#define ICO_APF_RESOURCE_MSG_LEN    256 /* Maximum length of message            */
#define ICO_APF_RESOURCE_WSOCK_BUFS 128 /* Number of the buffer for send data   */
#define ICO_APF_RESOURCE_WSOCK_BUFR 64  /* Number of the buffer for receive data*/
#define ICO_APF_RESOURCE_IP_LEN     20  /* IP address length                    */

/* Define connection type                       */
#define ICO_APF_COM_TYPE_SERVER         0xf0    /* server type                  */
#define ICO_APF_COM_TYPE_CONNECTION     0x0f    /* connection type              */
#define ICO_APF_COM_TYPE_APPSCTL        0x01    /* connect to AppsController    */
#define ICO_APF_COM_TYPE_SOUNDMGR       0x02    /* connect to Multi Sound Manager*/
#define ICO_APF_COM_TYPE_SVR_APPSCTL    0x11    /* server(listen) for AppsController*/
#define ICO_APF_COM_TYPE_SVR_SOUNDMGR   0x12    /* server(listen) for Sound Manager*/

/* Defines of Input/Output Event Flags          */
#define ICO_APF_COM_POLL_READ   1       /* input flag                           */
#define ICO_APF_COM_POLL_WRITE  2       /* output flag                          */
#define ICO_APF_COM_POLL_ERROR  4       /* error flag                           */

/* Data queue for send/receive                  */
typedef struct  _ico_apf_com_buffer  {
    short       cmd;                    /* command                              */
    short       res;                    /* resource                             */
    int         pid;                    /* requeste client pid                  */
    char        appid[ICO_UXF_MAX_PROCESS_NAME+1];
                                        /* application id (for Web application) */
    char        msg[ICO_APF_RESOURCE_MSG_LEN];
                                        /* message                              */
}   ico_apf_com_buffer_t;

/* Listener function prototype                  */
typedef struct  _ico_apf_com_handle ico_apf_com_handle_t;
typedef void (*ico_apf_com_eventlistener)(ico_apf_com_handle_t *handle, int cmd, int res,
                                          int pid, char *appid, char *msg, void* user_data);
typedef struct  _ico_apf_com_poll   ico_apf_com_poll_t;
typedef void (*ico_apf_com_pollfd_cb)(ico_apf_com_poll_t *fd_ctl[], const int num_fds);

/* AppsController Handle                        */
struct  _ico_apf_com_handle {
    ico_apf_com_handle_t    *next;      /* next handle                          */
    short   hostid;                     /* Host Id(unused)                      */
    char    service_on;                 /* connected(=1)/ not connect(=0)       */
    char    type;                       /* connection type                      */
    char    ip[ICO_APF_RESOURCE_IP_LEN];/* IP address                           */
    int     port;                       /* port number                          */
    int     fd;                         /* Socket file descriptor               */
    ico_apf_com_poll_t  *poll;          /* poll table                           */
    int     pid;                        /* client pid (server only)             */

    struct libwebsocket_context *wsi_context;
                                        /* Context of libwebsockets             */
    struct libwebsocket *wsi_connection;/* WSI of libwebsockets                 */

    short   retry;                      /* Retry counter                        */
    short   stoprecv;                   /* Receive stopped(flow control)        */
    unsigned short  shead;              /* Head position of send datas          */
    unsigned short  stail;              /* Tail position of send datas          */
    ico_apf_com_buffer_t *sbuf[ICO_APF_RESOURCE_WSOCK_BUFS];
                                        /* Send buffers                         */
    unsigned short  rhead;              /* Head position of receive datas       */
    unsigned short  rtail;              /* Tail position of receive datas       */
    ico_apf_com_buffer_t *rbuf[ICO_APF_RESOURCE_WSOCK_BUFR];
                                        /* Receive buffers                      */
    ico_apf_com_eventlistener   listener;
                                        /* Listener function(if NULL, not regist)*/
    void            *user_data;         /* Listener argument                    */
};

/* Structure of File Descriptor's Controller    */
struct _ico_apf_com_poll {
    ico_apf_com_poll_t  *next;          /* next table                           */
    int     fd;                         /* number of changing File Descriptor   */
    int     flags;                      /* input/output/error flag              */
    void    *apf_fd_control;            /* address of AppFW's controll table    */
    void    *user_data;                 /* user data                            */
};

/* API functions prototype                      */
ico_apf_com_handle_t *ico_apf_com_init_client(const char *uri, const int type);
ico_apf_com_handle_t *ico_apf_com_init_server(const char *uri, const int type);
int ico_apf_com_term_client(ico_apf_com_handle_t *handle);
int ico_apf_com_term_server(ico_apf_com_handle_t *handle);
int ico_apf_com_isconnected(ico_apf_com_handle_t *handle);
int ico_apf_com_send(ico_apf_com_handle_t *handle,
                     const int cmd, const int res, const char *appid, char *msg);
int ico_apf_com_addeventlistener(ico_apf_com_handle_t *handle,
                                 ico_apf_com_eventlistener listener,
                                 void *user_data);
int ico_apf_com_removeeventlistener(ico_apf_com_handle_t *handle);
void ico_apf_com_dispatch(ico_apf_com_handle_t *handle, const int timeoutms);

/* API for file descriptor poll                 */
int ico_apf_com_poll_fd_control(ico_apf_com_pollfd_cb poll_fd_cb);
int ico_apf_com_poll_fd_event(ico_apf_com_poll_t *fd_ctl, int flags);

#ifdef  __cplusplus
}
#endif
#endif /*__ICO_APF_PRIVATEI_H__*/

