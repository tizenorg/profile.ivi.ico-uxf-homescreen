/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   test suite for App Resource Control API
 *
 * @date    July-31-2013
 */

#include<string.h>

#include <ail.h>

/* ----------------------------------------------- */
/* Main                                            */
/* ----------------------------------------------- */
int
main(int argc, char **argv)
{
    int id;

    for (id = 0; id < argc; id++) {
        if (strcmp(argv[id], "-add") == 0) {
            ail_desktop_add("test.ico.res.app");
        }
        else if (strcmp(argv[id], "-del") == 0) {
            ail_desktop_remove("test.ico.res.app");
        }
    }

    return 0;
}
/* vim: set expandtab ts=4 sw=4: */
