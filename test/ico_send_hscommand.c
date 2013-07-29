/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   applicatoin that send a command to homescreen
 *
 * @date    Feb-15-2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#include <ico_uws.h>

#include "home_screen.h"
#include "home_screen_res.h"


/*============================================================================*/
/* Define data types                                                          */
/*============================================================================*/
#define ICO_HSCMD_WS_TIMEOUT 0.05
#define ICO_HSCMD_WS_ADDRESS "127.0.0.1"
#define ICO_HSCMD_WS_PROTOCOL_NAME ICO_HS_PROTOCOL_CM

//#define hscmd_trace(...)
#define hscmd_trace(fmt, arg...) fprintf(stderr, ""fmt"\n",##arg)

/*============================================================================*/
/* static(internal) functions prototype                                       */
/*============================================================================*/
static void hscmd_callback_uws(const struct ico_uws_context *context,
                            const ico_uws_evt_e event, const void *id,
                            const ico_uws_detail *detail, void *data);

static void hscmd_create_ws_context(void);
static void hscmd_destroy_ws_context(void);
static void hscmd_ws_service_loop(void);
static void hscmd_usage(const char *prog);

/*============================================================================*/
/* variabe & table                                                            */
/*============================================================================*/
static int hscmd_ws_port = ICO_HS_WS_PORT;
static int hscmd_ws_connected = 0;
static struct ico_uws_context *hscmd_uws_context = NULL;
static void *hscmd_uws_id = NULL;
static FILE *hscmd_fp;
static int hscnd_send_end = 0;

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/*
 * @brief   hscmd_callback_uws
 *          callback function from UWS
 *      
 * @param[in]   context             context
 * @param[in]   event               event kinds
 * @param[in]   id                  client id
 * @param[in]   detail              event detail
 * @param[in]   data                user data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void 
hscmd_callback_uws(const struct ico_uws_context *context,
                const ico_uws_evt_e event, const void *id,
                const ico_uws_detail *detail, void *data)
{
    unsigned char msg[ICO_HS_TEMP_BUF_SIZE];
    unsigned char *send;
    char *in;
    long fsize;
    int len;

    hscmd_trace("hscmd_callback_uws %p", context);

    switch (event) {
    case ICO_UWS_EVT_OPEN:
        hscmd_trace("hscmd_callback_uws: ICO_UWS_EVT_OPEN(id=%d)", (int)id);
        hscmd_uws_id = (void *)id;
        len = sprintf((char *)msg, "%s %s", ICO_HS_MSG_HEAD_CM, HS_REQ_ANS_HELLO);
        ico_uws_send((struct ico_uws_context *)context, (void *)id, msg, len);
        break;

    case ICO_UWS_EVT_CLOSE:
        hscmd_trace("hscmd_callback_uws: ICO_UWS_EVT_CLOSE(id=%d)", (int)id);
        hscmd_uws_context = NULL;
        hscmd_uws_id = NULL;
        break;

    case ICO_UWS_EVT_RECEIVE:
        hscmd_trace("hscmd_callback_uws: ICO_UWS_EVT_RECEIVE(id=%d, msg=%s, len=%d)",
                   (int)id, (char *)detail->_ico_uws_message.recv_data,
                   detail->_ico_uws_message.recv_len);
        in = (char *)detail->_ico_uws_message.recv_data;

        if(strncmp("ANS HELLO", in, 9) == 0) {
            fseek(hscmd_fp, 0, SEEK_END);
            fsize = ftell(hscmd_fp);
            fseek(hscmd_fp,  0L, SEEK_SET);

            len = (int)fsize + 4;
            send = (void *)malloc(len);

            memset(send, 0, len);

            sprintf((char *)send, "%s ", ICO_HS_MSG_HEAD_CM);

            fread(send + 4, 1, fsize, hscmd_fp);
            hscmd_trace("hscmd_callback_uws: send (%s)", send);

            ico_uws_send((struct ico_uws_context *)context, (void *)id, send, len);

            hscnd_send_end = 1;
        }
        break;

    case ICO_UWS_EVT_ERROR:
        hscmd_trace("hscmd_callback_uws: ICO_UWS_EVT_ERROR(id=%d, err=%d)",
                   (int)id, detail->_ico_uws_error.code);
        break;

    case ICO_UWS_EVT_ADD_FD:
        hscmd_trace("hscmd_callback_uws: ICO_UWS_EVT_ADD_FD(id=%d, fd=%d)",
                   (int)id, detail->_ico_uws_fd.fd);
        break;
    
    case ICO_UWS_EVT_DEL_FD:
        hscmd_trace("hscmd_callback_uws: ICO_UWS_EVT_DEL_FD(id=%d, fd=%d)",
                   (int)id, detail->_ico_uws_fd.fd);
        break;

    default:
        break;
    }

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hscmd_create_ws_context
 *          connect to the homescreen using websocket.
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hscmd_create_ws_context(void)
{
    int ret;
    char uri[ICO_HS_TEMP_BUF_SIZE];

    /* set up URI "ws://HOST:PORT" */
    sprintf(uri, "ws://%s:%d", ICO_HS_WS_HOST, hscmd_ws_port);

    hscmd_uws_context = ico_uws_create_context(uri, ICO_HS_PROTOCOL);

    hscmd_ws_connected = 0;
    if (hscmd_uws_context == NULL) {
        fprintf(stderr, "libwebsocket_create_context failed.\n");
    } else {
        /* set callback */
        ret = ico_uws_set_event_cb(hscmd_uws_context, hscmd_callback_uws, NULL);
        if (ret != ICO_UWS_ERR_NONE) {
            hscmd_trace("hscmd_create_ws_context: cannnot set callback");
        }
        hscmd_ws_connected = 1;
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hscmd_destroy_ws_context
 *          destroy websokets connection.
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hscmd_destroy_ws_context(void)
{
    if (hscmd_uws_context) {
        ico_uws_service(hscmd_uws_context);
        usleep(50 * 1000);
        ico_uws_unset_event_cb(hscmd_uws_context);
        ico_uws_close(hscmd_uws_context);
        hscmd_uws_context = NULL;
        hscmd_ws_connected = 0;
    }
}

static void
hscmd_ws_service_loop(void)
{
    while (hscmd_ws_connected) {
        ico_uws_service(hscmd_uws_context);
        usleep(50 * 1000);
        if (hscnd_send_end == 1) {
            hscmd_destroy_ws_context();
        }
    }
}

static void
hscmd_usage(const char *prog)
{
    fprintf(stderr, "Usage: %s filename\n", prog);
    exit(0);
}

/*--------------------------------------------------------------------------*/
/*
 * @brief   test command tools
 *          send json format command to
 *          main routine
 *
 * @param   main() finction's standard parameter (argc,argv)
 * @return  result
 * @retval  0       success
 * @retval  1       failed
 */
/*--------------------------------------------------------------------------*/
int
main(int argc, char *argv[])
{

    /* read json file */
    if (argc < 1) {
        hscmd_usage(argv[0]);
    }

    hscmd_fp = fopen(argv[1], "rb");
    if (hscmd_fp == NULL) {
        hscmd_usage(argv[0]);
    }

    /* Init websockets */
    hscmd_create_ws_context();

    hscmd_ws_service_loop();

    hscmd_destroy_ws_context();

    fclose(hscmd_fp);

    return 0;
}

