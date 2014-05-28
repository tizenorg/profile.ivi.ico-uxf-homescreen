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
#include "CicoHSMenuWindow.h"
#include "CicoHSMenuTouch.h"
#include "CicoResourceConfig.h"
#include "CicoHomeScreen.h"
#include "CicoHSSystemState.h"
#include <stdio.h>

/*============================================================================*/
/* static members                                                             */
/*============================================================================*/
CicoHSMenuWindow *CicoHSMenuWindow::menu_window_instance;
int  CicoHSMenuWindow::menu_tile_width = 290;
int  CicoHSMenuWindow::menu_tile_height = 290;

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::CicoHSMenuWindo
 *          Constractor
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoHSMenuWindow::CicoHSMenuWindow(void)
{
    /*initialzie values*/
    terminate_mode = false;

    InitAppTiles();

    evas = NULL;
    canvas = NULL;
    rectangle = NULL;

    for (int ii = 0;ii < ICO_HS_MENU_MAX_TILE_NUM;ii++) {
        menu_tile[ii] = NULL;
    }
    for (int ii = 0;ii < ICO_HS_MENU_MAX_MENU_PAGE_NUM;ii++) {
        page_pointer[ii] = NULL;
    }

    surface = 0;

    menu_window_instance = this;

    life_cycle_controller = CicoHSLifeCycleController::getInstance();

    CicoResourceConfig::GetImagePath(img_dir_path, ICO_HS_MAX_PATH_BUFF_LEN);

    m_showState = false;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::~CicoHSMenuWindo
 *          Destractor
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoHSMenuWindow::~CicoHSMenuWindow(void)
{
    /* Do not somthing to do */
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::SetMenuBack
 *          create object and show (background of menu)
 *
 * @param[in]   none
 * @return      ERROR or OK
 */
/*--------------------------------------------------------------------------*/
int
CicoHSMenuWindow::SetMenuBack(void)
{
    char img_path[ICO_HS_MAX_PATH_BUFF_LEN];
    /* set menu back */
    /* image file name*/
    snprintf(img_path, sizeof(img_path), "%s%s",
             img_dir_path, ICO_HS_IMAGE_FILE_MENU_BACK_GROUND);

    /* set object*/
    if (CicoHSSystemState::getInstance()->getNightMode())   {
        rectangle = EvasObjectRectangleCreate(evas, 0, 0, 0, 178);
    }
    else {
        rectangle = EvasObjectRectangleCreate(evas, 120, 120, 120, 178);
    }
    evas_object_move(rectangle, 0, 0);
    evas_object_resize(rectangle, width, height);
    evas_object_show(rectangle);

    /* set object*/
    canvas = EvasObjectImageCreate(evas, img_path, NULL);
    int err = evas_object_image_load_error_get(canvas);
    if (err != EVAS_LOAD_ERROR_NONE) {
        ICO_ERR("CicoHSMenuWindow::SetMenuBack: backgound image is not exist");
        ICO_TRA("CicoHSMenuWindow::SetMenuBack Leave(ERR)");
        evas_object_del(canvas);
        return ICO_ERROR;
    }
    evas_object_event_callback_add(canvas, EVAS_CALLBACK_MOUSE_DOWN,
                                   CicoHSMenuTouch::TouchDownMenu, NULL);
    evas_object_event_callback_add(canvas, EVAS_CALLBACK_MOUSE_UP,
                                   CicoHSMenuTouch::TouchUpMenu, NULL);
    evas_event_callback_add(evas, EVAS_CALLBACK_CANVAS_FOCUS_OUT,
                            CicoHSMenuTouch::FocusOutEvas, (void *)canvas);

    evas_object_move(canvas, 0, 0);
    evas_object_resize(canvas, width, height);
    evas_object_show(canvas);

    ICO_TRA("CicoHSMenuWindow::SetMenuBack Leave(EOK)");
    return ICO_OK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::FreeMenuBack
 *          free object (background of menu)
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::FreeMenuBack(void)
{
    evas_object_del(canvas);
    evas_object_del(rectangle);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::GetLayout
 *          read layout file
 *
 * @param[in]   filename   layout file name
 * @param[out]   layout   layout data
 * @param[out]   num      num of tile
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::GetLayout(const char *filename,CicoLayoutInfo *layout,int *num)
{
    int ret;
    FILE *fp;
    *num = 0;
    fp = fopen(filename,"r");
    if(fp == NULL){
        return;
    }
    int idx =0;
    while( ( ret = fscanf( fp, "%d,%d,%d,%d,%d",
        &layout[idx].appidx, &layout[idx].page, &layout[idx].position,
        &layout[idx].tile_width, &layout[idx].tile_height ) ) != EOF ){
        idx++;
    }
    *num = idx;
    fclose(fp);

}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::GetCategory
 *          read category file
 *
 * @param[in]   filename   category file name
 * @param[out]  category   category data
 * @param[out]  num        num of category
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::GetCategory(const char *filename, int *category, int *num)
{
    int ret;
    FILE *fp;
    *num = 0;
    fp = fopen(filename,"r");
    if(fp == NULL){
        return;
    }
    int idx =0;
    while( ( ret = fscanf( fp, "%d", &category[idx]
        ) ) != EOF ){
        idx++;
    }
    *num = idx;
    fclose(fp);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::SetLayout
 *          write layout file
 *
 * @param[in]   filename   layout file name
 * @param[in]   layout   layout data
 * @param[in]   num      num of tile
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::SetLayout(const char *filename,CicoLayoutInfo *layout,int num)
{
    int ret;
    FILE *fp;

    fp = fopen(filename,"w+");
    if (fp == NULL) {
        return;
    }
    for (int ii = 0;ii < num;ii++) {
       ret = fprintf(fp, "%d,%d,%d,%d,%d\n",
                     layout[ii].appidx,layout[ii].page,layout[ii].position,
                     layout[ii].tile_width,layout[ii].tile_height);
       if (ret < 0) {
           break;
       }
    }
    fclose(fp);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::InitAppTiles
 *          Initialization object (tiles)
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::InitAppTiles(void)
{
    ICO_TRA("CicoHSMenuWindow::InitAppTiles Enter");

    current_page = 0;
    subcurrent_page = 0;

    for (int ii = 0; ii < ICO_HS_MENU_MAX_MENU_CATEGORY_NUM; ii++){
        category_info[ii].id= 0;
        category_info[ii].page = 0;
        category_info[ii].subpage_max = 0;
        category_info[ii].tile_num = 0;
    }

    all_tile_num = 0;
    all_page_num = 0;

    ICO_TRA("CicoHSMenuWindow::InitAppTiles Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::SetAppTiles
 *          create object and show (tiles)
 *
 * @param[in]   none
 * @return      ERROR or OK
 */
/*--------------------------------------------------------------------------*/
int
CicoHSMenuWindow::SetAppTiles(void)
{
    int tile_num = 0;
    int category_num = 0;
    int category[ICO_HS_MENU_MAX_TILE_NUM];
    int ctg_idx = 0;
    int position = 0;
    int page = 0;
    int subpage = 0;
    current_page = 0;

    /* get APP information */
    std::vector<CicoAilItems> aillist =
        life_cycle_controller->getAilList();

    /* get category infomation */
    GetCategory(ICO_HS_MENU_CATEGORY_FILE_PATH, category, &category_num);
    ICO_DBG("CicoHSMenuWindow::SetAppTiles :category_num %d", category_num);

    for (int ii = 0; ii < category_num ; ii++) {
       ICO_DBG("CicoHSMenuWindow::SetAppTiles :category[%d] %d", ii, category[ii]);
    }

    /* other category add */
    all_category_num = category_num + 1;

    /* page set */
    for (int ii = 0; ii < all_category_num ; ii++) {
        if(ii == 0) {
            /* other category */
            category_info[ii].id = ICO_HS_MENU_OTHER_CATEGORY_ID;
            category_info[ii].page = ii;
        }
        else {
            /* category */
            category_info[ii].id = category[ii - 1];
            category_info[ii].page = ii;
        }
    }

    /*first time layout*/
    for (unsigned int ii = 0; ii < aillist.size(); ii++) {

        /*all application num*/
        if((aillist[ii].m_noIcon) ||
                (strcmp(aillist[ii].m_group.c_str(),
                ICO_HS_GROUP_SPECIAL) == 0)) {
            ICO_DBG("CicoHSMenuWindow::SetAppTiles :ignore app appid = [%s] noIcon = [%d]",
                    aillist[ii].m_appid.c_str(),aillist[ii].m_noIcon);
            continue;
        }
        ICO_DBG("CicoHSMenuWindow::SetAppTiles :SetApp appid = [%s] noIcon =[%d]",
                aillist[ii].m_appid.c_str(),aillist[ii].m_noIcon);
        ICO_DBG("CicoHSMenuWindow::SetAppTile :aillist[%d].m_categoryID = [%d]",
                ii, aillist[ii].m_categoryID);

        /* Categories */
        for (ctg_idx = 0; ctg_idx < category_num ; ctg_idx++ ) {
            if (aillist[ii].m_categoryID == category[ctg_idx]) {
                SetCategoryInfo(category[ctg_idx]);
                GetTileInfo(category[ctg_idx], &page, &subpage, &position);
                break;
            }
        }

        /* Other categories */
        if (ctg_idx == category_num) {
            SetCategoryInfo(ICO_HS_MENU_OTHER_CATEGORY_ID);
            GetTileInfo(ICO_HS_MENU_OTHER_CATEGORY_ID,
                        &page, &subpage, &position);
        }

        /* put tile */
        menu_tile[tile_num] =
            new CicoHSMenuTile(aillist[ii].m_appid.c_str(),
            aillist[ii].m_icon.c_str(),page, subpage, position,
            CicoHSMenuWindow::Tile_Width(), CicoHSMenuWindow::Tile_Height());

        tile_num++;
    }

    /*menu num*/
    all_tile_num = tile_num;
    all_page_num = all_category_num;

    for (int ii = 0; ii < all_category_num ; ii++) {
        ICO_DBG("CicoHSMenuWindow::SetAppTile :category_info[%d].id = [%d]",
                ii, category_info[ii].id);
        ICO_DBG("CicoHSMenuWindow::SetAppTile :category_info[%d].subpage_max = [%d]",
                ii, category_info[ii].subpage_max);
        ICO_DBG("CicoHSMenuWindow::SetAppTile :category_info[%d].page = [%d]",
                ii, category_info[ii].page);
        ICO_DBG("CicoHSMenuWindow::SetAppTile :category_info[%d].tile_num = [%d]",
                ii , category_info[ii].tile_num);
    }

    /*in case of over max num*/
    if (all_page_num > ICO_HS_MENU_MAX_MENU_PAGE_NUM) {
       all_page_num = ICO_HS_MENU_MAX_MENU_PAGE_NUM;
    }

    /*make tiles*/
    for (int ii = 0; ii < tile_num; ii++) {
        if (menu_tile[ii] == NULL) {
            continue;
        }
        menu_tile[ii]->CreateObject(evas);
        if (menu_tile[ii]->GetPage() != 0) {
            /*out of window*/
            menu_tile[ii]->OffsetMove(width,0);
        }
        if (menu_tile[ii]->GetSubPage() != 0) {
            /*out of window*/
            menu_tile[ii]->OffsetMove(height,0);
        }
    }
    return ICO_OK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::FreeAppTiles
 *          free object (tiles)
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::FreeAppTiles(void)
{
    for (int ii = 0; ii < all_tile_num; ii++) {
        if (menu_tile[ii] == NULL) {
            continue;
        }
        menu_tile[ii]->FreeObject();
        delete menu_tile[ii];
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::RenewAppTiles
 *          renewal of a menu
 *
 * @param   none
 * @return  none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::RenewAppTiles(void)
{
    ICO_TRA("CicoHSMenuWindow::RenewAppTiles Enter");

    int cnt, cnt2;
    int ret;

    /* backup old data */
    int all_tile_num_org = all_tile_num;
    CicoHSMenuTile *menu_tile_org[ICO_HS_MENU_MAX_TILE_NUM];
    for (cnt=0; cnt < all_tile_num_org; cnt++) {
        menu_tile_org[cnt]=menu_tile[cnt];
    }

    /* initialization */
    InitAppTiles();

    /* set app tiles */
    ret = SetAppTiles();
    if(ret != ICO_OK){
        ICO_ERR("CicoHSMenuWindow::RenewAppTiles: could not make tiles.");
    }

    /* update app info list */
    CicoHomeScreen::RenewAppInfoList();

    /* set thumbnail from org */
    for (cnt=0; cnt < all_tile_num; cnt++) {
        if (menu_tile[cnt] == NULL) {
            continue;
        }
        for (cnt2=0; cnt2 < all_tile_num_org; cnt2++) {
            if (menu_tile_org[cnt2] == NULL) {
                continue;
            }
            if (strncmp(menu_tile[cnt]->GetAppId(),
                        menu_tile_org[cnt2]->GetAppId(), ICO_HS_MAX_PROCESS_NAME) == 0) {
                menu_tile[cnt]->SetOrgThumbnail( menu_tile_org[cnt2] );
                break;
            }
        }
    }

    /* free org app tiles */
    for (cnt2=0; cnt2 < all_tile_num_org; cnt2++) {
        if (menu_tile_org[cnt2] == NULL) {
            continue;
        }
        menu_tile_org[cnt2]->FreeObject();
        delete menu_tile_org[cnt2];
    }

    ICO_TRA("CicoHSMenuWindow::RenewAppTiles Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::SetPagePointer
 *          create object and show (page pointer)
 *
 * @param[in]   none
 * @return      ERROR or OK
 */
/*--------------------------------------------------------------------------*/
int
CicoHSMenuWindow::SetPagePointer(void)
{
    /*debug*/
    for (int ii = 0; ii < all_page_num; ii++) {

        char img_path[ICO_HS_MAX_PATH_BUFF_LEN];
        /* set menu back */
        /* image file name*/
        if(ii == current_page){
            snprintf(img_path,sizeof(img_path),"%s%s",
                     img_dir_path,ICO_HS_IMAGE_FILE_MENU_PAGEPOINTER_P);
        }else{
            snprintf(img_path,sizeof(img_path),"%s%s",
                     img_dir_path,ICO_HS_IMAGE_FILE_MENU_PAGEPOINTER_N);
        }
        /* set object*/
        page_pointer[ii] = EvasObjectImageCreate(evas, img_path, NULL);
        int err = evas_object_image_load_error_get(page_pointer[ii]);
        if (err != EVAS_LOAD_ERROR_NONE) {
            ICO_ERR("CicoHSMenuWindow::SetPagePointer: page pointer image is not exist");
            evas_object_del(page_pointer[ii]);
            return ICO_ERROR;
        }
        evas_object_event_callback_add(page_pointer[ii],
                                       EVAS_CALLBACK_MOUSE_DOWN,
                                       CicoHSMenuTouch::TouchDownMenu, NULL);
        evas_object_event_callback_add(page_pointer[ii],
                                       EVAS_CALLBACK_MOUSE_UP,
                                       CicoHSMenuTouch::TouchUpMenu, NULL);

        int pos_x = ICO_HS_MENU_PAGE_POINTER_START_POS_X +
                    (((width - ICO_HS_MENU_PAGE_POINTER_START_POS_X * 2) /
                    all_page_num ) * ii) +
                    (((width - ICO_HS_MENU_PAGE_POINTER_START_POS_X * 2) /
                    all_page_num ) / 2)  -
                    (ICO_HS_MENU_PAGE_POINTER_WIDTH / 2);
        evas_object_move(page_pointer[ii], pos_x,
                         ICO_HS_MENU_PAGE_POINTER_START_POS_Y);
        evas_object_resize(page_pointer[ii],
                         ICO_HS_MENU_PAGE_POINTER_WIDTH,
                         ICO_HS_MENU_PAGE_POINTER_HEIGHT);
        evas_object_show(page_pointer[ii]);
    }
    return ICO_OK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::FreePagePointer
 *          free object (page pointer)
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::FreePagePointer(void)
{
    for (int ii = 0; ii < all_page_num; ii++) {
        evas_object_del(page_pointer[ii]);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::SetTerminateButton
 *          create object and show (terminate button)
 *
 * @param[in]   none
 * @return      ERROR or OK
 */
/*--------------------------------------------------------------------------*/
int
CicoHSMenuWindow::SetTerminateButton(void)
{
    int err;
    char img_path[ICO_HS_MAX_PATH_BUFF_LEN];
    /* set menu back */
    /* image file name*/

    /* set object*/
    terminate_back = EvasObjectRectangleCreate(evas, 0, 0, 0, 178);
    evas_object_move(terminate_back, 0, 0);
    evas_object_resize(terminate_back, width, height);
    evas_object_layer_set(terminate_back, ICO_HS_MENU_TERMINATE_BUTTON_LAYER);
    /* set object*/
    snprintf(img_path,sizeof(img_path),"%s%s",
                     img_dir_path,ICO_HS_IMAGE_FILE_MENU_TERMINATE_YES);
    terminate_button_yes = EvasObjectImageCreate(evas, img_path, NULL);
    err = evas_object_image_load_error_get(terminate_button_yes);
    if (err != EVAS_LOAD_ERROR_NONE) {
        ICO_ERR("CicoHSMenuWindow::SetTerminateButton: image is not exist");
        evas_object_del(terminate_button_yes);
        return ICO_ERROR;
    }
    evas_object_event_callback_add(terminate_button_yes, EVAS_CALLBACK_MOUSE_UP,
                                       CicoHSMenuTouch::TouchUpTerminateYes, NULL);
    evas_object_move(terminate_button_yes, width / 2  - 100 - 64,
                                           height / 2 + 100);
    evas_object_resize(terminate_button_yes,100,64);
    evas_object_layer_set(terminate_button_yes, ICO_HS_MENU_TERMINATE_BUTTON_LAYER);

    /* set object*/
    snprintf(img_path,sizeof(img_path),"%s%s",
                     img_dir_path, ICO_HS_IMAGE_FILE_MENU_TERMINATE_NO);
    terminate_button_no = EvasObjectImageCreate(evas, img_path, NULL);
    err = evas_object_image_load_error_get(terminate_button_no);
    if (err != EVAS_LOAD_ERROR_NONE) {
        ICO_ERR("CicoHSMenuWindow::SetTerminateButton: image is not exist");
        evas_object_del(terminate_button_no);
        return ICO_ERROR;
    }
    evas_object_event_callback_add(terminate_button_no, EVAS_CALLBACK_MOUSE_UP,
                                       CicoHSMenuTouch::TouchUpTerminateNo, NULL);
    evas_object_move(terminate_button_no, width / 2  + 64,
                                           height / 2 + 100);
    evas_object_resize(terminate_button_no,100,64);
    evas_object_layer_set(terminate_button_no, ICO_HS_MENU_TERMINATE_BUTTON_LAYER);

    /* set object*/
    snprintf(img_path,sizeof(img_path),"%s%s",
                     img_dir_path,ICO_HS_IMAGE_FILE_MENU_TERMINATE_REALLY);
    terminate_really = EvasObjectImageCreate(evas, img_path, NULL);
    err = evas_object_image_load_error_get(terminate_really);
    if (err != EVAS_LOAD_ERROR_NONE) {
        ICO_ERR("CicoHSMenuWindow::SetTerminateButton: image is not exist");
        evas_object_del(terminate_really);
        return ICO_ERROR;
    }
    evas_object_move(terminate_really, width / 2 - 100,
                                          height / 2 - 100);
    evas_object_resize(terminate_really,200,64);
    evas_object_layer_set(terminate_really, ICO_HS_MENU_TERMINATE_BUTTON_LAYER);

    return ICO_OK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::FreeTeiminateButton
 *          free object (terminate button)
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::FreeTerminateButton(void)
{
    evas_object_del(terminate_back);
    evas_object_del(terminate_really);
    evas_object_del(terminate_button_yes);
    evas_object_del(terminate_button_no);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::ShowTerminateButton
 *          show terminate button
 *
 * @param[in]   none
 * @return      ERROR or OK
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::ShowTerminateButton(void)
{
   evas_object_show(terminate_back);
   evas_object_show(terminate_really);
   evas_object_show(terminate_button_yes);
   evas_object_show(terminate_button_no);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::HideTerminateButton
 *          show terminate button
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::HideTerminateButton(void)
{
   evas_object_hide(terminate_back);
   evas_object_hide(terminate_really);
   evas_object_hide(terminate_button_yes);
   evas_object_hide(terminate_button_no);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::SetPageCursor
 *          create object and show (page cursor)
 *
 * @param[in]   none
 * @return      ERROR or OK
 */
/*--------------------------------------------------------------------------*/
int
CicoHSMenuWindow::SetPageCursor(void)
{
    char img_path[ICO_HS_MAX_PATH_BUFF_LEN];

    /* up cursor */
    snprintf(img_path,sizeof(img_path),"%s%s",
                 img_dir_path,ICO_HS_IMAGE_FILE_MENU_PAGE_UP_CURSOR);

    /* set object*/
    page_up_cursor = EvasObjectImageCreate(evas, img_path, NULL);
    int err = evas_object_image_load_error_get(page_up_cursor);
    if (err != EVAS_LOAD_ERROR_NONE) {
        ICO_ERR("CicoHSMenuWindow::SetPagePointer: page up cursor image is not exist");
        evas_object_del(page_up_cursor);
        return ICO_ERROR;
    }

    evas_object_move(page_up_cursor, ICO_HS_MENU_PAGE_UP_CURSOR_START_POS_X,
                     ICO_HS_MENU_PAGE_UP_CURSOR_START_POS_Y);
    evas_object_resize(page_up_cursor,
                     ICO_HS_MENU_PAGE_UP_CURSOR_WIDTH,
                     ICO_HS_MENU_PAGE_UP_CURSOR_HEIGHT);

    /* down cursor */
    snprintf(img_path,sizeof(img_path),"%s%s",
                 img_dir_path,ICO_HS_IMAGE_FILE_MENU_PAGE_DOWN_CURSOR);

    /* set object*/
    page_down_cursor = EvasObjectImageCreate(evas, img_path, NULL);
    err = evas_object_image_load_error_get(page_down_cursor);
    if (err != EVAS_LOAD_ERROR_NONE) {
        ICO_ERR("CicoHSMenuWindow::SetPagePointer: page down cursor image is not exist");
        evas_object_del(page_down_cursor);
        return ICO_ERROR;
    }

    evas_object_move(page_down_cursor, ICO_HS_MENU_PAGE_DOWN_CURSOR_START_POS_X,
                     ICO_HS_MENU_PAGE_DOWN_CURSOR_START_POS_Y);
    evas_object_resize(page_down_cursor,
                     ICO_HS_MENU_PAGE_DOWN_CURSOR_WIDTH,
                     ICO_HS_MENU_PAGE_DOWN_CURSOR_HEIGHT);

    DspCtrlPageCursor();

    return ICO_OK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::FreePageCursor
 *          free object (page cursor)
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::FreePageCursor(void)
{
    evas_object_del(page_up_cursor);
    evas_object_del(page_down_cursor);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::CreateMenuWindow
 *          Create menu window (page pointer)
 *
 * @param[in]   pos_x   window position x
 * @param[in]   pos_y   window position y
 * @param[in]   width   window width
 * @param[in]   height  window height
 * @return      none
 */
/*--------------------------------------------------------------------------*/
int
CicoHSMenuWindow::CreateMenuWindow(int pos_x, int pos_y, int width, int height)
{
    int ret;

    /*create window*/
    ret = CreateWindow(ICO_HS_MENU_WINDOW_TITLE,
                       pos_x, pos_y, width, height, EINA_TRUE);
    if(ret != ICO_OK){
       return ret;
    }

    /* set tile size    */
    CicoHSMenuWindow::menu_tile_width = (width - (ICO_HS_MENUTILE_START_POS_X * 2)
                                         - (ICO_HS_MENUTILE_SPACE_TILE_AND_TILE
                                             * (ICO_HS_MENUTILE_ROW-1)))
                                        / ICO_HS_MENUTILE_ROW;
    if (CicoHSMenuWindow::menu_tile_width < 150)    {
        CicoHSMenuWindow::menu_tile_width = 150;
    }
    CicoHSMenuWindow::menu_tile_height = (height - (ICO_HS_STATUSBAR_WINDOW_HEIGHT
                                                    + ICO_HS_CONTROLBAR_WINDOW_HEIGHT
                                                    + ICO_HS_MENUTILE_HEIGHT + 8)
                                          - (ICO_HS_MENUTILE_SPACE_TILE_AND_TILE
                                             * (ICO_HS_MENUTILE_COLUMN-1)))
                                         / ICO_HS_MENUTILE_COLUMN;
    if (CicoHSMenuWindow::menu_tile_height < 150)   {
        CicoHSMenuWindow::menu_tile_height = 150;
    }
    ICO_DBG("CicoHSMenuWindow::CreateMenuWindow: tile size(w/h)=%d/%d",
            CicoHSMenuWindow::menu_tile_width, CicoHSMenuWindow::menu_tile_height);
    if (CicoHSMenuWindow::menu_tile_width > CicoHSMenuWindow::menu_tile_height) {
        CicoHSMenuWindow::menu_tile_width = CicoHSMenuWindow::menu_tile_height;
    }
    else    {
        CicoHSMenuWindow::menu_tile_height = CicoHSMenuWindow::menu_tile_width;
    }

    /*get evas*/
    evas = ecore_evas_get(window);
    if (!evas) {
        ICO_ERR("CicoHSMenuWindow::CreateMenuWindow: could not get evas.");
        return ICO_ERROR;
    }

    /* MenuBack */
    ret = SetMenuBack();
    if(ret != ICO_OK){
        ICO_ERR("CicoHSMenuWindow::CreateMenuWindow: could not make back ground.");
        FreeWindow();
        return ICO_ERROR;
    }
    /* App tiles*/
    ret = SetAppTiles();
    if(ret != ICO_OK){
        ICO_ERR("CicoHSMenuWindow::CreateMenuWindow: could not make tiles.");
        FreeMenuBack();
        FreeWindow();
        return ICO_ERROR;
    }

    /* Page Pointer */
    ret = SetPagePointer();
    if(ret != ICO_OK){
        ICO_ERR("CicoHSMenuWindow::CreateMenuWindow: could not make page pointer.");
        FreeAppTiles();
        FreeMenuBack();
        FreeWindow();
        return ICO_ERROR;
    }

    /* Page Cursor */
    ret = SetPageCursor();
    if(ret != ICO_OK){
        ICO_ERR("CicoHSMenuWindow::CreateMenuWindow: could not make page cursor.");
        FreePagePointer();
        FreeAppTiles();
        FreeMenuBack();
        FreeWindow();
        return ICO_ERROR;
    }

    /* Terminate Button */
    ret = SetTerminateButton();
    if(ret != ICO_OK){
        ICO_ERR("CicoHSMenuWindow::CreateMenuWindow: could not make teminate button.");
        FreePageCursor();
        FreePagePointer();
        FreeAppTiles();
        FreeMenuBack();
        FreeWindow();
        return ICO_ERROR;
    }

    /* Initialize Action*/
    CicoHSMenuTouch::Initialize(this);

    return ICO_OK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::FreeMenuWindow
 *          free menu window
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::FreeMenuWindow(void)
{
    FreeTerminateButton();
    FreePageCursor();
    FreePagePointer();
    FreeAppTiles();
    FreeMenuBack();
    FreeWindow();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::MoveToNextAnimation
 *          animation parts (move to next)
 *
 * @param[in]   data
 * @param[in]   pos
 * @return      EINA_TRUE
 */
/*--------------------------------------------------------------------------*/
Eina_Bool
CicoHSMenuWindow::MoveToNextAnimation(void *data,double pos)
{
    int current_page;
    double frame = ecore_animator_pos_map(pos, ECORE_POS_MAP_LINEAR, 0.5, 1);
    current_page = reinterpret_cast<int>(data);

    for (int ii = 0;ii < menu_window_instance->all_tile_num;ii++) {
        if (menu_window_instance->menu_tile[ii] == NULL) {
            continue;
        }
        if ((menu_window_instance->menu_tile[ii]->GetPage() != current_page -1) ||
            (menu_window_instance->menu_tile[ii]->GetSubPage() != 0)) {
            continue;
        }
        menu_window_instance->menu_tile[ii]->OffsetMove(-1 *
                              (menu_window_instance->width * frame),0);
    }

    for (int ii = 0;ii < menu_window_instance->all_tile_num;ii++) {
        if (menu_window_instance->menu_tile[ii] == NULL) {
            continue;
        }
        if ((menu_window_instance->menu_tile[ii]->GetPage() != current_page) ||
            (menu_window_instance->menu_tile[ii]->GetSubPage() != 0)) {
            continue;
        }
        menu_window_instance->menu_tile[ii]->
            OffsetMove(menu_window_instance->width
                       - (menu_window_instance->width * frame),0);
    }
    return EINA_TRUE;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::MoveToBackAnimation
 *          animation parts (move to back)
 *
 * @param[in]   data
 * @param[in]   pos
 * @return      EINA_TRUE
 */
/*--------------------------------------------------------------------------*/
Eina_Bool
CicoHSMenuWindow::MoveToBackAnimation(void *data,double pos)
{
    int current_page;
    double frame = ecore_animator_pos_map(pos, ECORE_POS_MAP_LINEAR, 0.5, 1);
    current_page = reinterpret_cast<int>(data);

    for (int ii = 0;ii < menu_window_instance->all_tile_num;ii++) {
        if (menu_window_instance->menu_tile[ii] == NULL) {
            continue;
        }
        if ((menu_window_instance->menu_tile[ii]->GetPage() != current_page) ||
            (menu_window_instance->menu_tile[ii]->GetSubPage() != 0)) {
            continue;
        }
        menu_window_instance->menu_tile[ii]->OffsetMove(-1 *
                                 menu_window_instance->width +
                                 (menu_window_instance->width * frame),0);
    }
    for (int ii = 0;ii < menu_window_instance->all_tile_num;ii++) {
        if (menu_window_instance->menu_tile[ii] == NULL) {
            continue;
        }
        if ((menu_window_instance->menu_tile[ii]->GetPage() != current_page + 1) ||
            (menu_window_instance->menu_tile[ii]->GetSubPage() != 0)) {
            continue;
        }
        menu_window_instance->menu_tile[ii]->
            OffsetMove(menu_window_instance->width * frame,0);
    }
    return EINA_TRUE;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::MoveToNextSubAnimation
 *          animation parts (move to next)
 *
 * @param[in]   data
 * @param[in]   pos
 * @return      EINA_TRUE
 */
/*--------------------------------------------------------------------------*/
Eina_Bool
CicoHSMenuWindow::MoveToNextSubAnimation(void *data,double pos)
{
    CicoCurrentPage *current_info;
    double frame = ecore_animator_pos_map(pos, ECORE_POS_MAP_LINEAR, 0.5, 1);
    current_info = (CicoCurrentPage *)(data);

    for (int ii = 0;ii < menu_window_instance->all_tile_num;ii++) {
        if (menu_window_instance->menu_tile[ii] == NULL) {
            continue;
        }
        if (menu_window_instance->menu_tile[ii]->GetSubPage()
            != current_info->subcurrent_page -1) {
            continue;
        }
        if (menu_window_instance->menu_tile[ii]->GetPage() != current_info->current_page) {
            continue;
        }
        menu_window_instance->menu_tile[ii]->
            OffsetMove(0, -1 * (menu_window_instance->height * frame));
    }

    for (int ii = 0;ii < menu_window_instance->all_tile_num;ii++) {
        if (menu_window_instance->menu_tile[ii] == NULL) {
            continue;
        }
        if (menu_window_instance->menu_tile[ii]->GetSubPage()
            != current_info->subcurrent_page) {
            continue;
        }

        if (menu_window_instance->menu_tile[ii]->GetPage() != current_info->current_page) {
            continue;
        }
        menu_window_instance->menu_tile[ii]->OffsetMove(0, menu_window_instance->height -
                                   (menu_window_instance->height * frame));
    }
    return EINA_TRUE;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::MoveToBackAnimation
 *          animation parts (move to back)
 *
 * @param[in]   data
 * @param[in]   pos
 * @return      EINA_TRUE
 */
/*--------------------------------------------------------------------------*/
Eina_Bool
CicoHSMenuWindow::MoveToBackSubAnimation(void *data,double pos)
{

    CicoCurrentPage *current_info;
    double frame = ecore_animator_pos_map(pos, ECORE_POS_MAP_LINEAR, 0.5, 1);
    current_info = (CicoCurrentPage *)(data);

    for (int ii = 0;ii < menu_window_instance->all_tile_num;ii++) {
        if (menu_window_instance->menu_tile[ii] == NULL) {
            continue;
        }
        if (menu_window_instance->menu_tile[ii]->GetSubPage()
            != current_info->subcurrent_page) {
            continue;
        }
        if (menu_window_instance->menu_tile[ii]->GetPage() != current_info->current_page) {
            continue;
        }
        menu_window_instance->menu_tile[ii]->
            OffsetMove(0, -1 * menu_window_instance->height +
                       (menu_window_instance->height * frame));
    }
    for (int ii = 0;ii < menu_window_instance->all_tile_num;ii++) {
        if (menu_window_instance->menu_tile[ii] == NULL) {
            continue;
        }
        if (menu_window_instance->menu_tile[ii]->GetSubPage()
            != current_info->subcurrent_page + 1) {
            continue;
        }
        if (menu_window_instance->menu_tile[ii]->GetPage() != current_info->current_page) {
            continue;
        }
        menu_window_instance->menu_tile[ii]->
            OffsetMove(0, menu_window_instance->height * frame);
    }
    return EINA_TRUE;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::GoNextMenu
 *          menu change to next
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::GoNextMenu(void)
{
    char img_path[ICO_HS_MAX_PATH_BUFF_LEN];

    if ((current_page >= all_page_num -1) || (subcurrent_page > 0)) {
        return;
    }

    /*page pointer*/
    snprintf(img_path, sizeof(img_path), "%s%s",
             img_dir_path, ICO_HS_IMAGE_FILE_MENU_PAGEPOINTER_N);
    evas_object_image_file_set(page_pointer[current_page], img_path, NULL);

    /* increment*/
    ++current_page;

    /* display cursor */
    DspCtrlPageCursor();

    /*page pointer*/
    snprintf(img_path, sizeof(img_path), "%s%s",
             img_dir_path, ICO_HS_IMAGE_FILE_MENU_PAGEPOINTER_P);
    evas_object_image_file_set(page_pointer[current_page], img_path, NULL);

    /*tile*/
    ecore_animator_frametime_set(1.0f / 30);
    ecore_animator_timeline_add(0.4, MoveToNextAnimation,
                                reinterpret_cast<void*>(current_page));
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::GoNextMenu
 *          menu change to back
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::GoBackMenu(void)
{
    char img_path[ICO_HS_MAX_PATH_BUFF_LEN];

    if((current_page <= 0) || (subcurrent_page > 0)){
        return;
    }

    /*page pointer*/
    snprintf(img_path,sizeof(img_path),"%s%s",img_dir_path,ICO_HS_IMAGE_FILE_MENU_PAGEPOINTER_N);
    evas_object_image_file_set(page_pointer[current_page], img_path, NULL);

    /*decrement*/
    --current_page;

    /* display cursor */
    DspCtrlPageCursor();

    /*page pointer*/
    snprintf(img_path,sizeof(img_path),"%s%s",img_dir_path,ICO_HS_IMAGE_FILE_MENU_PAGEPOINTER_P);
    evas_object_image_file_set(page_pointer[current_page], img_path, NULL);

    /*tile*/
    ecore_animator_frametime_set(1.0f / 30);
    ecore_animator_timeline_add(0.4, MoveToBackAnimation,
                                reinterpret_cast<void*>(current_page));

}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::DownNextMenu
 *          menu change to next
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::DownNextMenu(void)
{
    for (int i = 0; i < all_category_num; i++) {
        if (current_page == category_info[i].page) {
            if (subcurrent_page >= category_info[i].subpage_max){
                return;
            }
        }
    }

    /* increment*/
    ++subcurrent_page;

    /* display cursor */
    DspCtrlPageCursor();

    /* current page set */
    current_info.current_page = current_page;
    current_info.subcurrent_page = subcurrent_page;

    /*tile*/
    ecore_animator_frametime_set(1.0f / 30);
    ecore_animator_timeline_add(0.4, MoveToNextSubAnimation,
                                (void*)(&current_info));
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::UpBackMenu
 *          menu change to back
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::UpBackMenu(void)
{
    if(subcurrent_page <= 0){
        return;
    }

    /*decrement*/
    --subcurrent_page;

    /* display cursor */
    DspCtrlPageCursor();

    /* current page set */
    current_info.current_page = current_page;
    current_info.subcurrent_page = subcurrent_page;

    /*tile*/
    ecore_animator_frametime_set(1.0f / 30);
    ecore_animator_timeline_add(0.4, MoveToBackSubAnimation,
                                (void*)(&current_info));
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::DspCtrlPageCursor
 *          display control page cursor
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::DspCtrlPageCursor(void)
{
    int subpage_max = 0;
    for (int i = 0; i < all_category_num ; i++) {
        if (current_page == category_info[i].page) {
            subpage_max = category_info[i].subpage_max;
            break;
        }
    }

    if (subcurrent_page > 0) {
        evas_object_show(page_up_cursor);
    }
    else {
        evas_object_hide(page_up_cursor);
    }

    if (subcurrent_page < subpage_max) {
        evas_object_show(page_down_cursor);
    }
    else {
        evas_object_hide(page_down_cursor);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::SetCategoryInfo
 *          set category info
 *
 * @param[in]         id         menu category id
 * @return            none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::SetCategoryInfo(int id)
{
    for (int i = 0; i < all_category_num ; i++) {
        if (category_info[i].id == id) {
            /* sub page max */
            category_info[i].subpage_max =
                category_info[i].tile_num / ICO_HS_MENUTILE_NUM;

            /* tile num */
            category_info[i].tile_num++;
        }
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::GetTileInfo
 *          get tile info
 *
 * @param[in]       id         menu category id
 * @param[out]      page       menu page
 * @param[out]      subpage    menu sub page
 * @param[out]      position   menu tile position
 * @return          none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::GetTileInfo(int id, int *page, int *subpage, int *position)
{
    for (int i = 0; i < all_category_num ; i++) {
        if (category_info[i].id == id) {
            if (category_info[i].tile_num == 0) {
                ICO_ERR("CicoHSMenuWindow::GetTileInfo: number of tiles is 0.");
                return;
            }

            /* page */
            *page = category_info[i].page;
            /* sub page */
            *subpage = (category_info[i].tile_num -1) / ICO_HS_MENUTILE_NUM;
            /* pasition */
            *position = (category_info[i].tile_num -1) % ICO_HS_MENUTILE_NUM;
        }
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::SetMenuWindowID
 *          set appid and surface
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::SetMenuWindowID(const char *appid,int surface)
{
    strncpy(this->appid,appid,ICO_HS_MAX_PROCESS_NAME);
    this->surface = surface;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::Show
 *          show menu window
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::Show(ico_syc_animation_t *animation)
{
    ICO_TRA("CicoHSMenuWindow::Show Enter");

    // if regulation == true, forbid show window.
    if (true == CicoHSSystemState::getInstance()->getRegulation()) {
        return;
    }
    m_showState = true;

    if (true == life_cycle_controller->isAilRenew()) {
        ICO_DBG("CicoHSMenuWindow::Show : renew app tiles");
        RenewAppTiles();
        life_cycle_controller->ailRenewFlagOff();
    }

    // Show menu tiles
    for (int ii = 0; ii < all_tile_num; ii++) {
        if (menu_tile[ii])  {
            menu_tile[ii]->ShowMenu(true);
        }
    }

    ico_syc_show(appid, surface, animation);

    ICO_TRA("CicoHSMenuWindow::Show Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::Hide
 *          hide menu window
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::Hide(ico_syc_animation_t *animation)
{
    ICO_TRA("CicoHSMenuWindow::Hide Enter");

    if(terminate_mode == true){
        ChangeNormalMode();
    }
    m_showState = false;
    if (surface)    {
        ico_syc_hide(appid, surface, animation);
    }

    // Hide menu tiles
    for (int ii = 0; ii < all_tile_num; ii++) {
        if (menu_tile[ii])  {
            menu_tile[ii]->ShowMenu(false);
        }
    }
    ICO_TRA("CicoHSMenuWindow::Hide Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::ExecuteApp
 *          execute application
 *
 * @param[in]   appid   application id
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::ExecuteApp(const char *appid)
{
    if(terminate_mode == true){
        ICO_DBG("CicoHSMenuWindow::ExecuteApp: exet %s but terminate_mode", appid);
        return;
    }
    CicoHomeScreen::ExecuteApp(appid);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::TerminateApp
 *          teminate application
 *
 * @param[in]   appid   application id
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::TerminateApp(const char *appid)
{
    CicoHomeScreen::TerminateApp(appid);

    HideTerminateButton();
    ChangeNormalMode();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::ChangeTerminateMode
 *          change to terminate mode
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::ChangeTerminateMode(void)
{
    bool check = false;

    /*check */
    for (int ii = 0; ii < all_tile_num; ii++) {
        if(menu_tile[ii] == NULL){
            continue;
        }
        if(CicoHomeScreen::GetAppStatus(menu_tile[ii]->GetAppId()) == true){
            check = true;
            break;
        }
    }
    if(check == false){
        return;
    }

    /*show term icon*/
    for (int ii = 0; ii < all_tile_num; ii++) {
        if(menu_tile[ii] == NULL){
            continue;
        }
        if(CicoHomeScreen::GetAppStatus(menu_tile[ii]->GetAppId()) == false){
            continue;
        }
        menu_tile[ii]->ShowTermIcon();
    }
    terminate_mode = true;
    ICO_DBG("CicoHSMenuWindow::ChangeTerminateMode: change to terminate mode");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::ChangeNormalMode
 *          change to terminate mode
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::ChangeNormalMode(void)
{
    /*hide term icon*/
    for (int ii = 0; ii < all_tile_num; ii++) {
        if(menu_tile[ii] == NULL){
            continue;
        }
        menu_tile[ii]->HideTermIcon();
    }
    terminate_mode = false;
    ICO_DBG("CicoHSMenuWindow::ChangeNormalMode: change to normal mode");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::ValidMenuIcon
 *          tile is icon
 *
 * @param[in]   appid    application ID
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::ValidMenuIcon(const char *appid)
{
    ICO_DBG("CicoHSMenuWindow::ValidMenuIcon(%s)", appid ? appid : "(NULL)" );
    for (int ii = 0; ii < all_tile_num; ii++) {
        if (strncmp(menu_tile[ii]->GetAppId(), appid, ICO_HS_MAX_PROCESS_NAME) == 0) {
            menu_tile[ii]->ValidMenuIcon();
            break;
        }
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::ValidThumbnail
 *          tile is thumbnail
 *
 * @param[in]   appid    application ID
 * @param[in]   surface  surface
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::ValidThumbnail(const char *appid, int surface)
{
    for (int ii = 0; ii < all_tile_num; ii++) {
        if (strncmp(menu_tile[ii]->GetAppId(), appid, ICO_HS_MAX_PROCESS_NAME) == 0) {
            menu_tile[ii]->ValidThumbnail(surface);
            break;
        }
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::SetThumbnail
 *          tile is thumbnail
 *
 * @param[in]   appid    application ID
 * @param[in]   info     thumbnail information
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::SetThumbnail(const char *appid, ico_syc_thumb_info_t *info)
{
    for (int ii = 0; ii < all_tile_num; ii++) {
        if (strncmp(menu_tile[ii]->GetAppId(), appid, ICO_HS_MAX_PROCESS_NAME) == 0) {
            menu_tile[ii]->SetThumbnail(info);
            break;
        }
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::SetNightMode
 *          set night mode color theme chagne
 *
 * @param   none
 * @return  none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuWindow::SetNightMode(void)
{
    ICO_TRA("CicoHSMenuWindow::SetNightMode Enter");
    if (true == CicoHSSystemState::getInstance()->getNightMode()) {
        evas_object_color_set(rectangle,0,0,0,178);
    }
    else {
        evas_object_color_set(rectangle,120,120,120,178);
    }
    ICO_TRA("CicoHSMenuWindow::SetNightMode Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::Tile_Width
 *          menu tile width
 *
 * @param   none
 * @return  tile width
 */
/*--------------------------------------------------------------------------*/
int
CicoHSMenuWindow::Tile_Width(void)
{
    return CicoHSMenuWindow::menu_tile_width;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuWindow::Tile_Height
 *          menu tile height
 *
 * @param   none
 * @return  tile height
 */
/*--------------------------------------------------------------------------*/
int
CicoHSMenuWindow::Tile_Height(void)
{
    return CicoHSMenuWindow::menu_tile_height;
}
// vim: set expandtab ts=4 sw=4:
