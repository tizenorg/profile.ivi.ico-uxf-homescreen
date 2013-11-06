/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Menu Window class
 *
 * @date    Aug-08-2013
 */
#ifndef __CICO_HS_MENU_WINDOW_H__
#define __CICO_HS_MENU_WINDOW_H__
#include <stdio.h>
#include <pthread.h>
#include <libwebsockets.h>

#include "ico_syc_common.h"
#include "ico_syc_winctl.h"

#include <unistd.h>
#include <Eina.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Wayland.h>
#include <Ecore_Evas.h>
#include <Edje.h>

#include "CicoHomeScreenConfig.h"
#include "CicoHSWindow.h"
#include "CicoHSMenuTile.h"

#include "CicoHomeScreenCommon.h"

#include "CicoSCAilItems.h"
#include "CicoSCLifeCycleController.h"

#define ICO_HS_MENU_SHOWING 0
#define ICO_HS_MENU_HIDING 1

#define ICO_HS_MENU_PAGE_POINTER_START_POS_X 300
#define ICO_HS_MENU_PAGE_POINTER_START_POS_Y 75
#define ICO_HS_MENU_PAGE_POINTER_WIDTH 64
#define ICO_HS_MENU_PAGE_POINTER_HEIGHT 64

#define ICO_HS_MENU_PAGE_UP_CURSOR_START_POS_X 50
#define ICO_HS_MENU_PAGE_UP_CURSOR_START_POS_Y 50
#define ICO_HS_MENU_PAGE_UP_CURSOR_WIDTH 64
#define ICO_HS_MENU_PAGE_UP_CURSOR_HEIGHT 64

#define ICO_HS_MENU_PAGE_DOWN_CURSOR_START_POS_X 50
#define ICO_HS_MENU_PAGE_DOWN_CURSOR_START_POS_Y 1630
#define ICO_HS_MENU_PAGE_DOWN_CURSOR_WIDTH 64
#define ICO_HS_MENU_PAGE_DOWN_CURSOR_HEIGHT 64

#define ICO_HS_MENU_HIDE_ANIMATION_SLIDE "Slide"
#define ICO_HS_MENU_SHOW_ANIMATION_SLIDE "Slide"
#define ICO_HS_MENU_HIDE_ANIMATION_FADE "Fade"
#define ICO_HS_MENU_SHOW_ANIMATION_FADE "Fade"
#define ICO_HS_MENU_ANIMATION_DURATION 300

#define ICO_HS_MENU_OTHER_CATEGORY_ID -1

/* Layer */
#define ICO_HS_MENU_TERMINATE_BUTTON_LAYER 5

typedef struct _CicoLayoutInfo{
   int appidx;
   int page;
   int position;
   int tile_width;
   int tile_height;
} CicoLayoutInfo;

typedef struct _CicoCurrentPage{
    int current_page;
    int subcurrent_page;
} CicoCurrentPage;

typedef struct _CicoCategoryInfo {
    int id;
    int page;
    int subpage_max;
    int tile_num;
} CicoCategoryInfo;

/*  tile size   */

class CicoHSMenuWindow :public CicoHSWindow
{
  public:
    CicoHSMenuWindow();
    ~CicoHSMenuWindow(void);
    int Initialize(void);
    void Finalize(void);
    void SetMenuWindowID(const char* appid,int sruface);
    int CreateMenuWindow(int pos_x,int pos_y,int width,int height);
    void FreeMenuWindow(void);
    void HSWindowSetting(int orientation);
    void GoNextMenu(void);
    void GoBackMenu(void);
    static Eina_Bool MoveToNextAnimation(void *data,double pos);
    static Eina_Bool MoveToBackAnimation(void *data,double pos);
    void Show(ico_syc_animation_t *animation);
    void Hide(ico_syc_animation_t *animation);
    void ExecuteApp(const char *appid);
    void TerminateApp(const char *appid);
    void ChangeTerminateMode(void);
    void ChangeNormalMode(void);
    void ShowTerminateButton(void);
    void HideTerminateButton(void);
    void ValidMenuIcon(const char *appid);
    void ValidThumbnail(const char *appid, int surface);
    void SetThumbnail(const char *appid, ico_syc_thumb_info_t *info);

    static Eina_Bool MoveDownAnimation(void *data,double pos);
    static Eina_Bool MoveUpAnimation(void *data,double pos);

    void SetNightMode(void);

    void DspCtrlPageCursor(void);
    void DownNextMenu(void);
    void UpBackMenu(void);
    static Eina_Bool MoveToNextSubAnimation(void *data,double pos);
    static Eina_Bool MoveToBackSubAnimation(void *data,double pos);

    static int Tile_Width(void);
    static int Tile_Height(void);

  private:
    int SetMenuBack(void);
    void FreeMenuBack(void);
    void InitAppTiles(void);
    int SetAppTiles(void);
    void FreeAppTiles(void);
    void RenewAppTiles(void);
    int SetPagePointer(void);
    void FreePagePointer(void);
    int SetPageCursor(void);
    void FreePageCursor(void);
    int SetTerminateButton(void);
    void FreeTerminateButton(void);
    void GetLayout(const char *filename,CicoLayoutInfo *layout,int *num);
    void SetLayout(const char *filename,CicoLayoutInfo *layout,int num);
    void GetCategory(const char *filename,int *category,int *num);
    void SetCategoryInfo(int id);
    void GetTileInfo(int id, int *page, int *subpage, int *position);

    bool terminate_mode;
    /*for menu control*/
    int current_page;
    int subcurrent_page;
    CicoCurrentPage current_info;
    CicoCategoryInfo category_info[ICO_HS_MENU_MAX_MENU_CATEGORY_NUM];
    int all_category_num;
    int all_tile_num;
    int all_page_num;
    /* image directory path */
    char img_dir_path[ICO_HS_MAX_PATH_BUFF_LEN];
    /* evas object */
    Evas *evas;
    /* menu back */
    Evas_Object *canvas;
    Evas_Object *rectangle;
    /* tiles */
    CicoHSMenuTile *menu_tile[ICO_HS_MENU_MAX_TILE_NUM];
    /* Page pointer */
    Evas_Object *page_pointer[ICO_HS_MENU_MAX_MENU_PAGE_NUM];
    /* Page cursor */
    Evas_Object *page_up_cursor;
    Evas_Object *page_down_cursor;
    /* for window control */
    char appid[ICO_HS_MAX_PROCESS_NAME];
    int surface;
    /*terminate button*/
    Evas_Object *terminate_back;
    Evas_Object *terminate_really;
    Evas_Object *terminate_button_yes;
    Evas_Object *terminate_button_no;

    CicoSCLifeCycleController *life_cycle_controller;
    /* my instance (for callback) */
    static CicoHSMenuWindow *menu_window_instance;
    bool m_showState;
    /* tile size    */
    static int  menu_tile_width;
    static int  menu_tile_height;

  protected:
    CicoHSMenuWindow operator=(const CicoHSMenuWindow&);
    CicoHSMenuWindow(const CicoHSMenuWindow&);
};
#endif

