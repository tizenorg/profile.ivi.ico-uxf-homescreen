#ifndef __CICOONSCREENCONTROLWINDOW_H_
#define __CICOONSCREENCONTROLWINDOW_H_
/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   window control for onscreen application
 *
 * @date    Feb-15-2013
 */
#include <unistd.h>
#include <Eina.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Wayland.h>
#include <Ecore_Evas.h>
#include <Edje.h>

#include "home_screen.h"
#include "home_screen_res.h"
#include "CicoHomeScreenConfig.h"
#include "CicoHomeScreenSound.h"

/*============================================================================*/
/* Define data types                                                          */
/*============================================================================*/
#define ICO_ONS_VERTICAL    (1)
#define ICO_ONS_HORIZONTAL  (2)

/*============================================================================*/
/* Class Declaration (CicoOnScreenControlWindow)                              */
/*============================================================================*/
class CicoOnScreenControlWindow                                                     
{
 public:
    CicoOnScreenControlWindow(); /*constractor*/
    ~CicoOnScreenControlWindow(); /*destractor*/

    int CreateEcoreEvas(void);
    void ResizeWindow(CicoHomeScreenConfig *homescreen_config,int orientation);
    void ShowWindow(void);
    static void StartEcoreLoop(void);
    static void QuitEcoreLoop(Ecore_Evas*);

    int GetWidth(void);
    int GetHeight(void);
    int InitEvas(void);
    void DeletePreImage(void);
    int CreateAndShowEdje(void);
    Eina_List *GetEdjeFileCollectionList(const char *edje_file);
    int SetEdjeFile(Eina_List*,const char *edje_file);
    Eina_List *GetEdjeObjectAccessPartList(void);
    Evas_Object * GetEdjeObjectPartObject(const char * data);
    void EvasObjectEventCallBack(Evas_Object *obj,
                                 Evas_Callback_Type type,
                                 void (*func)(void*,
                                    Evas*,
                                    Evas_Object*,
                                    void*),
                                 void *data);
    Evas_Object * EvasObjectImageFilledAdd(void);
    void EvasObjectDel(Evas_Object * object);
    void UpdateEvasObject(Evas_Object *img,
                          const char *imgfile,
                          const char *partname);
    void EinaListAppend(Evas_Object *img);
  private:
    int width, height;  
    Ecore_Evas *window; /* ecore-evas object */
    Evas *evas;         /* evas object */
    Evas_Object *edje;  /* loaded edje objects */
    Eina_List *img_list;

  protected:
    CicoOnScreenControlWindow operator = (const CicoOnScreenControlWindow&); 
    CicoOnScreenControlWindow(const CicoOnScreenControlWindow&); 
};

#endif

