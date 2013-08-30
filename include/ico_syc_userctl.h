/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of User Control API
 *          for privilege applications
 *
 * @date    July-31-2013
 */

#ifndef _ICO_SYC_USERCTL_H_
#define _ICO_SYC_USERCTL_H_

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_change_user
 *          Change the login user.
 *          If fails to authenticate the user,
 *          callback function notifies the result.
 *
 * @param[in]   name                    user name
 * @param[in]   password                user's password
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void ico_syc_change_user(const char *name, const char *password);

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_get_userlist
 *          Get the user list.
 *          Callback function notifies the user list.
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void ico_syc_get_userlist(void);


#ifdef __cplusplus
}
#endif
#endif /*_ICO_SYC_USERCTL_H_*/
/* vim:set expandtab ts=4 sw=4: */
