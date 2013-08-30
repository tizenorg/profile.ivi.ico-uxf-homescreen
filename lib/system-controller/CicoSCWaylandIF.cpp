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
 *  @file   CicoSCWaylandIF.cpp
 *
 *  @brief  
 */
//==========================================================================

#include "CicoSCWaylandIF.h"
#include "CicoLog.h"


const char * CicoSCWaylandIF::ICO_WL_WIN_MGR_IF        = "ico_window_mgr";
const char * CicoSCWaylandIF::ICO_WL_INPUT_MGR_CTRL_IF = "ico_input_mgr_control";
const char * CicoSCWaylandIF::ICO_WL_EXINPUT_IF        = "ico_exinput";
const char * CicoSCWaylandIF::ICO_WL_INPUT_MGR_DEV_IF  = "ico_input_mgr_device";
const char * CicoSCWaylandIF::ICO_WL_OUTPUT_IF         = "wl_output";
const char * CicoSCWaylandIF::ICO_WL_SHM_IF            = "wl_shm";

CicoSCWaylandIF::CicoSCWaylandIF()
      : m_initialized(false)
{
}

CicoSCWaylandIF::~CicoSCWaylandIF()
{
}

void
CicoSCWaylandIF::initInterface(void               *data,
                               struct wl_registry *registry,
                               uint32_t           name,
                               const char         *interface,
                               uint32_t           version)
{
    ICO_WRN("CicoSCWaylandIF::initInterface called.");
}

bool
CicoSCWaylandIF::isInitialized(void)
{
    return m_initialized;
}
// vim:set expandtab ts=4 sw=4:
