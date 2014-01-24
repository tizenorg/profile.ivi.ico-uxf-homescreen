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
 *  @file   CicoWindowInterface.h
 *
 *  @brief  This file is definition of CicoWindowInterface class
 */
//==========================================================================
#ifndef CICO_WINDOW_INTERFACE_H_
#define CICO_WINDOW_INTERFACE_H_

//==========================================================================
/**
 *  @brief  This class provide the window interfaces
 */
//==========================================================================
class CicoWindowInterface
{
public:
    virtual bool Terminate(void) = 0;
    virtual void Show(void) = 0;
    virtual void Hide(void) = 0;
    virtual void SetPos(int x, int y) = 0;
    virtual void SetSize(int w, int h) = 0;
};
#endif  // __CICO_WINDOW_INTERFACE_H__
// vim: set expandtab ts=4 sw=4:
