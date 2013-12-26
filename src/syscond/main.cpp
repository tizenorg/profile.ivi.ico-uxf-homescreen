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
 *  @file   main.cpp
 *
 *  @brief  This file is implimantion of system-controller
 *          application main function
 */
//==========================================================================

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <exception>
#include <iostream>
#include <string>

#include "CicoEFLApp.h"
#include "CicoSysConLogConfig.h"
#include "CicoSysConDaemon.h"
#include "CicoLog.h"
#include <Eina.h>

//--------------------------------------------------------------------------
/**
 *  @brief  get log level
 *
 *  @praram [in] logConf    CicoSysConLogConfig instance
 */
//--------------------------------------------------------------------------
int
getLogLevel(CicoSysConLogConfig* logConf)
{
    int level = 0;
    if (true == logConf->isPRF()) {
        level |= ICO_LOG_LVL_PRF;
    }
    if (true == logConf->isTRA()) {
        level |= ICO_LOG_LVL_TRA;
    }
    if (true == logConf->isDBG()) {
        level |= ICO_LOG_LVL_DBG;
    }
    if (true == logConf->isINF()) {
        level |= ICO_LOG_LVL_INF;
    }
    if (true == logConf->isWRN()) {
        level |= ICO_LOG_LVL_WRN;
    }
    if (true == logConf->isCRI()) {
        level |= ICO_LOG_LVL_CRI;
    }
    if (true == logConf->isERR()) {
        level |= ICO_LOG_LVL_ERR;
    }
    if (true == logConf->isFlush()) {
        level |= ICO_LOG_FLUSH;
    }
    else {
        level |= ICO_LOG_NOFLUSH;
    }

    return level;
}

//--------------------------------------------------------------------------
/**
 *  @brief   system-controller main
 *
 *  @param [in] argc    counts of argment
 *  @param [in] argv    argment
 *  @return     result
 *  @retval     0       success
 *  @retval     -1      error
 */
//--------------------------------------------------------------------------
int
main(int argc, char **argv)
{
    int     i;

    // check '--user' option
    for (i = 1; i < argc; i++)  {
        if (strcmp(argv[i], "--user") == 0) {
            i ++;
            if (i < argc)   {
                if (strcmp(argv[i], cuserid(NULL)) != 0)    {
                    printf("=== start IcoSysConDaemon main entry(user=%s)\n", argv[i]);
                    printf("=== abort IcoSysConDaemon main exit(cannot run in a '%s')\n",
                           cuserid(NULL));
                    return 0;
                }
            }
        }
    }

    try {
        printf("=== start IcoSysConDaemon main entry(uid=%d)\n", getuid());

        CicoSysConLogConfig* logConf = CicoSysConLogConfig::getInstance();
        logConf->load("/usr/apps/org.tizen.ico.system-controller/res/config/system-controller.xml");

        // setup ecore log print
        if (logConf->isEcore()) {
            eina_init();
            eina_log_level_set(EINA_LOG_LEVEL_DBG);
        }

        // setup ico_log print
        ico_log_open(logConf->getLogName().c_str());
        ico_log_set_level(getLogLevel(logConf));
        ICO_INF("START_MODULE SystemController");

        int ret = 0;
        CicoSysConDaemon daemon;

        ret = daemon.start(argc, argv);

        ICO_DBG("ret = %d error=%s", ret, daemon.appfwErrorToString(ret));

        daemon.stop();
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
    ICO_INF("END_MODULE SystemController");

    return 0;
}
// vim: set expandtab ts=4 sw=4:
