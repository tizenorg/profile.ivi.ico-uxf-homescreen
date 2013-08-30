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
 *  @file   CicoGeometry.h
 *
 *  @brief  
 */
/*========================================================================*/    

#ifndef __CICO_GEOMETRY_H__
#define __CICO_GEOMETRY_H__

#ifdef __cplusplus
extern "C" {
#endif

class CicoGeometry {
public:
    /**
     *  Constructor
     */
    CicoGeometry(int x, int y, int w, int h);
   
    /**
     *  Assignment Operator
     */
    CicoGeometry& operator=(const CicoGeometry &object);

    /**
     *  Destructor
     */
    ~CicoGeometry();

    /**
     *  Copy Constructor
     */
    CicoGeometry(const CicoGeometry &object);

private:
    /**
     *  Default Constructor
     */
    CicoGeometry();
 
public:
    int _x;
    int _y;
    int _w;
    int _h;
};

#ifdef __cplusplus
}
#endif

#endif  /* __CICO_GEOMETRY_H__ */
/* vim:set expandtab ts=4 sw=4: */
