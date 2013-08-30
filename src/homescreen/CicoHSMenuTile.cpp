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
#include "CicoHSMenuTile.h"
#include "CicoHSMenuTouch.h"

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTile::CicoHSMenuTile
 *          Constractor
 *
 * @param[in]   appid
 * @param[in]   icon_image_path
 * @param[in]   page
 * @param[in]   position
 * @param[in]   width
 * @param[in]   height
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoHSMenuTile::CicoHSMenuTile(const char *appid,
      const char *icon_image_path,int page,int position,int width,int height)
{
    thumbnail = NULL;
    if(appid != NULL){
        strncpy(this->appid,appid,ICO_HS_MAX_PROCESS_NAME);
    }
    if((icon_image_path != NULL) && (strlen(icon_image_path) != 0) ){
        strncpy(this->icon_image_path,icon_image_path,ICO_HS_MAX_PATH_BUFF_LEN);
    }else{
        strncpy(this->icon_image_path,ICO_HS_MENUTILE_DEFAULT_ICON_PATH
               ,ICO_HS_MAX_PATH_BUFF_LEN);
    }
    ICO_DBG("CicoHSMEnutTile::CicoHSMenuTile:image_path %s:%s",appid,this->icon_image_path);
    this->page = page;
    this->position = position;
    this->width = width;
    this->height = height;
    pos_x = GetPositionX();
    pos_y = GetPositionY();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTile::CicoHSMenuTile
 *          Destractor
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoHSMenuTile::~CicoHSMenuTile(void)
{

}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTile::CreateObject
 *          create tile object
 *
 * @param[in]   evas
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuTile::CreateObject(Evas *evas)
{
    /*tile*/
    tile = evas_object_image_filled_add(evas);
    evas_object_image_file_set(tile,icon_image_path, NULL);
    evas_object_move(tile,pos_x,pos_y);
    evas_object_resize(tile, width, height);
    evas_object_event_callback_add(tile, EVAS_CALLBACK_MOUSE_DOWN,
                                       CicoHSMenuTouch::TouchDownMenu,appid);
    evas_object_event_callback_add(tile, EVAS_CALLBACK_MOUSE_UP,
                                       CicoHSMenuTouch::TouchUpMenu,appid);
    evas_object_show(tile);

    /*term Icon*/
    term_icon = evas_object_image_filled_add(evas);
    evas_object_image_file_set(term_icon,ICO_HS_MENUTILE_TERM_ICON_PATH, NULL);
    evas_object_move(term_icon,pos_x + width - ICO_HS_MENUTILE_TERM_ICON_WIDTH
                     ,pos_y);
    evas_object_resize(term_icon, ICO_HS_MENUTILE_TERM_ICON_WIDTH,
                       ICO_HS_MENUTILE_TERM_ICON_HEIGHT);
    evas_object_event_callback_add(term_icon, EVAS_CALLBACK_MOUSE_UP,
                                       CicoHSMenuTouch::TouchUpTerm,appid);

}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTile::ShowTermIcon
 *          showing term icon
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuTile::ShowTermIcon()
{
    evas_object_show(term_icon);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTile::HideTermIcon
 *          hiding term icon
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuTile::HideTermIcon(void)
{
    evas_object_hide(term_icon);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTile::FreeObject
 *          free tile object
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuTile::FreeObject(void)
{
    if(tile != NULL){
       evas_object_del(tile);
    }
    if(term_icon != NULL){
       evas_object_del(term_icon);
    }
    
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTile::Resize
 *          resize tile 
 *
 * @param[in]   width         width
 * @param[in]   height        height
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuTile::Resize(int width,int height)
{
    this->width = width;
    this->height = height;
    evas_object_resize(tile, width, height);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTile::MovePosition
 *          move tile base position
 *
 * @param[in]   page        page
 * @param[in]   position    position of tile
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void 
CicoHSMenuTile::MovePosition(int page,int position)
{
    this->page = page;
    this->position= position;
    pos_x = GetPositionX();
    pos_y = GetPositionY();

    evas_object_move(tile,pos_x,pos_y);
    evas_object_move(term_icon,pos_x + width - ICO_HS_MENUTILE_TERM_ICON_WIDTH,pos_y);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTile::OffsetMove
 *          move from base position
 *
 * @param[in]   offset_x    offset_x from base position
 * @param[in]   offset_y    offset_y from base position
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void 
CicoHSMenuTile::OffsetMove(int offset_x,int offset_y)
{
    pos_x = GetPositionX() + offset_x;
    pos_y = GetPositionY() + offset_y;

    evas_object_move(tile,pos_x,pos_y);
    evas_object_move(term_icon,pos_x + width - ICO_HS_MENUTILE_TERM_ICON_WIDTH,pos_y);
}


/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTile::GetImagePath
 *          get icon image path
 *
 * @param[in]   none
 * @return      icon image path
 */
/*--------------------------------------------------------------------------*/
const char*
CicoHSMenuTile::GetImagePath(void)
{
    return icon_image_path;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTile::GetAppId
 *          get application id
 *
 * @param[in]   none
 * @return      application id
 */
/*--------------------------------------------------------------------------*/
const char*
CicoHSMenuTile::GetAppId(void)
{
    return appid;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTile::GetWidth
 *          get width
 *
 * @param[in]   none
 * @return      width
 */
/*--------------------------------------------------------------------------*/
int 
CicoHSMenuTile::GetWidth(void)
{
    return width;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTile::GetHeight
 *          get height
 *
 * @param[in]   none
 * @return      height
 */
/*--------------------------------------------------------------------------*/
int 
CicoHSMenuTile::GetHeight(void)
{
    return height;
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTile::GetPosition
 *          get position
 *
 * @param[in]   none
 * @return      position
 */
/*--------------------------------------------------------------------------*/
int
CicoHSMenuTile::GetPosition(void)
{
    return position;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTile::GetPage
 *          get page
 *
 * @param[in]   none
 * @return      page
 */
/*--------------------------------------------------------------------------*/
int
CicoHSMenuTile::GetPage(void)
{
    return page;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTile::GetPositionX
 *          get position x
 *
 * @param[in]   none
 * @return      position x
 */
/*--------------------------------------------------------------------------*/
int
CicoHSMenuTile::GetPositionX(void)
{
    int ret = 0;

    if((position == ICO_HS_MENUTILE_POSITION_0) ||
       (position == ICO_HS_MENUTILE_POSITION_3) ||
       (position == ICO_HS_MENUTILE_POSITION_6) ||
       (position == ICO_HS_MENUTILE_POSITION_9)){
        ret = ICO_HS_MENUTILE_START_POS_X;
    }
    else if((position == ICO_HS_MENUTILE_POSITION_1) ||
       (position == ICO_HS_MENUTILE_POSITION_4) ||
       (position == ICO_HS_MENUTILE_POSITION_7) ||
       (position == ICO_HS_MENUTILE_POSITION_10)){
        ret = ICO_HS_MENUTILE_START_POS_X + 
            (ICO_HS_MENUTILE_WIDTH + ICO_HS_MENUTILE_SPACE_TILE_AND_TILE);
    }
    else if((position == ICO_HS_MENUTILE_POSITION_2) ||
       (position == ICO_HS_MENUTILE_POSITION_5) ||
       (position == ICO_HS_MENUTILE_POSITION_8) ||
       (position == ICO_HS_MENUTILE_POSITION_11)){
        ret = ICO_HS_MENUTILE_START_POS_X + 
            ((ICO_HS_MENUTILE_WIDTH + ICO_HS_MENUTILE_SPACE_TILE_AND_TILE) * 2);
    }
    return ret;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTile::GetPositionY
 *          get position y
 *
 * @param[in]   none
 * @return      position y
 */
/*--------------------------------------------------------------------------*/
int
CicoHSMenuTile::GetPositionY(void)
{
    int ret = 0;

    if((position == ICO_HS_MENUTILE_POSITION_0) ||
       (position == ICO_HS_MENUTILE_POSITION_1) ||
       (position == ICO_HS_MENUTILE_POSITION_2)){
        ret = ICO_HS_MENUTILE_START_POS_Y;
    }
    else if((position == ICO_HS_MENUTILE_POSITION_3) ||
       (position == ICO_HS_MENUTILE_POSITION_4) ||
       (position == ICO_HS_MENUTILE_POSITION_5)){
        ret = ICO_HS_MENUTILE_START_POS_Y + 
              (ICO_HS_MENUTILE_HEIGHT + ICO_HS_MENUTILE_SPACE_TILE_AND_TILE);
    }
    else if((position == ICO_HS_MENUTILE_POSITION_6) ||
       (position == ICO_HS_MENUTILE_POSITION_7) ||
       (position == ICO_HS_MENUTILE_POSITION_8)){
        ret = ICO_HS_MENUTILE_START_POS_Y + 
              ((ICO_HS_MENUTILE_HEIGHT + ICO_HS_MENUTILE_SPACE_TILE_AND_TILE) * 2);
    }
    else if((position == ICO_HS_MENUTILE_POSITION_9) ||
       (position == ICO_HS_MENUTILE_POSITION_10) ||
       (position == ICO_HS_MENUTILE_POSITION_11)){
        ret = ICO_HS_MENUTILE_START_POS_Y + 
              ((ICO_HS_MENUTILE_HEIGHT + ICO_HS_MENUTILE_SPACE_TILE_AND_TILE) * 3);
    }
    return ret;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTile::ValidMenuIcon(
 *          tile is icon
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuTile::ValidMenuIcon(void)
{
    evas_object_image_file_set(tile,icon_image_path, NULL);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTile::ValidThumbnail(
 *          tile is thumbnail
 *
 * @param[in]   surface   surface
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuTile::ValidThumbnail(int surface)
{
    if(thumbnail != NULL){
        ico_syc_unmap_thumb(thumb_surface);
        thumbnail = NULL;
    }
    ico_syc_prepare_thumb(surface,0);
    thumb_surface = surface;

    ICO_DBG("CicoHSMenuTile::ValidThumbnail: prepare thumbnail 0x%d",surface);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTile::SetThumbnail
 *          tile is thumbnail
 *
 * @param[in]   surface   surface
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuTile::SetThumbnail(int surface)
{
    if(thumb_surface != surface){
        ico_syc_unmap_thumb(thumb_surface);
        thumbnail = NULL;
    }
    thumbnail = ico_syc_map_thumb(surface);
    if(thumbnail == NULL){
        ICO_ERR("CicoHSMenuTile::SetThumbnail: failed get thumbnail 0x%x",thumbnail);
        return;
    }
  
    ICO_DBG("CicoHSMenuTile::SetThumbnail: get thumbnail 0x%x",thumbnail->data);

    evas_object_image_data_set(tile,thumbnail->data);

}
