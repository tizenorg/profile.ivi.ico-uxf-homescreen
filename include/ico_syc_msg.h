/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of System Controller internal definition
 *          to send and receive message
 *
 * @date    July-31-2013
 */

#ifndef _ICO_SYC_MSG_H_
#define _ICO_SYC_MSG_H_

#include <json-glib/json-glib.h>

#include <ico_uws.h>

#include "ico_syc_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * typedef message
 */
typedef JsonGenerator *msg_t;

/*
 * typedef message string
 */
typedef gchar *msg_str_t;

/*============================================================================*/
/* internal functions                                                         */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_init_msg
 *          Initialize to create and sent message.
 *
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void ico_syc_init_msg(void);

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_send_msg
 *          Send message to System Controller.
 *
 * @param[in]   msg                     json object (msg_t)
 * @return      result
 * @return      0                       success
 * @return      not 0                   error
 */
/*--------------------------------------------------------------------------*/
int ico_syc_send_msg(msg_t msg);

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_get_command
 *          Get command from message data.
 *
 * @param[in]   data                    message data
 * @param[in]   len                     length of data
 * @return      command
 * @return      command                 success
 * @return      -1                      error
 */
/*--------------------------------------------------------------------------*/
int ico_syc_get_command(const void *data, size_t len);

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_get_str_member
 *          Get string member from json object.
 *
 * @param[in]   obj                     json object
 * @param[in]   name                    json object member name
 * @return      string data
 * @return      not NULL                success
 * @return      NULL                    error
 */
/*--------------------------------------------------------------------------*/
char *ico_syc_get_str_member(JsonObject *obj, const gchar *name);

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_get_int_member
 *          Get int member from json object.
 *
 * @param[in]   obj                     json object
 * @param[in]   name                    json object member name
 * @return      integer data
 * @return      num                     success
 * @return      -1                      error
 */
/*--------------------------------------------------------------------------*/
int ico_syc_get_int_member(JsonObject *obj, const gchar *name);

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_free_msg
 *          Free the json object.
 *
 * @param[in]   msg                     json object (msg_t)
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void ico_syc_free_msg(msg_t msg);

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_term_msg
 *          Terminate to create and send message.
 *
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void ico_syc_term_msg(void);


#ifdef __cplusplus
}
#endif
#endif /*_ICO_SYC_MSG_H_*/
/* vim:set expandtab ts=4 sw=4: */
