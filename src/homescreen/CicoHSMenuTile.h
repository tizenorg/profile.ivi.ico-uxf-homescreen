/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   tile in menu
 *
 * @date    Aug-08-2013
 */
#ifndef __CICO_HS_MENU_TILE_H__
#define __CICO_HS_MENU_TILE_H__

#include <stdio.h>
#include <string.h>

#include <Eina.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Wayland.h>
#include <Ecore_Evas.h>
#include <Edje.h>

#include "ico_syc_common.h"
#include "ico_syc_winctl.h"

#include "CicoHomeScreenCommon.h"

/*1920 * 1080 display*/
/*orientation is vertical*/
/* max is (4 * 3)*/
/*it can't use all height 1920 ,it needs deduct ControlBar 128 and StatusBar 64*/
/*home screen use 1728 * 1080*/
/*one rectangle size is 340 * 340 and space = 15*/
/**/
/*raw is four */

/*** position       ***/
/***+---+---+---+******/
/***+ 0 + 1 + 2 +******/
/***+ 3 + 4 + 5 +******/
/***+ 6 + 7 + 8 +******/
/***+ 9 +10 +11 +******/
/***+---+---+---+******/

#define ICO_HS_MENUTILE_ROW 3
#define ICO_HS_MENUTILE_COLUMN 4
#define ICO_HS_MENUTILE_NUM (ICO_HS_MENUTILE_COLUMN * ICO_HS_MENUTILE_ROW)
#define ICO_HS_MENUTILE_POSITION_0 0
#define ICO_HS_MENUTILE_POSITION_1 1
#define ICO_HS_MENUTILE_POSITION_2 2
#define ICO_HS_MENUTILE_POSITION_3 3
#define ICO_HS_MENUTILE_POSITION_4 4
#define ICO_HS_MENUTILE_POSITION_5 5
#define ICO_HS_MENUTILE_POSITION_6 6
#define ICO_HS_MENUTILE_POSITION_7 7
#define ICO_HS_MENUTILE_POSITION_8 8
#define ICO_HS_MENUTILE_POSITION_9 9
#define ICO_HS_MENUTILE_POSITION_10 10
#define ICO_HS_MENUTILE_POSITION_11 11
#define ICO_HS_MENUTILE_POSITION_RESERVE 0xFFFF

#define ICO_HS_MENUTILE_WIDTH 290
#define ICO_HS_MENUTILE_HEIGHT 290
#define ICO_HS_MENUTILE_SPACE_TILE_AND_TILE 25

#define ICO_HS_MENUTILE_START_POS_X 80
#define ICO_HS_MENUTILE_START_POS_Y 210

#define ICO_HS_MENUTILE_TERM_ICON_PATH "/usr/apps/org.tizen.ico.homescreen/res/images/termIcon.png"
#define ICO_HS_MENUTILE_DEFAULT_ICON_PATH "/usr/apps/org.tizen.ico.homescreen/res/images/tizen_32.png"
#define ICO_HS_MENUTILE_TERM_ICON_WIDTH 64
#define ICO_HS_MENUTILE_TERM_ICON_HEIGHT 64

#define ICO_HS_MENUTILE_THUMBNAIL_FPS_SHOW      15          /* 15 frames/sec        */
#define ICO_HS_MENUTILE_THUMBNAIL_FPS_HIDE      -1          /* meanse 'not update'  */
#define ICO_HS_MENUTILE_THUMBNAIL_REDUCE_PIX    4
#define ICO_HS_MENUTILE_THUMBNAIL_REDUCE_PIX2   6
#define ICO_HS_MENUTILE_THUMBNAIL_REDUCE_RATE  10
#define ICO_HS_MENUTILE_THUMBNAIL_REDUCTION     4

/*shared memory buffer*/
#define ICO_HS_THUMB_ICODIR       "/tmp/ico"
#define ICO_HS_THUMB_FILEDIR       "/thumbnail"
#define ICO_HS_THUMB_FILEEXT       "pixel"

struct _CicoHSMenuTile_thumb {
    int         surface;                // surface id
    int         type;                   // frame buffer type
    int         width;                  // frame buffer width
    int         height;                 // frame buff height
    int         stride;                 // frame buffer stride
    int         format;                 // frame buffer format(only EGL_TEXTURE_RGBA)
    int         fbcount;                // frame buffer change counter
    char        *pixel_data;            // glReadPixels data buffer
    int         pixel_bufsize;          // data buffer size
    Ecore_Timer *thumb_timer;           // retry timer
    int         orgsurface;             // original surface id
};

class CicoHSMenuTile
{
  public:
    CicoHSMenuTile(const char *appid,const char *icon_image_path
        ,int page,int subpage,int position,int width,int height);
    ~CicoHSMenuTile(void);
    void CreateObject(Evas *evas);
    void FreeObject(void);
    void Resize(int width,int height);
    void MovePosition(int page,int poisition);
    void OffsetMove(int offset_x,int offset_y);
    const char* GetImagePath(void);
    int GetWidth(void);
    int GetHeight(void);
    int GetPosition(void);
    int GetPage(void);
    int GetSubPage(void);
    int GetPositionX(void);
    int GetPositionY(void);
    int GetAppIdx(void);
    const char* GetAppId(void);
    void ShowTermIcon(void);
    void HideTermIcon(void);
    void ValidMenuIcon(void);
    void ValidThumbnail(int surface);
    void SetThumbnail(ico_syc_thumb_info_t *info);
    void ShowMenu(bool show);
    void SetOrgThumbnail(CicoHSMenuTile *orgTile);
    void RetryThumbnail(void);
    static Eina_Bool TimerThumbnail(void *data);

  private:
    char appid[ICO_HS_MAX_PROCESS_NAME];
    char icon_image_path[ICO_HS_MAX_PATH_BUFF_LEN];
    int page;
    int subpage;
    int position;
    int width;
    int height;
    int pos_x;
    int pos_y;
    int thumb_reduce_x;
    int thumb_reduce_y;
    bool app_running;
    bool menu_show;
    Evas        *menu_evas;
    Evas_Object *tile;
    Evas_Object *thumb_tile;
    Evas_Object *icon;
    Evas_Object *small_icon;
    Evas_Object *term_icon;
    struct _CicoHSMenuTile_thumb thumb;

  protected:
    CicoHSMenuTile operator=(const CicoHSMenuTile&);
    CicoHSMenuTile(const CicoHSMenuTile&);
};
#endif
// vim: set expandtab ts=4 sw=4:
