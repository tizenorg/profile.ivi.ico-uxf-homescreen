/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   heade for input device
 *
 * @date    Feb-28-2013
 */

#include    <stdio.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    <string.h>
#include    <errno.h>

#include    "ico_uxf.h"
#include    "ico_uxf_private.h"
#include    "ico_input_mgr-client-protocol.h"

extern Ico_Uxf_Api_Mng         gIco_Uxf_Api_Mng;

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_inputdev_attribute_get: get input device attribute
 *
 * @param[in]   devidx          input device index (0-)
 * @return      result
 * @retval      !=NULL          success(input device attribute address)
 * @retval      ==NULL          error(device dose not exist)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API Ico_Uxf_InputDev *
ico_uxf_inputdev_attribute_get(const int devidx)
{
    Ico_Uxf_InputDev    *pdev;
    int                 idx;

    uifw_trace("ico_uxf_inputdev_attribute_get: Enter(%d)", devidx);

    if (gIco_Uxf_Api_Mng.Initialized <= 0) {
        uifw_warn("ico_uxf_window_layer: Leave(ESRCH)");
        return NULL;
    }

    pdev = gIco_Uxf_Api_Mng.InputDev;
    idx = 0;
    while ((pdev != NULL) && (idx < devidx))    {
        idx ++;
        pdev = pdev->next;
    }

    if ((pdev == NULL) || (idx != devidx))  {
        uifw_trace("ico_uxf_inputdev_attribute_get: Leave(ENOENT)");
        return NULL;
    }

    uifw_trace("ico_uxf_inputdev_attribute_get: Leave(EOK, device=%s)", pdev->device);
    return pdev;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_inputsw_attribute_get: get input switch attribute
 *
 * @param[in]   inputdev        input device attribute
 * @param[in]   swidx           input switch index (0-)
 * @return      result
 * @retval      !=NULL          success(input switch attribute address)
 * @retval      ==NULL          error(device dose not exist)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API Ico_Uxf_InputSw *
ico_uxf_inputsw_attribute_get(Ico_Uxf_InputDev *inputdev, const int swidx)
{
    Ico_Uxf_InputSw     *psw;
    int                 idx;

    uifw_trace("ico_uxf_inputsw_attribute_get: Enter(%s,%d)",
               inputdev ? inputdev->device : "(NULL)", swidx);

    if (gIco_Uxf_Api_Mng.Initialized <= 0) {
        uifw_warn("ico_uxf_inputsw_attribute_get: Leave(ESRCH)");
        return NULL;
    }
    if ((inputdev == NULL) || (swidx < 0) || (swidx >= inputdev->numInputSw))   {
        uifw_warn("ico_uxf_inputsw_attribute_get: Leave(device ENOENT)");
        return NULL;
    }

    psw = inputdev->inputSw;
    idx = 0;
    while ((psw != NULL) && (idx < swidx))  {
        idx ++;
        psw = psw->next;
    }

    if ((psw == NULL) || (idx != swidx))   {
        uifw_warn("ico_uxf_inputsw_attribute_get: Leave(switch ENOENT)");
        return NULL;
    }

    uifw_trace("ico_uxf_inputsw_attribute_get: Leave(EOK, switch=%s.%s)",
               inputdev->device, psw->swname);
    return psw;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Send request to Multi Input Manager
 *
 *  @param[in]  add         switch add(=1)/delete(=0)
 *  @param[in]  appid       target application id
 *  @param[in]  device      target input device name
 *  @param[in]  input       target input switch number
 *  @return     result status
 *  @retval     ICO_UXF_EOK success(cullentry always success)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_input_control(const int add, const char *appid, const char *device, const int input)
{
    apfw_trace("ico_uxf_input_control: input switch %s %s %s.%d",
               add ? "add" : "del", appid ? appid : "all-app",
               device ? device : "all-device", input);

    if (add)    {
        ico_input_mgr_control_add_input_app(gIco_Uxf_Api_Mng.Wayland_InputMgr, appid,
                                            device, input, 0);
    }
    else    {
        ico_input_mgr_control_del_input_app(gIco_Uxf_Api_Mng.Wayland_InputMgr, appid,
                                            device, input);
    }
    return ICO_UXF_EOK;
}

