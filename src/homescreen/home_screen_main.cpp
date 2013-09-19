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

#include <fstream>
#include <string>
#include <iostream>
#include <cstdio>
#include <bundle.h>
#include <aul/aul.h>
#include <home_screen_main.h>
#include <cerrno>
#include "CicoHomeScreenConfig.h"
#include "ico_syc_public.h"
#include "CicoSCConf.h"
#include "CicoSCSystemConfig.h"

using namespace std;

string g_login_user_name;

bool launchApps(const string& filepath);

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

    /* PARAM GET LOGIN USER */
    bundle *b = bundle_import_from_argv(argc, argv); // import from argc+argv
    const char* valusr = bundle_get_val(b, ICO_SYC_APP_BUNDLE_KEY1);
    const char* valpath = bundle_get_val(b, ICO_SYC_APP_BUNDLE_KEY2);
    if ((NULL != valusr) && (0 != valusr)) {
        g_login_user_name = valusr;
    }
    else {
        g_login_user_name.clear();
    }
    string user_start_apps_path;
    if ((NULL != valpath) && (0 != valpath)) {
        user_start_apps_path = valpath;
    }
    else {
        user_start_apps_path.clear();
    }
    ICO_DBG("PARAM=\"%s\", \"%s\"", g_login_user_name.c_str(),
            user_start_apps_path.c_str());
    bundle_free(b);
    valusr = valpath = NULL;
    
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
    launchApps(user_start_apps_path);
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

/*--------------------------------------------------------------------------*/
/**
 * @brief   launch applications
 *
 * @param   filepath  start applications list file path
 * @return  bool
 * @retval  true  success
 * @retval  false fail
 */
/*--------------------------------------------------------------------------*/
bool launchApps(const string& filepath)
{
    ICO_DBG("launcApps start = %s", filepath.c_str());
    struct stat stat_buf;
    if (0 != stat(filepath.c_str(), &stat_buf)) {
        ICO_DBG("launcApps end false(%d, %s)", errno, filepath.c_str());
        return false;
    }
    int i = 0;
    bool rb = false;
    string tagApp;
    ifstream ifs(filepath.c_str());
    while(ifs >> tagApp) {
        i++;
        if (true == tagApp.empty()) {
            continue;
        }
        int r = aul_launch_app(tagApp.c_str(), NULL);
        ICO_DBG("aul_launch_app %d:appid(%s), r(%d)", i, tagApp.c_str(), r);
        if (AUL_R_OK == r) {
            rb = true;
        }
    }
    ICO_DBG("launcApps end %s", rb? "true": "false");
    return rb;
}
