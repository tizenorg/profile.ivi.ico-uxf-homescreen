/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   touch action for onscreen application
 *
 * @date    Feb-15-2013
 */
#include "CicoOnScreenAction.h"

/*============================================================================*/
/* Declare static values                                                      */
/*============================================================================*/
char CicoOnScreenAction::edje_str[ICO_ONS_BUF_SIZE];
int CicoOnScreenAction::wait_reply = ICO_ONS_NO_WAIT;
int CicoOnScreenAction::ws_port;
CicoOnScreenContext* CicoOnScreenAction::context;
CicoHomeScreenSound* CicoOnScreenAction::sound;

/*============================================================================*/
/* Functions Declaration (CicoOnScreenAction)                                 */
/*============================================================================*/

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenAction::CreateContext
 *          create context
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void CicoOnScreenAction::CreateContext(void)
{
    context = new CicoOnScreenContext(ws_port);

    /* set callback */
    if(context == NULL){
        return;
    }
    context->SetCallBack(UwsCallBack);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenAction::DeleteContext
 *          delete context
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void CicoOnScreenAction::DeleteContext(void)
{
    if(context == NULL){
        return;
    }
    delete context;
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenAction::Initialize
 *          initialize
 *
 * @param[in]   ws_port    port of websocket
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void CicoOnScreenAction::Initialize(CicoHomeScreenConfig *homescreen_config,CicoHomeScreenSound *sound_tmp)
{
    
    wait_reply = ICO_ONS_NO_WAIT;

    ws_port = ICO_HS_WS_PORT;
  
    sound = sound_tmp;

    /* copy ws_port */
    if (homescreen_config != NULL) {
        ws_port = homescreen_config->ConfigGetInteger(ICO_HS_CONFIG_ONSCREEN,
                                         ICO_HS_CONFIG_WS_PORT,
                                         ICO_HS_WS_PORT);
    }

    /* create context*/
    CreateContext(); 

    /* ecore timer add*/
    ecore_timer_add(ICO_ONS_WS_TIMEOUT,EcoreEvent, NULL);
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenAction::Finalize
 *          finalize
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void CicoOnScreenAction::Finalize(void)
{
    /* create context*/
    DeleteContext(); 

}

/*--------------------------------------------------------------------------*/
/*
 * @brief   CicoOnScreenAction::EdjeParseStr
 *          parse string by " "
 *
 * @param[in]   in         text    
 * @param[in]   arg_num    arg_num
 * @return      none
 */
/*--------------------------------------------------------------------------*/
char *
CicoOnScreenAction::EdjeParseStr(void *in, int arg_num)
{
    int i;
    char *data;

    ICO_DBG("CicoOnScreenAction::EdjeParseStr: %s, arg = %d", in, arg_num);
    data = strtok(reinterpret_cast<char*>(in), " ");
    /* arg_num : 0 to n */
    for (i = 0; i < arg_num; i++) {
        data = strtok(NULL, " ");
    }
    ICO_DBG("CicoOnScreenAction::EdjeParseStr: data: %s", data);
    return data;
}
/*--------------------------------------------------------------------------*/
/*
 * @brief   CicoOnScreenAction::UwsCallBack
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
void
CicoOnScreenAction::UwsCallBack(const struct ico_uws_context *uws_context,
                const ico_uws_evt_e event, const void *id,
                const ico_uws_detail *detail, void *data)
{
    ICO_DBG("CicoOnScreenAction::UwsCallBack: %p", uws_context);
    char *in;

    switch (event) {
    case ICO_UWS_EVT_OPEN:
        ICO_DBG("CicoOnScreenAction::UwsCallBack: ICO_UWS_EVT_OPEN(id=%d)", 
                   reinterpret_cast<int>(id));
        context->SetUwsId(const_cast<void *>(id));
        context->EventMessage(const_cast<char*>("%s ANS HELLO"), ICO_HS_MSG_HEAD_OS);
        break;

    case ICO_UWS_EVT_CLOSE:
        ICO_DBG("CicoOnScreenAction::UwsCallBack: ICO_UWS_EVT_CLOSE(id=%d)",
                   reinterpret_cast<int>(id));
        context->CloseUwsContext();
        context->SetWsConnected(0);
        context->SetUwsId(NULL);
        break;

    case ICO_UWS_EVT_RECEIVE:
        ICO_DBG("CicoOnScreenAction::UwsCallBack: ICO_UWS_EVT_RECEIVE(id=%d, msg=%s, len=%d)",
                   reinterpret_cast<int>(id), static_cast<char *>(detail->_ico_uws_message.recv_data),
                   detail->_ico_uws_message.recv_len);
        in = static_cast<char *>(detail->_ico_uws_message.recv_data);

        if (strlen(in) == 0)
            break;
        wait_reply = ICO_ONS_NO_WAIT;
        /* onscreen activate request */
        if (strncmp("OPEN", in, 4) == 0) {
            ICO_DBG("%s", in);
            strncpy(edje_str, EdjeParseStr(in, 1), sizeof(edje_str));
            ICO_DBG("CicoOnScreenAction::UwsCallBack: %s", &edje_str[0]);
            if (CicoOnScreenAppList::LoadIconsEdjeFile(&edje_str[0]) == 0) {
                context->EventMessage(const_cast<char*>("%s RESULT SUCCESS"), ICO_HS_MSG_HEAD_OS);
            }
            else {
                context->EventMessage(const_cast<char*>("%s RESULT FAILED"), ICO_HS_MSG_HEAD_OS);
            }
        }
        break;

    case ICO_UWS_EVT_ERROR:
        ICO_DBG("CicoOnScreenAction::UwsCallBack: ICO_UWS_EVT_ERROR(id=%d, err=%d)",
                   reinterpret_cast<int>(id), detail->_ico_uws_error.code);
        break;

    case ICO_UWS_EVT_ADD_FD:
        ICO_DBG("CicoOnScreenAction::UwsCallBack: ICO_UWS_EVT_ADD_FD(id=%d, fd=%d)",
                   reinterpret_cast<int>(id), detail->_ico_uws_fd.fd);
        break;

    case ICO_UWS_EVT_DEL_FD:
        ICO_DBG("CicoOnScreenAction::UwsCallBack: ICO_UWS_EVT_DEL_FD(id=%d, fd=%d)",
                   reinterpret_cast<int>(id), detail->_ico_uws_fd.fd);
        break;

    default:
        break;
    }

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   OnScreenAction::EcoreEvent
 *          timer handler, it is called by Ecore.
 *
 * @param[in]   data                user data
 * @return      call back setting
 * @retval      ECORE_CALLBACK_RENEW    set callback
 */
/*--------------------------------------------------------------------------*/
Eina_Bool 
CicoOnScreenAction::EcoreEvent(void *data)
{
    if (context->GetWsConnected()) {
        ico_uws_service(context->GetUwsContext());
    }
    else {
        if (context != NULL || context->GetUwsContext() != NULL) {
            DeleteContext();
        }
        CreateContext(); 
    }

    return ECORE_CALLBACK_RENEW;
}


/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenAction::TouchUpEdje
 *          processing when edje button touch up.
 *
 * @param[in]   data                user data
 * @param[in]   obj                 evas object of the button
 * @param[in]   event_info          evas event infomation
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoOnScreenAction::TouchUpEdje(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    if (wait_reply == ICO_ONS_CMD_WAIT){
        return;
    } 
    wait_reply = ICO_ONS_CMD_WAIT;

    /* get name from userdata */
    if (data != NULL) {
        ICO_DBG("CicoOnScreenAction::TouchUpEdje: user data is %s", 
                   static_cast<const char *>(data));
    }
    else {
        ICO_DBG("CicoOnScreenAction::TouchUpEdje: user data is NULL");
    }
    context->EventMessage(const_cast<char*>("%s TOUCH %s %s"), ICO_HS_MSG_HEAD_OS, edje_str, data);

    /* operation sound */
    sound->PlaySound(sound->GetSoundFileName(ICO_HS_SND_TYPE_DEFAULT));
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenAction::TouchUpNext
 *          processing when next button touch up.
 *
 * @param[in]   data                user data
 * @param[in]   obj                 evas object of the button
 * @param[in]   event_info          evas event infomation
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoOnScreenAction::TouchUpNext(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
   
    if (wait_reply == ICO_ONS_CMD_WAIT){
        return;
    }
    wait_reply = ICO_ONS_CMD_WAIT;

    CicoOnScreenAppList::NextList();

    /* get name from userdata */
    if (data != NULL) {
        ICO_DBG("CicoOnScreenAction::TouchUpNext: user data is %s", 
                   static_cast<const char *>(data));
    }
    else {
        ICO_DBG("OCicoOnScreenAction::TouchUpNext: user data is NULL");
    }
    context->EventMessage(static_cast<char*>("%s TOUCH %s %s"), ICO_HS_MSG_HEAD_OS, edje_str, data);

    /* operation sound */
    sound->PlaySound(sound->GetSoundFileName(ICO_HS_SND_TYPE_DEFAULT));
    
}


