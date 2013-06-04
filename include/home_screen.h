/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of homescreen application
 *
 * @date    Feb-15-2013
 */

#ifndef _HOME_SCREEN_H_
#define _HOME_SCREEN_H_

#include "ico_uxf.h"

/*============================================================================*/
/* definition                                                                 */
/*============================================================================*/
#define ICO_HS_OK               (0)
#define ICO_HS_ERR              (-1)

#define ICO_HS_TOUCH_TOBE_HIDE  (1)
#define ICO_HS_TOUCH_TOBE_SHOW  (2)
#define ICO_HS_TOUCH_IN_HIDE    (3)
#define ICO_HS_TOUCH_IN_SHOW    (4)

#define ICO_HS_TILE_NOCHANGE    (1)
#define ICO_HS_TILE_APPLIST     (2)
#define ICO_HS_TILE_OTHERS      (5)
#define ICO_HS_TILE_SMALL       (11)
#define ICO_HS_TILE_HWIDE       (21)
#define ICO_HS_TILE_VWIDE       (12)
#define ICO_HS_TILE_LARGE       (22)

#define ICO_HS_TILE_CHAN        (18)
#define ICO_HS_TILE_SIZE_S      (170)
#define ICO_HS_TILE_SIZE_W      (ICO_HS_TILE_SIZE_S*2 + ICO_HS_TILE_CHAN)

#define ICO_HS_CONFIG_HOMESCREEN    "homescreen"
#define ICO_HS_CONFIG_ONSCREEN      "onscreen"
#define ICO_HS_CONFIG_STATUBAR      "statusbar"
#define ICO_HS_CONFIG_SOUND         "sound"

#define ICO_HS_CONFIG_TILE_COLCNT   "tileinfo_colcnt"
#define ICO_HS_CONFIG_TILE_ROWCNT   "tileinfo_rowcnt"
#define ICO_HS_CONFIG_TILE_VALID    "tileinfo_valid"
#define ICO_HS_CONFIG_TILE_TYPE     "tileinfo_type"
#define ICO_HS_CONFIG_TILE_APP      "tileinfo_app"

#define ICO_HS_CONFIG_TILE_ROWCNT_DEFAULT   (4)
#define ICO_HS_CONFIG_TILE_COLCNT_DEFAULT   (5)
#define ICO_HS_CONFIG_TILE_APP_DEFAULT    "none"

#define ICO_HS_CONFIG_WS_PORT       "wsport"
#define ICO_HS_CONFIG_ORIENTAION    "orientation"

#define ICO_HS_CONFIG_BG            "bgimage"

#define ICO_HS_CONFIG_SB            "statusbar"
#define ICO_HS_CONFIG_ONS           "onscreen"
#define ICO_HS_APPID_DEFAULT_SB     "org.tizen.ico.statusbar"    /* default status bar appid */
#define ICO_HS_APPID_DEFAULT_ONS    "org.tizen.ico.onscreen"    /* default on screen appid */
#define ICO_HS_APPID_DEFAULT_TOUCH  "TOUCH_SCREEN"    /* default touch screen name */

#define ICO_HS_GROUP_SPECIAL        "menu"

#define ICO_HS_ONS_APPLI_KILL_NAME   "appli_kill.edj"
#define ICO_HS_ONS_APPLI_LIST_NAME   "appli_list.edj"
#define ICO_HS_ONS_PART_RECT_NAME    "rect_"
#define ICO_HS_ONS_PART_NEXT_NAME    "next_bt"
#define ICO_HS_ONS_PART_CANCEL_NAME  "cancel_bt"

#define ICO_HS_WS_PORT              (10001)

#define ICO_HS_APP_NUM_MAX          (128)
#define ICO_HS_WINDOW_NUM_MAX       (10)
#define ICO_HS_MSG_SIZE             (1024)
#define ICO_HS_TEMP_BUF_SIZE        (256)
#define ICO_HS_NOTILE_APP_MAX       (20)

#define ICO_HS_PROTOCOL_CM          "gui-protocol"
                  /* protocol to communicate external tools */
#define ICO_HS_PROTOCOL_SB          "statusbar-protocol"
                  /* protocol to communicate status bar     */
#define ICO_HS_PROTOCOL_OS          "onscreen-protocol"
                  /* protocol to communicate onscreen       */
#define ICO_HS_PROTOCOL_APP         "app-protocol"
            /* protocol to communicate application(not used)*/
#define ICO_HS_PROTOCOL_TYPE_CM      (1)
#define ICO_HS_PROTOCOL_TYPE_SB      (2)
#define ICO_HS_PROTOCOL_TYPE_OS      (3)
#define ICO_HS_PROTOCOL_TYPE_APP     (4)

#define ICO_HS_SIZE_SB_HEIGHT        (64)
#define ICO_HS_SIZE_LAYOUT_WIDTH     (768)
#define ICO_HS_SIZE_LAYOUT_HEIGHT    (1024)

/* define for statusbar configuration */
#define ICO_SB_CONFIG_SHTCT_APP      "shortcut_app"

/* define for sound configuration */
#define ICO_HS_SND_TYPE_DEFAULT      (1)

#define ICO_HS_SND_CNAME_DEFAULT     "sound_default"
#define ICO_HS_SND_FNAME_DEFAULT     "default.wav"

/*============================================================================*/
/* definition                                                                 */
/*============================================================================*/
typedef struct _hs_window_data hs_window_data_t;
struct _hs_window_data {
    char appid[ICO_UXF_MAX_PROCESS_NAME+1];
    char exe_name[ICO_UXF_MAX_PROCESS_NAME+1];
    int terminate;
    int visible;
    int raise;
    int resize_w;
    int resize_h;
    int move_x;
    int move_y;
    int category;
};

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
extern void hs_tile_free_app(int idx);
extern void hs_tile_kill_app(const char *appid);
extern void hs_tile_set_app(int idx, const char *appid);
extern int hs_tile_get_index_app(const char *appid);
extern int hs_tile_get_index_pos(int x, int y, int w, int h);
extern int hs_tile_get_minchange(void);
extern void hs_tile_show_screen(void);
extern int hs_get_index_appscreendata(const char *appid);
extern void hs_req_ctlr_tile(void);
extern void hs_click_escutcheon(void);
extern int hs_click_applist(void);
extern void hs_show_onscreen(void);
extern void hs_hide_onscreen(void);
extern void hs_show_appscreen(const char *appid);
extern void hs_tile_delete_hide(const char *appid);

extern void hs_get_image_path(char *buff, int len);
extern void hs_get_sound_path(char *buff, int len);
extern void hs_get_edj_path(char *buff, int len);
extern void hs_get_ons_edj_path(char *buff, int len);
extern void hs_get_conf_path(char *buff, int len);

extern hs_window_data_t hs_command_tile_req[ICO_HS_APP_NUM_MAX];
extern hs_window_data_t hs_app_screen_window[ICO_HS_APP_NUM_MAX];
extern char hs_name_homescreen[ICO_UXF_MAX_PROCESS_NAME+1];
extern char hs_kill_appid[ICO_UXF_MAX_PROCESS_NAME];
extern char hs_active_onscreen[ICO_UXF_MAX_PROCESS_NAME];

extern int hs_snd_play(char *wav_file);
extern char *hs_snd_get_filename(int type);
extern int hs_snd_init(void);
#endif    /* _HOME_SCREEN_H_ */

/**
 * End of File. (home_screen.h)
 */
