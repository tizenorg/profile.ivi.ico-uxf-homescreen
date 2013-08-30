/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

/*========================================================================*/    
/**
 *  @file   CicoBackground.h
 *
 *  @brief  
 */
/*========================================================================*/    

#include "CicoEvasObject.h"
#include "CicoColor.h"

#ifndef __CICO_BACKGROUND_H__
#define __CICO_BACKGROUND_H__

#ifdef __cplusplus
extern "C" {
#endif

class CicoBackground : public CicoEvasObject {
public:
    /* Constructor */
    CicoBackground(const Evas *parent,
                   const CicoGeometry &geometry,
                   const CicoColor &color);
   
    /* Destructor */
    virtual ~CicoBackground();

    /* Get this object geometry */
    void setColor(const CicoColor &color);

protected:
    /* Default Constructor */
    CicoBackground();
 
    /* Assignment Operator */
    CicoBackground& operator=(const CicoBackground &object);

    /* Copy Constructor */
    CicoBackground(const CicoBackground &object);

private:
    /* color of background object  */
    CicoColor _color;
};

#ifdef __cplusplus
}
#endif

#endif  /* __CICO_BACKGROUND_H__ */
/* vim:set expandtab ts=4 sw=4: */
