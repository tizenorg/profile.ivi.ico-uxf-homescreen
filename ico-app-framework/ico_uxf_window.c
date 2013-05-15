/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of Window control for HomeScreen
 *
 * @date    Feb-28-2013
 */

#include    <stdio.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    <string.h>
#include    <errno.h>

#include    "ico_uxf.h"
#include    "ico_uxf_private.h"
#include    "ico_window_mgr-client-protocol.h"

extern Ico_Uxf_Api_Mng         gIco_Uxf_Api_Mng;

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_window_layer: Set window layer
 *
 * @param[in]   window          window Id
 * @param[in]   layer           layer Id
 * @return      result
 * @retval      ICO_UXF_EOK     success
 * @retval      ICO_UXF_ESRCH   error(not initialized)
 * @retval      ICO_UXF_ENOENT  error(window or layer dose not exist)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_window_layer(const int window, const int layer)
{
    Ico_Uxf_Mng_Window  *winmng;
    Ico_Uxf_Mng_Layer   *layermng;

    uifw_trace("ico_uxf_window_layer: Enter(%08x,%d)", window, layer);

    if (gIco_Uxf_Api_Mng.Initialized <= 0) {
        uifw_warn("ico_uxf_window_layer: Leave(ESRCH)");
        return ICO_UXF_ESRCH;
    }

    winmng = ico_uxf_mng_window(window, 0);
    if ((! winmng) || (! winmng->mng_display)) {
        uifw_warn("ico_uxf_window_layer: Leave(ENOENT)");
        return ICO_UXF_ENOENT;
    }

    layermng = ico_uxf_mng_layer(winmng->attr.display, layer, 0);
    if ((! layermng) || (! layermng->mng_display)) {
        uifw_warn("ico_uxf_window_layer: Leave(ENOENT)");
        return ICO_UXF_ENOENT;
    }

    uifw_trace("ico_uxf_window_layer: ico_window_mgr_set_window_layer(%08x,%d)",
               window, layer);
    winmng->mng_layer = layermng;
    ico_window_mgr_set_window_layer(gIco_Uxf_Api_Mng.Wayland_WindowMgr, window, layer);
    wl_display_flush(gIco_Uxf_Api_Mng.Wayland_Display);

    uifw_trace("ico_uxf_window_layer: Leave(EOK)");
    return ICO_UXF_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_layer_visible: Control layer visibility
 *
 * @param[in]   display         display Id
 * @param[in]   layer           layer Id
 * @param[in]   visible         layer show(1)/hide(0)
 * @return      result
 * @retval      ICO_UXF_EOK     success
 * @retval      ICO_UXF_ESRCH   error(not initialized)
 * @retval      ICO_UXF_ENOENT  error(layer dose not exist)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_layer_visible(const int display, const int layer, const int visible)
{
    Ico_Uxf_Mng_Layer   *layermng;

    uifw_trace("ico_uxf_layer_visible: Enter(%d,%d,%d)", display, layer, visible);

    if (gIco_Uxf_Api_Mng.Initialized <= 0) {
        uifw_warn("ico_uxf_layer_visible: Leave(ESRCH)");
        return ICO_UXF_ESRCH;
    }

    layermng = ico_uxf_mng_layer(display, layer, 0);
    if ((! layermng) || (! layermng->mng_display)) {
        uifw_warn("ico_uxf_layer_visible: Leave(ENOENT)");
        return ICO_UXF_ENOENT;
    }

    uifw_trace("ico_uxf_layer_visible: ico_window_mgr_set_layer_visible(%d,%d)",
               layer, visible);
    ico_window_mgr_set_layer_visible(gIco_Uxf_Api_Mng.Wayland_WindowMgr, layer, visible);
    wl_display_flush(gIco_Uxf_Api_Mng.Wayland_Display);

    uifw_trace("ico_uxf_layer_visible: Leave(EOK)");
    return ICO_UXF_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_window_resize: resize window(surface) size
 *
 * @param[in]   window          window Id(same as ico_window_mgr surface Id)
 * @param[in]   w               window width
 * @param[in]   h               window height
 * @return      result
 * @retval      ICO_UXF_EOK     success
 * @retval      ICO_UXF_ESRCH   error(not initialized)
 * @retval      ICO_UXF_ENOENT  error(layer dose not exist)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_window_resize(const int window, const int w, const int h)
{
    Ico_Uxf_Mng_Window  *winmng;

    uifw_trace("ico_uxf_window_resize: Enter(%08x,%d,%d)", window, w, h);

    if (gIco_Uxf_Api_Mng.Initialized <= 0) {
        uifw_warn("ico_uxf_window_resize: Leave(ESRCH)");
        return ICO_UXF_ESRCH;
    }

    winmng = ico_uxf_mng_window(window, 0);
    if ((! winmng) || (! winmng->mng_layer))   {
        uifw_warn("ico_uxf_window_resize: Leave(ENOENT)");
        return ICO_UXF_ENOENT;
    }

    uifw_trace("ico_uxf_window_resize: layer(%d,%d)",
               winmng->mng_layer->attr.w, winmng->mng_layer->attr.h);
    if ((w <= 0) || (w > winmng->mng_layer->attr.w) ||
        (h <= 0) || (h > winmng->mng_layer->attr.h))   {
        uifw_warn("ico_uxf_window_resize: Leave(EINVAL)");
        return ICO_UXF_EINVAL;
    }

    if ((w != winmng->attr.w) || (h != winmng->attr.h))    {
        winmng->attr.w = w;
        winmng->attr.h = h;

        ico_window_mgr_set_positionsize(gIco_Uxf_Api_Mng.Wayland_WindowMgr, window,
                                        winmng->attr.x, winmng->attr.y,
                                        winmng->attr.w, winmng->attr.h);
        wl_display_flush(gIco_Uxf_Api_Mng.Wayland_Display);
    }
    uifw_trace("ico_uxf_window_resize: Leave(EOK)");
    return ICO_UXF_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_window_move: move window(surface) geometry
 *
 * @param[in]   window          window Id(same as ico_window_mgr surface Id)
 * @param[in]   x               window upper-left X coodinate of display
 * @param[in]   y               window upper-left Y coodinate of display
 * @return      result
 * @retval      ICO_UXF_EOK     success
 * @retval      ICO_UXF_ESRCH   error(not initialized)
 * @retval      ICO_UXF_ENOENT  error(layer dose not exist)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_window_move(const int window, const int x, const int y)
{
    Ico_Uxf_Mng_Window  *winmng;

    uifw_trace("ico_uxf_window_move: Enter(%08x,%d,%d)", window, x, y);

    if(gIco_Uxf_Api_Mng.Initialized <= 0)   {
        uifw_warn("ico_uxf_window_move: Leave(ESRCH)");
        return ICO_UXF_ESRCH;
    }

    winmng = ico_uxf_mng_window(window, 0);
    if ((! winmng) || (! winmng->mng_layer))   {
        uifw_warn("ico_uxf_window_move: Leave(ENOENT)");
        return ICO_UXF_ENOENT;
    }

    winmng->attr.x = x;
    winmng->attr.y = y;

    ico_window_mgr_set_positionsize(gIco_Uxf_Api_Mng.Wayland_WindowMgr, window,
                                    winmng->attr.x, winmng->attr.y,
                                    winmng->attr.w, winmng->attr.h);
    wl_display_flush(gIco_Uxf_Api_Mng.Wayland_Display);

    uifw_trace("ico_uxf_window_move: Leave(EOK)");
    return ICO_UXF_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_window_raise: raise window(surface)
 *
 * @param[in]   window          window Id(same as ico_window_mgr surface Id)
 * @return      result
 * @retval      ICO_UXF_EOK     success
 * @retval      ICO_UXF_ESRCH   error(not initialized)
 * @retval      ICO_UXF_ENOENT  error(layer dose not exist)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_window_raise(const int window)
{
    Ico_Uxf_Mng_Window  *winmng;

    uifw_trace("ico_uxf_window_raise: Enter(%08x)", window);

    if (gIco_Uxf_Api_Mng.Initialized <= 0) {
        uifw_warn("ico_uxf_window_raise: Leave(ESRCH)");
        return ICO_UXF_ESRCH;
    }

    winmng = ico_uxf_mng_window(window, 0);
    if ((! winmng) || (! winmng->mng_display)) {
        uifw_warn("ico_uxf_window_raise: Leave(ENOENT)");
        return ICO_UXF_ENOENT;
    }

    uifw_trace("ico_uxf_window_raise: ico_window_mgr_set_visible(%08x,%d,%d)", window, 9, 1);
    ico_window_mgr_set_visible(gIco_Uxf_Api_Mng.Wayland_WindowMgr, window, 9, 1);
    wl_display_flush(gIco_Uxf_Api_Mng.Wayland_Display);

    uifw_trace("ico_uxf_window_raise: Leave(EOK)");
    return ICO_UXF_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_window_lower: lower window(surface)
 *
 * @param[in]   window          window Id(same as ico_window_mgr surface Id)
 * @return      result
 * @retval      ICO_UXF_EOK     success
 * @retval      ICO_UXF_ESRCH   error(not initialized)
 * @retval      ICO_UXF_ENOENT  error(layer dose not exist)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_window_lower(const int window)
{
    Ico_Uxf_Mng_Window  *winmng;

    uifw_trace("ico_uxf_window_lower: Enter(%08x)", window);

    if (gIco_Uxf_Api_Mng.Initialized <= 0) {
        uifw_warn("ico_uxf_window_lower: Leave(ESRCH)");
        return ICO_UXF_ESRCH;
    }

    winmng = ico_uxf_mng_window(window, 0);
    if ((! winmng) || (! winmng->mng_display)) {
        uifw_warn("ico_uxf_window_lower: Leave(ENOENT)");
        return ICO_UXF_ENOENT;
    }

    uifw_trace("ico_uxf_window_lower: ico_window_mgr_set_visible(%08x,%d,%d)", window, 9, 0);
    ico_window_mgr_set_visible(gIco_Uxf_Api_Mng.Wayland_WindowMgr, window, 9, 0);
    wl_display_flush(gIco_Uxf_Api_Mng.Wayland_Display);

    uifw_trace("ico_uxf_window_lower: Leave(EOK)");
    return ICO_UXF_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_window_active: active window(surface)
 *
 * @param[in]   window          window Id(same as ico_window_mgr surface Id)
 * @param[in]   target          target(pointer and/or keyboard)
 * @return      result
 * @retval      ICO_UXF_EOK     success
 * @retval      ICO_UXF_ESRCH   error(not initialized)
 * @retval      ICO_UXF_ENOENT  error(layer dose not exist)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_window_active(const int window, const int target)
{
    Ico_Uxf_Mng_Window  *winmng;
    Ico_Uxf_Mng_Process *proc;
    Ico_Uxf_Mng_Process *aproc;
    int                 hash;

    uifw_trace("ico_uxf_window_active: Enter(%08x,%x)", window, target);

    if (gIco_Uxf_Api_Mng.Initialized <= 0) {
        uifw_warn("ico_uxf_window_active: Leave(ESRCH)");
        return ICO_UXF_ESRCH;
    }

    winmng = ico_uxf_mng_window(window, 0);
    if ((! winmng) || (! winmng->mng_display)) {
        uifw_warn("ico_uxf_window_active: Leave(ENOENT)");
        return ICO_UXF_ENOENT;
    }

    uifw_trace("ico_uxf_window_active: ico_window_mgr_set_active(%08x)", window);
    ico_window_mgr_set_active(gIco_Uxf_Api_Mng.Wayland_WindowMgr, window, target);
    wl_display_flush(gIco_Uxf_Api_Mng.Wayland_Display);

    /* reset all active window widthout target window   */
    for (hash = 0; hash < ICO_UXF_MISC_HASHSIZE; hash++)   {
        winmng = gIco_Uxf_Api_Mng.Hash_WindowId[hash];
        while (winmng) {
            if (winmng->attr.window == window)  {
                winmng->attr.active = target;
            }
            else    {
                if (target == 0)    {
                    winmng->attr.active = 0;
                }
                else    {
                    winmng->attr.active &= ~target;
                }
            }
            winmng = winmng->nextidhash;
        }
    }

    /* reset all active application without this application    */
    aproc = NULL;
    for (hash = 0; hash < ICO_UXF_MISC_HASHSIZE; hash++)    {
        proc = gIco_Uxf_Api_Mng.Hash_ProcessId[hash];
        while (proc)    {
            if (proc->attr.mainwin.window == window)    {
                aproc = proc;
            }
            else    {
                if (target == 0)    {
                    proc->attr.active = 0;
                }
                else    {
                    proc->attr.active &= ~target;
                }
            }
            proc = proc->nextidhash;
        }
    }
    aproc->attr.active = target;

    uifw_trace("ico_uxf_window_active: Leave(EOK)");
    return ICO_UXF_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_window_event_mask: set window(surface) event mask
 *
 * @param[in]   window          window Id(same as ico_window_mgr surface Id)
 * @param[in]   mask            event mask
 * @return      result
 * @retval      ICO_UXF_EOK     success
 * @retval      ICO_UXF_ESRCH   error(not initialized)
 * @retval      ICO_UXF_ENOENT  error(layer dose not exist)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_window_event_mask(const int window, const unsigned int mask)
{
    Ico_Uxf_Mng_Window  *winmng;

    uifw_trace("ico_uxf_window_event_mask: Enter(%08x,%08x)", window, mask);

    if (gIco_Uxf_Api_Mng.Initialized <= 0) {
        uifw_warn("ico_uxf_window_event_mask: Leave(ESRCH)");
        return ICO_UXF_ESRCH;
    }

    winmng = ico_uxf_mng_window(window, 0);
    if (! winmng)  {
        uifw_warn("ico_uxf_window_event_mask: Leave(ENOENT)");
        return ICO_UXF_ENOENT;
    }

    winmng->attr.eventmask = mask;

    uifw_trace("ico_uxf_window_event_mask: Leave(EOK)");
    return ICO_UXF_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_window_attribute_get: get window(surface) attribute
 *
 * @param[in]   window          window Id(same as ico_window_mgr surface Id)
 * @param[out]  attr            window attribute
 * @return      result
 * @retval      ICO_UXF_EOK     success
 * @retval      ICO_UXF_ESRCH   error(not initialized)
 * @retval      ICO_UXF_ENOENT  error(layer dose not exist)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_window_attribute_get(const int window, Ico_Uxf_WindowAttr *attr)
{
    Ico_Uxf_Mng_Window  *winmng;

    uifw_trace("ico_uxf_window_attribute_get: Enter(%08x)", window);

    if (gIco_Uxf_Api_Mng.Initialized <= 0) {
        uifw_warn("ico_uxf_window_attribute_get: Leave(ESRCH)");
        return ICO_UXF_ESRCH;
    }

    winmng = ico_uxf_mng_window(window, 0);
    if (! winmng)  {
        uifw_warn("ico_uxf_window_attribute_get: Leave(ENOENT)");
        return ICO_UXF_ENOENT;
    }

    memcpy((char *)attr, (char *)&(winmng->attr), sizeof(Ico_Uxf_WindowAttr));

    uifw_trace("ico_uxf_window_attribute_get: Leave(EOK)");
    return ICO_UXF_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_window_query_windows: get all windows attribute
 *
 * @param[in]   display         target display Id
 * @param[out]  attr            window attribute array
 * @param[in]   num             number of window attribute area
 * @return      result
 * @retval      >= 0            success(number of windows)
 * @retval      ICO_UXF_ESRCH   error(not initialized)
 * @retval      ICO_UXF_EINVAL  error(illegal parameter 'num')
 * @retval      ICO_UXF_E2BIG   error(number of windows over 'num')
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_window_query_windows(const int display, Ico_Uxf_WindowAttr attr[], const int num)
{
    int                 n;
    Ico_Uxf_Mng_Window  *winmng;
    int                 hash;

    uifw_trace("ico_uxf_window_query_windows: Enter(%d,,%d)", display, num);

    if (gIco_Uxf_Api_Mng.Initialized <= 0) {
        uifw_warn("ico_uxf_window_query_windows: Leave(ESRCH)");
        return ICO_UXF_ESRCH;
    }

    if (num <= 0)  {
        uifw_warn("ico_uxf_window_query_windows: Leave(EINVAL)");
        return ICO_UXF_EINVAL;
    }

    n = 0;
    for (hash = 0; hash < ICO_UXF_MISC_HASHSIZE; hash++)   {
        winmng = gIco_Uxf_Api_Mng.Hash_WindowId[hash];
        while (winmng) {
            if (n >= num)      break;
            memcpy((char *)(&attr[n]), (char *)&(winmng->attr), sizeof(Ico_Uxf_WindowAttr));
            n ++;
            winmng = winmng->nextidhash;
        }
        if (winmng)    break;
    }
    if (winmng)    {
        n = ICO_UXF_E2BIG;
        uifw_trace("ico_uxf_window_query_windows: Leave(E2BIG)");
    }
    else    {
        uifw_trace("ico_uxf_window_query_windows: Leave(%d)", n);
    }
    return n;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_window_control: target window control from AppsController
 *
 * @param[in]   appid       Application Id
 * @param[in]   winidx      Window index
 * @param[in]   control     Window control
 *                            ICO_UXF_APPSCTL_INVISIBLE:   Invisible by low priority
 *                            ICO_UXF_APPSCTL_REGULATION:  Invisible by regulation
 *                            ICO_UXF_APPSCTL_TEMPVISIBLE: Visible as HomeScreen menu
 * @param[in]   onoff       Flag On(1)/Off(0)
 *                            ICO_UXF_APPSCTL_INVISIBLE:   On(1) is invisible
 *                            ICO_UXF_APPSCTL_REGULATION:  On(1) is invisible
 *                            ICO_UXF_APPSCTL_TEMPVISIBLE: On(1) is visible(menu)
 * @return  result
 * @retval  ICO_UXF_EOK         success
 * @retval  ICO_UXF_ESRCH       error(not initialized)
 * @retval  ICO_UXF_ENOENT      error(not exist)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_window_control(const char *appid, const int winidx,
                       const int control, const int onoff)
{
#define MAX_APP_WINDOWS 20
    Ico_Uxf_ProcessWin  procattr[MAX_APP_WINDOWS];
    int         nwin;
    Ico_Uxf_Mng_Window  *winmng;            /* window management table      */
    int         hash;
    int         newcontrol;
    int         i;

    uifw_trace("ico_uxf_window_control: Enter(%s,%d,%x,%d)",
               appid, winidx, control, onoff);

    if (gIco_Uxf_Api_Mng.Initialized <= 0)  {
        uifw_warn("ico_uxf_window_control: Leave(ESRCH)");
        return ICO_UXF_ESRCH;
    }

    if (control == ICO_UXF_APPSCTL_TEMPVISIBLE) {
        if (onoff != gIco_Uxf_Api_Mng.AppsCtlVisible)   {
            if (onoff > 0)  gIco_Uxf_Api_Mng.AppsCtlVisible = 1;
            else            gIco_Uxf_Api_Mng.AppsCtlVisible = 0;

            /* control all windows          */
            for (hash = 0; hash < ICO_UXF_MISC_HASHSIZE; hash++)    {
                winmng = gIco_Uxf_Api_Mng.Hash_WindowId[hash];
                while (winmng)  {
                    if ((winmng->attr.window > 0) && (winmng->attr.visible))    {
                        if ((gIco_Uxf_Api_Mng.AppsCtlVisible != 0) ||
                            (winmng->attr.control == 0))    {
                            i = 1;
                        }
                        else    {
                            i = 0;
                        }
                        uifw_trace("ico_uxf_window_control: ico_window_mgr_set_visible"
                                   "(%08x,%d,9)", winmng->attr.window, i);
                        ico_window_mgr_set_visible(gIco_Uxf_Api_Mng.Wayland_WindowMgr,
                                                   winmng->attr.window, i, 9);
                        wl_display_flush(gIco_Uxf_Api_Mng.Wayland_Display);
                    }
                    winmng = winmng->nextidhash;
                }
            }
        }
        uifw_trace("ico_uxf_window_control: Leave(EOK)");
    }
    else    {
        /* get current application status       */
        if (winidx >= 0)    {
            nwin = ico_uxf_process_window_get_one(appid, procattr, winidx);
            if (nwin == 0)  nwin = 1;
        }
        else    {
            /* all window           */
            nwin = ico_uxf_process_window_get(appid, procattr, MAX_APP_WINDOWS);
        }
        if (nwin == ICO_UXF_E2BIG)  nwin = MAX_APP_WINDOWS;
        if (nwin < 0)  {
            uifw_trace("ico_uxf_window_control: Leave(Unknown appid(%s))", appid);
            return ICO_UXF_ENOENT;
        }
        for (i = 0; i < nwin; i++)  {
            if (procattr[i].window == 0)    continue;
            winmng = ico_uxf_mng_window(procattr[i].window, 0);
            if (! winmng)   {
                uifw_trace("ico_uxf_window_control: winid=%08x not exist",
                           procattr[i].window);
                continue;
            }
            newcontrol = winmng->attr.control;
            if (onoff)  newcontrol |= control;
            else        newcontrol &= (~control);

            uifw_trace("ico_uxf_window_control: control %08x %02x=>%02x",
                       procattr[i].window, winmng->attr.control, newcontrol);
            if (winmng->attr.control != newcontrol) {
                winmng->attr.control = newcontrol;
                if (winmng->attr.visible != 0)  {
                    if (gIco_Uxf_Api_Mng.AppsCtlVisible)    newcontrol = 0;
                    /* request to Weston(Window Manager)     */
                    uifw_trace("ico_uxf_window_control: visible(%08x,%d) to weston",
                               procattr[i].window, (newcontrol != 0) ? 0 : 1);
                    ico_window_mgr_set_visible(gIco_Uxf_Api_Mng.Wayland_WindowMgr,
                                               procattr[i].window,
                                               (newcontrol != 0) ? 0 : 1, 9);
                    wl_display_flush(gIco_Uxf_Api_Mng.Wayland_Display);
                }
            }
            uifw_trace("ico_uxf_window_control: Leave(EOK) control=%x(%d) visible=%d",
                       winmng->attr.control, gIco_Uxf_Api_Mng.AppsCtlVisible,
                       winmng->attr.visible);
        }
    }
    return ICO_UXF_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_window_show: show a target window
 *          if target window's state is visible, do nothing and return success
 *
 * @param[in]   window      Window identity
 * @return  result
 * @retval  ICO_UXF_EOK         success
 * @retval  ICO_UXF_ESRCH       error(not initialized)
 * @retval  ICO_UXF_ENOENT      error(not exist)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_window_show(const int window)
{
    Ico_Uxf_Mng_Window  *winmng;            /* window management table      */

    uifw_trace("ico_uxf_window_show: Enter(%08x)", window);

    if (gIco_Uxf_Api_Mng.Initialized <= 0) {
        uifw_warn("ico_uxf_window_show: Leave(ESRCH)");
        return ICO_UXF_ESRCH;
    }

    winmng = ico_uxf_mng_window(window, 0);
    if ((! winmng) || (! winmng->mng_display)) {
        uifw_warn("ico_uxf_window_show: Leave(ENOENT)");
        return ICO_UXF_ENOENT;
    }
    winmng->attr.visible = 1;

    if (winmng->attr.control == 0)  {
        /* request to display to Weston(Window Manager)     */
        uifw_trace("ico_uxf_window_show: ico_window_mgr_set_visible(%08x,%d,%d)",
                   window, 1, 9);
        ico_window_mgr_set_visible(gIco_Uxf_Api_Mng.Wayland_WindowMgr, window, 1, 9);
        wl_display_flush(gIco_Uxf_Api_Mng.Wayland_Display);
    }
    uifw_trace("ico_uxf_window_show: Leave(EOK) control=%x", winmng->attr.control);
    return ICO_UXF_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_window_hide: hide a target window
 *          if target window's state is unvisible, do nothing and return success
 *
 * @param[in]   window      target window identity
 * @return  result
 * @retval  ICO_UXF_EOK         success
 * @retval  ICO_UXF_ESRCH       error(not initialized)
 * @retval  ICO_UXF_ENOENT      error(not exist)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_window_hide(const int window)
{
    Ico_Uxf_Mng_Window  *winmng;            /* window management table      */

    uifw_trace("ico_uxf_window_hide: Enter(%08x)", window);

    if (gIco_Uxf_Api_Mng.Initialized <= 0) {
        uifw_warn("ico_uxf_window_hide: Leave(ESRCH)");
        return ICO_UXF_ESRCH;
    }

    winmng = ico_uxf_mng_window(window, 0);
    if ((! winmng) || (! winmng->mng_display)) {
        uifw_warn("ico_uxf_window_hide: Leave(ENOENT)");
        return ICO_UXF_ENOENT;
    }
    winmng->attr.visible = 0;

    /* request to hide to Weston(Window Manager)    */
    uifw_trace("ico_uxf_window_hide: ico_window_mgr_set_visible(%08x,%d,%d)", window, 0, 9);
    ico_window_mgr_set_visible(gIco_Uxf_Api_Mng.Wayland_WindowMgr, window, 0, 9);
    wl_display_flush(gIco_Uxf_Api_Mng.Wayland_Display);

    uifw_trace("ico_uxf_window_hide: Leave(EOK)");
    return ICO_UXF_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_window_visible_raise: control status of visible(show/hide)
 *                                        and order(raise/lower)
 *
 * @param[in]   window      target window identity
 * @param[in]   visible     visible(1)/unvisible(0)
 * @param[in]   raise       forefront(1)/maximum back(0)
 * @return  result
 * @retval  ICO_UXF_EOK         success
 * @retval  ICO_UXF_ESRCH       error(not initialized)
 * @retval  ICO_UXF_ENOENT      error(not exist)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_window_visible_raise(const int window, const int visible, const int raise)
{
    Ico_Uxf_Mng_Window  *winmng;            /* window management table      */
    int                 vis = visible;

    uifw_trace("ico_uxf_window_visible_raise: Enter(%08x,%d,%d)", window, visible, raise);

    if (gIco_Uxf_Api_Mng.Initialized <= 0)  {
        uifw_warn("ico_uxf_window_visible_raise: Leave(ESRCH)");
        return ICO_UXF_ESRCH;
    }

    winmng = ico_uxf_mng_window(window, 0);
    if ((! winmng) || (! winmng->mng_display))  {
        uifw_warn("ico_uxf_window_visible_raise: Leave(ENOENT)");
        return ICO_UXF_ENOENT;
    }

    if (vis == 0)   {
        winmng->attr.visible = 0;
    }
    else if (vis == 1)  {
        winmng->attr.visible = 1;
        if ((winmng->attr.control != 0) && (gIco_Uxf_Api_Mng.AppsCtlVisible == 0))  {
            uifw_trace("ico_uxf_window_visible_raise: change to hide(control=%x menu=%d",
                       winmng->attr.control, gIco_Uxf_Api_Mng.AppsCtlVisible);
            vis = 0;
        }
    }

    if ((vis != 9) || (raise != 9)) {
        /* request to visible status and order status to Weston(Window Manager)     */
        uifw_trace("ico_uxf_window_visible_raise: ico_window_mgr_set_visible(%08x,%d,%d)",
                   window, vis, raise);
        ico_window_mgr_set_visible(gIco_Uxf_Api_Mng.Wayland_WindowMgr, window, vis, raise);
        wl_display_flush(gIco_Uxf_Api_Mng.Wayland_Display);
    }
    uifw_trace("ico_uxf_window_visible_raise: Leave(EOK)");
    return ICO_UXF_EOK;
}

