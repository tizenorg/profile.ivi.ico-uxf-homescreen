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
 *  @file   CicoSCVInfoManager.cpp
 *
 *  @brief  This file implementation of CicoSCVInfoManager class
 */
//==========================================================================

#include <string>

#include <Ecore.h>

#include "CicoLog.h"
#include <ico_dbus_amb_efl.h>
#include "CicoSCVInfoManager.h"
#include "CicoSystemConfig.h"
#include "CicoConf.h"
#include "CicoSCVInfo.h"

//==========================================================================    
//  private static variable
//==========================================================================    
CicoSCVInfoManager* CicoSCVInfoManager::ms_myInstance = NULL;
int CicoSCVInfoManager::ms_ecoreEvType= 0;
dbus_type CicoSCVInfoManager::ms_dbusTypes[ICO_TYPE_MAX];
int CicoSCVInfoManager::ms_retryCnt = 0;

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSCVInfoManager::CicoSCVInfoManager()
    : m_initialized(false)
{
    ms_dbusTypes[ICO_TYPE_UNKNOW]  = DBUS_TYPE_INT32;
    ms_dbusTypes[ICO_TYPE_BOOLEAN] = DBUS_TYPE_BOOLEAN;
    ms_dbusTypes[ICO_TYPE_INT8]    = DBUS_TYPE_BYTE;
    ms_dbusTypes[ICO_TYPE_UINT8]   = DBUS_TYPE_BYTE;
    ms_dbusTypes[ICO_TYPE_INT16]   = DBUS_TYPE_INT16;
    ms_dbusTypes[ICO_TYPE_UINT16]  = DBUS_TYPE_UINT16;
    ms_dbusTypes[ICO_TYPE_INT32]   = DBUS_TYPE_INT32;
    ms_dbusTypes[ICO_TYPE_UINT32]  = DBUS_TYPE_UINT32;
    ms_dbusTypes[ICO_TYPE_INT64]   = DBUS_TYPE_INT64;
    ms_dbusTypes[ICO_TYPE_UINT64]  = DBUS_TYPE_UINT64;
    ms_dbusTypes[ICO_TYPE_DOUBLE]  = DBUS_TYPE_DOUBLE;
    ms_dbusTypes[ICO_TYPE_STRING]  = DBUS_TYPE_STRING;

    ms_ecoreEvType = ecore_event_type_new();
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSCVInfoManager::~CicoSCVInfoManager()
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  get instance of CicoSystemConfig
 *
 *  @return  pointer of CicoSystemConfig object
 */
//--------------------------------------------------------------------------
CicoSCVInfoManager*
CicoSCVInfoManager::getInstance(void)
{
    if (NULL == ms_myInstance) {
        ms_myInstance = new CicoSCVInfoManager();
    }
    return ms_myInstance;
}

//--------------------------------------------------------------------------
/**
 *  @brief  startup communication of vehicle information 
 *
 *  @return result
 *  @retval true    success
 *  @retval false   error
 */
//--------------------------------------------------------------------------
bool
CicoSCVInfoManager::startup(void)
{
    ICO_TRA("CicoSCVInfoManager::startup Enter");

    if (true == m_initialized) {
        ICO_DBG("already initialized.");
        ICO_TRA("CicoSCVInfoManager::startup Leave(true)");
    }

    ICO_DBG("called: ico_dbus_amb_start");
    int ret = ico_dbus_amb_start();
    if (0 != ret) {
        ICO_ERR("ico_dbus_amb_start failed. ret=%d", ret);
        ICO_TRA("CicoSCVInfoManager::startup Leave(false)");
        return false;
    }

    m_initialized = true;

    startMonitoring();
   

    ICO_TRA("CicoSCVInfoManager::startup Leave(true)");
    return true;
}
  
//--------------------------------------------------------------------------
/**
 *  @brief  teardown communication of vehicle information
 *
 *  @return result
 *  @retval true    success
 *  @retval false   error
 */
//--------------------------------------------------------------------------
bool
CicoSCVInfoManager::teardown(void)
{
    ICO_TRA("CicoSCVInfoManager::teardown Enter");

    if (false == m_initialized) {
        ICO_DBG("not initialized.");
        ICO_TRA("CicoSCVInfoManager::teardown Leave(true)");
    }

    m_initialized = false;

    stopMonitoring();

    ICO_DBG("called: ico_dbus_amb_end");
    int ret = ico_dbus_amb_end();
    if (0 != ret) {
        ICO_ERR("ico_dbus_amb_end faile. ret=%d", ret);
        ICO_TRA("CicoSCVInfoManager::teardown Leave(false)");
        return false;
    }

    ICO_TRA("CicoSCVInfoManager::teardown Leave(true)");
    return true;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get ecore event type of changed vehicle information
 *
 *  @return ecore event type
 */
//--------------------------------------------------------------------------
int
CicoSCVInfoManager::getEcoreEvType(void)
{
    return ms_ecoreEvType;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get vehicle information
 *
 *  @param [in] property    vehicle information property
 *
 *  @return result
 *  @retval true    success
 *  @retval false   error
 */
//--------------------------------------------------------------------------
bool
CicoSCVInfoManager::getVehicleInfo(int property)
{
    ICO_TRA("CicoSCVInfoManager::getVehicleInfo Enter(prop=%d)", property);

    if (false == m_initialized) {
        ICO_DBG("not initialized.");
        ICO_TRA("CicoSCVInfoManager::getVehicleInfo Leave(false)");
        return false;
    }

    CicoSystemConfig* sysconf = CicoSystemConfig::getInstance();
    CicoSCVehicleInfoConf* viconf = sysconf->getVehicleInfoConf();
    std::map<int, CicoSCVIPropertyConf*>::iterator itr;
    itr = viconf->properties.find(property);
    if (itr == viconf->properties.end()) {
        ICO_WRN("not found propetry(%d)", property);
        return false;
    }
 
    ICO_DBG("called: ico_dbus_amb_get(objname=%s prop=%s zone=%d type=%d)",
            itr->second->objname.c_str(),
            itr->second->property.c_str(),
            itr->second->zone,
            ms_dbusTypes[itr->second->type]);
    int ret = ico_dbus_amb_get(itr->second->objname.c_str(),
                               itr->second->property.c_str(),
                               itr->second->zone,
                               ms_dbusTypes[itr->second->type],
                               CicoSCVInfoManager::dbusAMBGetCB,
                               itr->second);
    if (0 != ret) {
        ICO_WRN("ico_dbus_amb_get failed. ret=%d", ret);
        ICO_TRA("CicoSCVInfoManager::getVehicleInfo Leave(false)");
        return false;
    }

    ICO_TRA("CicoSCVInfoManager::getVehicleInfo Leave(true)");
    return true;
}

//==========================================================================
//  private functions
//==========================================================================

//--------------------------------------------------------------------------
/**
 *  @brief  start vehicle information monitoring
 */
//--------------------------------------------------------------------------
void
CicoSCVInfoManager::startMonitoring(void)
{
    ICO_TRA("CicoSCVInfoManager::startMonitoring Enter");

    CicoSystemConfig* sysconf = CicoSystemConfig::getInstance();
    CicoSCVehicleInfoConf* viconf = sysconf->getVehicleInfoConf();
    std::map<int, CicoSCVIPropertyConf*>::iterator itr;
    itr = viconf->properties.begin();

    for (; itr != viconf->properties.end(); ++itr) {
        ICO_DBG("called: ico_dbus_amb_find_property"
                "(objname=%s prop=%s zone=%d type=%d)",
                itr->second->objname.c_str(),
                itr->second->property.c_str(),
                itr->second->zone,
                ms_dbusTypes[itr->second->type]);
        int ret = ico_dbus_amb_find_property(itr->second->objname.c_str(),
                                             itr->second->property.c_str(),
                                             itr->second->zone,
                                             ms_dbusTypes[itr->second->type],
                                             CicoSCVInfoManager::dbusAMBFindCB,
                                             itr->second);
        if (0 != ret) {
            ICO_WRN("ico_dbus_amb_find_property failed. ret=%d", ret);
        }
    }

    ICO_TRA("CicoSCVInfoManager::startMonitoring Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  stop vehicle information monitoring
 */
//--------------------------------------------------------------------------
void
CicoSCVInfoManager::stopMonitoring(void)
{
    ICO_TRA("CicoSCVInfoManager::stopMonitoring Enter");

    CicoSystemConfig* sysconf = CicoSystemConfig::getInstance();
    CicoSCVehicleInfoConf* viconf = sysconf->getVehicleInfoConf();
    std::map<int, CicoSCVIPropertyConf*>::iterator itr;
    itr = viconf->properties.begin();
    for (; itr != viconf->properties.end(); ++itr) {
        ICO_DBG("called: ico_dbus_amb_unsubscribe"
                "(objname=%s prop=%s zone=%d type=%d)",
                itr->second->objname.c_str(),
                itr->second->property.c_str(),
                itr->second->zone,
                ms_dbusTypes[itr->second->type]);
        int ret = ico_dbus_amb_unsubscribe(itr->second->objname.c_str(),
                                           itr->second->property.c_str(),
                                           itr->second->zone);
        if (0 != ret) {
            ICO_WRN("ico_dbus_amb_subscribe failed. ret=%d", ret);
        }
    }

    ICO_TRA("CicoSCVInfoManager::stopMonitoring Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback function of Ecore_End_Cb
 *
 *  @param [in] user_data   user data
 *  @param [in] func_data   funnction data
 */
//--------------------------------------------------------------------------
void
CicoSCVInfoManager::evVInfoFree(void* user_data, void* func_data)
{
    ICO_TRA("CicoSCVInfoManager::evVInfoFree Enter");
    delete static_cast<CicoSCVInfo*>(func_data);
    ICO_TRA("CicoSCVInfoManager::evVInfoFree Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback function of ecore timer
 *
 *  @param [in] data    user data
 */
//--------------------------------------------------------------------------
Eina_Bool
CicoSCVInfoManager::ecoreRetryFindPropCB(void *data)
{
    ICO_TRA("CicoSCVInfoManager::ecoreRetryFindPropCB Enter");
    if (NULL != data) {
        CicoSCVIPropertyConf* viProp = static_cast<CicoSCVIPropertyConf*>(data);
        ICO_DBG("called: ico_dbus_amb_find_property"
                "(objname=%s prop=%s zone=%d type=%d)",
                viProp->objname.c_str(),
                viProp->property.c_str(),
                viProp->zone,
                ms_dbusTypes[viProp->type]);
        int ret = ico_dbus_amb_find_property(viProp->objname.c_str(),
                                             viProp->property.c_str(),
                                             viProp->zone,
                                             ms_dbusTypes[viProp->type],
                                             CicoSCVInfoManager::dbusAMBFindCB,
                                             viProp);
        if (0 != ret) {
            ICO_WRN("ico_dbus_amb_property failed. ret=%d", ret);
        }

    }
    ICO_TRA("CicoSCVInfoManager::ecoreRetryFindPropCB Leave(cancel)");
    return ECORE_CALLBACK_CANCEL;
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback function of ico_dbus_amb_get result
 *
 *  @param [in] objectname  dbus object name
 *  @param [in] property    vehicle information's name
 *  @param [in] type        type of vechcle information
 *  @param [in] data        user data
 *  @param [in] error       error information
 */
//--------------------------------------------------------------------------
void
CicoSCVInfoManager::dbusAMBFindCB(const char *objectname,
                                 const char *property,
                                 dbus_type type,
                                 void *data,
                                 ico_dbus_error_t *error)
{
    ICO_TRA("CicoSCVInfoManager::dbusAMBFindCB Enter"
            "(objname=%s prop=%s type=%d)",
            objectname, property, type);

    if (NULL == data) {
        ICO_WRN("data is null");
        return;
    }

    CicoSCVIPropertyConf* viProp = static_cast<CicoSCVIPropertyConf*>(data);

    // if find property is error, retry.
    if (NULL != error) {
        CicoSystemConfig* sysconf = CicoSystemConfig::getInstance();
        CicoSCVehicleInfoConf* viconf = sysconf->getVehicleInfoConf();
        ICO_DBG("retryCnt=%d/%d", ms_retryCnt, viconf->retryCnt);
        if (viconf->retryCnt > ms_retryCnt) {
            ms_retryCnt++;
            ICO_DBG("called: ecore_timer_add");
            Ecore_Timer *timer = NULL;
            timer = ecore_timer_add((double)viconf->waitTime/1000.0f,
                                    CicoSCVInfoManager::ecoreRetryFindPropCB,
                                    viProp);
            if (NULL == timer) {
                ICO_ERR("ecore_timer_add() failed.");
            }

        }
        ICO_TRA("CicoSCVInfoManager::dbusAMBFindCB Levae");
        return;
    }

    // get vehicle information
    ICO_DBG("called: ico_dbus_amb_get(objname=%s prop=%s zone=%d type=%d)",
            viProp->objname.c_str(),
            viProp->property.c_str(),
            viProp->zone,
            ms_dbusTypes[viProp->type]);
    int ret = ico_dbus_amb_get(viProp->objname.c_str(),
                               viProp->property.c_str(),
                               viProp->zone,
                               ms_dbusTypes[viProp->type],
                               CicoSCVInfoManager::dbusAMBGetCB,
                               viProp);
    if (0 != ret) {
        ICO_WRN("ico_dbus_amb_subscribe failed. ret=%d", ret);
    }

    // subscribe vehicle information
    ICO_DBG("called: ico_dbus_amb_subscribe"
            "(objname=%s prop=%s zone=%d type=%d)",
            viProp->objname.c_str(),
            viProp->property.c_str(),
            viProp->zone,
            ms_dbusTypes[viProp->type]);
    ret = ico_dbus_amb_subscribe(viProp->objname.c_str(),
                                 viProp->property.c_str(),
                                 viProp->zone,
                                 ms_dbusTypes[viProp->type],
                                 CicoSCVInfoManager::dbusAMBNotifyCB,
                                 viProp);
    if (0 != ret) {
        ICO_WRN("ico_dbus_amb_subscribe failed. ret=%d", ret);
    }

    ICO_TRA("CicoSCVInfoManager::dbusAMBFindCB Levae");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback function of ico_dbus_amb_get result
 *
 *  @param [in] objectname  dbus object name
 *  @param [in] property    vehicle information's name
 *  @param [in] type        type of vechcle information
 *  @param [in] value       vechcle information's value
 *  @param [in] data        user data
 */
//--------------------------------------------------------------------------
void
CicoSCVInfoManager::dbusAMBGetCB(const char *objectname,
                                 const char *property,
                                 dbus_type type,
                                 union dbus_value_variant value,
                                 void *data)
{
    ICO_TRA("CicoSCVInfoManager::dbusAMBGetCB Enter"
            "(objname=%s prop=%s type=%d)",
            objectname, property, type);

    if (NULL == data) {
        ICO_WRN("data is null");
        return;
    }

    CicoSCVIPropertyConf* viConf = static_cast<CicoSCVIPropertyConf*>(data);

    CicoSCVInfo* vinfo = new CicoSCVInfo(type, viConf->id, value);
    ecore_event_add(ms_ecoreEvType, vinfo,
                    CicoSCVInfoManager::evVInfoFree, NULL);

    ICO_TRA("CicoSCVInfoManager::dbusAMBGetCB Levae");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback function of ico_dbus_amb_get result
 *
 *  @param [in] objectname  dbus object name
 *  @param [in] property    vehicle information's name
 *  @param [in] type        type of vechcle information
 *  @param [in] value       vechcle information's value
 *  @param [in] sequence    number of sequence
 *  @parma [in] tv          recored time
 *  @param [in] data        user data
 */
//--------------------------------------------------------------------------
void
CicoSCVInfoManager::dbusAMBNotifyCB(const char *objectname,
                                    const char *property,
                                    dbus_type type,
                                    union dbus_value_variant value,
                                    int sequence,
                                    struct timeval tv,
                                    void *data)
{
    ICO_TRA("CicoSCVInfoManager::dbusAMBNotifyCB Enter"
            "(objname=%s prop=%s type=%d)",
            objectname, property, type);

    if (NULL == data) {
        ICO_WRN("data is null");
        return;
    }

    CicoSCVIPropertyConf* viConf = static_cast<CicoSCVIPropertyConf*>(data);

    CicoSCVInfo* vinfo = new CicoSCVInfo(type, viConf->id, value);
    ecore_event_add(ms_ecoreEvType, vinfo,
                    CicoSCVInfoManager::evVInfoFree, NULL);

    ICO_TRA("CicoSCVInfoManager::dbusAMBNotifyCB Leave");
}
// vim:set expandtab ts=4 sw=4:
