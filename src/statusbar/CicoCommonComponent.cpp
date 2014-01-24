/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

//==========================================================================
/**
 *  @file   CicoCommonComponent.cpp
 *
 *  @brief  This file is definition of CicoCommonComponent class
 */
//==========================================================================

#include <algorithm>
#include <cassert>

#include <ico_log.h>
#include "CicoCommonComponent.h"

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 *
 *  @param[in]  none
 *  @return     none
 */
//--------------------------------------------------------------------------
CicoCommonComponent::CicoCommonComponent()
    : posx_(0), posy_(0), width_(0), height_(0)
{
    ICO_TRA("CicoCommonComponent::CicoCommonComponent Enter");
    ICO_TRA("CicoCommonComponent::CicoCommonComponent Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 *
 *  param[in]   none
 *  return      none
 */
//--------------------------------------------------------------------------
CicoCommonComponent::~CicoCommonComponent()
{
    ICO_TRA("CicoCommonComponent::~CicoCommonComponent Enter");
    ICO_TRA("CicoCommonComponent::~CicoCommonComponent Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief terminate component
 *
 *  @param[in]  none
 *  @return     bool
 *  @retval     true: success 
 */
//--------------------------------------------------------------------------
bool
CicoCommonComponent::Terminate(void)
{
    ICO_TRA("CicoCommonComponent::Terminate Enter");
    std::list<std::shared_ptr<CicoCommonModule>>::iterator itr;
    std::list<std::shared_ptr<CicoCommonModule>>::iterator itr_end;
    for (itr = modulelist_.begin(); itr != itr_end; itr++) {
        (*itr)->Terminate();
    }

    modulelist_.clear();
    ICO_TRA("CicoCommonComponent::Terminate Leave");
    return true;
}

//--------------------------------------------------------------------------
/**
 *  @brief  show component
 *
 * @param[in]   none
 * @return      none
 */
//--------------------------------------------------------------------------
void
CicoCommonComponent::Show(void)
{
    ICO_TRA("CicoCommonComponent::Show Enter");

    std::list<std::shared_ptr<CicoCommonModule>>::iterator itr;
    std::list<std::shared_ptr<CicoCommonModule>>::iterator itr_end;
    itr_end = modulelist_.end();
    for (itr = modulelist_.begin(); itr != itr_end; itr++) {
        (*itr)->Show();
    }

    ICO_TRA("CicoCommonComponent::Show Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  hide component
 *  @param[in]   none
 *  @return      none
  */
//--------------------------------------------------------------------------
void
CicoCommonComponent::Hide(void)
{
    ICO_TRA("CicoCommonComponent::Hide Enter");
    std::list<std::shared_ptr<CicoCommonModule>>::iterator itr;
    std::list<std::shared_ptr<CicoCommonModule>>::iterator itr_end;
    itr_end = modulelist_.end();
    for (itr = modulelist_.begin(); itr != itr_end; itr++) {
        (*itr)->Hide();
    }
    ICO_TRA("CicoCommonComponent::Hide Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  set position of component
 *
 *  @param [in] x   position of x
 *  @param [in] y   position of y
 *  @return     none
 */
//--------------------------------------------------------------------------
void
CicoCommonComponent::SetPos(int x, int y)
{
    ICO_TRA("CicoCommonComponent::SetPos Enter(x/y=%d/%d)", x, y);
    int diffx = x - posx_;
    int diffy = y - posy_;
    posx_ = x;
    posy_ = y;
    std::list<std::shared_ptr<CicoCommonModule>>::iterator itr;
    std::list<std::shared_ptr<CicoCommonModule>>::iterator itr_end;
    itr_end = modulelist_.end();
    for (itr = modulelist_.begin(); itr != itr_end; itr++) {
        (*itr)->Move(diffx, diffy);
    }
    ICO_TRA("CicoCommonComponent::SetPos Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  set size of component
 *
 *  @param [in] w   width of component
 *  @param [in] h   height of component
 *  @return     none
*/
//--------------------------------------------------------------------------
void
CicoCommonComponent::SetSize(int w, int h)
{
    ICO_TRA("CicoCommonComponent::SetSize Enter(w/h=%d/%d)", w, h);
    double rate_w = width_ / w;
    double rate_h = height_ / h;
    width_ = w;
    height_ = h;
    std::list<std::shared_ptr<CicoCommonModule>>::iterator itr;
    std::list<std::shared_ptr<CicoCommonModule>>::iterator itr_end;
    itr_end = modulelist_.end();
    for (itr = modulelist_.begin(); itr != itr_end; itr++) {
        (*itr)->Reallocate(posx_, posy_, rate_w, rate_h);
}
    ICO_TRA("CicoCommonComponent::SetSize Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  pack module
 *
 *  @param [in] module  add module object
 *  @return     none 
*/
//--------------------------------------------------------------------------
void
CicoCommonComponent::PackModule(std::shared_ptr<CicoCommonModule> module)
{
    modulelist_.push_back(module);
}

//--------------------------------------------------------------------------
/**
 *  @brief  unpack module
 *
 *  @param [in] module  remove module object
 *  @return     none
 */
//--------------------------------------------------------------------------
void
CicoCommonComponent::UnpackModule(std::shared_ptr<CicoCommonModule> module)
{
    modulelist_.remove(module);
}
// vim: set expandtab ts=4 sw=4:
