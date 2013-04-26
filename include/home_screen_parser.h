/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of command parser tools
 *
 * @date    Feb-15-2013
 */

#ifndef _HOME_SCREEN_PARSER_H_
#define _HOME_SCREEN_PARSER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <glib-object.h>
#include <json-glib/json-glib.h>

#include "ico_uxf.h"
#include "home_screen.h"
#include "ico_uxf_conf.h"

/*============================================================================*/
/* definition                                                                 */
/*============================================================================*/
#define UNEXECUTED          0
#define EXECUTED            1

#define IGNORE              (-1)
#define EXECUTE             (0)

typedef struct _hs_command_req hs_command_req_t;
struct _hs_command_req {
    struct {
        char    appid[ICO_UXF_MAX_PROCESS_NAME+1];
        struct {
            char    *appid;
        } execute;
        struct {
            char *appid;
        } terminate;
        struct {
            char *appid;
        } suspend;
        struct {
            char *appid;
        } resume;
    } program;
    struct {
        int status;
        struct {
            int window;
        } show;
        struct {
            int window;
        } hide;
        struct {
            int window;
            int visible;
            int raise;
        } visible_raise;
        struct {
            int window;
            int w;
            int h;
        } resize;
        struct {
            int window;
            int size_m;
            int size_n;
        } resize_tile;
        struct {
            int window;
            int x;
            int y;
        } move;
        struct {
            int window;
            int m;
            int n;
        } move_tile;
        struct {
            int window;
        } raise;
        struct {
            int window;
        } lower;
        struct {
            int window;
            unsigned int    mask;
        } event_mask;
    } window[ICO_HS_WINDOW_NUM_MAX];
    struct {
        struct {
            int  input;
        } add_app;
        struct {
            int  input;
        } del_app;
    } input;
};

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
extern int hs_parse_form(char *, int);
extern int hs_conftrol_form(const char*, int);
extern void hs_dump_app(void);
extern int hs_control_app_screen(void);
extern void hs_get_tilescreendata(void);

extern int hs_req_touch;
extern int hs_stat_touch;

#endif /* _HOME_SCREEN_PARSER_H_ */
