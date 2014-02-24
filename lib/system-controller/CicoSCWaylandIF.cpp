/*
 * Copyright (c) 2013-2014, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

//==========================================================================
/**
 *  @file   CicoSCWaylandIF.cpp
 *
 *  @brief  This file implementation of CicoSCWaylandIF class
 */
//==========================================================================

#include "CicoSCWaylandIF.h"
#include "CicoLog.h"


const char * CicoSCWaylandIF::ICO_WL_WIN_MGR_IF        = "ico_window_mgr";
const char * CicoSCWaylandIF::ICO_WL_INPUT_MGR_CTRL_IF = "ico_input_mgr_control";
const char * CicoSCWaylandIF::ICO_WL_EXINPUT_IF        = "ico_exinput";
const char * CicoSCWaylandIF::ICO_WL_INPUT_MGR_DEV_IF  = "ico_input_mgr_device";
const char * CicoSCWaylandIF::ICO_WL_IVI_APPLICATION_IF= "ivi_application";
const char * CicoSCWaylandIF::ICO_WL_IVI_CONTROLLER_IF = "ivi_controller";
const char * CicoSCWaylandIF::ICO_WL_OUTPUT_IF         = "wl_output";


//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSCWaylandIF::CicoSCWaylandIF()
      : m_initialized(false)
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSCWaylandIF::~CicoSCWaylandIF()
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 *
 *  @param [in] date        user data
 *  @param [in] registry    wayland registry
 *  @param [in] name        wayland display id
 *  @parma [in] interface   wayland interface name
 *  @parma [in] version     wayland interface version number
 */
//--------------------------------------------------------------------------
void
CicoSCWaylandIF::initInterface(void               *data,
                               struct wl_registry *registry,
                               uint32_t           name,
                               const char         *interface,
                               uint32_t           version)
{
    ICO_WRN("CicoSCWaylandIF::initInterface called.");
}

//--------------------------------------------------------------------------
/**
 *  @brief  inquire whether initialized
 *
 *  @return true on initialized, false no not initialized
 *  @retval true    initialized
 *  @retval false   not initialized
 */
//--------------------------------------------------------------------------
bool
CicoSCWaylandIF::isInitialized(void)
{
    return m_initialized;
}
// vim:set expandtab ts=4 sw=4:
