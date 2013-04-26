/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file for homescreen communication
 *
 * @date    Feb-15-2013
 */

#ifndef __HOME_SCREEN_LIB_H__
#define __HOME_SCREEN_LIB_H__

/*============================================================================*/
/* definition                                                                 */
/*============================================================================*/
struct socket_data {
    struct libwebsocket_context *ws_context;
    int port;
};

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
extern int hs_lib_main(int port);
extern int hs_lib_event_message(int type, char *format, ...);

#endif /* __HOME_SCREEN_LIB_H__ */

