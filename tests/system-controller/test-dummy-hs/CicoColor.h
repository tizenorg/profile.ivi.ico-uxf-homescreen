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
 *  @file   CicoColor.h
 *
 *  @brief  
 */
/*========================================================================*/    

#ifndef __CICO_COLOR_H__
#define __CICO_COLOR_H__

#ifdef __cplusplus
extern "C" {
#endif

class CicoColor {
public:
    /**
     *  Constructor
     */
    CicoColor(int r, int g, int b, int a);
   
    /**
     *  Assignment Operator
     */
    CicoColor& operator=(const CicoColor &object);

    /**
     *  Destructor
     */
    ~CicoColor();

    /**
     *  Copy Constructor
     */
    CicoColor(const CicoColor &object);

private:
    /**
     *  Default Constructor
     */
    CicoColor();
 
public:
    int _r;
    int _g;
    int _b;
    int _a;
};

#ifdef __cplusplus
}
#endif

#endif  /* __CICO_COLOR_H__ */
/* vim:set expandtab ts=4 sw=4: */
