/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   statusbar application
 *
 * @date    Feb-15-2013
 */

#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include <bundle.h>

#include <ico_log.h>

//#include "CicoHomeScreenCommon.h"
#include "CicoGKeyFileConfig.h"
#include "CicoStatusBarControlWindow.h"


//#include "status_bar.h"

#define ICO_SB_APPID_DEFAULT    "org.tizen.ico.statusbar"

/*--------------------------------------------------------------------------*/
/*
 * @brief   status bar application
 *          main routine
 *
 * @param   main() finction's standard parameter (argc,argv)
 * @return  result
 * @retval  0       success
 * @retval  1       failed
 */
/*--------------------------------------------------------------------------*/
int
main(int argc, char *argv[])
{
    int orientation = ICO_SB_HORIZONTAL;
    int ret;
    bundle *b;
    const char *val;

    /* log setting */
    if (getenv("PKG_NAME")) {
        ico_log_open(getenv("PKG_NAME"));
    }
    else {
        ico_log_open(ICO_SB_APPID_DEFAULT);
    }
    ICO_INF("START_MODULE Status Bar");

    /* get argment */
    b = bundle_import_from_argv(argc, argv);
    val = bundle_get_val(b, "orientation");
    if (val != NULL) {
        if (strcmp(val, "vertical") == 0) {
            orientation = ICO_SB_VERTICAL;
        }
        else if (strcmp(val, "horizontal") == 0) {
            orientation = ICO_SB_HORIZONTAL;
        }
    }

    orientation = ICO_SB_VERTICAL;

    /*HomeScreenConfig*/
    CicoGKeyFileConfig *homescreen_config = new CicoGKeyFileConfig();
    homescreen_config->Initialize(ICO_STATUSBAR_CONFIG_FILE);

    /* controlWindow instance */
    CicoStatusBarControlWindow* control_window = 
                                new CicoStatusBarControlWindow();
    control_window->Initialize(homescreen_config);
    CicoStatusBarControlWindow::SetInstanceForClock(control_window);

    ret=control_window->CreateEcoreEvas();
    if(ret < 0){
        ICO_WRN("status_bar: error use ecore")
        return -1;
    }

    /* resize window*/
    control_window->ResizeWindow(orientation);  
   
    /* show window*/
    control_window->ShowWindow();
   
    /* init evas*/
    ret  = control_window->InitEvas();
    if(ret < 0){
        return -1;
    }

    /* status bar parts set*/
    /* clock */
    control_window->ShowClock();

    /* Init websockets */
    
    /* Runs the application main loop */
    control_window->StartEcoreLoop();
  
    /* initialize CicoStatusBarAction */
    
    /* delete control window*/
    control_window->Finalize();
    delete control_window;

    /*delete homescreen config*/
    delete homescreen_config;
    ICO_INF("END_MODULE Status Bar");
    
    return 0;
}
// vim: set expandtab ts=4 sw=4:
