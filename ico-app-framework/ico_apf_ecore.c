/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   client library for Ecore(EFL) application
 *
 * @date    Feb-28-2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/time.h>
#include <Ecore.h>

#include "ico_apf_private.h"
#include "ico_apf_ecore.h"

/* define static function prototype         */
static Eina_Bool ico_apf_ecore_fdevent(void *data, Ecore_Fd_Handler *handler);
static void ico_apf_ecore_fdcontrol(ico_apf_com_poll_t *fd_ctl[], const int num_fds);

/*--------------------------------------------------------------------------*/
/**
 *  @brief  ico_apf_ecore_init
 *          This function connects to AppsController for Ecore application.
 *          If you use AppsController's function, you must call this function.
 *
 *  @param[in]  uri             server URI
 *  @return     result status
 *  @retval     ICO_APF_E_NONE      success
 *  @retval     ICO_APF_E_IO_ERROR  error(failed)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_ecore_init(const char *uri)
{
    apfw_trace("ico_apf_ecore_init: Enter(uri=%s)", uri ? uri : "(NULL)");

    /* initialize resource controller           */
    if (ico_apf_resource_init_client(uri) !=  ICO_APF_RESOURCE_E_NONE)  {
        apfw_error("ico_apf_ecore_init: Leave(Resource initialize Error)");
        return ICO_APF_E_IO_ERROR;
    }

    /* set file descriptor to Ecore main loop   */
    ico_apf_com_poll_fd_control(ico_apf_ecore_fdcontrol);

    apfw_trace("ico_apf_ecore_init: Leave(OK)");
    return ICO_APF_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  ico_apf_ecore_term
 *          This function connects to AppsController for Ecore application.
 *          If you use AppsController's function, you must call this function.
 *
 *  @param      none
 *  @return     none
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API void
ico_apf_ecore_term(void)
{
    apfw_trace("ico_apf_ecore_term: Enter");

    /* terminate resource controller            */
    ico_apf_resource_term_client();

    apfw_trace("ico_apf_ecore_term: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  ico_apf_ecore_init_server
 *          This function connects to AppsController for Ecore application.
 *          If you use AppsController's function, you must call this function.
 *
 *  @param[in]  appid           id of application
 *  @return     result status
 *  @retval     ICO_APF_E_NONE      success
 *  @retval     ICO_APF_E_IO_ERROR  error(failed)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_ecore_init_server(const char *uri)
{
    apfw_trace("ico_apf_ecore_init_server: Enter(uri=%s)", uri ? uri : "(NULL)");

    /* initialize resource controller           */
    if (ico_apf_resource_init_server(uri) !=  ICO_APF_RESOURCE_E_NONE)  {
        apfw_error("ico_apf_ecore_init_server: Leave(Resource initialize Error)");
        return ICO_APF_E_IO_ERROR;
    }

    /* set file descriptor to Ecore main loop   */
    ico_apf_com_poll_fd_control(ico_apf_ecore_fdcontrol);

    apfw_trace("ico_apf_ecore_init_server: Leave(OK)");
    return ICO_APF_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  ico_apf_ecore_term_server
 *          This function connects to AppsController for Ecore application.
 *          If you use AppsController's function, you must call this function.
 *
 *  @param      none
 *  @return     none
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API void
ico_apf_ecore_term_server(void)
{
    apfw_trace("ico_apf_ecore_term_server: Enter");

    /* terminate resource controller            */
    ico_apf_resource_term_server();

    apfw_trace("ico_apf_ecore_term_server: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_comm_connect
 *          callback function called by Ecore when a file descriptor had a change
 *
 * @param[in]   data            user data(ico_apf_com_poll_t)
 * @param[in]   handler         Ecore file descriptor handler
 * @return      call back setting
 * @retval      ECORE_CALLBACK_RENEW    set callback
 */
/*--------------------------------------------------------------------------*/
static Eina_Bool
ico_apf_ecore_fdevent(void *data, Ecore_Fd_Handler *handler)
{
    int     flags;

    flags = (ecore_main_fd_handler_active_get(handler, ECORE_FD_READ))
                    ? ICO_APF_COM_POLL_READ : 0;
    if (ecore_main_fd_handler_active_get(handler, ECORE_FD_WRITE))
        flags |= ICO_APF_COM_POLL_WRITE;
    if (ecore_main_fd_handler_active_get(handler, ECORE_FD_ERROR))
        flags |= ICO_APF_COM_POLL_ERROR;

    ico_apf_com_poll_fd_event((ico_apf_com_poll_t *)data, flags);

    return ECORE_CALLBACK_RENEW;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_ecore_fdcontrol
 *          This function connects to AppsController.
 *
 * @param[in]   fd_ctl      file descriptors
 * @param[in]   num_fds     number of file descriptors
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
ico_apf_ecore_fdcontrol(ico_apf_com_poll_t *fd_ctl[], const int num_fds)
{
    Ecore_Fd_Handler_Flags flags;
    int     i;

    for (i = 0; i < num_fds; i++) {
        if (fd_ctl[i]->flags) {
            flags = (fd_ctl[i]->flags & ICO_APF_COM_POLL_READ) ? ECORE_FD_READ : 0;

            if (fd_ctl[i]->flags & ICO_APF_COM_POLL_WRITE)   flags |= ECORE_FD_WRITE;
            if (fd_ctl[i]->flags & ICO_APF_COM_POLL_ERROR)   flags |= ECORE_FD_ERROR;
            if (! fd_ctl[i]->user_data) {
                /* add file descriptor  */
                fd_ctl[i]->user_data = (void *)
                    ecore_main_fd_handler_add(fd_ctl[i]->fd, flags,
                                              ico_apf_ecore_fdevent,
                                              (void *)fd_ctl[i], NULL, NULL);
            }
            else {
                /* change event         */
                ecore_main_fd_handler_active_set((Ecore_Fd_Handler *)fd_ctl[i]->user_data,
                                                 flags);
            }
        }
        else {
            /* remove file descriptor   */
            ecore_main_fd_handler_del((Ecore_Fd_Handler *)fd_ctl[i]->user_data);
        }
    }
}

