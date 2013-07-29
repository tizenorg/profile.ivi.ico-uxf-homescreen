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
#include    <ico_uws.h>

#define TYPE_NULL   0
#define TYPE_BOOL   1
#define TYPE_BYTE   2
#define TYPE_INT16  3
#define TYPE_UINT16 4
#define TYPE_INT32  5
#define TYPE_UINT32 6
#define TYPE_DOUBLE 7
#define TYPE_STRING 8
#define TYPE_SHIFT  12

#define LWS_DEFAULTIP       "127.0.0.1" /* connection default ip(localhost) */
#define LWS_DEFAULTPORT     25010       /* connection default port          */
#define LWS_PROTOCOLNAME    "standarddatamessage-only"
                                        /* connection protocol name         */
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
    { "Shift", "SHIFT", {TYPE_SHIFT, TYPE_BYTE, TYPE_NULL, 0} },
    { "ShiftPosition", "SHIFT", {TYPE_SHIFT, TYPE_BYTE, TYPE_NULL, 0} },
    { "Break_Signal", "BRAKE_SIGNAL", {TYPE_BOOL, TYPE_NULL, 0,0} },
    { "BreakSignal", "BRAKE_SIGNAL", {TYPE_BOOL, TYPE_NULL, 0,0} },
    { "Break", "BRAKE_SIGNAL", {TYPE_BOOL, TYPE_NULL, 0,0} },
    { "Blinker", "TURN_SIGNAL", {TYPE_INT32, TYPE_NULL, 0, 0} },
    { "Winker", "TURN_SIGNAL", {TYPE_INT32, TYPE_NULL, 0, 0} },
    { "TurnSignal", "TURN_SIGNAL", {TYPE_INT32, TYPE_NULL, 0, 0} },
    { "Turn", "TURN_SIGNAL", {TYPE_INT32, TYPE_NULL, 0, 0} },
    { "lightStatus", "LIGHTSTATUS", {TYPE_BOOL, TYPE_BOOL, TYPE_BOOL, TYPE_BOOL} },
    { "light", "LIGHTSTATUS", {TYPE_BOOL, TYPE_BOOL, TYPE_BOOL, TYPE_BOOL} },
    { "WATER_TEMP", "WATER_TEMP", {TYPE_INT32, TYPE_NULL, 0, 0} },
    { "\0", "\0", {TYPE_NULL, 0,0,0} } };

struct KeyDataMsg_t
{
    char KeyEventType[64];
    struct timeval recordtime;
    struct KeyData
    {
        int common_status;
        char status[];
    } data;
};

static struct ico_uws_context   *context = NULL;
                                        /* connection context               */
static void                     *connect_id = NULL;
                                        /* connection connection id         */
static int  connected = 0;              /* connection flag                  */

static void uws_callback(const struct ico_uws_context *context,
                         const ico_uws_evt_e reason, const void *id,
                         const ico_uws_detail *detail, void *user_data);

static void
uws_callback(const struct ico_uws_context *context, const ico_uws_evt_e reason,
             const void *id, const ico_uws_detail *detail, void *user_data)
{
    if (reason == ICO_UWS_EVT_OPEN)  {
        connect_id = (void *)id;
        connected = 1;
    }
    else if (reason == ICO_UWS_EVT_ERROR)   {
        fprintf(stderr, "Communication Error[%d]\n", detail->_ico_uws_error.code);
    }
    /* do nothing       */
}

static void
init_comm(const int port, const char *spadr)
{
    int     rep;
    char    uri_name[128];

    snprintf(uri_name, sizeof(uri_name), "ws://%s:%d", spadr, port);
    connected = 0;
    context = ico_uws_create_context(uri_name, LWS_PROTOCOLNAME);
    if (context == NULL)    {
        fprintf(stderr, "Can not create conextion context(uri=%s port=%d)\n",
                uri_name, port);
        exit(2);
    }
    ico_uws_set_event_cb(context, uws_callback, NULL);

    /* wait for connection          */
    for (rep = 0; rep < (2*1000); rep += 50)    {
        if (connected)  break;
        ico_uws_service(context);
    }
}

