/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   onscreen application
 *
 * @date    Feb-15-2013
 */
#include "CicoOnScreenContext.h"

/*============================================================================*/
/* Functions Declaration (CicoOnScreenContext)                                */
/*============================================================================*/

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenContext::CloseUwsContext()
 *          close context
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void CicoOnScreenContext::CloseUwsContext(void)
{
    ico_uws_close(uws_context);
    uws_context = NULL;
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenContext::setUwsId()
 *          setting id value
 *
 * @param[in]   id       setting value
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void CicoOnScreenContext::SetUwsId(void *id)
{
    uws_id = id;
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenContext::CicoOnScreenContext
 *          constractor
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoOnScreenContext::CicoOnScreenContext(int ws_port_tmp)
{
   
    char uri[ICO_HS_TEMP_BUF_SIZE];

    uws_id = NULL;
    ws_connected = 0;
    uws_context = NULL;

    /* set up URI "ws://HOST:PORT" */
    ws_port = ws_port_tmp;
    sprintf(uri, "ws://%s:%d", ICO_HS_WS_HOST, ws_port);

    /* create context */
    uws_context = ico_uws_create_context(uri, ICO_HS_PROTOCOL);
    ICO_DBG("CicoOnScreenContext::CicoOnScreenContext: ctx = %p",
               uws_context);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenContext::~CicoOnScreenContext
 *          constractor
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoOnScreenContext::~CicoOnScreenContext()
{
    ico_uws_close(uws_context);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenContext::SetCallBack
 *          set callback function
 *
 * @param[in]   callback    callback function
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoOnScreenContext::SetCallBack(void (*callback)(const ico_uws_context*,
                                                  ico_uws_evt_e, 
                                                  const void*, 
                                                  const ico_uws_detail*, 
                                                  void*))
{
    int ret;
    /* set callback */
    ws_connected = 0;
    
    if (uws_context == NULL) {
        ICO_DBG("CicoOnScreenContext::SetCallBack: libwebsocket_create_context failed.");
    }
    else {
        /* set callback */
        ret = ico_uws_set_event_cb(uws_context,callback, NULL);
        if (ret != ICO_UWS_ERR_NONE) {
             ICO_DBG("CicoOnScreenContext::SetCallBack: cannnot set callback");
        }
        ws_connected = 1;
    }
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenContext::EventMessage
 *          send message
 *
 * @param[in]   wsi                 libwebsockets management table to send
 * @param[in]   format              message to send
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void 
CicoOnScreenContext::EventMessage(char *format, ...)
{
    va_list list;
    unsigned char message[ICO_HS_TEMP_BUF_SIZE];

    va_start(list, format);
    vsnprintf(reinterpret_cast<char*>(message), sizeof(message), format, list);
    va_end(list);

    ICO_DBG("CicoOnScreenContext::EventMessage: ons_event_message %s", message);

    ico_uws_send(uws_context,uws_id, message, strlen(reinterpret_cast<char*>(message)));

    return;
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenContext::GetWsConnected
 *          get current value of ws_connected
 *
 * @param[in]   none
 * @return      ws_connected
 */
/*--------------------------------------------------------------------------*/
int
CicoOnScreenContext::GetWsConnected(void)
{
    return ws_connected;
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenContext::GetUwsContext
 *          get current value of uws_context
 *
 * @param[in]   none
 * @return      uws_context
 */
/*--------------------------------------------------------------------------*/
struct ico_uws_context *
CicoOnScreenContext::GetUwsContext(void)
{
    return uws_context;
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenContext::SetWsConnected
 *          set value to ws_connected
 *
 * @param[in]   value  value to ws_connected
 * @return      void
 */
/*--------------------------------------------------------------------------*/
void
CicoOnScreenContext::SetWsConnected(int value)
{
   ws_connected = value;
}

