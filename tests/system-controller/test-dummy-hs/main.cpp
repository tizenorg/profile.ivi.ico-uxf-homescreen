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
#include <Ecore.h>
#include <Eina.h>
#include <Ecore_Evas.h>

#include <pthread.h>
pthread_t thread;

void *
uiMain(void *args)
{
    CicoUISample uiSample;
    uiSample.start(0, NULL);

    return NULL;
}

#include <readline/readline.h>
#include <readline/history.h>
#define MAX_HISTORY_NO 3
void
consoleMain(void)
{
    char *prompt = (char*)"ico >";
    char *line   = NULL;
    int history_no = 0;
    HIST_ENTRY *history = NULL;

    printf("consolMain Enter\n");
    while (1) {
        line = readline(prompt);

        if (strcmp(line, "exit") == 0) {
            break;
        }

        add_history(line);
        if (++history_no > MAX_HISTORY_NO) {
            history = remove_history(0);
            free(history);
        }

        free(line);
    }

    clear_history();
}

int
main(int argc, char **argv)
{
    //eina_init();
    //eina_log_level_set(EINA_LOG_LEVEL_DBG);

    try {
        CicoLog::getInstance()->openLog("DummyHomeScreen"/*TOOD*/);

#if 1
        /* pthread start */
        int ret = pthread_create(&thread, NULL, uiMain, (void *)NULL);
        if (-1 == ret) {
            ICO_ERR("pthread_create failed(%d:%s)",
                 errno, strerror(errno));
            return -1;
        }
#endif
        consoleMain();

        ecore_main_loop_quit();
        ecore_evas_shutdown();
        pthread_join(thread, NULL);

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
