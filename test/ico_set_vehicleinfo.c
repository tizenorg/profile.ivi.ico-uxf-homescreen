/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   System Test Tool for set VehicleInfo
 *
 * @date    Apr-09-2013
 */

#define MSG_INTERFACE   0               /* 1= Message Queue Interface       */
#define LWS_INTERFACE   1               /* 1= WebSockets Interface          */

#include    <stdio.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    <string.h>
#include    <errno.h>
#include    <pthread.h>
#include    <sys/ioctl.h>
#include    <sys/ipc.h>
#include    <sys/msg.h>
#include    <sys/time.h>
#include    <sys/types.h>
#if     LWS_INTERFACE > 0               /* WebSocket Interface              */
#include    <libwebsockets.h>
#endif /*LWS_INTERFACE*/                /* WebSocket Interface              */

#define TYPE_NULL   0
#define TYPE_BOOL   1
#define TYPE_BYTE   2
#define TYPE_INT16  3
#define TYPE_UINT16 4
#define TYPE_INT32  5
#define TYPE_UINT32 6
#define TYPE_DOUBLE 7
#define TYPE_STRING 8

#if     LWS_INTERFACE > 0               /* WebSocket Interface              */
#define LWS_DEFAULTIP       "127.0.0.1" /* websockets default ip(localhost) */
#define LWS_DEFAULTPORT     25010       /* websockets default port          */
#define LWS_PROTOCOLNAME    "standarddatamessage-only"
                                        /* websockets protocol name         */
#endif /*LWS_INTERFACE*/                /* WebSocket Interface              */

static const struct {
    char        *prop;
    char        *eventtype;
    unsigned char   datatype[4];
}               vehicleinfo_key[] = {
    { "VehicleSpeed", "VELOCITY", {TYPE_INT32, TYPE_NULL, 0,0} },
    { "Speed", "VELOCITY", {TYPE_INT32, TYPE_NULL, 0,0} },
    { "Velocity", "VELOCITY", {TYPE_INT32, TYPE_NULL, 0,0} },
    { "Location", "LOCATION", {TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE, TYPE_NULL} },
    { "Direction", "DIRECTION", {TYPE_DOUBLE, TYPE_NULL, 0,0} },
    { "EngineSpeed", "ENGINE_SPEED", {TYPE_INT32, TYPE_NULL, 0, 0} },
    { "Engine", "ENGINE_SPEED", {TYPE_INT32, TYPE_NULL, 0, 0} },
    { "Shift", "SHIFT", {TYPE_BYTE, TYPE_BYTE, TYPE_NULL, 0} },
    { "ShiftPosition", "SHIFT", {TYPE_BYTE, TYPE_BYTE, TYPE_NULL, 0} },
    { "Break_Signal", "BRAKE_SIGNAL", {TYPE_BOOL, TYPE_NULL, 0,0} },
    { "BreakSignal", "BRAKE_SIGNAL", {TYPE_BOOL, TYPE_NULL, 0,0} },
    { "Break", "BRAKE_SIGNAL", {TYPE_BOOL, TYPE_NULL, 0,0} },
    { "Blinker", "TURN_SIGNAL", {TYPE_INT32, TYPE_NULL, 0, 0} },
    { "Winker", "TURN_SIGNAL", {TYPE_INT32, TYPE_NULL, 0, 0} },
    { "TurnSignal", "TURN_SIGNAL", {TYPE_INT32, TYPE_NULL, 0, 0} },
    { "WATER_TEMP", "WATER_TEMP", {TYPE_INT32, TYPE_NULL, 0, 0} },
    { "\0", "\0", {TYPE_NULL, 0,0,0} } };

struct KeyDataMsg_t
{
#if     MSG_INTERFACE > 0               /* Message Queue Interface      */
    long mtype;
#endif /*MSG_INTERFACE*/                /* Message Queue Interface      */
    char KeyEventType[64];
    struct timeval recordtime;
    struct KeyData
    {
        int common_status;
        char status[];
    } data;
};

#if     MSG_INTERFACE > 0               /* Message Queue Interface      */
static int  sndqueuekey = 55555;
static int  sndqueueid = 0;

static int  mqid = -1;
#endif /*MSG_INTERFACE*/                /* Message Queue Interface      */

#if     LWS_INTERFACE > 0               /* WebSocket Interface              */
static struct libwebsocket_context  *context = NULL;
                                        /* websockets context               */
static struct libwebsocket          *websocket = NULL;
                                        /* websockets connection            */
static int  connected = 0;              /* connection flag                  */

static int  lws_callback(struct libwebsocket_context *context, struct libwebsocket *wsi,
                         enum libwebsocket_callback_reasons reason,
                         void *user, void *in, size_t len);

static struct libwebsocket_protocols protocols[] = {
            {LWS_PROTOCOLNAME, lws_callback, 0},
            {NULL, NULL, -1}
        };
#endif /*LWS_INTERFACE*/                /* WebSocket Interface              */

#if     MSG_INTERFACE > 0               /* Message Queue Interface      */
static void
init_comm(const int mqkey)
{
    char    dummy[256];

    if (mqkey == 0) {
        mqid = -1;
    }
    else    {
        mqid = msgget(mqkey, 0);
        if (mqid < 0)   {
            mqid = msgget(mqkey, IPC_CREAT);
        }
        if (mqid < 0)   {
            fprintf(stderr, "Can not create message queue(%d(0x%x))[%d]\n",
                    mqkey, mqkey, errno);
            return;
        }
        while (msgrcv(mqid, dummy, sizeof(dummy)-sizeof(long), 0, IPC_NOWAIT) > 0)  ;
    }
}
#endif /*MSG_INTERFACE*/                /* Message Queue Interface      */

#if     LWS_INTERFACE > 0               /* WebSocket Interface              */
static int
lws_callback(struct libwebsocket_context *context, struct libwebsocket *wsi,
             enum libwebsocket_callback_reasons reason, void *user,
             void *in, size_t len)
{
    if (reason == LWS_CALLBACK_CLIENT_ESTABLISHED)  {
        connected = 1;
    }
    /* do nothing       */
    return 0;
}

static void
init_comm(const int port, const char *spadr)
{
    int     rep;

    context = libwebsocket_create_context(CONTEXT_PORT_NO_LISTEN,
                                          spadr, protocols,
                                          libwebsocket_internal_extensions,
                                          NULL, NULL, -1, -1, 0);
    if (context == NULL)    {
        fprintf(stderr, "Can not create libwebsockets context(ip=%s port=%d)\n",
                spadr, port);
        exit(2);
    }

    connected = 0;
    websocket = libwebsocket_client_connect(context, spadr, port,
                                            0, "/", spadr, "websocket",
                                            protocols[0].name, -1);
    if (websocket == NULL)  {
        fprintf(stderr, "Can not connect libwebsockets context(ip=%s port=%d)\n",
                spadr, port);
        exit(2);
    }
    /* wait for connection          */
    for (rep = 0; rep < (2*1000); rep += 50)    {
        if (connected)  break;
        libwebsocket_service(context, 50);
    }
}
#endif /*LWS_INTERFACE*/                /* WebSocket Interface              */

static void
init_vehicleinfo(void)
{
#if     MSG_INTERFACE > 0               /* Message Queue Interface      */
    sndqueueid = msgget(sndqueuekey, 0);
    if (sndqueueid < 0) {
        fprintf(stderr, "Send Message Queue(%d(0x%x)) dose not exist[%d].\n",
                sndqueuekey, sndqueuekey, errno);
    }
#endif /*MSG_INTERFACE*/                /* Message Queue Interface      */
}

