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
 *  @file   CicoCommonComponent.h
 *
 *  @brief  This file is definition of CicoCommonComponent class
 */
//==========================================================================
#ifndef __CICO_COMMON_COMPONENT_H__
#define __CICO_COMMON_COMPONENT_H__

#include <Elementary.h>
#include <ctime>
#include <list>
#include <memory>
#include <vector>
#include "CicoComponentInterface.h"
#include "CicoModuleImplementation.h"

//==========================================================================
/**
 *  @brief  This class provide the common component interfaces
 */
//==========================================================================
class CicoCommonComponent : public CicoComponentInterface
{
public:
    // default constructor
    CicoCommonComponent();

    // destructor
    virtual ~CicoCommonComponent();

    // terminate component
    virtual bool Terminate(void);

    // show component
    virtual void Show(void);

    // hide component
    virtual void Hide(void);

    // set position of  component
    virtual void SetPos(int x, int y);

    // set size of component
    virtual void SetSize(int w, int h);

protected:
    // pack module 
    void PackModule(std::shared_ptr<CicoCommonModule> module);

    // unpack module 
    void UnpackModule(std::shared_ptr<CicoCommonModule> module);

protected:
    int posx_;      /// position x
    int posy_;      /// position y
    int width_;     /// componet width
    int height_;    /// componet height
    std::list<std::shared_ptr<CicoCommonModule> > modulelist_;  ///module list
};
#endif  // __CICO_COMMON_COMPONENT_H__
// vim: set expandtab ts=4 sw=4:
