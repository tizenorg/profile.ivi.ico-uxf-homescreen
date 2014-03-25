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
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ico_window_mgr.h>

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
                               const char *icon_image_path,
                               int page, int subpage,
                               int position, int width, int height)
{
    ICO_TRA("CicoHSMenuTile::CicoHSMenuTile Enter");

    if (appid != NULL) {
        strncpy(this->appid, appid, ICO_HS_MAX_PROCESS_NAME);
    }
    if ((icon_image_path != NULL) && (strlen(icon_image_path) != 0)) {
        strncpy(this->icon_image_path,icon_image_path,ICO_HS_MAX_PATH_BUFF_LEN);
    }
    else {
        strncpy(this->icon_image_path,ICO_HS_MENUTILE_DEFAULT_ICON_PATH,
                ICO_HS_MAX_PATH_BUFF_LEN);
    }
    ICO_DBG("CicoHSMenuTile::CicoHSMenuTile:image_path %s:%s",
            appid, this->icon_image_path);
    thumb.surface = 0;
    thumb.fbcount = 0;
    thumb.pixel_data = NULL;
    this->page = page;
    this->subpage = subpage;
    this->position = position;
    this->width = width;
    this->height = height;
    pos_x = GetPositionX();
    pos_y = GetPositionY();

    (void) mkdir(ICO_HS_THUMB_ICODIR, 0755);
    (void) mkdir(ICO_HS_THUMB_ICODIR ICO_HS_THUMB_FILEDIR, 0755);

    ICO_TRA("CicoHSMenuTile::CicoHSMenuTile Leave");
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
    ICO_DBG("CicoHSMenuTile::CreateObject Enter(appid=%08x<%s>)", (int)this->appid, appid);

    /*initial vaule*/
    menu_evas = evas;
    menu_show = false;
    app_running = false;
    thumb_tile = NULL;
    small_icon = NULL;
    thumb_reduce_x = ICO_HS_MENUTILE_THUMBNAIL_REDUCE_PIX;
    thumb_reduce_y = ICO_HS_MENUTILE_THUMBNAIL_REDUCE_PIX;

    /*tile*/
    tile = evas_object_image_filled_add(evas);
    evas_object_image_file_set(tile, icon_image_path, NULL);
    evas_object_move(tile, pos_x, pos_y);
    evas_object_resize(tile, width, height);
    evas_object_event_callback_add(tile, EVAS_CALLBACK_MOUSE_DOWN,
                                   CicoHSMenuTouch::TouchDownMenu, appid);
    evas_object_event_callback_add(tile, EVAS_CALLBACK_MOUSE_UP,
                                   CicoHSMenuTouch::TouchUpMenu, appid);
    icon = tile;
    evas_object_show(icon);

    /*term Icon*/
    term_icon = evas_object_image_filled_add(evas);
    evas_object_image_file_set(term_icon, ICO_HS_MENUTILE_TERM_ICON_PATH, NULL);
    evas_object_move(term_icon, pos_x + width - ICO_HS_MENUTILE_TERM_ICON_WIDTH, pos_y);
    evas_object_resize(term_icon, ICO_HS_MENUTILE_TERM_ICON_WIDTH,
                       ICO_HS_MENUTILE_TERM_ICON_HEIGHT);
    evas_object_event_callback_add(term_icon, EVAS_CALLBACK_MOUSE_DOWN,
                                   CicoHSMenuTouch::TouchDownTerm, appid);
    evas_object_event_callback_add(term_icon, EVAS_CALLBACK_MOUSE_UP,
                                   CicoHSMenuTouch::TouchUpTerm, appid);
    ICO_DBG("CicoHSMenuTile::CreateObject Leave(%s Tile=(%d,%d) w/h=%d/%d",
            appid, pos_x, pos_y, width, height);
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
    evas_object_raise(term_icon);
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
    char    sWork[80];

    ICO_DBG("CicoHSMenuTile::FreeObject(appid=%08x<%s>)", (int)this->appid, appid);

    if (thumb.surface)  {
        sprintf(sWork, "%s/%08x.pixel", ICO_HS_THUMB_ICODIR ICO_HS_THUMB_FILEDIR,
                thumb.surface);
        (void) unlink(sWork);
        ico_syc_unmap_thumb(thumb.surface);
        thumb.surface = 0;
    }
    if (tile != NULL){
        evas_object_del(tile);
        tile = NULL;
    }
    if (small_icon != NULL){
        evas_object_del(small_icon);
        small_icon = NULL;
    }
    if (thumb_tile != NULL)  {
        evas_object_del(thumb_tile);
        thumb_tile = NULL;
    }
    if (term_icon != NULL)  {
        evas_object_del(term_icon);
        term_icon = NULL;
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTile::Resize
 *          resize tile(currently unused)
 *
 * @param[in]   width         width
 * @param[in]   height        height
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuTile::Resize(int width, int height)
{
    ICO_DBG("CicoHSMenuTile::Resize (%d,%d)-(%d,%d) Unused",
            this->width, this->height, width, height);
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
CicoHSMenuTile::MovePosition(int page, int position)
{
    this->page = page;
    this->position= position;
    pos_x = GetPositionX();
    pos_y = GetPositionY();

    ICO_DBG("CicoHSMenuTile::MovePosition(appid=%08x<%s> tile=(%d,%d))",
            (int)this->appid, appid, pos_x, pos_y);

    evas_object_move(tile, pos_x, pos_y);
    if (thumb_tile) {
        evas_object_move(thumb_tile, pos_x + thumb_reduce_x, pos_y + thumb_reduce_y);
    }
    if (small_icon) {
        evas_object_move(small_icon,
                         pos_x + thumb_reduce_x - ICO_HS_MENUTILE_THUMBNAIL_REDUCE_PIX2,
                         pos_y + height - thumb_reduce_y - height
                             / ICO_HS_MENUTILE_THUMBNAIL_REDUCTION
                             + ICO_HS_MENUTILE_THUMBNAIL_REDUCE_PIX2);
    }
    evas_object_move(term_icon, pos_x + width - ICO_HS_MENUTILE_TERM_ICON_WIDTH, pos_y);
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
CicoHSMenuTile::OffsetMove(int offset_x, int offset_y)
{
    pos_x = GetPositionX() + offset_x;
    pos_y = GetPositionY() + offset_y;

    ICO_DBG("CicoHSMenuTile::OffsetMove(appid=%08x<%s> offset=%d,%d tile=(%d,%d) obj=%08x %08x %08x)",
            (int)this->appid, appid, offset_x, offset_y, pos_x, pos_y,
            (int)this->icon, (int)this->tile, (int)this->thumb_tile);

    evas_object_move(tile, pos_x, pos_y);
    if (thumb_tile) {
        evas_object_move(thumb_tile, pos_x + thumb_reduce_x, pos_y + thumb_reduce_y);
    }
    if (small_icon) {
        evas_object_move(small_icon,
                         pos_x + thumb_reduce_x - ICO_HS_MENUTILE_THUMBNAIL_REDUCE_PIX2,
                         pos_y + height - thumb_reduce_y - height
                             / ICO_HS_MENUTILE_THUMBNAIL_REDUCTION
                             + ICO_HS_MENUTILE_THUMBNAIL_REDUCE_PIX2);
    }
    evas_object_move(term_icon, pos_x + width - ICO_HS_MENUTILE_TERM_ICON_WIDTH, pos_y);
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
 * @brief   CicoHSMenuTile::GetSubPage
 *          get sub page
 *
 * @param[in]   none
 * @return      page
 */
/*--------------------------------------------------------------------------*/
int
CicoHSMenuTile::GetSubPage(void)
{
    return subpage;
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

    if ((position == ICO_HS_MENUTILE_POSITION_0) ||
        (position == ICO_HS_MENUTILE_POSITION_3) ||
        (position == ICO_HS_MENUTILE_POSITION_6) ||
        (position == ICO_HS_MENUTILE_POSITION_9))   {
        ret = ICO_HS_MENUTILE_START_POS_X;
    }
    else if ((position == ICO_HS_MENUTILE_POSITION_1) ||
             (position == ICO_HS_MENUTILE_POSITION_4) ||
             (position == ICO_HS_MENUTILE_POSITION_7) ||
             (position == ICO_HS_MENUTILE_POSITION_10)) {
        ret = ICO_HS_MENUTILE_START_POS_X +
              (CicoHSMenuWindow::Tile_Width() + ICO_HS_MENUTILE_SPACE_TILE_AND_TILE);
    }
    else if ((position == ICO_HS_MENUTILE_POSITION_2) ||
             (position == ICO_HS_MENUTILE_POSITION_5) ||
             (position == ICO_HS_MENUTILE_POSITION_8) ||
             (position == ICO_HS_MENUTILE_POSITION_11)) {
        ret = ICO_HS_MENUTILE_START_POS_X +
              ((CicoHSMenuWindow::Tile_Width() + ICO_HS_MENUTILE_SPACE_TILE_AND_TILE) * 2);
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

    if ((position == ICO_HS_MENUTILE_POSITION_0) ||
        (position == ICO_HS_MENUTILE_POSITION_1) ||
        (position == ICO_HS_MENUTILE_POSITION_2))   {
        ret = ICO_HS_MENUTILE_START_POS_Y;
    }
    else if ((position == ICO_HS_MENUTILE_POSITION_3) ||
             (position == ICO_HS_MENUTILE_POSITION_4) ||
             (position == ICO_HS_MENUTILE_POSITION_5))  {
        ret = ICO_HS_MENUTILE_START_POS_Y +
              (CicoHSMenuWindow::Tile_Height() + ICO_HS_MENUTILE_SPACE_TILE_AND_TILE);
    }
    else if ((position == ICO_HS_MENUTILE_POSITION_6) ||
             (position == ICO_HS_MENUTILE_POSITION_7) ||
             (position == ICO_HS_MENUTILE_POSITION_8))  {
        ret = ICO_HS_MENUTILE_START_POS_Y +
              ((CicoHSMenuWindow::Tile_Height() + ICO_HS_MENUTILE_SPACE_TILE_AND_TILE) * 2);
    }
    else if ((position == ICO_HS_MENUTILE_POSITION_9) ||
             (position == ICO_HS_MENUTILE_POSITION_10) ||
             (position == ICO_HS_MENUTILE_POSITION_11)) {
        ret = ICO_HS_MENUTILE_START_POS_Y +
              ((CicoHSMenuWindow::Tile_Height() + ICO_HS_MENUTILE_SPACE_TILE_AND_TILE) * 3);
    }
    return ret;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTile::ValidMenuIcon
 *          tile is icon(app terminated)
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuTile::ValidMenuIcon(void)
{
    if (app_running)    {
        ICO_DBG("CicoHSMenuTile::ValidMenuIcon: %s show icon", appid);
        app_running = false;
        if (thumb.pixel_data)   {
            free(thumb.pixel_data);
            thumb.pixel_data = NULL;
        }
        if (icon == thumb_tile) {
            icon = tile;
            if (thumb_tile) {
                evas_object_hide(thumb_tile);
            }
            if (tile)   {
                evas_object_show(tile);
            }
        }
        if (small_icon) {
            evas_object_hide(small_icon);
        }
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTile::ValidThumbnail
 *          tile is thumbnail(app started)
 *
 * @param[in]   surface   surface
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuTile::ValidThumbnail(int surface)
{
    char    sWork[80];

    ICO_DBG("CicoHSMenuTile::ValidThumbnail(appid=%08x<%s>) run=%d surf=%08x",
            (int)this->appid, appid, app_running, surface);

    if ((! app_running) || (surface == 0))  {
        if (thumb.surface != 0) {
            sprintf(sWork, "%s/%08x.pixel", ICO_HS_THUMB_ICODIR ICO_HS_THUMB_FILEDIR,
                    thumb.surface);
            (void) unlink(sWork);
            ico_syc_unmap_thumb(thumb.surface);
            // delete image and texture
            if ((surface == 0) && (thumb.pixel_data))   {
                free(thumb.pixel_data);
                thumb.pixel_data = NULL;
            }
        }
        thumb.surface = surface;
        if (surface)    {
            app_running = true;
            sprintf(sWork, "%s/%08x.pixel", ICO_HS_THUMB_ICODIR ICO_HS_THUMB_FILEDIR,
                    thumb.surface);
            (void) unlink(sWork);
            ico_syc_map_thumb(thumb.surface,
                              menu_show ? ICO_HS_MENUTILE_THUMBNAIL_FPS_SHOW :
                                          ICO_HS_MENUTILE_THUMBNAIL_FPS_HIDE, sWork);
            if (! small_icon)   {
                small_icon = evas_object_image_filled_add(menu_evas);
                evas_object_image_file_set(small_icon, icon_image_path, NULL);
                evas_object_move(small_icon,
                                 pos_x + thumb_reduce_x
                                     - ICO_HS_MENUTILE_THUMBNAIL_REDUCE_PIX2,
                                 pos_y + height - thumb_reduce_y - height
                                     / ICO_HS_MENUTILE_THUMBNAIL_REDUCTION
                                     + ICO_HS_MENUTILE_THUMBNAIL_REDUCE_PIX2);
                evas_object_resize(small_icon, width / ICO_HS_MENUTILE_THUMBNAIL_REDUCTION,
                                   height / ICO_HS_MENUTILE_THUMBNAIL_REDUCTION);
                evas_object_event_callback_add(small_icon, EVAS_CALLBACK_MOUSE_DOWN,
                                               CicoHSMenuTouch::TouchDownMenu, appid);
                evas_object_event_callback_add(small_icon, EVAS_CALLBACK_MOUSE_UP,
                                               CicoHSMenuTouch::TouchUpMenu, appid);
                evas_object_raise(small_icon);
                evas_object_raise(term_icon);
            }
            evas_object_show(small_icon);
            ICO_DBG("CicoHSMenuTile::ValidThumbnail: %s(%08x) show small icon",
                    appid, surface);
        }
        else    {
            app_running = false;
            if (icon == thumb_tile) {
                ICO_DBG("CicoHSMenuTile::ValidThumbnail: %s change thumb->icon", appid);
                icon = tile;
                if (thumb_tile) {
                    evas_object_hide(thumb_tile);
                }
                evas_object_show(tile);
            }
            if (small_icon) {
                evas_object_hide(small_icon);
            }
            ICO_DBG("CicoHSMenuTile::ValidThumbnail: %s(%08x) hide small icon",
                    appid, surface);
        }
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTile::SetThumbnail
 *          tile is thumbnail
 *
 * @param[in]   info      thumbnail information, if NULL reset thumbnail
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuTile::SetThumbnail(ico_syc_thumb_info_t *info)
{
    Evas_Object         *old_icon = icon;
    struct ico_uifw_image_buffer *pixelbuf = NULL;
    int                 svx, svy;
    int                 unmap;
    int                 fd;
    char                sWork[80];

    ICO_DBG("CicoHSMenuTile::SetThumbnail(appid=%08x<%s>) info=%08x surf=%08x",
            (int)this->appid, appid, (int)info, info ? info->surface : 0);

    if ((info == NULL) || (info->surface == 0)) {
        unmap = 1;
    }
    else    {
        unmap = 0;
        if (thumb.surface != info->surface) {
            if (thumb.surface != 0) {
                ICO_DBG("CicoHSMenuTile::SetThumbnail: surface change(%08x->%08x)",
                        thumb.surface, info->surface);
                ico_syc_unmap_thumb(thumb.surface);
                sprintf(sWork, "%s/%08x.pixel", ICO_HS_THUMB_ICODIR ICO_HS_THUMB_FILEDIR,
                        thumb.surface);
                (void) unlink(sWork);
            }
            thumb.surface = info->surface;
            sprintf(sWork, "%s/%08x.pixel", ICO_HS_THUMB_ICODIR ICO_HS_THUMB_FILEDIR,
                    thumb.surface);
            (void) unlink(sWork);
            ico_syc_map_thumb(thumb.surface,
                              menu_show ? ICO_HS_MENUTILE_THUMBNAIL_FPS_SHOW :
                                          ICO_HS_MENUTILE_THUMBNAIL_FPS_HIDE, sWork);
        }
        else    {
            sprintf(sWork, "%s/%08x.pixel", ICO_HS_THUMB_ICODIR ICO_HS_THUMB_FILEDIR,
                    thumb.surface);
        }
        thumb.type = info->type;
        thumb.width = info->width;
        thumb.height = info->height;
        thumb.stride = info->stride;
        thumb.format = info->format;
#if 0       /* too many log */
        ICO_DBG("CicoHSMenuTile::SetThumbnail: make thumbnail %s(%08x) "
                "type=%d w/h/s=%d/%d/%d tile w/h=%d/%d",
                appid, thumb.surface, thumb.type,
                thumb.width, thumb.height, thumb.stride, width, height);
#endif
        if ((info->width <= 1) || (info->height <= 1))  {
            ICO_DBG("CicoHSMenuTile::SetThumbnail: small surface(%d,%d) skip",
                    info->width, info->height);
        }
        else    {
            // create thumbnail image
            svx = thumb_reduce_x;
            svy = thumb_reduce_y;
            if (thumb.width > (thumb.height + 64))  {
                thumb_reduce_x = ICO_HS_MENUTILE_THUMBNAIL_REDUCE_PIX;
                thumb_reduce_y = height / ICO_HS_MENUTILE_THUMBNAIL_REDUCE_RATE;
            }
            else if (thumb.width < (thumb.height - 64)) {
                thumb_reduce_y = ICO_HS_MENUTILE_THUMBNAIL_REDUCE_PIX;
                thumb_reduce_x = width / ICO_HS_MENUTILE_THUMBNAIL_REDUCE_RATE;
            }
            else    {
                thumb_reduce_x = ICO_HS_MENUTILE_THUMBNAIL_REDUCE_PIX;
                thumb_reduce_y = ICO_HS_MENUTILE_THUMBNAIL_REDUCE_PIX;
            }
            if (! thumb_tile)   {
                thumb_tile = evas_object_image_filled_add(menu_evas);
                evas_object_resize(thumb_tile, width - thumb_reduce_x * 2,
                                               height - thumb_reduce_y * 2);
                evas_object_move(thumb_tile, pos_x + thumb_reduce_x,
                                 pos_y + thumb_reduce_y);
                evas_object_event_callback_add(thumb_tile, EVAS_CALLBACK_MOUSE_DOWN,
                                               CicoHSMenuTouch::TouchDownMenu, appid);
                evas_object_event_callback_add(thumb_tile, EVAS_CALLBACK_MOUSE_UP,
                                               CicoHSMenuTouch::TouchUpMenu, appid);
                ICO_DBG("CicoHSMenuTile::SetThumbnail: create thumb_tile %s "
                        "tile=(%d+%d,%d+%d)", appid,
                        pos_x, thumb_reduce_x, pos_y, thumb_reduce_y);
                if (small_icon) {
                    evas_object_move(small_icon,
                                     pos_x + thumb_reduce_x
                                         - ICO_HS_MENUTILE_THUMBNAIL_REDUCE_PIX2,
                                     pos_y + height - thumb_reduce_y - height
                                         / ICO_HS_MENUTILE_THUMBNAIL_REDUCTION
                                         + ICO_HS_MENUTILE_THUMBNAIL_REDUCE_PIX2);
                    evas_object_raise(small_icon);
                }
                evas_object_raise(term_icon);
            }
            else if ((svx != thumb_reduce_x) || (svy != thumb_reduce_y))    {
                evas_object_resize(thumb_tile, width - thumb_reduce_x * 2,
                                               height - thumb_reduce_y * 2);
                evas_object_move(thumb_tile, pos_x + thumb_reduce_x,
                                 pos_y + thumb_reduce_y);
                if (small_icon) {
                    evas_object_move(small_icon,
                                     pos_x + thumb_reduce_x
                                         - ICO_HS_MENUTILE_THUMBNAIL_REDUCE_PIX2,
                                     pos_y + height - thumb_reduce_y - height
                                         / ICO_HS_MENUTILE_THUMBNAIL_REDUCTION
                                         + ICO_HS_MENUTILE_THUMBNAIL_REDUCE_PIX2);
                }
            }
            /* read surface image pixel         */
            int bufsize = ((thumb.width * thumb.height * 4 + 4096 - 1) / 4096) * 4095;
            if ((! thumb.pixel_data) || (bufsize > thumb.pixel_bufsize))    {
                if (thumb.pixel_data)   free(thumb.pixel_data);
                thumb.pixel_data = (char *)malloc(bufsize);
                thumb.pixel_bufsize = bufsize;
            }
            if (thumb.pixel_data)   {
                fd = open(sWork, O_RDONLY, 0644);
                if ((fd < 0) ||
                    (read(fd, thumb.pixel_data, bufsize) <= 0)) {
                    ICO_ERR("CicoHSMenuTile::SetThumbnail: can not read pixel file(%s)", sWork);
                    unmap = 1;
                }
                if (fd >= 0)    {
                    close(fd);
                    (void) unlink(sWork);
                }
                if (unmap == 0) {
                    evas_object_image_data_update_add(
                                    thumb_tile, 0, 0, thumb.width, thumb.height);
                    icon = thumb_tile;
                    evas_object_image_size_set(thumb_tile, thumb.width, thumb.height);
                    evas_object_image_data_set(thumb_tile, thumb.pixel_data);
                    evas_object_image_filled_set(thumb_tile, EINA_TRUE);
                    evas_object_resize(thumb_tile, width - thumb_reduce_x * 2,
                                       height - thumb_reduce_y * 2);
                    evas_object_move(thumb_tile,
                                     pos_x + thumb_reduce_x, pos_y + thumb_reduce_y);
                }
            }
            else    {
                ICO_ERR("CicoHSMenuTile::SetThumbnail: can not malloc pixel buffer");
                unmap = 1;
            }
        }
    }

    if (unmap > 0)  {
        ICO_DBG("CicoHSMenuTile::SetThumbnail: unmap thumbnail %08x", thumb.surface);
        if (thumb.surface)  {
            sprintf(sWork, "%s/%08x.pixel", ICO_HS_THUMB_ICODIR ICO_HS_THUMB_FILEDIR,
                    thumb.surface);
            (void) unlink(sWork);
            ico_syc_unmap_thumb(thumb.surface);
            thumb.surface = 0;
        }
        if (thumb.pixel_data)   {
            free(thumb.pixel_data);
            thumb.pixel_data = NULL;
        }
        icon = tile;
    }

    if (icon != old_icon)   {
        if (old_icon)   {
            evas_object_hide(old_icon);
        }
        evas_object_show(icon);
        if (small_icon) {
            if (icon == thumb_tile) {
                evas_object_show(small_icon);
                // remake thumbnail for first build image
                CicoHSMenuTile::SetThumbnail(info);
            }
            else    {
                evas_object_hide(small_icon);
            }
        }
    }
    if (pixelbuf != NULL)   {
        // free shared memory pixel buffer
        pixelbuf->reftime = pixelbuf->settime;
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSMenuTile::ShowMenu
 *          change menu show/hide for live thumbnail update cycle
 *
 * @param[in]   show    surface
 * @return      nonmenu show(true)/fide(false)
 */
/*--------------------------------------------------------------------------*/
void
CicoHSMenuTile::ShowMenu(bool show)
{
    char    sWork[80];
    menu_show = show;
    if ((thumb_tile) && (thumb.surface != 0)) {
        sprintf(sWork, "%s/%08x.pixel", ICO_HS_THUMB_ICODIR ICO_HS_THUMB_FILEDIR,
                thumb.surface);
        (void) unlink(sWork);
        ico_syc_map_thumb(thumb.surface,
                          menu_show ? ICO_HS_MENUTILE_THUMBNAIL_FPS_SHOW :
                                      ICO_HS_MENUTILE_THUMBNAIL_FPS_HIDE, sWork);
    }
}
// vim: set expandtab ts=4 sw=4:
