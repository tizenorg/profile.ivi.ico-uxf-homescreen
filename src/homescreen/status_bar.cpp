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
#include "status_bar.h"

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
        ico_log_open(ICO_HS_APPID_DEFAULT_SB);
    }

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
    CicoHomeScreenConfig *homescreen_config = new CicoHomeScreenConfig();
    homescreen_config->Initialize(ICO_STATUSBAR_CONFIG_FILE);

    /* init home screen soud */
    CicoHomeScreenSound *sound = new CicoHomeScreenSound();
    sound->Initialize(homescreen_config);

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

    /*delete homescreen sound*/
    delete sound;

    /*delete homescreen config*/
    delete homescreen_config;
    
    return 0;
}

