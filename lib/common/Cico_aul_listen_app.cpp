/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Homescreen application aul handler multi support
 *
 * @date    Sep-23-2013
 */
#include <iostream>
#include <string>
#include <vector>
#include <aul/aul.h>

using namespace std;


//bool aul_listen_app_launch_signal_del(int (*func) (int, void *));
//bool aul_listen_app_dead_signal_del(int (*func) (int, void *));

static vector<void*> launch_f;
static vector<void*> launch_p;
static vector<void*> dead_f;
static vector<void*> dead_p;

/**
 * @brief aul launch handler
 * @param pid
 * @param data user data
 */
static int app_launch_handler(int pid, void *data)
{
    vector<void*>::iterator itf = launch_f.begin();
    vector<void*>::iterator itp = launch_p.begin();
    while(itf != launch_f.end()) {
        void* f = *itf;
        void* p = *itp;
        int (*fnc)(int, void*) = (int (*)(int, void*))f;
        fnc(pid, p);
        ++itf;
        ++itp;
    }
    return 0;
}

/**
 * @brief aul dead handler
 * @param pid
 * @param data user data
 */
static int app_dead_handler(int pid, void *data)
{
    vector<void*>::iterator itf = dead_f.begin();
    vector<void*>::iterator itp = dead_p.begin();
    while(itf != dead_f.end()) {
        void* f = *itf;
        void* p = *itp;
        int (*fnc)(int, void*) = (int (*)(int, void*))f;
        fnc(pid, p);
        ++itf;
        ++itp;
    }
    return 0;
}

/**
 * @ init. aul callback
 */
void initAulListenXSignal()
{
    aul_listen_app_launch_signal(app_launch_handler, NULL);
    aul_listen_app_dead_signal(app_dead_handler, NULL);
}

/**
 * @brief aul_listien_app_launch_signal multi
 * @param func callback function
 * @param data user data
 */
bool aul_listen_app_launch_signal_add(int (*func) (int, void *), void *data)
{
    launch_f.push_back((void*)func);
    launch_p.push_back(data);
    return true;
}

/**
 * @brief aul_listien_app_dead_signal multi
 * @param func callback function
 * @param data user data
 */
bool aul_listen_app_dead_signal_add(int (*func) (int, void *), void *data)
{
    dead_f.push_back((void*)func);
    dead_p.push_back(data);
    return true;
}
// vim:set expandtab ts=4 sw=4:
