/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of Apprication Framework (Application Manager)
 *
 * @date    Feb-28-2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <aul/aul.h>

#include "ico_apf_private.h"
#include "ico_uxf_conf.h"

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_get_app_id: Get application id(in AppCore) from application process id
 *
 * @param[in]   pid                 application process id
 * @param[out]  appid               application id
 * @return      result
 * @retval      ICO_APP_CTL_E_NONE              success
 * @retval      ICO_APP_CTL_E_INVALID_PARAM     error(pid dose not exist)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_get_app_id(const int pid, char *appid)
{
    int     fd;
    int     ppid = pid;
    int     i, j;
    int     size;
    char    procpath[240];

    memset(appid, 0, ICO_UXF_MAX_PROCESS_NAME+1);

    if (ppid == 0)   {
        ppid = getpid();
    }

    /* Get applicationId from AppCore(AUL)  */
    if (aul_app_get_appid_bypid(ppid, appid, ICO_UXF_MAX_PROCESS_NAME)
                    != AUL_R_OK)    {
        snprintf(procpath, sizeof(procpath)-1, "/proc/%d/cmdline", ppid);
        fd = open(procpath, O_RDONLY);
        if (fd >= 0)    {
            size = read(fd, procpath, sizeof(procpath));
            for (; size > 0; size--)    {
                if (procpath[size-1])   break;
            }
            if (size > 0)   {
                /* Get program base name    */
                i = 0;
                for (j = 0; j < size; j++)  {
                    if (procpath[j] == 0)   break;
                    if (procpath[j] == '/') i = j + 1;
                }
                j = 0;
                for (; i < size; i++)   {
                    appid[j] = procpath[i];
                    if ((appid[j] == 0) ||
                        (j >= ICO_UXF_MAX_PROCESS_NAME))    break;
                    j++;
                }
                /* Search application number    */
                if ((appid[j] == 0) && (j < (ICO_UXF_MAX_PROCESS_NAME-1))) {
                    for (; i < size; i++)   {
                        if ((procpath[i] == 0) &&
                            (procpath[i+1] == '@')) {
                            strncpy(&appid[j], &procpath[i+1],
                                    ICO_UXF_MAX_PROCESS_NAME - j - 1);
                        }
                    }
                }
            }
            close(fd);
        }
        if (appid[0] == 0)  {
            apfw_trace("ico_apf_get_app_id: LEAVE(pid=%d dose not exist)", ppid);
            sprintf(appid, "?%d?", ppid);
            return ICO_APP_CTL_E_INVALID_PARAM;
        }
    }
    return ICO_APP_CTL_E_NONE;
}

