/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   window control
 *
 * @date    Aug-08-2013
 */
#include "CicoHSWindowController.h"
#include "CicoConf.h"
#include "CicoSystemConfig.h"

Ecore_Evas *CicoHSWindowController::ms_ecoreEvas = NULL;

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSWindowController::CicoHSWindowController
 *          Constructor
 */
/*--------------------------------------------------------------------------*/
CicoHSWindowController::CicoHSWindowController()
{
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSWindowController::CicoHSWindowController
 *          Destructor
 */
/*--------------------------------------------------------------------------*/
CicoHSWindowController::~CicoHSWindowController()
{
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSWindowController::Initialize
 *          Initialize
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSWindowController::Initialize(void)
{
    int ret;
    /* Reset a ecore_evas */
    ret = ecore_evas_init();
    ICO_DBG("CicoHSWindowController::Initialize: ecore_evas_init retrun %d",
            ret);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSWindowController::Finalize
 *          Finalize
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSWindowController::Finalize(void)
{
    /* end the ecore_evas */
    ecore_evas_shutdown();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSWindowController::GetFullScreenSize
 *          get size of full screen
 *
 * @param[in]   orientation    vertial or horizontal
 * @param[out]  width          width of window
 * @param[out]  height         heigth of window
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSWindowController::GetFullScreenSize(int orientation,
                                          int *width, int *height)
{
    ICO_TRA("CicoHSWindowController::GetFullScreenSize Enter");

    int display_width  = 0;
    int display_height = 0;

    ms_ecoreEvas = ecore_evas_new(NULL, 0, 0, 1, 1, "frame=0");

    /* getting size of screen */
    /* home screen size is full of display*/
    ecore_wl_screen_size_get(&display_width, &display_height);

    ICO_DBG("ecore_wl_screen_size_get => w/h=%d/%d",
            display_width, display_height);

#if 1       /* 2014/07/16 ecore_wl_screen_size_get() bug    */
    if ((display_width <= 0) || (display_height <= 0))  {
        const CicoSCDisplayConf* dispconf = CicoSystemConfig::getInstance()
                                                ->findDisplayConfbyId(0);
        if (dispconf)   {
            // if Ecore has no full screen size, get from configuration
            display_width = dispconf->width;
            display_height = dispconf->height;
            ICO_DBG("Screen size w/h=%d/%d from Config", display_width, display_height);
        }
        else    {
            // if Ecore and config has no full screen size, fixed vaule
            display_width = 1080;
            display_height = 1920;
            ICO_DBG("Screen size w/h=1080/1920 fixed");
        }
    }
#endif      /* 2014/07/16 ecore_wl_screen_size_get() bug    */

    *width = display_width;
    *height = display_height;

    ICO_TRA("CicoHSWindowController::GetFullScreenSize Leave(w/h=%d/%d)", *width, *height);
}
// vim:set expandtab ts=4 sw=4:
