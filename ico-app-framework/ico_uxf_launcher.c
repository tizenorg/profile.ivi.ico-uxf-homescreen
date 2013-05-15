/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   user experience library for HomeScreen
 *
 * @date    Feb-28-2013
 */

#include    <stdio.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    <string.h>
#include    <errno.h>

#include    "wayland-client.h"
#include    "wayland-util.h"
#include    "ico_uxf.h"                 /* UX-FW open definition header         */
#include    "ico_uxf_private.h"         /* UX-FW inner definition header        */
#include    "ico_uxf_conf.h"

/* valiable & table                     */
/* UX-FW API inner management table     */
extern Ico_Uxf_Api_Mng         gIco_Uxf_Api_Mng;


/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_process_execute: launch a program
 *
 * @param[in]   name        program name
 * @return  result
 * @retval  ICO_UXF_EOK         success
 * @retval  ICO_UXF_ESRCH       error(not initialized)
 * @retval  ICO_UXF_ENOENT      error(does not exist)
 * @retval  ICO_UXF_EBUSY       error(already launch)
 * @retval  ICO_UXF_EPERM       error(no authority)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_process_execute(const char *name)
{
    Ico_Uxf_Mng_Process *proc;              /* process management table         */
    int         hash;
    bundle      *appBundle = NULL;
    Ico_Uxf_conf_application    *apptbl = NULL;

    uifw_trace("ico_uxf_process_execute: Enter(%s)", name);

    if (gIco_Uxf_Api_Mng.Initialized <= 0) {
        uifw_warn("ico_uxf_process_execute: Leave(ESRCH)");
        return ICO_UXF_ESRCH;
    }

    for (hash = 0; hash < ICO_UXF_MISC_HASHSIZE; hash++)    {
        proc = gIco_Uxf_Api_Mng.Hash_ProcessId[hash];
        while (proc)    {
            uifw_trace("ico_uxf_process_execute: PROC(%s)", proc->attr.process);
            if (strcasecmp(proc->attr.process, name) == 0)  break;
            proc = proc->nextidhash;
        }
        if (proc) break;
    }

    if (! proc) {
        uifw_warn("ico_uxf_process_execute: Leave(ENOENT)");
        return ICO_UXF_ENOENT;
    }

    ico_uxf_enter_critical();               /* enter critical section           */

    if (proc->attr.status != ICO_UXF_PROCSTATUS_STOP)   {
        ico_uxf_leave_critical();           /* leave critical section           */
        uifw_warn("ico_uxf_process_execute: Leave(EBUSY)");
        return ICO_UXF_EBUSY;
    }

    /* in order to avoid double execute, status set starting    */
    proc->attr.status = ICO_UXF_PROCSTATUS_INIT;
    ico_uxf_leave_critical();               /* leave critical section           */

    /* setup option */
    appBundle = bundle_create();
    apptbl = (Ico_Uxf_conf_application *)ico_uxf_getAppByAppid(name);
    if ((appBundle != NULL) && (apptbl != NULL))    {
        char *opt = strdup(apptbl->exec);
        char *str = strtok(opt, " ");
        while (str != NULL) {
            str = strtok(NULL, " ");
            if (str != NULL)    {
                bundle_add(appBundle, str, "Application");
                uifw_trace("ico_uxf_process_execute: option(%s)", str);
            }
        }
        free(opt);
    }

    /* execute program                      */
    proc->attr.internalid = aul_launch_app(name ,appBundle);
    if (appBundle != NULL) {
        bundle_free(appBundle);
    }
    if (proc->attr.internalid < 0)  {
        uifw_error("ico_uxf_process_execute: Leave(ENOSYS), Launch App Error(%d)",
                   proc->attr.internalid);
        return ICO_UXF_ENOSYS;
    }

    proc->attr.status = ICO_UXF_PROCSTATUS_RUN;

    uifw_trace("ico_uxf_process_execute: Leave(%s)", proc->attr.process);
    return ICO_UXF_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_process_terminate: finish a program
 *
 * @param[in]   process     process id
 * @return  result
 * @retval  ICO_UXF_EOK         success
 * @retval  ICO_UXF_ESRCH       error(not initialized)
 * @retval  ICO_UXF_ENOENT      error(does not exist)
 * @retval  ICO_UXF_EAGAIN      error(already launch)
 * @retval  ICO_UXF_EPERM       error(no authority)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_process_terminate(const char *process)
{
    Ico_Uxf_Mng_Process     *proc;          /* process management table         */
    int                     ret;            /* process management table         */

    uifw_trace("ico_uxf_process_terminate: Enter(%s)", process);

    if (gIco_Uxf_Api_Mng.Initialized <= 0) {
        uifw_warn("ico_uxf_process_terminate: Leave(ESRCH)");
        return ICO_UXF_ESRCH;
    }

    /* target is other process */
    proc = ico_uxf_mng_process(process, 0);
    if (! proc)    {
        uifw_warn("ico_uxf_process_terminate: Leave(ENOENT)");
        return ICO_UXF_ENOENT;
    }

    ret = aul_terminate_pid(proc->attr.internalid);
    if (ret < 0) {
        uifw_warn("ico_uxf_process_terminate: Leave(ENOSYS) cannot terminate pid=%d",
                  proc->attr.internalid);
        return ICO_UXF_ENOSYS;
    }

    proc->attr.status = ICO_UXF_PROCSTATUS_STOP;
    proc->attr.internalid = -1;
    proc->attr.mainwin.window = 0;
    ico_uxf_free_procwin(proc);

    uifw_trace("ico_uxf_process_terminate: Leave(EOK)");
    return ICO_UXF_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_process_attribute_get: get a process's current state
 *
 * @param[in]   process     process's identity
 * @param[out]  attr        target process's current state
 * @return  result
 * @retval  ICO_UXF_EOK         success
 * @retval  ICO_UXF_ESRCH       error(not initialized)
 * @retval  ICO_UXF_ENOENT      error(does not exist)
 * @retval  ICO_UXF_EPERM       error(no authority)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_process_attribute_get(const char *process, Ico_Uxf_ProcessAttr *attr)
{
    Ico_Uxf_Mng_Process *proc;              /* process management table             */

    uifw_trace("ico_uxf_process_attribute_get: Enter(%s)", process);

    if (gIco_Uxf_Api_Mng.Initialized <= 0) {
        uifw_warn("ico_uxf_process_attribute_get: Leave(ESRCH)");
        return ICO_UXF_ESRCH;
    }

    proc = ico_uxf_mng_process(process, 0);
    if (! proc)    {
        uifw_warn("ico_uxf_process_attribute_get: Leave(ENOENT)");
        return ICO_UXF_ENOENT;
    }

    memcpy((char *)attr, (char *)&(proc->attr), sizeof(Ico_Uxf_ProcessAttr));

    uifw_trace("ico_uxf_process_attribute_get: Leave(EOK)");
    return ICO_UXF_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_process_is_active: get application activity
 *
 * @param[in]   process     process's identity
 * @return  result
 * @retval  2               process is child process
 * @retval  1               process is active
 * @retval  0               process is not active
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_process_is_active(const char *process)
{
    Ico_Uxf_Mng_Process *proc;              /* process management table             */

    if (gIco_Uxf_Api_Mng.Initialized <= 0) {
        uifw_warn("ico_uxf_process_is_active: False(ESRCH)");
        return 0;
    }

    proc = ico_uxf_mng_process(process, 0);
    if (! proc)    {
        uifw_warn("ico_uxf_process_is_active: False(ENOENT)");
        return 0;
    }

    if (proc->attr.child)   {
        uifw_trace("ico_uxf_process_is_active: %s is Child", process);
        return 2;
    }
    uifw_trace("ico_uxf_process_is_active: %s is %s",
               process, proc->attr.active ? "Active" : "Inactive");
    return proc->attr.active;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_process_window_get: get a window defined by the process
 *
 * @param[in]   process     process identity
 * @param[out]  attr        return a window definition
 *                          First is main window of the process,
 *                          second henceforth is sub window of the process.
 *                          At present support only main window.
 * @param[in]   num         number of window definition(= array size)
 * @return  result
 * @retval  >= 0            success(number of windows)
 * @retval  ICO_UXF_E2BIG   The number of the process has exceeded num
 *                          The attribute of the process up to a num piece is returned to attr
 * @retval  ICO_UXF_ENOENT  error(does not exist)
 * @retval  ICO_UXF_EINVAL  error(num is 0 or less)
 * @retval  ICO_UXF_ESRCH   error(not initialized)
 * @retval  ICO_UXF_EPERM   error(no authority)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_process_window_get(const char *process, Ico_Uxf_ProcessWin *attr, const int num)
{
    Ico_Uxf_Mng_Process *proc;              /* process management table         */
    Ico_Uxf_Mng_ProcWin *pwin;              /* Process possession window table  */
    int         n;                          /* number of window                 */

    uifw_trace("ico_uxf_process_window_get: Enter(%s,,%d)", process, num);

    if (gIco_Uxf_Api_Mng.Initialized <= 0) {
        uifw_warn("ico_uxf_process_window_get: Leave(ESRCH)");
        return ICO_UXF_ESRCH;
    }

    if (num <= 0)  {
        uifw_warn("ico_uxf_process_window_get: Leave(EINVAL)");
        return ICO_UXF_EINVAL;
    }

    proc = ico_uxf_mng_process(process, 0);
    if (! proc)    {
        uifw_trace("ico_uxf_process_window_get: Leave(ENOENT)");
        return ICO_UXF_ENOENT;
    }

    memcpy((char *)&(attr[0]), (char *)&(proc->attr.mainwin), sizeof(Ico_Uxf_ProcessWin));

    pwin = proc->procwin;
    for (n = 1; n < num; n++)  {
        if (! pwin)    break;
        memcpy((char *)&(attr[n]), (char *)&(pwin->attr), sizeof(Ico_Uxf_ProcessWin));
        pwin = pwin->next;
    }

    if (pwin)  {
        n = ICO_UXF_E2BIG;
        uifw_trace("ico_uxf_process_window_get: Leave(E2BIG)");
    }
    else    {
        uifw_trace("ico_uxf_process_window_get: Leave(%d)", n);
    }
    return n;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_process_window_get_one: get a window defined by the process
 *
 * @param[in]   process     process identity(appid)
 * @param[out]  attr        return a window definition
 * @param[in]   winidx      window index(0 is main window, 1-N is sub window)
 * @return  result
 * @retval  ICO_UXF_EOK     success
 * @retval  ICO_UXF_ENOENT  error(does not exist)
 * @retval  ICO_UXF_EINVAL  error(winidx is negative)
 * @retval  ICO_UXF_ESRCH   error(not initialized)
 * @retval  ICO_UXF_EPERM   error(no authority)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_process_window_get_one(const char *process,
                               Ico_Uxf_ProcessWin *attr, const int winidx)
{
    Ico_Uxf_Mng_Process *proc;              /* process management table         */
    Ico_Uxf_Mng_ProcWin *pwin;              /* Process possession window table  */
    int         n;                          /* number of window                 */

    uifw_trace("ico_uxf_process_window_get_one: Enter(%s,,%d)", process, winidx);

    if (gIco_Uxf_Api_Mng.Initialized <= 0) {
        uifw_warn("ico_uxf_process_window_get_one: Leave(ESRCH)");
        return ICO_UXF_ESRCH;
    }

    if (winidx < 0)    {
        uifw_warn("ico_uxf_process_window_get_one: Leave(EINVAL)");
        return ICO_UXF_EINVAL;
    }

    proc = ico_uxf_mng_process(process, 0);
    if (! proc)    {
        uifw_warn("ico_uxf_process_window_get_one: Leave(ENOENT, no appid)");
        return ICO_UXF_ENOENT;
    }
    if (winidx >= proc->attr.numwindows) {
        uifw_warn("ico_uxf_process_window_get_one: Leave(ENOENT, winidx over)");
        return ICO_UXF_ENOENT;
    }

    if (winidx == 0)    {
        memcpy(attr, &proc->attr.mainwin, sizeof(Ico_Uxf_ProcessWin));
    }
    else    {
        pwin = proc->procwin;
        for (n = 1; n < winidx; n++)   {
            if (! pwin) {
                uifw_warn("ico_uxf_process_window_get_one: Leave(ENOENT, winidx over)");
                return ICO_UXF_ENOENT;
            }
            pwin = pwin->next;
        }
        memcpy(attr, &pwin->attr, sizeof(Ico_Uxf_ProcessWin));
    }
    uifw_trace("ico_uxf_process_window_get_one: Leave(EOK)");
    return ICO_UXF_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_process_query_processes: get all process current status
 *
 * @param[out]  attr        process's current status
 * @return  result
 * @retval  >= 0            success(number of process)
 * @retval  ICO_UXF_E2BIG   The number of the process has exceeded num
 *                          The attribute of the process up to a num piece is returned to attr
 * @retval  ICO_UXF_EOK     success
 * @retval  ICO_UXF_ESRCH   error(not initialized)
 * @retval  ICO_UXF_EINVAL  error(num is 0 or less)
 * @retval  ICO_UXF_EPERM   error(no authority)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_process_query_processes(Ico_Uxf_ProcessAttr attr[], const int num)
{
    int         n;                          /* number of process                */
    Ico_Uxf_Mng_Process *mng;               /* process management table         */
    int         hash;

    uifw_trace("ico_uxf_process_query_processes: Enter(,%d)", num);

    if (gIco_Uxf_Api_Mng.Initialized <= 0) {
        uifw_warn("ico_uxf_process_query_processes: Leave(ESRCH)");
        return ICO_UXF_ESRCH;
    }

    if (num <= 0)  {
        uifw_warn("ico_uxf_process_query_processes: Leave(EINVAL)");
        return ICO_UXF_EINVAL;
    }

    n = 0;
    for (hash = 0; hash < ICO_UXF_MISC_HASHSIZE; hash++)   {
        mng = gIco_Uxf_Api_Mng.Hash_ProcessId[hash];
        while (mng)    {
            if (n >= num)  break;
            memcpy((char *)(&attr[n]), (char *)&(mng->attr), sizeof(Ico_Uxf_ProcessAttr));
            n ++;
            mng = mng->nextidhash;
        }
        if (mng)   break;
    }

    if (mng)   {
        n = ICO_UXF_E2BIG;
        uifw_trace("ico_uxf_process_query_processes: Leave(E2BIG)");
    }
    else    {
        uifw_trace("ico_uxf_process_query_processes: Leave(%d)", n);
    }
    return n;
}

