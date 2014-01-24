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
 *  @file   CicoCommonModule.cpp
 *
 *  @brief  This file is implimention of CicoCommonModule class
 */
//==========================================================================

#include <Evas.h>
#include <cassert>

#include <ico_log.h>
#include "CicoModuleInterface.h"
#include "CicoModuleImplementation.h"

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 *
 *  @param[in]  none
 *  @return     none
 */
//--------------------------------------------------------------------------
CicoCommonModule::CicoCommonModule() 
    : evasobj_(NULL), posx_(0), posy_(0), width_(0), height_(0)
{
    ICO_TRA("CicoCommonModule::CicoCommonModule Enter");
    ICO_TRA("CicoCommonModule::CicoCommonModule Leave");

}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 *
 *  @param[in]  none
 *  @return     none
 */
//--------------------------------------------------------------------------
CicoCommonModule::~CicoCommonModule()
{
    ICO_TRA("CicoCommonModule::~CicoCommonModule Enter");
    ICO_TRA("CicoCommonModule::~CicoCommonModule Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief terminate module
 *
 *  @param[in]  none
 *  @return     none
 */
//--------------------------------------------------------------------------
bool
CicoCommonModule::Terminate(void)
{
    ICO_TRA("CicoCommonModule::Terminate Enter");
    if (evas_object_ref_get(evasobj_) > 1) {
        evas_object_unref(evasobj_);
    }
    else {
        evas_object_del(evasobj_);
    }
    ICO_TRA("CicoCommonModule::Terminate Leave");
    return true;
}

//--------------------------------------------------------------------------
/**
 *  @brief show module 
 *
 *  @param[in]  none
 *  @return     none
 */
//--------------------------------------------------------------------------
void
CicoCommonModule::Show(void)
{
    // ICO_TRA("CicoCommonModule::Show Enter");
    assert(evasobj_!=NULL);
    evas_object_show(evasobj_);
    assert (evas_object_visible_get(evasobj_) == EINA_TRUE);
    // ICO_TRA("CicoCommonModule::Show Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief hide module
 *
 *  @param[in]  none
 *  @return     none
 */
//--------------------------------------------------------------------------
void
CicoCommonModule::Hide(void)
{
    ICO_TRA("CicoCommonModule::Hide Enter");
    assert(evasobj_!=NULL);
    evas_object_hide(evasobj_);
    ICO_TRA("CicoCommonModule::Hide Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief set position of module
 *
 *  @param[in]  x   position x
 *  @param[in]  y   position y
 *  @return     none
 */
//--------------------------------------------------------------------------
void
CicoCommonModule::SetPos(int x, int y)
{
    //ICO_TRA("CicoCommonModule::SetPos Enter(x/y=%d/%d)", x, y);
    posx_ = x;
    posy_ = y;
    assert(evasobj_!=NULL);

    evas_object_move(evasobj_, posx_, posy_);
    //ICO_TRA("CicoCommonModule::SetPos Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief set size of module
 *
 *  @param[in]  w   module width
 *  @param[in]  h   module height
 *  @return     none
 */
//--------------------------------------------------------------------------
void
CicoCommonModule::SetSize(int w, int h)
{
    //ICO_TRA("CicoCommonModule::SetSize Enter(w/h=%d/%d", w, h);
    width_ = w;
    height_ = h;
    assert(evasobj_!=NULL);
    evas_object_resize(evasobj_, width_, height_);
    //ICO_TRA("CicoCommonModule::SetSize Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief move module
 *
 *  @param[in]  diffx   displacement of position-x
 *  @param[in]  diffy   displacement of position-y
 *  @return     none
 */
//--------------------------------------------------------------------------
void
CicoCommonModule::Move(int diffx, int diffy)
{
    ICO_TRA("CicoCommonModule::Move Enter(x/y=%d/%d", diffx, diffy);
    posx_ += diffx;
    posy_ += diffy;
    SetPos(posx_, posy_);
    ICO_TRA("CicoCommonModule::Move Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief resize module
 *
 *  @param[in]  ratew   resize rate of width
 *  @param[in]  rateh   resize rate of height
 *  @return     none
 */
//--------------------------------------------------------------------------
void
CicoCommonModule::Resize(double ratew, double rateh)
{
    width_ *= ratew;
    height_ *= rateh;
    SetSize(width_, height_);
}

//--------------------------------------------------------------------------
/**
 *  @brief reset position and size of module
 *
 *  @param[in]  standardx   standard position x
 *  @param[in]  standardy   standard position y
 *  @param[in]  ratew       resize rate of width
 *  @param[in]  rateh       resize rate of height
 *  @return     none
 */
//--------------------------------------------------------------------------
void
CicoCommonModule::Reallocate(int standardx, int standardy,
                             double ratew, double rateh)
{
    posx_ += (posx_ - standardx) * ratew;
    posy_ += (posy_ - standardy) * rateh;
    SetPos(posx_, posy_);
    Resize(ratew, rateh);
}

//--------------------------------------------------------------------------
/**
 *  @brief get raw data
 *
 *  @param[in]  none
 *  @return     evasobj_
 */
//--------------------------------------------------------------------------
Evas_Object *
CicoCommonModule::GetRawData(void) const
{
    return evasobj_;
}
// vim: set expandtab ts=4 sw=4:
