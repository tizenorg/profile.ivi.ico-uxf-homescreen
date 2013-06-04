/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   parser of jason receive from external homescreen tools
 *
 * @date    Feb-15-2013
 */

#include "home_screen_parser.h"

/*============================================================================*/
/* static(internal) functions prototype                                       */
/*============================================================================*/
static int hs_parse_value_string(JsonObject *, char *, char *);
static int hs_parse_value_integer(JsonObject *, char *, int *);
static int hs_hs_parse_value_integer2(JsonObject *, char *, int *, int *);
static int hs_hs_parse_value_integer3(JsonObject *, char *, int *, int *, int *);
static int hs_parse_program(JsonNode *, int);
static int hs_parse_input(JsonNode *, int);
static int hs_parse_window(JsonNode *, int, int);
static int hs_parse_application(JsonNode *, int);

/*============================================================================*/
/* variables and tables                                                */
/*============================================================================*/
int hs_req_touch = ICO_HS_TOUCH_TOBE_SHOW; /* request about TOUCH */
int hs_stat_touch = ICO_HS_TOUCH_IN_SHOW; /* status of TOUCH */
static hs_command_req_t hs_command_req[ICO_HS_APP_NUM_MAX];
static int hs_command_delay;

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_parse_form
 *          parse json command main
 *
 * @param[in]   data                json data
 * @param[in]   length              json data length
 * @return      result
 * @retval      0                   Success
 * @retval      1                   Failed
 */
/*--------------------------------------------------------------------------*/
int
hs_parse_form(char *data, int length)
{
    GError *error;

    int st;
    int ii, jj;
    int n;
    JsonParser *parser;
    JsonNode *root;
    JsonObject *object;
    JsonArray *array;
    JsonNode *node;

    hs_command_delay = 0;
    memset(hs_command_req, 0, sizeof(hs_command_req));

    for (ii = 0; ii < ICO_HS_APP_NUM_MAX; ii++) {
        hs_command_req[ii].program.execute.appid = NULL;
        hs_command_req[ii].program.terminate.appid = NULL;
        hs_command_req[ii].program.suspend.appid = NULL;
        hs_command_req[ii].program.resume.appid = NULL;
        for (jj = 0; jj < ICO_HS_WINDOW_NUM_MAX; jj++) {
            hs_command_req[ii].window[jj].status = UNEXECUTED;
            hs_command_req[ii].window[jj].show.window = IGNORE;
            hs_command_req[ii].window[jj].hide.window = IGNORE;
            hs_command_req[ii].window[jj].visible_raise.window = IGNORE;
            hs_command_req[ii].window[jj].resize.window = IGNORE;
            hs_command_req[ii].window[jj].resize_tile.window = IGNORE;
            hs_command_req[ii].window[jj].move.window = IGNORE;
            hs_command_req[ii].window[jj].move_tile.window = IGNORE;
            hs_command_req[ii].window[jj].raise.window = IGNORE;
            hs_command_req[ii].window[jj].lower.window = IGNORE;
            hs_command_req[ii].window[jj].event_mask.window = IGNORE;
        }
        hs_command_req[ii].input.add_app.input = IGNORE;
        hs_command_req[ii].input.del_app.input = IGNORE;
    }

    g_type_init();

    parser = json_parser_new();

    error = NULL;
    json_parser_load_from_data(parser, data, length, &error);
    if (error) {
        uifw_error("json_parser_load_from_data = %s", error->message);
        g_error_free(error);
        g_object_unref(parser);
        return 1;
    }

    root = json_parser_get_root(parser);
    if (!root) {
        uifw_error("json_parser_get_root");
        g_object_unref(parser);
        return 1;
    }

    if (JSON_NODE_TYPE(root) != JSON_NODE_OBJECT) {
        uifw_error("type error = %d", JSON_NODE_TYPE(root));
        return 1;
    }

    object = json_node_get_object(root);
    if (!object) {
        uifw_error("json_node_get_object");
        g_object_unref(parser);
        return 1;
    }

    n = json_object_get_size(object);
    if (n < 1) {
        uifw_error("json_object_get_size = %d", n);
        g_object_unref(parser);
        return 1;
    }
    else if (n > 1) {
        st = hs_parse_value_integer(object, "delay", &hs_command_delay);
        if (st != 0) {
            uifw_error("hs_parse_value_integer = %d", st);
        }
        uifw_debug("delay = %d", hs_command_delay);
    }

    node = json_object_get_member(object, "application");
    if (!node) {
        uifw_error("json_object_get_member(%s)", "application");
        g_object_unref(parser);
        return 1;
    }

    if (JSON_NODE_TYPE(node) != JSON_NODE_ARRAY) {
        uifw_error("type error = %d", JSON_NODE_TYPE(node));
        g_object_unref(parser);
        return 1;
    }

    array = json_node_get_array(node);
    if (!array) {
        uifw_error("json_node_get_array");
        g_object_unref(parser);
        return 1;
    }

    n = json_array_get_length(array);
    if (n < 0) {
        uifw_error("json_array_get_length = %d", n);
        g_object_unref(parser);
        return 1;
    }

    for (ii = 0; ii < n; ii++) {
        node = json_array_get_element(array, ii);
        if (!node) {
            uifw_error("json_array_get_element");
            g_object_unref(parser);
            return 1;
        }

        if (JSON_NODE_TYPE(node) != JSON_NODE_OBJECT) {
            uifw_error("type error = %d", JSON_NODE_TYPE(node));
            g_object_unref(parser);
            return 1;
        }

        st = hs_parse_application(node, ii);
        if (st != 0) {
            uifw_error("hs_parse_application = %d", st);
            g_object_unref(parser);
            return 1;
        }
    }

    g_object_unref(parser);

    /* get now status of TOUCH */
    if (hs_stat_touch == ICO_HS_TOUCH_IN_HIDE) {
        hs_req_touch = ICO_HS_TOUCH_TOBE_HIDE;
    }
    else if (hs_stat_touch == ICO_HS_TOUCH_IN_SHOW) {
        hs_req_touch = ICO_HS_TOUCH_TOBE_SHOW;
    }
    /* check if TOUCH is raise or not */
    for (ii = 0; ii < ICO_HS_APP_NUM_MAX; ii++) {
        /* is TOUCH? */
        uifw_debug("hs_parse_form: Appid = %s",
                   hs_command_req[ii].program.appid);
        if (strlen(hs_command_req[ii].program.appid) == 0)
            break;
        if (strncmp(hs_command_req[ii].program.appid,
                    ICO_HS_APPID_DEFAULT_TOUCH, ICO_UXF_MAX_PROCESS_NAME) != 0) {
            continue;
        }

        if ((hs_command_req[ii].window[0].hide.window != IGNORE)
                || ((hs_command_req[ii].window[0].visible_raise.window
                        != IGNORE)
                        && (hs_command_req[ii].window[0].visible_raise.visible
                                == 0))) {
            /* TOUCH is to be hide */
            hs_req_touch = ICO_HS_TOUCH_TOBE_HIDE;
        }
        else if ((hs_command_req[ii].window[0].show.window != IGNORE)
                || ((hs_command_req[ii].window[0].visible_raise.window
                        != IGNORE)
                        && (hs_command_req[ii].window[0].visible_raise.visible
                                == 1))) {
            /* TOUCH is to be show */
            hs_req_touch = ICO_HS_TOUCH_TOBE_SHOW;
        }
    }

    return 0;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_control_app_screen
 *          this function is called when received json command, and change
 *          applicatoin window that json command indicate.
 *
 * @param       none
 * @return      result
 * @retval      0                   Success
 * @retval      1                   Failed
 */
/*--------------------------------------------------------------------------*/
int
hs_control_app_screen(void)
{
    int ii, jj, ret, winNo, idx;
    Ico_Uxf_ProcessWin wins[2];
    Ico_Uxf_ProcessAttr proc;
    char appid[ICO_UXF_MAX_PROCESS_NAME + 1];

    for (ii = 0; ii < ICO_HS_APP_NUM_MAX; ii++) {
        if (strlen(hs_command_req[ii].program.appid) == 0)
            break;
        memset(appid, 0, sizeof(appid));
        strncpy(appid, hs_command_req[ii].program.appid,
                ICO_UXF_MAX_PROCESS_NAME);
        winNo = 0;
        if (strncmp(hs_command_req[ii].program.appid,
                    ICO_HS_APPID_DEFAULT_TOUCH, ICO_UXF_MAX_PROCESS_NAME) == 0) {
            memset(appid, 0, sizeof(appid));
            strncpy(appid, hs_name_homescreen, ICO_UXF_MAX_PROCESS_NAME);
            winNo = 1;
        }
        ret = ico_uxf_process_attribute_get(appid, &proc);
        if (ret < 0)
            continue;
        if ((proc.status == ICO_UXF_PROCSTATUS_STOP) || (proc.status
                == ICO_UXF_PROCSTATUS_TERM)) {
            if (hs_command_req[ii].program.execute.appid != NULL) {
                ret = ico_uxf_process_execute(
                        hs_command_req[ii].program.execute.appid);
                if (ret >= 0) {
                    idx = hs_tile_get_minchange();
                    hs_tile_set_app(idx,
                                    hs_command_req[ii].program.execute.appid);
                }
                uifw_debug("HOMESCREEN excute %s = %d",
                           hs_command_req[ii].program.execute.appid, 0);
                if (hs_command_delay > 0)
                    usleep(hs_command_delay);
            }
        }

        /* regist app screen info */
        idx = hs_get_index_appscreendata(hs_command_req[ii].program.appid);
        if (idx >= 0) {
            if (hs_command_req[ii].program.execute.appid != NULL) {
                strcpy(hs_app_screen_window[idx].exe_name,
                       hs_command_req[ii].program.execute.appid);
            }
            for (jj = 0; jj < ICO_HS_WINDOW_NUM_MAX; jj++) {
                /* default invisible */
                if (hs_command_req[ii].window[jj].visible_raise.window
                        != IGNORE) {
                    hs_app_screen_window[idx].visible
                            = hs_command_req[ii].window[jj].visible_raise.visible; /* visible */
                    hs_app_screen_window[idx].raise
                            = hs_command_req[ii].window[jj].visible_raise.raise; /* raise */
                }
                if (hs_command_req[ii].window[jj].show.window != IGNORE) {
                    hs_app_screen_window[idx].visible = 1;
                }
                if (hs_command_req[ii].window[jj].hide.window != IGNORE) {
                    hs_app_screen_window[idx].visible = 0;
                }
                if (hs_command_req[ii].window[jj].raise.window != IGNORE) {
                    hs_app_screen_window[idx].raise = 1;
                }
                if (hs_command_req[ii].window[jj].lower.window != IGNORE) {
                    hs_app_screen_window[idx].raise = 0;
                }
                if (hs_command_req[ii].window[jj].resize.window != IGNORE) {
                    hs_app_screen_window[idx].resize_w
                            = hs_command_req[ii].window[jj].resize.w; /* resize_x */
                    hs_app_screen_window[idx].resize_h
                            = hs_command_req[ii].window[jj].resize.h; /* resize_y */
                }
                if (hs_command_req[ii].window[jj].move.window != IGNORE) {
                    hs_app_screen_window[idx].move_x
                            = hs_command_req[ii].window[jj].move.x; /* move_x */
                    hs_app_screen_window[idx].move_y
                            = hs_command_req[ii].window[jj].move.y; /* move_y */
                }
            }
        }

        ret = ico_uxf_process_window_get(appid, wins, 2);
        if (ret >= 0) {
            hs_conftrol_form(hs_command_req[ii].program.appid,
                             wins[winNo].window);
        }
    }
    if (hs_req_touch == ICO_HS_TOUCH_TOBE_SHOW) {
        hs_stat_touch = ICO_HS_TOUCH_IN_SHOW;
    }
    else if (hs_req_touch == ICO_HS_TOUCH_TOBE_HIDE) {
        hs_stat_touch = ICO_HS_TOUCH_IN_HIDE;
    }

    return 0;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_conftrol_form
 *          change the application window parameter.
 *
 * @param[in]   process             applicatoin id
 * @return      window              window
 * @retval      0                   Success
 * @retval      1                   Failed
 */
/*--------------------------------------------------------------------------*/
int
hs_conftrol_form(const char *process, int window)
{
    int ii, jj, kk, idx;
    int st = 0;

    uifw_trace("in(%s,%d)", process, window);

    Ico_Uxf_WindowAttr windows[ICO_HS_APP_NUM_MAX];
    int num = ico_uxf_window_query_windows(ICO_UXF_DISPLAY_DEFAULT, windows,
                                           ICO_HS_APP_NUM_MAX);
    int count;
    int target;

    for (ii = 0; ii < ICO_HS_APP_NUM_MAX; ii++) {
        if (strncmp(hs_command_req[ii].program.appid, process,
                    ICO_UXF_MAX_PROCESS_NAME) != 0)
            continue;

        if (hs_command_req[ii].program.terminate.appid != NULL) {
            st = ico_uxf_process_terminate(hs_command_req[ii].program.terminate.appid);
            idx = hs_tile_get_index_app(hs_command_req[ii].program.terminate.appid);
            if (idx >= 0) {
                hs_tile_free_app(idx);
            }
            else {
                hs_tile_delete_hide(hs_command_req[ii].program.terminate.appid);
            }
            uifw_debug("FORM terminate %s = %d",
                       hs_command_req[ii].program.terminate.appid, st);
            if (hs_command_delay > 0)
                usleep(hs_command_delay);
        }

        if (window == 0)
            continue;

        for (jj = 0; jj < ICO_HS_WINDOW_NUM_MAX; jj++) {
            if (hs_command_req[ii].window[jj].status == EXECUTED)
                continue;

            if (hs_command_req[ii].window[jj].hide.window != IGNORE) {
                target = -1;
                if (hs_command_req[ii].window[jj].hide.window == 0) {
                    target = window;
                }
                else {
                    count = 0;
                    for (kk = 0; kk < num; kk++) {
                        if (process != windows[kk].process)
                            continue;
                        if (count == hs_command_req[ii].window[jj].hide.window) {
                            target = windows[kk].window;
                            break;
                        }
                        count++;
                    }
                }
                if (target >= 0) {
                    st = ico_uxf_window_hide(target);
                }
                uifw_debug("FORM hide %d = %d", target, st);
                if (hs_command_delay > 0)
                    usleep(hs_command_delay);
            }

            if (hs_command_req[ii].window[jj].resize.window != IGNORE) {
                target = -1;
                if (hs_command_req[ii].window[jj].resize.window == 0) {
                    target = window;
                }
                else {
                    count = 0;
                    for (kk = 0; kk < num; kk++) {
                        if (process != windows[kk].process)
                            continue;
                        if (count == hs_command_req[ii].window[jj].resize.window) {
                            target = windows[kk].window;
                            break;
                        }
                        count++;
                    }
                }
                if (target >= 0) {
                    st = ico_uxf_window_resize(target,
                                    hs_command_req[ii].window[jj].resize.w,
                                    hs_command_req[ii].window[jj].resize.h);
                }
                uifw_debug("FORM resize %d %d %d = %d", target,
                           hs_command_req[ii].window[jj].resize.w,
                           hs_command_req[ii].window[jj].resize.h, st);
                if (hs_command_delay > 0)
                    usleep(hs_command_delay);
            }

            if (hs_command_req[ii].window[jj].move.window != IGNORE) {
                target = -1;
                if (hs_command_req[ii].window[jj].move.window == 0) {
                    target = window;
                }
                else {
                    count = 0;
                    for (kk = 0; kk < num; kk++) {
                        if (process != windows[kk].process)
                            continue;
                        if (count == hs_command_req[ii].window[jj].move.window) {
                            target = windows[kk].window;
                            break;
                        }
                        count++;
                    }
                }
                if (target >= 0) {
                    st = ico_uxf_window_move(target,
                                       hs_command_req[ii].window[jj].move.x,
                                       hs_command_req[ii].window[jj].move.y);
                }
                uifw_debug("FORM move %d %d %d = %d", target,
                           hs_command_req[ii].window[jj].move.x,
                           hs_command_req[ii].window[jj].move.y, st);
                if (hs_command_delay > 0)
                    usleep(hs_command_delay);
            }

            if (hs_command_req[ii].window[jj].visible_raise.window != IGNORE) {
                target = -1;
                if (hs_command_req[ii].window[jj].visible_raise.window == 0) {
                    target = window;
                }
                else {
                    count = 0;
                    for (kk = 0; kk < num; kk++) {
                        if (process != windows[kk].process)
                            continue;
                        if (count == hs_command_req[ii].window[jj].visible_raise.window) {
                            target = windows[kk].window;
                            break;
                        }
                        count++;
                    }
                }
                if (target >= 0) {
                    st = ico_uxf_window_visible_raise(target,
                                  hs_command_req[ii].window[jj].visible_raise.visible,
                                  hs_command_req[ii].window[jj].visible_raise.raise);
                }
                uifw_debug("FORM visible_raise %d %d %d = %d", target,
                           hs_command_req[ii].window[jj].visible_raise.visible,
                           hs_command_req[ii].window[jj].visible_raise.raise,
                           st);
                if (hs_command_delay > 0)
                    usleep(hs_command_delay);
            }

            if (hs_command_req[ii].window[jj].raise.window != IGNORE) {
                target = -1;
                if (hs_command_req[ii].window[jj].raise.window == 0) {
                    target = window;
                }
                else {
                    count = 0;
                    for (kk = 0; kk < num; kk++) {
                        if (process != windows[kk].process)
                            continue;
                        if (count == hs_command_req[ii].window[jj].raise.window) {
                            target = windows[kk].window;
                            break;
                        }
                        count++;
                    }
                }
                if (target >= 0) {
                    st = ico_uxf_window_raise(target);
                }
                uifw_debug("FORM raise %d = %d", target, st);
                if (hs_command_delay > 0)
                    usleep(hs_command_delay);
            }

            if (hs_command_req[ii].window[jj].lower.window != IGNORE) {
                target = -1;
                if (hs_command_req[ii].window[jj].lower.window == 0) {
                    target = window;
                }
                else {
                    count = 0;
                    for (kk = 0; kk < num; kk++) {
                        if (process != windows[kk].process)
                            continue;
                        if (count == hs_command_req[ii].window[jj].lower.window) {
                            target = windows[kk].window;
                            break;
                        }
                        count++;
                    }
                }
                if (target >= 0) {
                    st = ico_uxf_window_lower(target);
                }
                uifw_debug("FORM lower %d = %d", target, st);
                if (hs_command_delay > 0)
                    usleep(hs_command_delay);
            }

            if (hs_command_req[ii].window[jj].event_mask.window != IGNORE) {
                target = -1;
                if (hs_command_req[ii].window[jj].event_mask.window == 0) {
                    target = window;
                }
                else {
                    count = 0;
                    for (kk = 0; kk < num; kk++) {
                        if (process != windows[kk].process)
                            continue;
                        if (count == hs_command_req[ii].window[jj].event_mask.window) {
                            target = windows[kk].window;
                            break;
                        }
                        count++;
                    }
                }
                if (target >= 0) {
                    st = ico_uxf_window_event_mask(target,
                                    hs_command_req[ii].window[jj].event_mask.mask);
                }
                uifw_debug("FORM event_mask %d %08x = %d", target,
                           hs_command_req[ii].window[jj].event_mask.mask, st);
                if (hs_command_delay > 0)
                    usleep(hs_command_delay);
            }

            /* last show */
            if (hs_command_req[ii].window[jj].show.window != IGNORE) {
                target = -1;
                if (hs_command_req[ii].window[jj].show.window == 0) {
                    target = window;
                }
                else {
                    count = 0;
                    for (kk = 0; kk < num; kk++) {
                        if (process != windows[kk].process)
                            continue;
                        if (count == hs_command_req[ii].window[jj].show.window) {
                            target = windows[kk].window;
                            break;
                        }
                        count++;
                    }
                }
                if (target >= 0) {
                    st = ico_uxf_window_show(target);
                }
                uifw_debug("FORM show %d = %d", target, st);
                if (hs_command_delay > 0)
                    usleep(hs_command_delay);
            }

            hs_command_req[ii].window[jj].status = EXECUTED;
        }
    }

    uifw_debug("out(%d)", st);
    return st;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_dump_app
 *          dump application window status.
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
hs_dump_app(void)
{
    int ii, jj;
    for (ii = 0; ii < ICO_HS_APP_NUM_MAX; ii++) {
        if (hs_command_req[ii].program.appid[0] == 0)
            continue;
        uifw_debug("id: %s", hs_command_req[ii].program.appid);
        if (hs_command_req[ii].program.execute.appid != NULL) {
            uifw_debug(" execute: %s", hs_command_req[ii].program.execute.appid);
        }
        if (hs_command_req[ii].program.terminate.appid != NULL) {
            uifw_debug(" terminate: %s",
                       hs_command_req[ii].program.terminate.appid);
        }
        if (hs_command_req[ii].program.suspend.appid != NULL) {
            uifw_debug(" suspend: %s", hs_command_req[ii].program.suspend.appid);
        }
        if (hs_command_req[ii].program.resume.appid != NULL) {
            uifw_debug(" resume: %d", hs_command_req[ii].program.resume.appid);
        }
        for (jj = 0; jj < ICO_HS_WINDOW_NUM_MAX; jj++) {
            if (hs_command_req[ii].window[jj].show.window != IGNORE) {
                uifw_debug(" show: %d",
                           hs_command_req[ii].window[jj].show.window);
            }
            if (hs_command_req[ii].window[jj].hide.window != IGNORE) {
                uifw_debug(" hide: %d",
                           hs_command_req[ii].window[jj].hide.window);
            }
            if (hs_command_req[ii].window[jj].visible_raise.window != IGNORE) {
                uifw_debug(" visible_raise: %d %d %d",
                           hs_command_req[ii].window[jj].visible_raise.window,
                           hs_command_req[ii].window[jj].visible_raise.visible,
                           hs_command_req[ii].window[jj].visible_raise.raise);
            }
            if (hs_command_req[ii].window[jj].resize.window != IGNORE) {
                uifw_debug(" resize: %d %d %d",
                           hs_command_req[ii].window[jj].resize.window,
                           hs_command_req[ii].window[jj].resize.w,
                           hs_command_req[ii].window[jj].resize.h);
            }
            if (hs_command_req[ii].window[jj].resize_tile.window != IGNORE) {
                uifw_debug(" resize_tile: %d %d %d",
                           hs_command_req[ii].window[jj].resize_tile.window,
                           hs_command_req[ii].window[jj].resize_tile.size_m,
                           hs_command_req[ii].window[jj].resize_tile.size_n);
            }
            if (hs_command_req[ii].window[jj].move.window != IGNORE) {
                uifw_debug(" move: %d %d %d",
                           hs_command_req[ii].window[jj].move.window,
                           hs_command_req[ii].window[jj].move.x,
                           hs_command_req[ii].window[jj].move.y);
            }
            if (hs_command_req[ii].window[jj].move_tile.window != IGNORE) {
                uifw_debug(" move_tile: %d %d %d",
                           hs_command_req[ii].window[jj].move_tile.window,
                           hs_command_req[ii].window[jj].move_tile.m,
                           hs_command_req[ii].window[jj].move_tile.n);
            }
            if (hs_command_req[ii].window[jj].raise.window != IGNORE) {
                uifw_debug(" raise: %d",
                           hs_command_req[ii].window[jj].raise.window);
            }
            if (hs_command_req[ii].window[jj].lower.window != IGNORE) {
                uifw_debug(" lower: %d",
                           hs_command_req[ii].window[jj].lower.window);
            }
            if (hs_command_req[ii].window[jj].event_mask.window != IGNORE) {
                uifw_debug(" event_mask: %d %08x",
                           hs_command_req[ii].window[jj].event_mask.window,
                           hs_command_req[ii].window[jj].event_mask.mask);
            }
        }
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_get_tilescreendata
 *          check json command for tile screen 
 *
 * @param       none
 * @return      result
 * @retval      0                   Success
 * @retval      1                   Failed
 */
/*--------------------------------------------------------------------------*/
void
hs_get_tilescreendata(void)
{
    int ii, jj, idx = -1;
    memset(hs_command_tile_req, 0x00, sizeof(hs_command_tile_req)); /* init */
    for (ii = 0; ii < ICO_HS_APP_NUM_MAX; ii++) {
        if (strlen(hs_command_req[ii].program.appid) == 0)
            continue;
        strncpy(hs_command_tile_req[ii].appid,
                hs_command_req[ii].program.appid, ICO_UXF_MAX_PROCESS_NAME);
        if (hs_command_req[ii].program.execute.appid != NULL) {
            strncpy(hs_command_tile_req[ii].exe_name,
                    hs_command_req[ii].program.execute.appid,
                    ICO_UXF_MAX_PROCESS_NAME); /* execute name */
        }
        hs_command_tile_req[ii].terminate = IGNORE;
        if (hs_command_req[ii].program.terminate.appid != NULL) {
            hs_command_tile_req[ii].terminate = EXECUTE; /* terminate id */
        }
        for (jj = 0; jj < ICO_HS_WINDOW_NUM_MAX; jj++) {
            if (hs_command_req[ii].window[jj].visible_raise.window != IGNORE) {
                hs_command_tile_req[ii].visible
                        = hs_command_req[ii].window[jj].visible_raise.visible; /* visible */
                hs_command_tile_req[ii].raise
                        = hs_command_req[ii].window[jj].visible_raise.raise; /* raise */
            }
            if (hs_command_req[ii].window[jj].resize.window != IGNORE) {
                hs_command_tile_req[ii].resize_w
                        = hs_command_req[ii].window[jj].resize.w; /* resize_x */
                hs_command_tile_req[ii].resize_h
                        = hs_command_req[ii].window[jj].resize.h; /* resize_y */
            }
            if (hs_command_req[ii].window[jj].move.window != IGNORE) {
                hs_command_tile_req[ii].move_x
                        = hs_command_req[ii].window[jj].move.x; /* move_x */
                hs_command_tile_req[ii].move_y
                        = hs_command_req[ii].window[jj].move.y; /* move_y */
            }
        }

        /* set app to tile */
        idx = hs_tile_get_index_app(hs_command_tile_req[ii].appid);
        if (idx >= 0) {
            hs_tile_free_app(idx);
        }
        if (hs_command_tile_req[ii].terminate == IGNORE) {
            idx = hs_tile_get_index_pos(hs_command_tile_req[ii].move_x,
                                        hs_command_tile_req[ii].move_y,
                                        hs_command_tile_req[ii].resize_w,
                                        hs_command_tile_req[ii].resize_h);
            uifw_debug("hs_get_tilescreendata: hs_tile_get_index_pos=%d(%d, %d, %d, %d)",
                       idx, hs_command_tile_req[ii].move_x,
                       hs_command_tile_req[ii].move_y,
                       hs_command_tile_req[ii].resize_w,
                       hs_command_tile_req[ii].resize_h);
            if (idx >= 0) {
                hs_tile_set_app(idx, hs_command_tile_req[ii].appid);
            }
        }
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_parse_value_string
 *          read the string value that indicated by the key name
 *
 * @param[in]   object              json data
 * @param[in]   name                key name
 * @param[out]  value               the value stored
 * @return      result
 * @retval      0                   Success
 * @retval      1                   Failed
 */
/*--------------------------------------------------------------------------*/
static int
hs_parse_value_string(JsonObject *object, char *name, char *value)
{
    JsonNode *node;

    node = json_object_get_member(object, name);
    if (!node) {
        return 1;
    }

    if (JSON_NODE_TYPE(node) != JSON_NODE_VALUE) {
        uifw_error("type error = %d", JSON_NODE_TYPE(node));
        return 1;
    }

    GValue data = G_VALUE_INIT;
    json_node_get_value(node, &data);

    strcpy(value, g_value_get_string(&data));

    uifw_trace("out");
    return 0;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_parse_value_integer
 *          read the interger value that indicated by the key name
 *
 * @param[in]   object              json data
 * @param[in]   name                key name
 * @param[out]  value               the value stored
 * @return      result
 * @retval      0                   Success
 * @retval      1                   Failed
 */
/*--------------------------------------------------------------------------*/
static int
hs_parse_value_integer(JsonObject *object, char *name, int *value)
{
    JsonNode *node;
    node = json_object_get_member(object, name);
    if (!node) {
        return 1;
    }

    if (JSON_NODE_TYPE(node) != JSON_NODE_VALUE) {
        uifw_error("type error = %d", JSON_NODE_TYPE(node));
        return 1;
    }

    GValue data = G_VALUE_INIT;
    json_node_get_value(node, &data);

    char temp[ICO_HS_TEMP_BUF_SIZE];

    strcpy(temp, g_value_get_string(&data));

    char *p;
    p = strtok(temp, ",");
    if (!p) {
        uifw_error("strtok error = %s", temp);
        return 1;
    }

    if (strncmp(p, "0x", 2) == 0) {
        *value = strtol(p, NULL, 16);
    }
    else {
        *value = strtol(p, NULL, 10);
    }

    return 0;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_hs_parse_value_integer2
 *          read the two interger value that indicated by the key name
 *
 * @param[in]   object              json data
 * @param[in]   name                key name
 * @param[out]  value1              the 1st value stored
 * @param[out]  value2              the 2nd value stored
 * @return      result
 * @retval      0                   Success
 * @retval      1                   Failed
 */
/*--------------------------------------------------------------------------*/
static int
hs_hs_parse_value_integer2(JsonObject *object, char *name, int *value1,
                           int *value2)
{
    JsonNode *node;
    node = json_object_get_member(object, name);
    if (!node) {
        return 1;
    }

    if (JSON_NODE_TYPE(node) != JSON_NODE_VALUE) {
        uifw_error("type error = %d", JSON_NODE_TYPE(node));
        return 1;
    }

    GValue data = G_VALUE_INIT;
    json_node_get_value(node, &data);

    char temp[ICO_HS_TEMP_BUF_SIZE];

    strcpy(temp, g_value_get_string(&data));

    char *p;
    p = strtok(temp, ",");
    if (!p) {
        uifw_error("strtok error = %s", temp);
        return 1;
    }

    if (strncmp(p, "0x", 2) == 0) {
        *value1 = strtol(p, NULL, 16);
    }
    else {
        *value1 = strtol(p, NULL, 10);
    }

    p = strtok(NULL, ",");
    if (!p) {
        uifw_error("strtok error = %s", temp);
        return 1;
    }

    if (strncmp(p, "0x", 2) == 0) {
        *value2 = strtol(p, NULL, 16);
    }
    else {
        *value2 = strtol(p, NULL, 10);
    }

    return 0;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_hs_parse_value_integer3
 *          read the three interger value that indicated by the key name
 *
 * @param[in]   object              json data
 * @param[in]   name                key name
 * @param[out]  value1              the 1st value stored
 * @param[out]  value2              the 2nd value stored
 * @param[out]  value3              the 3rd value stored
 * @return      result
 * @retval      0                   Success
 * @retval      1                   Failed
 */
/*--------------------------------------------------------------------------*/
static int
hs_hs_parse_value_integer3(JsonObject *object, char *name, int *value1,
                           int *value2, int *value3)
{
    JsonNode *node;
    node = json_object_get_member(object, name);
    if (!node) {
        return 1;
    }

    if (JSON_NODE_TYPE(node) != JSON_NODE_VALUE) {
        uifw_error("type error = %d", JSON_NODE_TYPE(node));
        return 1;
    }

    GValue data = G_VALUE_INIT;
    json_node_get_value(node, &data);

    char temp[ICO_HS_TEMP_BUF_SIZE];

    strcpy(temp, g_value_get_string(&data));

    char *p;
    p = strtok(temp, ",");
    if (!p) {
        uifw_error("strtok error = %s", temp);
        return 1;
    }

    if (strncmp(p, "0x", 2) == 0) {
        *value1 = strtol(p, NULL, 16);
    }
    else {
        *value1 = strtol(p, NULL, 10);
    }

    p = strtok(NULL, ",");
    if (!p) {
        uifw_error("strtok error = %s", temp);
        return 1;
    }

    if (strncmp(p, "0x", 2) == 0) {
        *value2 = strtol(p, NULL, 16);
    }
    else {
        *value2 = strtol(p, NULL, 10);
    }

    p = strtok(NULL, ",");
    if (!p) {
        uifw_error("strtok error = %s", temp);
        return 1;
    }

    if (strncmp(p, "0x", 2) == 0) {
        *value3 = strtol(p, NULL, 16);
    }
    else {
        *value3 = strtol(p, NULL, 10);
    }

    return 0;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_parse_program
 *          parse "program" section
 *
 * @param[in]   node                json node
 * @param[in]   id                  sequence number of the node
 * @return      result
 * @retval      0                   Success
 * @retval      1                   Failed
 */
/*--------------------------------------------------------------------------*/
static int
hs_parse_program(JsonNode *node, int id)
{
    int st;
    JsonObject *object;
    char tmp[ICO_UXF_MAX_PROCESS_NAME + 1];

    object = json_node_get_object(node);
    if (!object) {
        uifw_error("json_node_get_object");
        return 1;
    }

    st = hs_parse_value_string(object, "id", hs_command_req[id].program.appid);
    if (st != 0) {
        uifw_error("hs_parse_value_string = %d", st);
        memset(hs_command_req[id].program.appid, 0, ICO_UXF_MAX_PROCESS_NAME
                + 1);
        return 1;
    }

    st = hs_parse_value_string(object, "execute", tmp);
    if (st != 0) {
        uifw_error("hs_parse_value_string = %d", st);
    }
    else {
        hs_command_req[id].program.execute.appid
                = hs_command_req[id].program.appid;
    }

    st = hs_parse_value_string(object, "terminate", tmp);
    if (st != 0) {
        uifw_error("hs_parse_value_string = %d", st);
    }
    else {
        hs_command_req[id].program.terminate.appid
                = hs_command_req[id].program.appid;
    }

    st = hs_parse_value_string(object, "suspend", tmp);
    if (st != 0) {
        uifw_error("hs_parse_value_string = %d", st);
    }
    else {
        hs_command_req[id].program.suspend.appid
                = hs_command_req[id].program.appid;
    }

    st = hs_parse_value_string(object, "resume", tmp);
    if (st != 0) {
        uifw_error("hs_parse_value_string = %d", st);
    }
    else {
        hs_command_req[id].program.resume.appid
                = hs_command_req[id].program.appid;
    }

    return 0;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_parse_input
 *          parse "input" section
 *
 * @param[in]   node                json node
 * @param[in]   id                  sequence number of the node
 * @return      result
 * @retval      0                   Success
 * @retval      1                   Failed
 */
/*--------------------------------------------------------------------------*/
static int
hs_parse_input(JsonNode *node, int id)
{
    int st;
    JsonObject *object;
    int tmp;

    object = json_node_get_object(node);
    if (!object) {
        uifw_error("json_node_get_object");
        return 1;
    }

    st = hs_parse_value_integer(object, "add_app", &tmp);
    if (st != 0) {
        hs_command_req[id].input.add_app.input = IGNORE;
    }
    else {
        hs_command_req[id].input.add_app.input = tmp;
    }

    st = hs_parse_value_integer(object, "del_app", &tmp);
    if (st != 0) {
        hs_command_req[id].input.del_app.input = IGNORE;
    }
    else {
        hs_command_req[id].input.del_app.input = tmp;
    }

    return 0;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_parse_window
 *          parse "window" section
 *
 * @param[in]   node                json node
 * @param[in]   id1                 sequence number of the node
 * @param[in]   id2                 sequence number of the window in the node
 * @return      result
 * @retval      0                   Success
 * @retval      1                   Failed
 */
/*--------------------------------------------------------------------------*/
static int
hs_parse_window(JsonNode *node, int id1, int id2)
{
    int st;
    JsonObject *object;

    object = json_node_get_object(node);
    if (!object) {
        uifw_error("json_node_get_object");
        return 1;
    }

    st = hs_parse_value_integer(object, "show",
                                &hs_command_req[id1].window[id2].show.window);
    if (st != 0) {
        hs_command_req[id1].window[id2].show.window = IGNORE;
    }

    st = hs_parse_value_integer(object, "hide",
                                &hs_command_req[id1].window[id2].hide.window);
    if (st != 0) {
        hs_command_req[id1].window[id2].hide.window = IGNORE;
    }

    st = hs_hs_parse_value_integer3(object,
                                    "visible_raise",
                                    &hs_command_req[id1].window[id2].visible_raise.window,
                                    &hs_command_req[id1].window[id2].visible_raise.visible,
                                    &hs_command_req[id1].window[id2].visible_raise.raise);
    if (st != 0) {
        hs_command_req[id1].window[id2].visible_raise.window = IGNORE;
    }

    st = hs_hs_parse_value_integer3(object,
                                    "resize",
                                    &hs_command_req[id1].window[id2].resize.window,
                                    &hs_command_req[id1].window[id2].resize.w,
                                    &hs_command_req[id1].window[id2].resize.h);
    if (st != 0) {
        hs_command_req[id1].window[id2].resize.window = IGNORE;
    }

    st = hs_hs_parse_value_integer3(object,
                                    "resize_tile",
                                    &hs_command_req[id1].window[id2].resize_tile.window,
                                    &hs_command_req[id1].window[id2].resize_tile.size_m,
                                    &hs_command_req[id1].window[id2].resize_tile.size_n);
    if (st != 0) {
        hs_command_req[id1].window[id2].resize_tile.window = IGNORE;
    }

    st = hs_hs_parse_value_integer3(object,
                                    "move",
                                    &hs_command_req[id1].window[id2].move.window,
                                    &hs_command_req[id1].window[id2].move.x,
                                    &hs_command_req[id1].window[id2].move.y);
    if (st != 0) {
        hs_command_req[id1].window[id2].move.window = IGNORE;
    }

    st = hs_hs_parse_value_integer3(object,
                                    "move_tile",
                                    &hs_command_req[id1].window[id2].move_tile.window,
                                    &hs_command_req[id1].window[id2].move_tile.m,
                                    &hs_command_req[id1].window[id2].move_tile.n);
    if (st != 0) {
        hs_command_req[id1].window[id2].move_tile.window = IGNORE;
    }

    st = hs_parse_value_integer(object, "raise",
                                &hs_command_req[id1].window[id2].raise.window);
    if (st != 0) {
        hs_command_req[id1].window[id2].raise.window = IGNORE;
    }

    st = hs_parse_value_integer(object, "lower",
                                &hs_command_req[id1].window[id2].lower.window);
    if (st != 0) {
        hs_command_req[id1].window[id2].lower.window = IGNORE;
    }

    st = hs_hs_parse_value_integer2(object,
                                    "event_mask",
                                    &hs_command_req[id1].window[id2].event_mask.window,
                                    (int *)&hs_command_req[id1].window[id2].event_mask.mask);
    if (st != 0) {
        hs_command_req[id1].window[id2].event_mask.window = IGNORE;
    }

    return 0;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_parse_application
 *          parse "application" section
 *
 * @param[in]   node                json node
 * @param[in]   id                  sequence number of the node
 * @return      result
 * @retval      0                   Success
 * @retval      1                   Failed
 */
/*--------------------------------------------------------------------------*/
static int
hs_parse_application(JsonNode *node, int id)
{
    int st;
    int ii;
    int n;
    JsonObject *object;
    JsonNode *program_node;
    JsonNode *window_node;
    JsonArray *window_array;
    JsonNode *input_node;

    object = json_node_get_object(node);
    if (!object) {
        uifw_error("json_node_get_object");
        return 1;
    }

    program_node = json_object_get_member(object, "program");
    if (!node) {
        uifw_error("json_object_get_member(%s)", "program");
        return 1;
    }

    if (JSON_NODE_TYPE(program_node) != JSON_NODE_OBJECT) {
        uifw_error("type error = %d", JSON_NODE_TYPE(program_node));
        return 1;
    }

    st = hs_parse_program(program_node, id);
    if (st != 0) {
        uifw_error("hs_parse_program = %d", st);
        return 1;
    }

    window_node = json_object_get_member(object, "window");
    if (!node) {
        uifw_error("json_object_get_member(%s)", "window");
        return 1;
    }

    if (JSON_NODE_TYPE(window_node) != JSON_NODE_ARRAY) {
        uifw_error("type error = %d", JSON_NODE_TYPE(window_node));
        return 1;
    }

    window_array = json_node_get_array(window_node);
    if (!window_array) {
        uifw_error("json_node_get_array");
        return 1;
    }

    n = json_array_get_length(window_array);
    if (n < 0) {
        uifw_error("json_array_get_length = %d", n);
        return 1;
    }

    for (ii = 0; ii < n; ii++) {
        window_node = json_array_get_element(window_array, ii);
        if (!window_node) {
            uifw_error("json_array_get_element");
            return 1;
        }

        if (JSON_NODE_TYPE(window_node) != JSON_NODE_OBJECT) {
            uifw_warn("type error = %d", JSON_NODE_TYPE(window_node));
            return 1;
        }

        st = hs_parse_window(window_node, id, ii);
        if (st != 0) {
            uifw_warn("hs_parse_window = %d", st);
            return 1;
        }
    }

    input_node = json_object_get_member(object, "input");
    if (!input_node) {
        uifw_error("json_object_get_member(%s)", "input");
        return 0;
    }

    if (JSON_NODE_TYPE(input_node) != JSON_NODE_OBJECT) {
        uifw_debug("type not exist = %d", JSON_NODE_TYPE(input_node));
        return 0;
    }

    st = hs_parse_input(input_node, id);
    if (st != 0) {
        uifw_error("hs_parse_input = %d", st);
        return 0;
    }

    return 0;
}

