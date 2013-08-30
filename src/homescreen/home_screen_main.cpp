/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   homescreen application main
 *
 * @date    Feb-15-2013
 */


#include <home_screen_main.h>
#include "CicoHomeScreenConfig.h"

/*--------------------------------------------------------------------------*/
/**
 * @brief   main
 *          homescreen main. initialize UXF, app manager, and ecore.
 *
 * @param[in]   argc                counts of argment
 * @param[in]   argv                argment
 * @return      result
 * @retval      >=0                 success
 * @retval      -1                  error
 */
/*--------------------------------------------------------------------------*/
int
main(int argc, char *argv[])
{
    int ret;

    eina_init();
    eina_log_level_set(EINA_LOG_LEVEL_DBG);

    ico_log_open("HomeScreen");

    /* init configuration */
    ICO_DBG("main: config initialize start");
    CicoHomeScreenConfig *config = new CicoHomeScreenConfig();
    config->Initialize(ICO_HOMESCREEN_CONFIG_FILE);
    ICO_DBG("main: config initialize end");
    
    /* init home screen soud */
    ICO_DBG("main: sound initialize start");
    CicoHomeScreenSound *sound = new CicoHomeScreenSound();
    sound->Initialize(config);
    ICO_DBG("main: sound initialize end");
    
    /*create homescreen*/
    ICO_DBG("main: homescreen initialize start");
    CicoHomeScreen *home_screen = new CicoHomeScreen();
    
    ret = home_screen->Initialize(ICO_ORIENTATION_VERTICAL,config);
    if(ret != ICO_OK){
        ICO_ERR("main: homescreen initialize failed");
        /*clear all classes*/
        delete home_screen;   
        delete sound;   
        delete config;
        exit(8);
    }
    ICO_DBG("main: homescreen initialize end");

    ICO_DBG("main: create homescreen ");
    
    /*home screen start and go into main loop*/
    home_screen->StartHomeScreen();
  
    ICO_DBG("main: end homescreen");
    
    /* when loop is terminated */
    /* delete homescreen */
    home_screen->Finalize();
 
    /*clear all classes*/
    delete home_screen;   

    delete sound;   

    delete config;

    return 0;
}
