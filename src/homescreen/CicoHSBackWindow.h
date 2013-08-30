/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Back Screen
 *
 * @date    Aug-08-2013
 */
#ifndef __CICO_HS_BACK_WINDOW_H__
#define __CICO_HS_BACK_WINDOW_H__
#include <stdio.h>
#include <pthread.h>
#include <libwebsockets.h>

#include "ico_syc_common.h"
#include "ico_syc_winctl.h"

#include <unistd.h>
#include <Eina.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Wayland.h>
#include <Ecore_Evas.h>
#include <Edje.h>

#include "CicoHomeScreenCommon.h"
#include "CicoHomeScreenConfig.h"
#include "CicoHSWindow.h"

class CicoHSBackWindow :public CicoHSWindow
{
  public:
    CicoHSBackWindow(void);
    ~CicoHSBackWindow(void);
    int Initialize(void);
    void Finalize(void);
    int CreateBackWindow(int pos_x,int pos_y,int width,int height);
    void FreeBackWindow(void);
  
  private:
    char img_dir_path[ICO_HS_MAX_PATH_BUFF_LEN];
    Evas *evas;         /* evas object */
    Evas_Object *canvas;
  
  protected:
    CicoHSBackWindow operator=(const CicoHSBackWindow&);
    CicoHSBackWindow(const CicoHSBackWindow&);
};
#endif

