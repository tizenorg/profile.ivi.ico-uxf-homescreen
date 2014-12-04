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

    ICO_DBG("CicoHSWindowController::Initialize: Enter");

    /* Reset a ecore_evas */
    ret = ecore_evas_init();
    ICO_DBG("CicoHSWindowController::Initialize: ecore_evas_init retrun %d", ret);
    ms_ecoreEvas = ecore_evas_new(NULL, 0, 0, 1, 1, "frame=0");
    ICO_DBG("CicoHSWindowController::Initialize: Leave");
}

Ecore_Evas *
CicoHSWindowController::GetBaseEvas(void)
{
    return CicoHSWindowController::ms_ecoreEvas;
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

    /* getting size of screen */
    /* home screen size is full of display*/
    ecore_wl_screen_size_get(&display_width, &display_height);

    ICO_DBG("ecore_wl_screen_size_get => w/h=%d/%d",
            display_width, display_height);

    *width = display_width;
    *height = display_height;

    ICO_TRA("CicoHSWindowController::GetFullScreenSize Leave(w/h=%d/%d)", *width, *height);
}
// vim:set expandtab ts=4 sw=4:
