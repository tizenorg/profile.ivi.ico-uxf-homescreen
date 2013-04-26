/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of Apprication Framework (declaration of API error code)
 *
 * @date    Feb-28-2013
 */

#ifndef __ICO_APF_ERROR_H__
#define __ICO_APF_ERROR_H__

#include <tizen.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumerations of common error code for Application Framework.
 */
typedef enum
{
    ICO_APF_E_NONE          = TIZEN_ERROR_NONE,  /**< Successful */
    ICO_APF_E_INVALID_PARAM = TIZEN_ERROR_INVALID_PARAMETER,
    ICO_APF_E_OUT_OF_MEMORY = TIZEN_ERROR_OUT_OF_MEMORY,
    ICO_APF_E_IO_ERROR      = TIZEN_ERROR_IO_ERROR,
    ICO_APF_E_UNKNOWN       = TIZEN_ERROR_UNKNOWN
} ico_app_fw_error_e;

/**
 * @brief Enumerations of error code for Application Communication.
 */
typedef enum
{
    ICO_APF_COM_E_CREATE_CTX    = -200, /**< Create context failed */
    ICO_APF_COM_E_CLT_CONNECT   = -201, /**< Client connect failed */
    ICO_APF_COM_E_CLOSED        = -202, /**< Connection closed */
    ICO_APF_COM_E_WRITE         = -203, /**< libwebsocket_write error */
    ICO_APF_COM_E_RESERVED      = -204, /**< Reserve data failed */
    ICO_APF_COM_E_NO_EXIST      = -205, /**< Uri does not exist */
    ICO_APF_COM_E_UNKNOWN       = -300, /**< Other error */
} ico_app_fw_com_error_e;

/**
 *@brief Enumerations of error code for Application Manager.
 */
typedef enum
{
    ICO_APP_CTL_E_NONE          = TIZEN_ERROR_NONE,  /**< Successful */
    ICO_APP_CTL_E_INVALID_PARAM = TIZEN_ERROR_INVALID_PARAMETER,
    ICO_APP_CTL_E_OUT_OF_MEM    = TIZEN_ERROR_OUT_OF_MEMORY,
    ICO_APP_CTL_E_IO_ERR        = TIZEN_ERROR_IO_ERROR,
} ico_app_ctl_error_e;

/**
 * @brief Enumerations of error code for Resource Manager.
 */
typedef enum _resmgr_error {
    ICO_APF_RESOURCE_E_NONE            =  0, /* Successful */
    ICO_APF_RESOURCE_E_UNKOWN          = -1, /* unkown error */
    ICO_APF_RESOURCE_E_INVAL           = -2, /* invalid parameter */
    ICO_APF_RESOURCE_E_NOT_IMPLEMENTED = -3, /* not implemented */
    ICO_APF_RESOURCE_E_NOT_INITIALIZED = -4, /* not initialized resmger module */
    ICO_APF_RESOURCE_E_INIT_COM_FAILD  = -5, /* intialize communication error */
    ICO_APF_RESOURCE_E_COMMUNICATION   = -6, /* communication error */
    ICO_APF_RESOURCE_E_MSG_PARSER      = -7, /* message parser faild */
    ICO_APF_RESOURCE_E_SERVER_CLOSED   = -8, /* server colosed */
    ICO_APF_RESOURCE_E_SEND_DATA       = -9, /* send data faile */
} ico_apf_resmgr_error_e;

#ifdef __cplusplus
}
#endif
#endif /* __ICO_APF_ERROR_H__ */
