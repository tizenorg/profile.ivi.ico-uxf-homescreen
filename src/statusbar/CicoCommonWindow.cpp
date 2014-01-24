/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <algorithm>

#include <ico_log.h>
#include "CicoCommonWindow.h"

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 *
 *  @param[in]  none
 *  @return     none
 */
//--------------------------------------------------------------------------
CicoCommonWindow::CicoCommonWindow()
    : window_(NULL), windowobj_(NULL),
      posx_(0), posy_(0), width_(0), height_(0)
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 *
 *  @param[in]  none
 *  @return     none
 */
//--------------------------------------------------------------------------
CicoCommonWindow::~CicoCommonWindow()
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  terminate the window
 *
 *  @param[in]  none
 *  @return true on success, false on error
 */
//--------------------------------------------------------------------------
bool
CicoCommonWindow::Terminate(void)
{
    ICO_TRA("CicoCommonWindow::Terminate Enter");
    std::list<CicoCommonComponent*>::iterator itr, itr_end;
    itr_end = componentlist_.end();
    for (itr = componentlist_.begin(); itr != itr_end; itr++) {
        delete (*itr);
    }
    componentlist_.clear();
    ICO_TRA("CicoCommonWindow::Terminate Leave");
    return true;
}

//--------------------------------------------------------------------------
/**
 *  @brief  show the window
 *
 *  @param[in]  none
 *  @return     none
 */
//--------------------------------------------------------------------------
void
CicoCommonWindow::Show(void)
{
    ICO_TRA("CicoCommonWindow::Show Enter");
    ecore_evas_show(window_);
    if (windowobj_ != NULL) {
        evas_object_show(windowobj_);
        std::list<CicoCommonComponent*>::iterator itr, itr_end;
        itr_end = componentlist_.end();
        for (itr = componentlist_.begin(); itr != itr_end; itr++) {
            (*itr)->Show();
        }
    }
    ICO_TRA("CicoCommonWindow::Show Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  hide the window
 *
 *  @param[in]  none
 *  @return     none
 */
//--------------------------------------------------------------------------
void
CicoCommonWindow::Hide(void)
{
    ICO_TRA("CicoCommonWindow::Hide Enter");
    ecore_evas_hide(window_);
    if (windowobj_ != NULL) {
        evas_object_hide(windowobj_);
        std::list<CicoCommonComponent*>::iterator itr, itr_end;
        itr_end = componentlist_.end();
        for (itr = componentlist_.begin(); itr != itr_end; itr++) {
            (*itr)->Hide();
        }
    }
    ICO_TRA("CicoCommonWindow::Show Enter");
}

//--------------------------------------------------------------------------
/**
 *  @brief  set position of the window
 *
 *  @param[in]  x   position x
 *  @param[in]  y   position y
 *  @return     none
 */
//--------------------------------------------------------------------------
void
CicoCommonWindow::SetPos(int x, int y)
{
    posx_ = x;
    posy_ = y;
    ecore_evas_move(window_, posx_, posy_);
    if (windowobj_ != NULL) {
        evas_object_move(windowobj_, posx_, posy_);
        std::list<CicoCommonComponent*>::iterator itr, itr_end;
        itr_end = componentlist_.end();
        for (itr = componentlist_.begin(); itr != itr_end; itr++) {
            (*itr)->SetPos(posx_, posy_);
        }
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief  set size of the window
 *
 *  @param[in]  w   window width
 *  @param[in]  h   window height
 *  @return     none
 */
//--------------------------------------------------------------------------
void
CicoCommonWindow::SetSize(int w, int h)
{
    width_ = w;
    height_ = h;
    ecore_evas_resize(window_, width_, height_);
    if (windowobj_ != NULL) {
        evas_object_resize(windowobj_, width_, height_);
        std::list<CicoCommonComponent*>::iterator itr, itr_end;
        itr_end = componentlist_.end();
        for (itr = componentlist_.begin(); itr != itr_end; itr++) {
            (*itr)->SetSize(width_, height_);
        }
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief  set component
 *
 *  @param [in] component   the object of component
 *  @return     none
 */
//--------------------------------------------------------------------------
void
CicoCommonWindow::SetComponent(CicoCommonComponent *component)
{
    componentlist_.push_back(component);
}

//--------------------------------------------------------------------------
/**
 *  @brief  unset component
 *
 *  @param [in] component   the object of component
 *  @return     none
 */
//--------------------------------------------------------------------------
void
CicoCommonWindow::UnsetComponent(CicoCommonComponent *component)
{
    componentlist_.remove(component);
}
// vim: set expandtab ts=4 sw=4:
