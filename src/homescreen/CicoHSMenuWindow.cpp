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
#include "CicoHomeScreen.h"
#include <stdio.h>

/*============================================================================*/
/* static members                                                             */
/*============================================================================*/
CicoHSMenuWindow *CicoHSMenuWindow::menu_window_instance;

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

    current_page = 0;
    all_tile_num = 0;
    all_page_num = 0;

    evas = NULL;
    canvas = NULL;
    rectangle = NULL;

    for(int ii = 0;ii < ICO_HS_MENU_MAX_TILE_NUM;ii++){
        menu_tile[ii] = NULL;
    }
    for(int ii = 0;ii < ICO_HS_MENU_MAX_MENU_PAGE_NUM;ii++){
        page_pointer[ii] = NULL;
    }

    surface = 0;   

    menu_window_instance = this;

    life_cycle_controller = CicoSCLifeCycleController::getInstance();

    CicoHomeScreenResourceConfig::GetImagePath(img_dir_path,
                                               ICO_HS_MAX_PATH_BUFF_LEN);
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
    snprintf(img_path,sizeof(img_path),"%s%s",img_dir_path,ICO_HS_IMAGE_FILE_MENU_BACK_GROUND);

    /* set object*/ 
    rectangle = evas_object_rectangle_add(evas);
    evas_object_color_set(rectangle,0,0,0,178); 
    evas_object_move(rectangle, 0, 0);
    evas_object_resize(rectangle, width,height);
    evas_object_show(rectangle);

    /* set object*/
    canvas = evas_object_image_filled_add(evas);
    evas_object_image_file_set(canvas, img_path, NULL);
    int err = evas_object_image_load_error_get(canvas);
    if (err != EVAS_LOAD_ERROR_NONE) {
        ICO_ERR("CicoHSMenuWindow::SetMenuBack: backgound image is not exist");
        evas_object_del(canvas);
        return ICO_ERROR;
    }
    evas_object_event_callback_add(canvas, EVAS_CALLBACK_MOUSE_DOWN,
                                       CicoHSMenuTouch::TouchDownMenu,NULL);
    evas_object_event_callback_add(canvas, EVAS_CALLBACK_MOUSE_UP,
                                       CicoHSMenuTouch::TouchUpMenu,NULL);
    evas_object_move(canvas, 0, 0);
    evas_object_resize(canvas, width,height);
    evas_object_show(canvas);

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
    if(fp == NULL){
        return;
    }
    for(int ii = 0;ii < num;ii++){
       ret = fprintf(fp,"%d,%d,%d,%d,%d\n",
                layout[ii].appidx,layout[ii].page,layout[ii].position,
                layout[ii].tile_width,layout[ii].tile_height);
       if(ret < 0){
           break;
       }
    }
    fclose(fp);
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
    current_page = 0;
    CicoLayoutInfo layout[ICO_HS_MENU_MAX_TILE_NUM];
   
    /* get APP information*/
    std::vector<CicoSCAilItems> aillist = 
        life_cycle_controller->getAilList();

    GetLayout(ICO_HS_MENU_LAYOUT_FILE_PATH,layout,&tile_num);
    if(tile_num != 0){
        /*layout file exist*/
        all_page_num = 0;
        for(int ii =0;ii < tile_num;ii++){
            
            /*put tile*/
            menu_tile[ii] = 
                new CicoHSMenuTile(aillist[layout[ii].appidx].m_appid.c_str(),
                aillist[layout[ii].appidx].m_icon.c_str(),
                layout[ii].page,layout[ii].position,
                layout[ii].tile_width,layout[ii].tile_height);
            /*update manu num*/
            if(layout[ii].page >= all_page_num){
                all_page_num = layout[ii].page + 1;
            }
        }
        /*menu num*/
        all_tile_num = tile_num;  
    }
    else{
        /*first time layout*/
        for(unsigned int ii = 0; ii < aillist.size(); ii++) {
            /*all application num*/
            if((aillist[ii].m_noIcon) ||
                    (strcmp(aillist[ii].m_group.c_str(), 
                     ICO_HS_GROUP_SPECIAL) == 0)) {
                ICO_DBG("CicoHSMenuWindow::SetAppTiles :ignore app %s %d",
                        aillist[ii].m_appid.c_str(),aillist[ii].m_noIcon);
                continue;
            }         
            ICO_DBG("CicoHSMenuWindow::SetAppTiles :SetApp %s %d",
                    aillist[ii].m_appid.c_str(),aillist[ii].m_noIcon);
            /* put tile */
            menu_tile[tile_num] = 
                new CicoHSMenuTile(aillist[ii].m_appid.c_str(),
                aillist[ii].m_icon.c_str(),
                tile_num / ICO_HS_MENUTILE_NUM,tile_num % ICO_HS_MENUTILE_NUM,
                ICO_HS_MENUTILE_WIDTH,ICO_HS_MENUTILE_HEIGHT);
            /* layout data */
            layout[tile_num].appidx = ii;
            layout[tile_num].page = tile_num / ICO_HS_MENUTILE_NUM;
            layout[tile_num].position = tile_num % ICO_HS_MENUTILE_NUM;
            layout[tile_num].tile_width = ICO_HS_MENUTILE_WIDTH;
            layout[tile_num].tile_height = ICO_HS_MENUTILE_HEIGHT;
             
            tile_num++;
        }
        /*write layout data*/
        SetLayout(ICO_HS_MENU_LAYOUT_FILE_PATH,layout,tile_num);

        /*menu num*/
        all_tile_num = tile_num;    
        all_page_num = all_tile_num / ICO_HS_MENUTILE_NUM;
        if(all_tile_num % ICO_HS_MENUTILE_NUM){
           ++all_page_num;
        }
    }
   
    /*in case of over max num*/
    if(all_page_num > ICO_HS_MENU_MAX_MENU_PAGE_NUM){
       all_page_num = ICO_HS_MENU_MAX_MENU_PAGE_NUM;
    }
 
    /*make tiles*/
    for (int ii = 0; ii < tile_num; ii++) {
        if(menu_tile[ii] == NULL){
            continue;
        }
        menu_tile[ii]->CreateObject(evas);
        if(menu_tile[ii]->GetPage() != 0){
            /*out of window*/
            menu_tile[ii]->OffsetMove(width,0);
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
    for(int ii = 0; ii < all_tile_num;ii++){
        if(menu_tile[ii] == NULL){
            continue;
        }
        menu_tile[ii]->FreeObject();
        delete menu_tile[ii];
    }
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
        page_pointer[ii] = evas_object_image_filled_add(evas);
        evas_object_image_file_set(page_pointer[ii], img_path, NULL);
        int err = evas_object_image_load_error_get(page_pointer[ii]);
        if (err != EVAS_LOAD_ERROR_NONE) {
            ICO_ERR("CicoHSMenuWindow::SetPagePointer: page pointer image is not exist");
            evas_object_del(page_pointer[ii]);
            return ICO_ERROR;
        }
        evas_object_event_callback_add(page_pointer[ii], 
                                       EVAS_CALLBACK_MOUSE_DOWN,
                                       CicoHSMenuTouch::TouchDownMenu,NULL);
        evas_object_event_callback_add(page_pointer[ii],
                                       EVAS_CALLBACK_MOUSE_UP,
                                       CicoHSMenuTouch::TouchUpMenu,NULL);

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
    terminate_back = evas_object_rectangle_add(evas);
    evas_object_color_set(terminate_back,0,0,0,178); 
    evas_object_move(terminate_back, 0, 0);
    evas_object_resize(terminate_back, width,height);

    /* set object*/
    snprintf(img_path,sizeof(img_path),"%s%s",
                     img_dir_path,ICO_HS_IMAGE_FILE_MENU_TERMINATE_YES);
    terminate_button_yes = evas_object_image_filled_add(evas);
    evas_object_image_file_set(terminate_button_yes, img_path, NULL);
    err = evas_object_image_load_error_get(terminate_button_yes);
    if (err != EVAS_LOAD_ERROR_NONE) {
        ICO_ERR("CicoHSMenuWindow::SetTerminateButton: image is not exist");
        evas_object_del(terminate_button_yes);
        return ICO_ERROR;
    }
    evas_object_event_callback_add(terminate_button_yes, EVAS_CALLBACK_MOUSE_UP,
                                       CicoHSMenuTouch::TouchUpTerminateYes,
                                       NULL);
    evas_object_move(terminate_button_yes, width / 2  - 100 - 64,
                                           height / 2 + 100);
    evas_object_resize(terminate_button_yes,100,64);

    /* set object*/
    snprintf(img_path,sizeof(img_path),"%s%s",
                     img_dir_path, ICO_HS_IMAGE_FILE_MENU_TERMINATE_NO);
    terminate_button_no = evas_object_image_filled_add(evas);
    evas_object_image_file_set(terminate_button_no, img_path, NULL);
    err = evas_object_image_load_error_get(terminate_button_no);
    if (err != EVAS_LOAD_ERROR_NONE) {
        ICO_ERR("CicoHSMenuWindow::SetTerminateButton: image is not exist");
        evas_object_del(terminate_button_no);
        return ICO_ERROR;
    }
    evas_object_event_callback_add(terminate_button_no, EVAS_CALLBACK_MOUSE_UP,
                                       CicoHSMenuTouch::TouchUpTerminateNo,
                                       NULL);
    evas_object_move(terminate_button_no, width / 2  + 64,
                                           height / 2 + 100);
    evas_object_resize(terminate_button_no,100,64);

    /* set object*/
    snprintf(img_path,sizeof(img_path),"%s%s",
                     img_dir_path,ICO_HS_IMAGE_FILE_MENU_TERMINATE_REALLY);
    terminate_really = evas_object_image_filled_add(evas);
    evas_object_image_file_set(terminate_really, img_path, NULL);
    err = evas_object_image_load_error_get(terminate_really);
    if (err != EVAS_LOAD_ERROR_NONE) {
        ICO_ERR("CicoHSMenuWindow::SetTerminateButton: image is not exist");
        evas_object_del(terminate_really);
        return ICO_ERROR;
    }
    evas_object_move(terminate_really, width / 2 - 100,
                                          height / 2 - 100);
    evas_object_resize(terminate_really,200,64);
    
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
CicoHSMenuWindow::CreateMenuWindow(int pos_x,int pos_y,int width,int height)
{
    int ret;
    
    /*create window*/
    ret = CreateWindow(ICO_HS_MENU_WINDOW_TITLE,
                       pos_x,pos_y,width,height,EINA_TRUE);
    if(ret != ICO_OK){
       return ret;
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

    /* Page Pointer */
    ret = SetTerminateButton();
    if(ret != ICO_OK){
        ICO_ERR("CicoHSMenuWindow::CreateMenuWindow: could not make teminate button.");
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
    
    for(int ii = 0;ii < menu_window_instance->all_tile_num;ii++){
        if(menu_window_instance->menu_tile[ii] == NULL){
            continue;
        }
        if(menu_window_instance->menu_tile[ii]->GetPage() != current_page -1){
            continue;
        }
        
        menu_window_instance->menu_tile[ii]->OffsetMove(-1 * 
                              (menu_window_instance->width * frame),0);
    } 

    for(int ii = 0;ii < menu_window_instance->all_tile_num;ii++){
        if(menu_window_instance->menu_tile[ii] == NULL){
            continue;
        }
        if(menu_window_instance->menu_tile[ii]->GetPage()  != current_page){
            continue;
        }

        menu_window_instance->menu_tile[ii]->OffsetMove(menu_window_instance->width -
                                   (menu_window_instance->width * frame),0);
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

    for(int ii = 0;ii < menu_window_instance->all_tile_num;ii++){
        if(menu_window_instance->menu_tile[ii] == NULL){
            continue;
        }
        if(menu_window_instance->menu_tile[ii]->GetPage()  != current_page){
            continue;
        }
        menu_window_instance->menu_tile[ii]->OffsetMove(-1 * 
                                 menu_window_instance->width + 
                                 (menu_window_instance->width * frame),0);
    }
    for(int ii = 0;ii < menu_window_instance->all_tile_num;ii++){
        if(menu_window_instance->menu_tile[ii] == NULL){
            continue;
        }
        if(menu_window_instance->menu_tile[ii]->GetPage()  != current_page + 1){
            continue;
        }
        menu_window_instance->menu_tile[ii]->OffsetMove(menu_window_instance->width * frame,0);
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
    if(current_page >= all_page_num -1){
        return;
    }
    /*page pointer*/
    snprintf(img_path,sizeof(img_path),"%s%s",img_dir_path,ICO_HS_IMAGE_FILE_MENU_PAGEPOINTER_N);
    evas_object_image_file_set(page_pointer[current_page], img_path, NULL);
    /* increment*/
    ++current_page;
    /*page pointer*/
    snprintf(img_path,sizeof(img_path),"%s%s",img_dir_path,ICO_HS_IMAGE_FILE_MENU_PAGEPOINTER_P);
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

    if(current_page <= 0){
        return;
    }
    /*page pointer*/
    snprintf(img_path,sizeof(img_path),"%s%s",img_dir_path,ICO_HS_IMAGE_FILE_MENU_PAGEPOINTER_N);
    evas_object_image_file_set(page_pointer[current_page], img_path, NULL);
    /*decrement*/
    --current_page; 
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
    ico_syc_show(appid,surface,animation);
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
    if(terminate_mode == true){
        ChangeNormalMode();
    }
    ico_syc_hide(appid,surface,animation);
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
    for(int ii = 0; ii < all_tile_num; ii++) {
        if(strncmp(menu_tile[ii]->GetAppId(),appid,ICO_HS_MAX_PROCESS_NAME)==0){
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
CicoHSMenuWindow::ValidThumbnail(const char *appid,int surface)
{
    for(int ii = 0; ii < all_tile_num; ii++) {
        if(strncmp(menu_tile[ii]->GetAppId(),appid,ICO_HS_MAX_PROCESS_NAME)==0){
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
 * @param[in]   surface  surface 
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void 
CicoHSMenuWindow::SetThumbnail(const char *appid,int surface)
{
    for(int ii = 0; ii < all_tile_num; ii++) {
        if(strncmp(menu_tile[ii]->GetAppId(),appid,ICO_HS_MAX_PROCESS_NAME)==0){
            menu_tile[ii]->SetThumbnail(surface);
            break;
        }
    }
}



