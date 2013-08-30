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
 *  @file   CicoEvasObject.h
 *
 *  @brief  Class definition of evas object
 */
/*========================================================================*/    

#include <Ecore_Evas.h>

#include "CicoGeometry.h"

#ifndef __CICO_EVAS_OBJECT_H__
#define __CICO_EVAS_OBJECT_H__

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------*/
/*
 * Forward declaration
 */
/*------------------------------------------------------------------------*/

/*------------------------------------------------------------------------*/
/**
 *  @class  CiconEvasObject
 *
 *  @brief  This class provide common funciton of evas object 
 */
/*------------------------------------------------------------------------*/
class CicoEvasObject {
public:
    typedef enum {
        OBJTYPE_UNKNOWN      = 0,
        OBJTYPE_RECTANGLE    = 1,
        OBJTYPE_POLYGON      = 2,
        OBJTYPE_LINE         = 3,
        OBJTYPE_IMAGE        = 4,
        OBJTYPE_FILLED_IMAGE = 5,
        OBJTYPE_TEXT         = 6,
        OBJTYPE_TEXTBLOCK    = 7,
    } ObjType;

public:
    /* Destructor */
    virtual ~CicoEvasObject();

    /* Show this object */
    void show(void);

    /* Hide this object */
    void hide(void);

    /* Is visible this object */
    bool isVisible(void);

    /* Move this object */
    void move(int x, int y);

    /* Resize this object */
    void resize(int w, int h);

    /* Get this object geometry */
    const CicoGeometry& getGeometry(void);

    /* Set this object layer */
    void setLayer(short l);

    /* Get this object layer */
    short getLayer(void) const;

    /* Get this evas object */
    Evas_Object* getObject(void) const;

    /* Set this evas object */
    void setClip(Evas_Object *clip);

    /* Set this evas object */
    static void event_cb(void *data,
                         Evas *evas, 
                         Evas_Object *obj,
                         void *event_info);

protected:
    /* Default Constructor */
    CicoEvasObject();
 
    /* Constructor */
    CicoEvasObject(const Evas *parent,
                   CicoEvasObject::ObjType type,
                   const CicoGeometry &geometry);

    /* Assignment Operator */
    CicoEvasObject& operator=(const CicoEvasObject &object);

    /* Copy Constructor */
    CicoEvasObject(const CicoEvasObject &object);

protected:
    /* parent object of this object */
    Evas *_parent;

    /* instance of this object */
    Evas_Object *_object;

    /* geometry of this object */
    CicoGeometry _geometry;
};

#ifdef __cplusplus
}
#endif

#endif  /* __CICO_EVAS_OBJECT_H__ */
/* vim:set expandtab ts=4 sw=4: */