static void
set_vehicleinfo(const char *cmd)
{
    int     i, j;
    int     idx, key, pt;
    int     msgsize;
    short   *sp;
    int     *ip;
    double  *dp;
    char    prop[64];
    char    value[128];
    int     sec, msec;
    struct  {
        struct KeyDataMsg_t     msg;
        char    dummy[128];
    }       msg;

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
    strcpy(msg.msg.KeyEventType, vehicleinfo_key[key].eventtype);
    gettimeofday(&(msg.msg.recordtime), NULL);
    msgsize = sizeof(msg) - 128;

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
            switch (vehicleinfo_key[key].datatype[idx] % 10) {
            case TYPE_BOOL:
            case TYPE_BYTE:
                if (vehicleinfo_key[key].datatype[idx] == TYPE_SHIFT)   {
                    if ((strcasecmp(&value[i], "sp") == 0) ||
                        (strcasecmp(&value[i], "s0") == 0)) {
                        strcpy(&value[i], "0");
                    }
                    else if (strcasecmp(&value[i], "sr") == 0)  {
                        strcpy(&value[i], "1");
                    }
                    else if (strcasecmp(&value[i], "sn") == 0)  {
                        strcpy(&value[i], "2");
                    }
                    else if ((strcasecmp(&value[i], "sd") == 0) ||
                             (strcasecmp(&value[i], "s4") == 0))    {
                        strcpy(&value[i], "4");
                    }
                    else if ((strcasecmp(&value[i], "s1") == 0) ||
                             (strcasecmp(&value[i], "sl") == 0))    {
                        strcpy(&value[i], "5");
                    }
                    else if (strcasecmp(&value[i], "s2") == 0)  {
                        strcpy(&value[i], "6");
                    }
                    else if (strcasecmp(&value[i], "s3") == 0)  {
                        strcpy(&value[i], "7");
                    }
                }
                msg.msg.data.status[pt++] = strtoul(&value[i], (char **)0, 0);
                msgsize += 1;
                break;
            case TYPE_INT16:
            case TYPE_UINT16:
                sp = (short *)&msg.msg.data.status[pt];
                *sp = strtol(&value[i], (char **)0, 0);
                pt += sizeof(short);
                msgsize += sizeof(short);
                break;
            case TYPE_INT32:
            case TYPE_UINT32:
                ip = (int *)&msg.msg.data.status[pt];
                *ip = strtol(&value[i], (char **)0, 0);
                pt += sizeof(int);
                msgsize += sizeof(int);
                break;
            case TYPE_DOUBLE:
                dp = (double *)&msg.msg.data.status[pt];
                *dp = strtod(&value[i], (char **)0);
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

    ico_uws_send(context, connect_id, (unsigned char *)&msg, msgsize);
}

static void
usage(const char *prog)
{
    fprintf(stderr, "Usage: %s [-port=port] [-ip=ip_addr] [propaty=value] [propaty=value] ...\n", prog);
    exit(0);
}

int
main(int argc, char *argv[])
{
    int     i, j;
    int     port = LWS_DEFAULTPORT;
    char    spadr[64];
    char    buf[240];

    strcpy(spadr, LWS_DEFAULTIP);

    j = 0;
    for (i = 1; i < argc; i++)  {
        if (argv[i][0] == '-')  {
            if (strncasecmp(argv[i], "-port=", 6) == 0)   {
                port = strtoul(&argv[i][6], (char **)0, 0);
            }
            else if (strncasecmp(argv[i], "-ip=", 4) == 0)   {
                memset(spadr, 0, sizeof(spadr));
                strncpy(spadr, &argv[i][4], sizeof(spadr)-1);
            }
            else    {
                usage(argv[0]);
            }
        }
        else    {
            j++;
        }
    }

    init_comm(port, spadr);

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
    if (context)    {
        ico_uws_unset_event_cb(context);
        ico_uws_close(context);
    }

    exit(0);
}

