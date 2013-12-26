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
 *  @file   CicoCommonDef.h
 *
 *  @brief  This file is definition of common value
 */
//==========================================================================
#ifndef __CICO_COMMON_DEF_H__
#define __CICO_COMMON_DEF_H__

//--------------------------------------------------------------------------
//  definition of layer type
//--------------------------------------------------------------------------
#define ICO_LAYER_TYPE_BACKGROUND   (1) ///! layer type of background
#define ICO_LAYER_TYPE_APPLICATION  (2) ///! layer type of application
#define ICO_LAYER_TYPE_HOMESCREEN   (3) ///! layer type of homescreen
#define ICO_LAYER_TYPE_INTERRPUTAPP (4) ///! layer type of homescreen
#define ICO_LAYER_TYPE_ONSCREEN     (5) ///! layer type of homescreen
#define ICO_LAYER_TYPE_INPUT        (101) ///! layer type of input panel(soft keyboard)
#define ICO_LAYER_TYPE_TOUCH        (102) ///! layer type of touch panel input
#define ICO_LAYER_TYPE_CURSOR       (103) ///! layer type of cursor

//--------------------------------------------------------------------------
//  range of zone id 
//--------------------------------------------------------------------------
// center display
#define ICO_DISPLAY0_ZONEID_MIN       (1)   ///! display0 zone id min
#define ICO_DISPLAY0_ZONEID_MAX       (20)  ///! display0 zone id max

// second display
#define ICO_DISPLAY1_ZONEID_MIN       (21)  ///! display1 zone id min
#define ICO_DISPLAY1_ZONEID_MAX       (40)  ///! display1 zone id max

//--------------------------------------------------------------------------
//  type
//--------------------------------------------------------------------------
typedef enum _ico_type
{
    ICO_TYPE_UNKNOW  = 0,
    ICO_TYPE_BOOLEAN,
    ICO_TYPE_INT8,
    ICO_TYPE_UINT8,
    ICO_TYPE_INT16,
    ICO_TYPE_UINT16,
    ICO_TYPE_INT32,
    ICO_TYPE_UINT32,
    ICO_TYPE_INT64,
    ICO_TYPE_UINT64,
    ICO_TYPE_DOUBLE,
    ICO_TYPE_STRING,

    ICO_TYPE_MAX,
} ico_type_t;

#endif  // __CICO_COMMON_DEF_H__
// vim:set expandtab ts=4 sw=4:
