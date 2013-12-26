/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

//==========================================================================
/**
 *  @file   home_screen_main.cpp
 *
 *  @brief  This file is implimantion of homescreen application main function
 */
//==========================================================================

#include <cstring>
#include <iostream>
#include <Eina.h>
#include "ico_log.h"
#include "CicoHSEFLApp.h"
#include "CicoHSCmdOpts.h"
#include "CicoGKeyFileConfig.h"

#define ICO_CONFIG_LOG       "log"
#define ICO_CONFIG_LOG_NAME  "filename"
#define ICO_CONFIG_LOG_LEVEL "loglevel"
#define ICO_CONFIG_LOG_FLUSH "logflush"
#define ICO_CONFIG_LOG_ECORE "ecorelog"

//--------------------------------------------------------------------------
/**
 *  @brief  setup log
 */
//--------------------------------------------------------------------------
void
setupLog(void)
{
    /* init configuration */
    CicoGKeyFileConfig hsConfig;
    hsConfig.Initialize(ICO_HOMESCREEN_CONFIG_FILE);

    // ico log open
    std::string name;
    name = hsConfig.ConfigGetString(ICO_CONFIG_LOG,
                                     ICO_CONFIG_LOG_NAME,
                                     "HomeScreen");
    ico_log_open(name.c_str());


    // ico log level
    int log_level = 0;
    std::string level = hsConfig.ConfigGetString(ICO_CONFIG_LOG,
                                                  ICO_CONFIG_LOG_LEVEL,
                                                  "all");

    if (NULL != strstr(level.c_str(), "performance")) {
        log_level |= ICO_LOG_LVL_PRF;
    }

    if (NULL != strstr(level.c_str(), "trace")) {
        log_level |= ICO_LOG_LVL_TRA;
    }

    if (NULL != strstr(level.c_str(), "debug")) {
        log_level |= ICO_LOG_LVL_DBG;
    }

    if (NULL != strstr(level.c_str(), "info")) {
        log_level |= ICO_LOG_LVL_INF;
    }

    if (NULL != strstr(level.c_str(), "warning")) {
        log_level |= ICO_LOG_LVL_WRN;
    }

    if (NULL != strstr(level.c_str(), "critical")) {
        log_level |= ICO_LOG_LVL_CRI;
    }

    if (NULL != strstr(level.c_str(), "error")) {
        log_level |= ICO_LOG_LVL_ERR;
    }

    std::string flush = hsConfig.ConfigGetString(ICO_CONFIG_LOG,
                                                  ICO_CONFIG_LOG_FLUSH,
                                                  "on");
    if (NULL != strstr(flush.c_str(), "on")) {
        log_level |= ICO_LOG_FLUSH;
    }
    else {
        log_level |= ICO_LOG_NOFLUSH;
    }

    if (NULL != strstr(level.c_str(), "none")) {
        ico_log_set_level(0);
    }
    else if (NULL == strstr(level.c_str(), "all")) {
        ico_log_set_level(log_level);
    }

    // eocre log print
    std::string ecore = hsConfig.ConfigGetString(ICO_CONFIG_LOG,
                                                  ICO_CONFIG_LOG_ECORE,
                                                  "on");
    if (NULL != strstr(ecore.c_str(), "on")) {
        eina_init();
        eina_log_level_set(EINA_LOG_LEVEL_DBG);
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief   homescreen main 
 *           homescreen main. initialize UXF, app manager, and ecore.
 *
 *  @param [in] argc    counts of argment
 *  @param [in] argv    argment
 *  @return     result
 *  @retval     0       success
 *  @retval     -1      error
 */
//--------------------------------------------------------------------------
int
main(int argc, char *argv[])
{
    try {
        printf("=== start HomeScreen main entry\n");
 
        // setupLog
        setupLog();
        ICO_INF( "START_MODULE HomeScreen" );

        // perse command options
        CicoHSCmdOpts::getInstance()->parse(argc, argv);

        // start homescreen
        CicoHSEFLApp hsEFLApp;
        int ret = hsEFLApp.start(argc, argv);

        ICO_DBG("ret = %d error=%s", ret, hsEFLApp.appfwErrorToString(ret));
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        ICO_ERR("main: catch exception [%s]", e.what());
        return -1;
    }
    catch (const std::string& str) {
        std::cerr << str << std::endl;
        ICO_ERR("main: catch exception [%s]", str.c_str());
        return -1;
    }
    catch (...) {
        ICO_ERR("main: catch exception unknown");
        return -1;
    }
    ICO_INF( "END_MODULE HomeScreen" );

    return 0;
}
// vim: set expandtab ts=4 sw=4:
