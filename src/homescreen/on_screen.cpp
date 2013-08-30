/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   onscreen application
 *
 * @date    Feb-15-2013
 */
#include "on_screen.h"

/*--------------------------------------------------------------------------*/
/*
 * @brief   onscreen application
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
    int ret;
    bundle *b;
    const char *val;
//    int orientation = ICO_ONS_HORIZONTAL;
    int orientation = ICO_ONS_VERTICAL;

    /* log setting */
    if (getenv("PKG_NAME")) {
        ico_uxf_log_open(getenv("PKG_NAME"));
    }
    else {
        ico_uxf_log_open(ICO_HS_APPID_DEFAULT_ONS);
    }

    /* get argment */
    /* setting horizontal or vartical */
    b = bundle_import_from_argv(argc, argv);
    val = bundle_get_val(b, "orientation");
    if (val != NULL) {
        if (strcmp(val, "vertical") == 0) {
            orientation = ICO_ONS_VERTICAL;
        }
        else if (strcmp(val, "horizontal") == 0) {
            orientation = ICO_ONS_HORIZONTAL;
        }
    }

    /*HomeScreenConfig*/
    CicoHomeScreenConfig *homescreen_config = new CicoHomeScreenConfig();
    homescreen_config->Initialize(ICO_ONSCREEN_CONFIG_FILE);

    /* init home screen soud */
    CicoHomeScreenSound *sound = new CicoHomeScreenSound();
    sound->Initialize(homescreen_config);

    /* controlWindow instance */
    CicoOnScreenControlWindow* control_window = new CicoOnScreenControlWindow();

    /* initialize app list*/ 
    CicoOnScreenAppList::Initialize(control_window);

    /* load configuration of applications*/
    CicoOnScreenAppList::LoadConfig();

    /* create new ecore_evas */
    ret = control_window->CreateEcoreEvas();
    if(ret < 0){
        return 1;
    }
    /* resize window */
    control_window->ResizeWindow(homescreen_config,orientation);

    /* show the window */
    control_window->ShowWindow();
  
    /* initialize CicoOnScreenAction */
    CicoOnScreenAction::Initialize(homescreen_config,sound);
    
    /* add callback to app configuration */
    CicoOnScreenAppList::SetConfigCallback();
  
    /* start ecore loop*/
    control_window->StartEcoreLoop();
  
    /* finalize CicoOnScreenAction */
    CicoOnScreenAction::Finalize();
    
    /* end ecore evas*/
    delete control_window;

    /*delete homescreen sound*/
    delete sound;

    /*delete homescreen config*/
    delete homescreen_config;
    
    return 0;
}

