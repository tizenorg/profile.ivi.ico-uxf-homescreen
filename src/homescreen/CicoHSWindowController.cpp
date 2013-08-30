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

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
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
 * @param[out]   width          width of window
 * @param[out]   height         heigth of window
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSWindowController::GetFullScreenSize(int orientation,int *width,int *height)
{
    int display_width, display_height;
    Ecore_Evas *window;
    ICO_DBG("CicoHSWindowController::GetFullScreenSize:ecore_evas_new start");
    window = ecore_evas_new(NULL,0,0,1,1,"frame=0");
    ICO_DBG("CicoHSWindowController::GetFullScreenSize: ecore_evas_new return 0x%x",window);
    /* processing one cycle */
//    ecore_main_loop_iterate();

    /* getting size of screen */
    /* home screen size is full of display*/
    
    ecore_wl_screen_size_get(&display_width, &display_height);
    if (orientation == ICO_ORIENTATION_VERTICAL) {
        *width = display_width > display_height ?
                                 display_height : display_width;
        *height = (display_width > display_height ?
                                 display_width : display_height);
    }
    else {
        *width = display_width < display_height ?
                                 display_height : display_width;
        *height = (display_width < display_height ?
                                   display_width : display_height);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSWindowController::StartEcoreLoop
 *          start loop
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSWindowController::StartEcoreLoop(void)
{
    /* Start main loop */
    ecore_main_loop_begin();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSWindowController::QuitEcoreLoop
 *          quit loop
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSWindowController::QuitEcoreLoop(void)
{
    /* Quit main loop */
    ecore_main_loop_quit();
}

