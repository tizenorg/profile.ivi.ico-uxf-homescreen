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
#include <vector>
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
#include "Cico_aul_listen_app.h"
#include "CicoHomeScreen.h"
#include "CicoSound.h"

using namespace std;

string g_login_user_name;

static Eina_Bool launchApps(void* data);
/**
 * ecore timer void data typedef
 */
typedef struct t_launcApps_data {
    string filePath;
    string filePathD;
    CicoHomeScreen *hs;
} launcApps_data_t;


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

    // load system config
    CicoSCSystemConfig::getInstance()->load(ICO_HS_LIFECYCLE_CONTROLLER_SETTING_PATH);

    /* init configuration */
    ICO_DBG("main: config initialize start");
    CicoHomeScreenConfig *config = new CicoHomeScreenConfig();
    config->Initialize(ICO_HOMESCREEN_CONFIG_FILE);
    ICO_DBG("main: config initialize end");

    /* get LOGIN-USER parameter */
    bundle *b = bundle_import_from_argv(argc, argv); // import from argc+argv
    const char* valusr = bundle_get_val(b, ICO_SYC_APP_BUNDLE_KEY1);
    const char* valpath = bundle_get_val(b, ICO_SYC_APP_BUNDLE_KEY2);
    const char* valpathD = bundle_get_val(b, ICO_SYC_APP_BUNDLE_KEY3);
    const char* valFlagPath = bundle_get_val(b, ICO_SYC_APP_BUNDLE_KEY4);
    if ((NULL != valusr) && (0 != valusr)) {
        g_login_user_name = valusr;
    }
    else {
        g_login_user_name.clear();
    }
    launcApps_data_t x;
    x.hs = NULL;

    if ((NULL != valpath) && (0 != valpath)) {
        x.filePath = valpath;
    }
    else {
        x.filePath.clear();
    }

    if ((NULL != valpathD) && (0 != valpathD)) {
        x.filePathD = valpathD;
    }
    else {
        x.filePathD.clear();
    }
    string flagPath;
    if ((NULL != valFlagPath) && (0 != valFlagPath)) {
        flagPath = valFlagPath;
    }
    else {
        flagPath.clear();
    }
    ICO_DBG("PARAM=\"%s\", \"%s\", \"%s\", \"%s\"", g_login_user_name.c_str(),
            x.filePath.c_str(), x.filePathD.c_str(), flagPath.c_str());
    bundle_free(b);
    valusr = valpath = valpathD = valFlagPath = NULL;

    /* init home screen soud */
    ICO_DBG("main: sound initialize start");
    CicoHomeScreenSound *sound = new CicoHomeScreenSound();
    sound->Initialize(config);
    ICO_DBG("main: sound initialize end");

    CicoSound::GetInstance()->Initialize(config);

    /*AUL Listen Signal set(launch/dead)*/
    initAulListenXSignal();

    /*create homescreen*/
    ICO_DBG("main: homescreen initialize start");
    CicoHomeScreen *home_screen = new CicoHomeScreen();

    ret = home_screen->Initialize(ICO_ORIENTATION_VERTICAL, config);
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
    /* application history class init. before call launchApps */
    home_screen->InitializeAppHistory(g_login_user_name, x.filePath,
                                      x.filePathD, flagPath);
    /* application history launch */
    x.hs = home_screen;
    ecore_timer_add(0.01, launchApps, &x);

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
 * @param   filepathD defalt start applications list file path
 * @return  bool
 * @retval  true  success
 * @retval  false fail
 */
/*--------------------------------------------------------------------------*/
static Eina_Bool launchApps(void* data)
{
    ICO_DBG("start");
    launcApps_data_t* x = (launcApps_data_t*) data;
    if ((NULL == x) || (NULL == x->hs)) {
        ICO_DBG("end");
        return ECORE_CALLBACK_CANCEL;
    }

    vector<pairAppidSubd> apps;
    x->hs->readStartupApp(apps);

    int sz = apps.size();
    for (int i =sz; i > 0; i--) {
        string appid = apps[i-1].first;
        bool bFLAG = apps[i-1].second;
        int pid = aul_launch_app(appid.c_str(), NULL);
        ICO_DBG("aul_launch_app[%d]%d:%s:%d", i, pid, appid.c_str(), (int)bFLAG);
        if ((0 < pid) && (NULL != x->hs)) {
            x->hs->startupCheckAdd(pid, appid, bFLAG);
        }
    }
    ICO_DBG("end");
    return ECORE_CALLBACK_CANCEL;
}
