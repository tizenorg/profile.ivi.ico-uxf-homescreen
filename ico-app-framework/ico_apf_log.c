/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Application Framework debug log function
 *
 * @date    Feb-28-2013
 */

#include    <stdio.h>
#include    <stdlib.h>
#include    <stdarg.h>
#include    <unistd.h>
#include    <string.h>
#include    <sys/time.h>
#include    <sys/types.h>
#include    <time.h>
#include    "ico_apf_log.h"
#include    "ico_apf_apimacro.h"

/* variabe & table                          */
/* output level debug log                   */
ICO_APF_API int ico_apf_log_level = 0x7fffffff;
static  int     ico_apf_log_flushmode = 1;
static  int     ico_apf_log_initialized = 0;

/* file descriptor of output debug log      */
static FILE     *sDbgFd = (FILE *)0;

/* name of output source module             */
static char     sDbgProg[32];

/* local time difference(sec)               */
static int      sTimeZone = (99*60*60);

/* output lines                             */
static int      sDbgLines = 0;


/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_log_print: printout log message
 *
 * @param[in]   fmt     message format(same as printf)
 * @param[in]   ...     arguments if need
 * @return      nothing
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API void
ico_apf_log_print(const char *fmt, ...)
{
    va_list     list;

    if (! sDbgFd)   {
        ico_apf_log_open(NULL);
    }
#if ICO_APF_LOG_STDOUT == 0
    else if (sDbgLines >= (ICO_APF_LOG_MAXLINES-2)) {
        if (sDbgLines >= ICO_APF_LOG_MAXLINES)  {
            ico_apf_log_close();
            ico_apf_log_open(sDbgProg);
        }
        else    {
            fflush(sDbgFd);
        }
    }
#endif /*ICO_APF_LOG_STDOUT*/
    if (sDbgFd) {
        va_start(list, fmt);
        vfprintf(sDbgFd, fmt, list);
        va_end(list);
        if (ico_apf_log_flushmode)  {
            fflush(sDbgFd);
        }
    }
    if (sDbgFd != stdout)   {
        sDbgLines ++;
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_log_open: open log file
 *
 * @param[in]   prog    program name
 * @return      nothing
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API void
ico_apf_log_open(const char *prog)
{
#if ICO_APF_LOG_STDOUT == 0
    int     idx;
    char    sPath[128];
    char    sPath2[128];
#endif /*ICO_APF_LOG_STDOUT*/

    if (sDbgFd) {
        fflush(sDbgFd);
        if (sDbgFd != stdout)   {
            fclose(sDbgFd);
        }
    }

    sDbgLines = 0;

    if ((! prog) || (*prog == 0))   {
        sDbgFd = stdout;
        sDbgProg[0] = 0;
        return;
    }
    else    {
        strncpy(sDbgProg, prog, sizeof(sDbgProg)-1);
        sDbgProg[sizeof(sDbgProg)-1] = 0;
    }
#if ICO_APF_LOG_STDOUT > 0
    sDbgFd = stdout;
#else  /*ICO_APF_LOG_STDOUT*/
    snprintf(sPath, sizeof(sPath)-1, "%s/%s.log%d",
             ICO_APF_LOG_DIR, sDbgProg, ICO_APF_LOG_MAXFILES-1);
    (void) remove(sPath);

    for (idx = (ICO_APF_LOG_MAXFILES-1); idx > 0; idx--)    {
        strcpy(sPath2, sPath);
        if (idx > 1)    {
            snprintf(sPath, sizeof(sPath)-1, "%s/%s.log%d",
                     ICO_APF_LOG_DIR, sDbgProg, idx-1);
        }
        else    {
            snprintf(sPath, sizeof(sPath)-1, "%s/%s.log",
                     ICO_APF_LOG_DIR, sDbgProg);
        }
        (void) rename(sPath, sPath2);
    }
    sDbgFd = fopen(sPath, "w");
    if (! sDbgFd)   {
        sDbgFd = stdout;
    }
    else if ((ico_apf_log_initialized == 0) &&
             (sDbgFd != stdout) && (sDbgFd != stderr))  {
        ico_apf_log_initialized = 1;
        fflush(stdout);
        fflush(stderr);
        stdout = sDbgFd;
        stderr = sDbgFd;
    }
#endif /*ICO_APF_LOG_STDOUT*/
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_log_close: close log file
 *
 * @param       nothing
 * @return      nothing
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API void
ico_apf_log_close(void)
{
#if ICO_APF_LOG_STDOUT == 0
    if (sDbgFd) {
        fflush(sDbgFd);
        if (sDbgFd != stdout)   {
            fclose(sDbgFd);
        }
        sDbgFd = (FILE *)0;
    }
#endif /*ICO_APF_LOG_STDOUT*/
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_log_flush: flush log file
 *
 * @param       nothing
 * @return      nothing
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API void
ico_apf_log_flush(void)
{
    if ((sDbgFd != NULL) && (ico_apf_log_flushmode == 0))   {
        fflush(sDbgFd);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_log_curtime: create current time string
 *
 * @param[in]   level   log level string(header of log message)
 * @return      current time string
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API char *
ico_apf_log_curtime(const char *level)
{
    struct timeval  NowTime;
    extern long     timezone;
    static char     sBuf[28];

    gettimeofday(&NowTime, (struct timezone *)0);
    if (sTimeZone > (24*60*60)) {
        tzset();
        sTimeZone = timezone;
    }
    NowTime.tv_sec -= sTimeZone;

    sprintf(sBuf, "%02d:%02d:%02d.%03d[%s]@%d",
            (int)((NowTime.tv_sec/3600) % 24),
            (int)((NowTime.tv_sec/60) % 60),
            (int)(NowTime.tv_sec % 60),
            (int)NowTime.tv_usec/1000, level, getpid());

    return sBuf;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_log_setlevel: set log output level
 *
 * @param[in]   loglevel    log output level
 * @return      nothing
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API void
ico_apf_log_setlevel(const int loglevel)
{
    ico_apf_log_level = loglevel & (~(ICO_APF_LOG_FLUSH|ICO_APF_LOG_NOFLUSH));

    if (loglevel & (ICO_APF_LOG_FLUSH|ICO_APF_LOG_NOFLUSH))    {
        if (loglevel & ICO_APF_LOG_FLUSH)  {
            ico_apf_log_flushmode = 1;
        }
        else    {
            ico_apf_log_flushmode = 0;
        }
    }
}

