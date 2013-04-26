/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of Apprication Framework (for Ecode(EFL) based application)
 *
 * @date    Feb-28-2013
 */

#ifndef __ICO_APF_ECORE_H__
#define __ICO_APF_ECORE_H__

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Initialize AppsController interface for Ecode(EFL) based application
 *
 *  @param[in]  uri         server URI
 *  @return     result status
 *  @retval     ICO_APF_E_NONE      success
 *  @retval     ICO_APF_E_IO_ERROR  error(failed)
 */
/*--------------------------------------------------------------------------*/
int ico_apf_ecore_init(const char *uri);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Terminame AppsController interface for Ecode(EFL) based application
 *
 *  @param      none
 *  @return     result status
 *  @return     none
 */
/*--------------------------------------------------------------------------*/
void ico_apf_ecore_term(void);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Initialize AppsController interface for server(AppsController)
 *
 *  @param[in]  uri         my URI
 *  @return     result status
 *  @retval     ICO_APF_E_NONE      success
 *  @retval     ICO_APF_E_IO_ERROR  error(failed)
 */
/*--------------------------------------------------------------------------*/
int ico_apf_ecore_init_server(const char *uri);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Terminame AppsController interface for server(AppsController)
 *
 *  @param      none
 *  @return     none
 */
/*--------------------------------------------------------------------------*/
void ico_apf_ecore_term_server(void);

#ifdef __cplusplus
}
#endif

#endif  /* __ICO_APF_ECORE_H__ */
