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

#include <libwebsockets.h>

#include "home_screen.h"
#include "home_screen_res.h"


/*============================================================================*/
/* Define data types                                                          */
/*============================================================================*/
#define ICO_HSCMD_WS_TIMEOUT 0.05
#define ICO_HSCMD_WS_ADDRESS "127.0.0.1"
#define ICO_HSCMD_WS_PROTOCOL_NAME ICO_HS_PROTOCOL_CM

/*============================================================================*/
/* static(internal) functions prototype                                       */
/*============================================================================*/
static int hscmd_callback_http(struct libwebsocket_context *context,
                         struct libwebsocket *wsi,
                         enum libwebsocket_callback_reasons reason, void *user,
                         void *in, size_t len);
static int hscmd_callback_command(struct libwebsocket_context *context,
                              struct libwebsocket *wsi,
                              enum libwebsocket_callback_reasons reason,
                              void *user, void *in, size_t len);
static void hscmd_create_ws_context(void);
static void hscmd_destroy_ws_context(void);
static void hscmd_ws_service_loop(void);
static void hscmd_usage(const char *prog);

/*============================================================================*/
/* variabe & table                                                            */
/*============================================================================*/
static int hscmd_ws_port = ICO_HS_WS_PORT;
static int hscmd_ws_connected = 0;
static struct libwebsocket_context *hscmd_ws_context;
static struct libwebsocket *hscmd_wsi_mirror;
static FILE *hscmd_fp;

static struct libwebsocket_protocols ws_protocols[] = {
    {
        "http-only",
        hscmd_callback_http,
        0
    },
    {
        "gui-protocol",
        hscmd_callback_command,
        0,
    },
    {
        /* end of list */
        NULL,
        NULL,
        0
    }
};

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
static int
hscmd_callback_http(struct libwebsocket_context *context,
                         struct libwebsocket *wsi,
                         enum libwebsocket_callback_reasons reason, void *user,
                         void *in, size_t len)
{
    return 0;
}

/*--------------------------------------------------------------------------*/
/*
 * @brief   hscmd_callback_command
 *          Connection status is notified from libwebsockets.
 *
 * @param[in]   context             libwebsockets context
 * @param[in]   wsi                 libwebsockets management table
 * @param[in]   reason              event type
 * @param[in]   user                intact
 * @param[in]   in                  receive message
 * @param[in]   len                 message size[BYTE]
 * @return      result
 * @retval      =0                  success
 * @retval      =1                  error
 */
/*--------------------------------------------------------------------------*/
static int
hscmd_callback_command(struct libwebsocket_context *context,
                              struct libwebsocket *wsi,
                              enum libwebsocket_callback_reasons reason,
                              void *user, void *in, size_t len)
{
    long    fsize;
    char    *sendMsg;

    switch(reason) {
    case LWS_CALLBACK_CLIENT_ESTABLISHED:
        break;
    case LWS_CALLBACK_CLIENT_RECEIVE:
        if(strncmp("ANS HELLO", in, 9) == 0) {
            fseek(hscmd_fp, 0, SEEK_END);
            fsize = ftell(hscmd_fp);
            fseek(hscmd_fp,  0L, SEEK_SET);

            sendMsg = (void *)malloc((int)fsize);

            memset(sendMsg, 0, fsize);

            fread(sendMsg, 1, fsize, hscmd_fp);
            libwebsocket_write( wsi, (unsigned char *)sendMsg, fsize, LWS_WRITE_BINARY);
            hscmd_destroy_ws_context();
        }
        break;
    case LWS_CALLBACK_CLOSED:
        hscmd_wsi_mirror = NULL;
        break;
    default:
        break;
    }

    return 0;
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
    hscmd_ws_context
        = libwebsocket_create_context(CONTEXT_PORT_NO_LISTEN, NULL, ws_protocols,
                                      libwebsocket_internal_extensions,
                                      NULL, NULL, -1, -1, 0);

    hscmd_ws_connected = 0;
    if (hscmd_ws_context == NULL) {
        fprintf(stderr, "libwebsocket_create_context failed.\n");
    } else {
        hscmd_wsi_mirror
            = libwebsocket_client_connect(
                    hscmd_ws_context, ICO_HSCMD_WS_ADDRESS, hscmd_ws_port,
                    0, "/", ICO_HSCMD_WS_ADDRESS, NULL,
                    ICO_HSCMD_WS_PROTOCOL_NAME, -1);
        if(hscmd_wsi_mirror != NULL) {
            hscmd_ws_connected = 1;
        }
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
    if (hscmd_ws_context) {
        libwebsocket_context_destroy(hscmd_ws_context);
        hscmd_ws_context = NULL;
        hscmd_ws_connected = 0;
    }
}

static void
hscmd_ws_service_loop(void)
{
    while (hscmd_ws_connected) {
        libwebsocket_service(hscmd_ws_context, 100);
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

