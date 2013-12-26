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
 *  @file   CicoSCVInfo.h
 *
 *  @brief  This file is definition of CicoSCVInfo class
 */
//==========================================================================
#ifndef __CICO_SC_VINFO_H__
#define __CICO_SC_VINFO_H__

#include <ico_dbus_amb_efl.h>

//--------------------------------------------------------------------------
/**
 *  @brief Enumerations of vehicle information property
 */
//--------------------------------------------------------------------------
typedef enum _ico_vinfo_prop {
    ICO_VINFO_VEHICLE_SPEED  = 1,   ///< vehicle speed
    ICO_VINFO_SHIFT_POSITION,       ///< shift position
    ICO_VINFO_LIGHT_LEFT,           ///< light left(Turn left)
    ICO_VINFO_LIGHT_RIGHT,          ///< light right(Turn right)
    ICO_VINFO_NIGHT_MODE,           ///< night mode
    ICO_VINFO_DRIVING_MODE,         ///< driving mode

    ICO_VINFO_MAX                   ///< maximum number of type
} ico_vinfo_prop_e;

//--------------------------------------------------------------------------
/**
 *  @brief This class holds vehicle information
 */
//--------------------------------------------------------------------------
class CicoSCVInfo
{
public:
    // default constructor
    CicoSCVInfo(int type, int prop, union dbus_value_variant value)
        : m_type(type), m_prop(prop), m_value(value) {}

    // destructor
    virtual ~CicoSCVInfo() {}

    // get value type
    int getType(void) { return m_type; }

    // get property
    int getProperty(void) { return m_prop; }

    // get boolean value
    bool getBooleanValue(void) { return (m_value.bval != 0) ? true : false; }

    // get int8_t value
    int8_t getint8Value(void) { return m_value.yval; }

    // get uint8_t value
    uint8_t getUint8Value(void) { return m_value.yval; }

    // get int16_t value
    int16_t getInt16Value(void) { return m_value.i16val; }

    // get uint16_t value
    uint16_t getUint16Value(void) { return m_value.ui16val; }

    // get int32_t value
    int32_t getInt32Value(void) { return m_value.i32val; }

    // get uint32_t value
    uint32_t getUint32Value(void) { return m_value.ui32val; }

    // get int64_t value
    int64_t getInt64Value(void) { return m_value.i64val; }

    // get uint64_t value
    uint64_t getUint64Value(void) { return m_value.ui64val; }

    // get double value
    double getDoubleValue(void) { return m_value.dval; }

    // get string value
    const char* getStringValue(void) { return (const char*)m_value.sval; }

private:
    // default constructor
    CicoSCVInfo();

    // assignment operator
    CicoSCVInfo& operator=(const CicoSCVInfo &object);

    // copy constructor
    CicoSCVInfo(const CicoSCVInfo &object);

private:
    // type of value
    int m_type;

    // property id of vehicle information
    int m_prop;

    // vehicle  information value
    union dbus_value_variant m_value;
};
#endif  // __CICO_SC_VINFO_H__
// vim:set expandtab ts=4 sw=4:
