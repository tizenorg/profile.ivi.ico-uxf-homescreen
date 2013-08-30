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

#include "CicoUISample.h"
#include "CicoLog.h"
#include <Eina.h>

int
main(int argc, char **argv)
{
    eina_init();
    eina_log_level_set(EINA_LOG_LEVEL_DBG);

    try {
        CicoLog::getInstance()->openLog("DummyHomeScreen"/*TOOD*/);

        int ret = 0;
        CicoUISample uiSample;

        ret = uiSample.start(argc, argv);

        ICO_DBG("ret = %d error=%s\n", ret, uiSample.app_error_to_string(ret));
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
