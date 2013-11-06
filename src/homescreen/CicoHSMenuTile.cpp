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
/* static members                                                             */
/*============================================================================*/
static int _CicoHSMenuTile_initialized = 0;
struct _CicoHSMenuTile_glfunc CicoHSMenuTile::glfunc;

static void SetYinvert(Evas_Object *obj);

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
                               const char *icon_image_path, int page, int subpage,
                               int position, int width, int height)
{
    if (_CicoHSMenuTile_initialized == 0)  {
        // Initialize and setting OpenGL/EGL functions
        ICO_DBG("CicoHSMenuTile::CicoHSMenuTile: initialize OpenGL/EGL functions");
        _CicoHSMenuTile_initialized = 1;

        CicoHSMenuTile::glfunc.egl_display
            = eglGetDisplay((EGLNativeDisplayType)ecore_wl_display_get());
        if (CicoHSMenuTile::glfunc.egl_display) {
            CicoHSMenuTile::glfunc.create_image
                = (PFNEGLCREATEIMAGEKHRPROC)eglGetProcAddress("eglCreateImageKHR");
            CicoHSMenuTile::glfunc.image_target_texture_2d
                = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)
                                        eglGetProcAddress("glEGLImageTargetTexture2DOES");
            CicoHSMenuTile::glfunc.destroy_image
                = (PFNEGLDESTROYIMAGEKHRPROC)eglGetProcAddress("eglDestroyImageKHR");
            if ((! CicoHSMenuTile::glfunc.create_image) ||
                (! CicoHSMenuTile::glfunc.image_target_texture_2d) ||
                (! CicoHSMenuTile::glfunc.destroy_image))   {
                ICO_ERR("CicoHSMenuTile::CicoHSMenuTile: can not get EGL functions");
                _CicoHSMenuTile_initialized = -1;
            }
        }
        else    {
            _CicoHSMenuTile_initialized = -1;
            ICO_ERR("CicoHSMenuTile::CicoHSMenuTile: can not initialize OpenGL/EGL");
        }
    }

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
    ICO_DBG("CicoHSMenuTile::CicoHSMenuTile:image_path %s:%s",appid,this->icon_image_path);
    thumb.surface = 0;
    thumb.fbcount = 0;
    this->page = page;
    this->subpage = subpage;
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
    evas_object_event_callback_add(term_icon, EVAS_CALLBACK_MOUSE_UP,
                                   CicoHSMenuTouch::TouchUpTerm, appid);
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
    if (thumb.surface)  {
        ico_syc_unmap_thumb(thumb.surface);
        thumb.surface = 0;
    }
    if (thumb.image)    {
        CicoHSMenuTile::glfunc.destroy_image(
                            CicoHSMenuTile::glfunc.egl_display, thumb.image);
        thumb.image = NULL;
    }
    if (thumb.texture)  {
        glDeleteTextures(1, &thumb.texture);
        thumb.texture = 0;
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
        if (icon == thumb_tile) {
            icon = tile;
            evas_object_hide(thumb_tile);
            evas_object_show(tile);
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
    if ((! app_running) || (surface == 0))  {
        if (thumb.surface != 0) {
            ico_syc_unmap_thumb(thumb.surface);
            // delete image and texture
            if (thumb.image)    {
                CicoHSMenuTile::glfunc.destroy_image(
                                    CicoHSMenuTile::glfunc.egl_display, thumb.image);
                thumb.image = NULL;
            }
            if (thumb.texture)  {
                glDeleteTextures(1, &thumb.texture);
                thumb.texture = 0;
            }
        }
        thumb.surface = surface;
        if (surface)    {
            app_running = true;
            ico_syc_map_thumb(thumb.surface, menu_show ? ICO_HS_MENUTILE_THUMBNAIL_FPS_SHOW :
                                                         ICO_HS_MENUTILE_THUMBNAIL_FPS_HIDE);
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
            ICO_DBG("CicoHSMenuTile::ValidThumbnail: %s show thumbnail %08x",
                    appid, surface);
        }
        else    {
            app_running = false;
            if (icon == thumb_tile) {
                icon = tile;
                evas_object_hide(thumb_tile);
                evas_object_show(tile);
            }
            if (small_icon) {
                evas_object_hide(small_icon);
            }
            ICO_DBG("CicoHSMenuTile::ValidThumbnail: %s hide thumbnail %08x",
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
    EGLint              attribs[9];
    Evas_Object         *old_icon = icon;
    Evas_Native_Surface nsurf;
    int                 svx, svy;

    if ((info == NULL) ||
        (thumb.surface != info->surface)) {
        ICO_DBG("CicoHSMenuTile::SetThumbnail: unmap thumbnail %x", thumb.surface);
        if (thumb.surface)  {
            ico_syc_unmap_thumb(thumb.surface);
            // delete image and texture
            if (thumb.image)    {
                CicoHSMenuTile::glfunc.destroy_image(
                                CicoHSMenuTile::glfunc.egl_display, thumb.image);
                thumb.image = NULL;
            }
            if (thumb.texture)  {
                glDeleteTextures(1, &thumb.texture);
                thumb.texture = 0;
            }
        }
        if (info != NULL)   {
            thumb.surface = info->surface;
            ico_syc_map_thumb(thumb.surface,
                              menu_show ? ICO_HS_MENUTILE_THUMBNAIL_FPS_SHOW :
                                          ICO_HS_MENUTILE_THUMBNAIL_FPS_HIDE);
        }
    }
    if ((info != NULL) && (info->name != 0) && (info->width > 1) && (info->height > 1)) {
        thumb.surface = info->surface;
        thumb.name = info->name;
        thumb.width = info->width;
        thumb.height = info->height;
        thumb.stride = info->stride;
        thumb.format = info->format;
        ICO_DBG("CicoHSMenuTile::SetThumbnail: create %s(%08x) "
                "name=%d w/h/s=%d/%d/%d tile w/h=%d/%d",
                appid, thumb.surface, thumb.name,
                thumb.width, thumb.height, thumb.stride, width, height);

        // delete image and texture
        if (thumb.image)    {
            CicoHSMenuTile::glfunc.destroy_image(
                            CicoHSMenuTile::glfunc.egl_display, thumb.image);
            thumb.image = NULL;
        }
        if (thumb.texture)  {
            glDeleteTextures(1, &thumb.texture);
            thumb.texture = 0;
        }

        // create image and texture
        if (_CicoHSMenuTile_initialized > 0)  {
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
            attribs[0] = EGL_WIDTH;
            attribs[1] = thumb.width;
            attribs[2] = EGL_HEIGHT;
            attribs[3] = thumb.height;
            attribs[4] = EGL_DRM_BUFFER_FORMAT_MESA;
            attribs[5] = EGL_DRM_BUFFER_FORMAT_ARGB32_MESA;     /* support only ARGB    */
            attribs[6] = EGL_DRM_BUFFER_STRIDE_MESA;
            attribs[7] = thumb.stride / 4;
            attribs[8] = EGL_NONE;

            thumb.image = CicoHSMenuTile::glfunc.create_image(
                              CicoHSMenuTile::glfunc.egl_display, EGL_NO_CONTEXT,
                              EGL_DRM_BUFFER_MESA, (EGLClientBuffer)thumb.name, attribs);
            if (thumb.image)    {
#if 0           /* too many logs    */
                ICO_DBG("CicoHSMenuTile::SetThumbnail: create image = %08x",
                        (int)thumb.image);
#endif
                /* create texture from image    */
                thumb.texture = 0;
                glGenTextures(1, &thumb.texture);
                glBindTexture(GL_TEXTURE_2D, thumb.texture);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                CicoHSMenuTile::glfunc.image_target_texture_2d(GL_TEXTURE_2D, thumb.image);
#if 0           /* too many logs    */
                ICO_DBG("CicoHSMenuTile::SetThumbnail: create texture = %08x",
                        (int)thumb.texture);
#endif
                /* draw texture to screen       */
                memset(&nsurf, 0, sizeof(nsurf));
                nsurf.version = EVAS_NATIVE_SURFACE_VERSION;
                nsurf.type = EVAS_NATIVE_SURFACE_OPENGL;
                nsurf.data.opengl.texture_id = thumb.texture;

                thumb.fbcount ++;
                if (thumb.fbcount <= 0) thumb.fbcount = 1;
                nsurf.data.opengl.framebuffer_id = thumb.fbcount;

                nsurf.data.opengl.internal_format = GL_BGRA_EXT;
                nsurf.data.opengl.format = GL_BGRA_EXT;
                nsurf.data.opengl.x = 0;
                nsurf.data.opengl.y = 0;
                nsurf.data.opengl.w = thumb.width;
                nsurf.data.opengl.h = thumb.height / 4;

                evas_object_image_native_surface_set(thumb_tile, &nsurf);

                // set Y invert to native OpenGL object (Evas bug fix)
                SetYinvert(thumb_tile);

                evas_object_image_data_update_add(thumb_tile,
                                                  0, 0, thumb.width, thumb.height);
                icon = thumb_tile;

                evas_object_image_size_set(thumb_tile, thumb.width, thumb.height);
                evas_object_resize(thumb_tile, width - thumb_reduce_x * 2,
                                               height - thumb_reduce_y * 2);
                evas_object_move(thumb_tile, pos_x + thumb_reduce_x, pos_y + thumb_reduce_y);
            }
            else    {
                ICO_ERR("CicoHSMenuTile::SetThumbnail: can not create image");
                icon = tile;
            }
        }
        else    {
            ICO_DBG("CicoHSMenuTile::SetThumbnail: OpenGL/EGL initialize error");
            icon = tile;
        }
    }
    else    {
        ICO_DBG("CicoHSMenuTile::SetThumbnail: no info(%08x) or name(%08x)",
                (int)info, info ? (int)info->name : 0);
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
        }
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
    menu_show = show;
    if ((thumb_tile) && (thumb.surface != 0)) {
        ico_syc_map_thumb(thumb.surface,
                          menu_show ? ICO_HS_MENUTILE_THUMBNAIL_FPS_SHOW :
                                      ICO_HS_MENUTILE_THUMBNAIL_FPS_HIDE);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   SetYinvert (static function)
*           set Y invert of OpenGL native object
 *
 * @param[in]   obj         target OpenGL native object
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
SetYinvert(Evas_Object *obj)
{
    // inport from evas-1.7.8/src/lib/canvas/evas_object_image.c _Evas_Object_Image
    struct local_Evas_Object_Image
    {
       uint32_t         magic;

       struct {
          int                  spread;
          Evas_Coord_Rectangle fill;
          struct {
             short         w, h, stride;
          } image;
          struct {
             short         l, r, t, b;
             unsigned char fill;
             double        scale;
          } border;

          Evas_Object   *source;
          Evas_Map      *defmap;
          const char    *file;
          const char    *key;
          int           frame;
          Evas_Colorspace cspace;

          unsigned char smooth_scale : 1;
          unsigned char has_alpha :1;
          unsigned char opaque :1;
          unsigned char opaque_valid :1;
       } cur, prev;

       int              pixels_checked_out;
       int              load_error;
       Eina_List        *pixel_updates;

       struct {
          unsigned char scale_down_by;
          double        dpi;
          short         w, h;
          struct {
             short      x, y, w, h;
          } region;
          Eina_Bool  orientation : 1;
       } load_opts;

       struct {
          Evas_Object_Image_Pixels_Get_Cb get_pixels;
          void                            *get_pixels_data;
       } func;

       Evas_Video_Surface video;
       unsigned int video_caps;

       const char       *tmpf;
       int              tmpf_fd;

       Evas_Image_Scale_Hint   scale_hint;
       Evas_Image_Content_Hint content_hint;

       void             *engine_data;

       unsigned char    changed : 1;
       unsigned char    dirty_pixels : 1;
       unsigned char    filled : 1;
       unsigned char    proxyrendering : 1;
       unsigned char    preloading : 1;
       unsigned char    video_rendering : 1;
       unsigned char    video_surface : 1;
       unsigned char    video_visible : 1;
       unsigned char    created : 1;
       unsigned char    proxyerror : 1;
    };

    // inport from evas-1.7.8/src/lib/include/evas_common.h  _RGBA_Image_Loadopts
    struct local_RGBA_Image_Loadopts
    {
       int              scale_down_by; // if > 1 then use this
       double           dpi; // if > 0.0 use this
       unsigned int     w, h; // if > 0 use this
       unsigned int     degree;//if>0 there is some info related with rotation
       struct {
          unsigned int  x, y, w, h;
       } region;

       Eina_Bool        orientation;
    };
    // inport from evas-1.7.8/src/modules/engines/gl_common/evas_gl_common.h  _Evas_GL_Image
    struct local_Evas_GL_Image
    {
       void *gc;
       void *im;
       void *tex;
       struct local_RGBA_Image_Loadopts load_opts;
       int              references;
       // if im->im == NULL, it's a render-surface so these here are used
       int              w, h;
       struct {
          int           space;
          void          *data;
          unsigned char no_free : 1;
       } cs;

       struct {
          void          *data;
          struct {
             void (*bind)   (void *data, void *image);
             void (*unbind) (void *data, void *image);
             void (*free)   (void *data, void *image);
             void *data;
          } func;
          int           yinvert;
          int           target;
          int           mipmap;
          unsigned char loose : 1;
       } native;

       int scale_hint, content_hint;
       int csize;

       Eina_List        *filtered;

       unsigned char    dirty : 1;
       unsigned char    cached : 1;
       unsigned char    alpha : 1;
       unsigned char    tex_only : 1;
    };
    // inport evas-1.7.8/src/lib/include/evas_private.h  _Evas_Object
    struct local_Evas_Object
    {
       EINA_INLIST;

       uint32_t                 magic;

       const char               *type;
       void                     *layer;

       struct {
          Evas_Map              *map;
          Evas_Object           *clipper;
          Evas_Object           *mask;
          Evas_Object           *map_parent;
          double                scale;
          Evas_Coord_Rectangle  geometry;
          Evas_Coord_Rectangle  bounding_box;
          struct {
             struct {
                Evas_Coord      x, y, w, h;
                unsigned char   r, g, b, a;
                Eina_Bool       visible : 1;
                Eina_Bool       dirty : 1;
             } clip;
          } cache;
          short                 layer;
          struct {
             unsigned char      r, g, b, a;
          } color;
          Eina_Bool             usemap : 1;
          Eina_Bool             valid_map : 1;
          Eina_Bool             visible : 1;
          Eina_Bool             have_clipees : 1;
          Eina_Bool             anti_alias : 1;
          Evas_Render_Op        render_op : 4;

          Eina_Bool             valid_bounding_box : 1;
          Eina_Bool             cached_surface : 1;
          Eina_Bool             parent_cached_surface : 1;
       } cur, prev;

       struct {
          void                  *surface; // surface holding map if needed
          int                   surface_w, surface_h; // current surface w & h alloc
       } map;

       Evas_Map                 *cache_map;
       char                     *name;

       void                     *interceptors;

       struct {
          Eina_List             *elements;
       } data;

       Eina_List                *grabs;

       void                     *callbacks;

       struct {
          Eina_List             *clipees;
          Eina_List             *changes;
       } clip;

       void *func;

       void                     *object_data;
       struct {
          Evas_Smart            *smart;
          Evas_Object           *parent;
       } smart;

       struct {
          Eina_List             *proxies;
          void                  *surface;
          int                   w,h;
          Eina_Bool             redraw;
       } proxy;

    #if 0 // filtering disabled
       Evas_Filter_Info           *filter;
    #endif

       void                     *size_hints;

       void                     *spans;

       int                      last_mouse_down_counter;
       int                      last_mouse_up_counter;
       int                      mouse_grabbed;

       int                      last_event;
       Evas_Callback_Type       last_event_type;

       struct {
            int                 in_move, in_resize;
       } doing;

      /* ptr array + data blob holding all interfaces private data for
       * this object */
       void                     **interface_privates;

       unsigned int             ref;

       unsigned char            delete_me;

       unsigned char            recalculate_cycle;
       Eina_Clist               calc_entry;

       Evas_Object_Pointer_Mode pointer_mode : 2;
       Eina_Bool                store : 1;
       Eina_Bool                pass_events : 1;
       Eina_Bool                freeze_events : 1;
       Eina_Bool                repeat_events : 1;
       struct  {
          Eina_Bool             pass_events : 1;
          Eina_Bool             pass_events_valid : 1;
          Eina_Bool             freeze_events : 1;
          Eina_Bool             freeze_events_valid : 1;
       } parent_cache;
       Eina_Bool                restack : 1;
       Eina_Bool                is_active : 1;
       Eina_Bool                precise_is_inside : 1;
       Eina_Bool                is_static_clip : 1;

       Eina_Bool                render_pre : 1;
       Eina_Bool                rect_del : 1;
       Eina_Bool                mouse_in : 1;
       Eina_Bool                pre_render_done : 1;
       Eina_Bool                intercepted : 1;
       Eina_Bool                focused : 1;
       Eina_Bool                in_layer : 1;
       Eina_Bool                no_propagate : 1;
       Eina_Bool                changed : 1;
       Eina_Bool                changed_move : 1;
       Eina_Bool                changed_color : 1;
       Eina_Bool                changed_map : 1;
       Eina_Bool                changed_pchange : 1;
       Eina_Bool                del_ref : 1;

       Eina_Bool                is_frame : 1;
       Eina_Bool                child_has_map : 1;
    };

    struct local_Evas_Object_Image  *image_obj;
    struct local_Evas_GL_Image      *im;

    image_obj = (struct local_Evas_Object_Image *)
                (((struct local_Evas_Object *)obj)->object_data);

    im = (struct local_Evas_GL_Image *)image_obj->engine_data;

    // Evas old version check and revise
    if (((int)im & 0xffff0000) == 0)    {
        // Evas old version
        image_obj = (struct local_Evas_Object_Image *)(((int *)image_obj) - 1);
        im = (struct local_Evas_GL_Image *)image_obj->engine_data;
    }
    im->native.yinvert = 1;
}
