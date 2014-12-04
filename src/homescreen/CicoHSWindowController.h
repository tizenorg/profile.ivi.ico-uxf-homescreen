/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   window control
 *
 * @date    Aug-08-2013
 */
#ifndef __CICO_HSWINDOW_CONTROLLER_H__
#define __CICO_HSWINDOW_CONTROLLER_H__
#include <stdio.h>
#include <pthread.h>
#include <libwebsockets.h>

#include <unistd.h>
#include <Eina.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Wayland.h>
#include <Ecore_Evas.h>
#include <Edje.h>

#include "CicoHSWindow.h"

class CicoHSWindowController
{
  public:
    CicoHSWindowController();
    ~CicoHSWindowController();
    static void Initialize(void);
    static void Finalize(void);
    static void GetFullScreenSize(int orientation,int *width,int *height);
    static Ecore_Evas *GetBaseEvas(void);

  protected:
    CicoHSWindowController operator=(const CicoHSWindowController&);
    CicoHSWindowController(const CicoHSWindowController&);
  private:
    static Ecore_Evas *ms_ecoreEvas;
};
#endif
// vim:set expandtab ts=4 sw=4:
