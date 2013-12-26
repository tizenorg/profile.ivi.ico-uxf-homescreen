#ifndef __CICOONSCREENCONTEXT_H__
#define __CICOONSCREENCONTEXT_H__
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
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "ico_uxf.h"
#include "ico_uxf_conf.h"
#include "ico_uxf_conf_ecore.h"

#include "home_screen.h"
#include "home_screen_res.h"


#include <ico_uws.h>

                                                                        
/*============================================================================*/
/* Class Declaration (CicoOnscreenContext)                               */
/*============================================================================*/
class CicoOnScreenContext                                                     
{
  public:
    CicoOnScreenContext(int ws_port);
    ~CicoOnScreenContext(void);
    void SetUwsId(void *id);
    int GetWsConnected(void); 
    struct ico_uws_context *GetUwsContext(void);
    void CloseUwsContext();
    void SetWsConnected(int value);
    void SetCallBack(void (*callback)(const ico_uws_context*,
                                      ico_uws_evt_e,
                                      const void*, 
                                      const ico_uws_detail*,
                                      void*));
    void EventMessage(char *format, ...);
  private:
    void *uws_id;
    int ws_port;
    int ws_connected;
    struct ico_uws_context *uws_context;
  protected:
    CicoOnScreenContext operator = (const CicoOnScreenContext&); 
    CicoOnScreenContext(const CicoOnScreenContext&); 
};
#endif