static void
set_vehicleinfo(const char *cmd)
{
    int     i, j;
    int     idx, key, pt;
    int     msgsize;
    char    prop[64];
    char    value[128];
    int     sec, msec;
    struct  {
        struct KeyDataMsg_t     msg;
        char    dummy[128];
    }       msg;
#if     LWS_INTERFACE > 0               /* WebSocket Interface              */
    unsigned char   buf[LWS_SEND_BUFFER_PRE_PADDING + 512 + LWS_SEND_BUFFER_POST_PADDING];
    unsigned char   *bufpt = &buf[LWS_SEND_BUFFER_PRE_PADDING];
#endif /*LWS_INTERFACE*/                /* WebSocket Interface              */

    j = 0;
    for (i = 0; cmd[i]; i++)    {
        if ((cmd[i] == '=') || (cmd[i] == ' ')) break;
        if (j < (int)(sizeof(prop)-1))  {
            prop[j++] = cmd[i];
        }
    }

    prop[j] = 0;
    j = 0;
    if (cmd[i] != 0)    {
        for (i++; cmd[i]; i++)  {
            if (cmd[i] == ' ')  continue;
            if (j < (int)(sizeof(value)-1)) {
                value[j++] = cmd[i];
            }
        }
    }
    value[j] = 0;

    if (strcasecmp(prop, "sleep") == 0) {
        sec = 0;
        msec = 0;
        for (i = 0; value[i]; i++)  {
            if (value[i] == '.')        break;
            sec = sec * 10 + (value[i] & 0x0f);
        }
        if (value[i] == '.')    {
            i++;
            if (value[i] != 0)  {
                msec = (value[i] & 0x0f) * 100;
                i++;
            }
            if (value[i] != 0)  {
                msec = msec + (value[i] & 0x0f) * 10;
                i++;
            }
            if (value[i] != 0)  {
                msec = msec + (value[i] & 0x0f);
            }
        }
        if (sec > 0)    sleep(sec);
        if (msec > 0)   usleep(msec * 1000);

        return;
    }

    for (key = 0; vehicleinfo_key[key].prop[0]; key++)  {
        if (strcasecmp(prop, vehicleinfo_key[key].prop) == 0)   break;
    }
    if (! vehicleinfo_key[key].prop[0]) {
        fprintf(stderr, "VehicleInfo UnKnown property[%s]\n", prop);
        return;
    }

    memset(&msg, 0, sizeof(msg));
#if     MSG_INTERFACE > 0               /* Message Queue Interface      */
    msg.msg.mtype = 1;
#endif /*MSG_INTERFACE*/                /* Message Queue Interface      */
    strcpy(msg.msg.KeyEventType, vehicleinfo_key[key].eventtype);
    gettimeofday(&(msg.msg.recordtime), NULL);
#if     MSG_INTERFACE > 0               /* Message Queue Interface      */
    msgsize = sizeof(msg) - 128 - sizeof(long);
#endif /*MSG_INTERFACE*/                /* Message Queue Interface      */
#if     LWS_INTERFACE > 0               /* WebSocket Interface              */
    msgsize = sizeof(msg) - 128;
#endif /*LWS_INTERFACE*/                /* WebSocket Interface              */

    i = 0;
    pt = 0;
    for (idx = 0; idx < 4; idx++)   {
        if (vehicleinfo_key[key].datatype[idx] == TYPE_NULL)    break;

        if (value[i])   {
            for (j = i; value[j]; j++)  {
                if ((value[j] == ',') || (value[j] == ';') ||
                    (value[j] == ':') || (value[j] == ' ')) break;
            }
            if (value[j] != 0)  {
                value[j++] = 0;
            }
            switch (vehicleinfo_key[key].datatype[idx]) {
            case TYPE_BOOL:
            case TYPE_BYTE:
                msg.msg.data.status[pt++] = strtoul(&value[i], (char **)0, 0);
                msgsize += 1;
                break;
            case TYPE_INT16:
            case TYPE_UINT16:
                *((short *)&msg.msg.data.status[pt]) = strtol(&value[i], (char **)0, 0);
                pt += sizeof(short);
                msgsize += sizeof(short);
                break;
            case TYPE_INT32:
            case TYPE_UINT32:
                *((int *)&msg.msg.data.status[pt]) = strtol(&value[i], (char **)0, 0);
                pt += sizeof(int);
                msgsize += sizeof(int);
                break;
            case TYPE_DOUBLE:
                *((double *)&msg.msg.data.status[pt]) = strtod(&value[i], (char **)0);
                pt += sizeof(double);
                msgsize += sizeof(double);
                break;
            default:
                break;
            }
            i = j;
        }
        else    {
            switch (vehicleinfo_key[key].datatype[idx]) {
            case TYPE_BOOL:
            case TYPE_BYTE:
                msgsize += 1;
                break;
            case TYPE_INT16:
            case TYPE_UINT16:
                msgsize += sizeof(short);
                break;
            case TYPE_INT32:
            case TYPE_UINT32:
                msgsize += sizeof(int);
                break;
            case TYPE_DOUBLE:
                msgsize += sizeof(double);
                break;
            default:
                break;
            }
        }
    }

#if     MSG_INTERFACE > 0               /* Message Queue Interface      */
    if (msgsnd(sndqueueid, &msg, msgsize, 0) < 0)   {
        fprintf(stderr, "Message Queue(%d(0x%x)) send error[%d].\n",
                sndqueuekey, sndqueuekey, errno);
    }
#endif /*MSG_INTERFACE*/                /* Message Queue Interface      */
#if     LWS_INTERFACE > 0               /* WebSocket Interface              */
    memcpy(bufpt, &msg, msgsize);
    if (libwebsocket_write(websocket, bufpt, msgsize, LWS_WRITE_BINARY) < 0)    {
        fprintf(stderr, "libwebsockets send error\n"); fflush(stderr);
    }
#endif /*LWS_INTERFACE*/                /* WebSocket Interface              */
}

static void
usage(const char *prog)
{
#if     LWS_INTERFACE > 0               /* WebSocket Interface              */
    fprintf(stderr, "Usage: %s [-port=port] [-ip=ip_addr] [propaty=value] [propaty=value] ...\n", prog);
#endif /*LWS_INTERFACE*/                /* WebSocket Interface              */
#if     MSG_INTERFACE > 0               /* Message Queue Interface      */
    fprintf(stderr, "Usage: %s [-ambkey=key] [-mq[=key]] [propaty=value] [propaty=value] ...\n", prog);
#endif /*MSG_INTERFACE*/                /* Message Queue Interface      */
    exit(0);
}

int
main(int argc, char *argv[])
{
    int     i, j;
#if     LWS_INTERFACE > 0               /* WebSocket Interface              */
    int     port = LWS_DEFAULTPORT;
    char    spadr[64];
#endif /*LWS_INTERFACE*/                /* WebSocket Interface              */
#if     MSG_INTERFACE > 0               /* Message Queue Interface      */
    int     mqkey = 0;
    struct {
        long    mtype;
        char    buf[240];
    }       mqbuf;
#endif /*MSG_INTERFACE*/                /* Message Queue Interface      */
    char    buf[240];

#if     LWS_INTERFACE > 0               /* WebSocket Interface              */
    strcpy(spadr, LWS_DEFAULTIP);
#endif /*LWS_INTERFACE*/                /* WebSocket Interface              */

    j = 0;
    for (i = 1; i < argc; i++)  {
        if (argv[i][0] == '-')  {
#if     MSG_INTERFACE > 0               /* Message Queue Interface      */
            if (strncasecmp(argv[i], "-ambkey=", 8) == 0)   {
                sndqueuekey = strtoul(&argv[i][8], (char **)0, 0);
            }
            else if (strncasecmp(argv[i], "-mq", 3) == 0)  {
                if (argv[i][3] == '=')  {
                    mqkey = strtol(&argv[i][4], (char **)0, 0);
                }
                else    {
                    mqkey = 55552;          /* default message queue key    */
                }
            }
#endif /*MSG_INTERFACE*/                /* Message Queue Interface      */
#if     LWS_INTERFACE > 0               /* WebSocket Interface              */
            if (strncasecmp(argv[i], "-port=", 6) == 0)   {
                port = strtoul(&argv[i][6], (char **)0, 0);
            }
            else if (strncasecmp(argv[i], "-ip=", 4) == 0)   {
                memset(spadr, 0, sizeof(spadr));
                strncpy(spadr, &argv[i][4], sizeof(spadr)-1);
            }
#endif /*LWS_INTERFACE*/                /* WebSocket Interface              */
            else    {
                usage(argv[0]);
            }
        }
        else    {
            j++;
        }
    }

#if     MSG_INTERFACE > 0               /* Message Queue Interface      */
    init_comm(mqkey);
#endif /*MSG_INTERFACE*/                /* Message Queue Interface      */
#if     LWS_INTERFACE > 0               /* WebSocket Interface              */
    init_comm(port, spadr);
#endif /*LWS_INTERFACE*/                /* WebSocket Interface              */

    init_vehicleinfo();

#if     MSG_INTERFACE > 0               /* Message Queue Interface      */
    if (mqid >= 0)  {
        while (1)   {
            memset(&mqbuf, 0, sizeof(mqbuf));
            if (msgrcv(mqid, &mqbuf, sizeof(mqbuf)-sizeof(long), 0, 0) < 0) break;
            k = 0;
            j = -1;
            for (i = 0; mqbuf.buf[i]; i++)    {
                if ((mqbuf.buf[i] == '#') || (mqbuf.buf[i] == '\n')
                    || (mqbuf.buf[i] == '\r'))    break;
                if (mqbuf.buf[i] == '\t') buf[k++] = ' ';
                else                        buf[k++] = mqbuf.buf[i];
                if ((j < 0) && (mqbuf.buf[i] != ' ')) j = i;
            }
            if (j < 0)  continue;
            buf[k] = 0;
            set_vehicleinfo(&buf[j]);
        }
        msgctl(mqid, IPC_RMID, NULL);
    }
    else
#endif /*MSG_INTERFACE*/                /* Message Queue Interface      */
    if (j <= 0) {
        while (fgets(buf, sizeof(buf), stdin))  {
            j = -1;
            for (i = 0; buf[i]; i++)    {
                if ((buf[i] == '#') || (buf[i] == '\n') || (buf[i] == '\r'))    break;
                if (buf[i] == '\t') buf[i] = ' ';
                if ((j < 0) && (buf[i] != ' ')) j = i;
            }
            if (j < 0)  continue;
            buf[i] = 0;
            set_vehicleinfo(&buf[j]);
        }
    }
    else    {
        for (i = 1; i < argc; i++)  {
            if (argv[i][0] == '-')  continue;
            set_vehicleinfo(argv[i]);
        }
    }
#if     LWS_INTERFACE > 0               /* WebSocket Interface              */
    if (context)    {
        libwebsocket_context_destroy(context);
    }
#endif /*LWS_INTERFACE*/                /* WebSocket Interface              */

    exit(0);
}

