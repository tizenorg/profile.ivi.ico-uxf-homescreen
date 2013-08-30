/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

/*========================================================================*/    
/**
 *  @file   main.cpp
 *
 *  @brief  
 */
/*========================================================================*/    

#include <exception>
#include <iostream>
#include <string>

#include "CicoEFLApp.h"
#include "CicoSysConDaemon.h"
#include "CicoLog.h"
#include <Eina.h>

int
main(int argc, char **argv)
{
    eina_init();
    eina_log_level_set(EINA_LOG_LEVEL_DBG);

    try {
        printf("=== start IcoSysConDaemon\n");
        ico_log_open("IcoSysConDaemon"/*TOOD*/);

        int ret = 0;
        CicoSysConDaemon daemon;

        ret = daemon.start(argc, argv);

        ICO_DBG("ret = %d error=%s\n", ret, daemon.app_error_to_string(ret));
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        ICO_ERR("main: catch exception [%s]", e.what());
    }
    catch (const std::string& str) {
        std::cerr << str << std::endl;
        ICO_ERR("main: catch exception [%s]", str.c_str());
    }
    catch (...) {
        ICO_ERR("main: catch exception unknown");
    }

    return 0;
}
/* vim: set expandtab ts=4 sw=4: */
