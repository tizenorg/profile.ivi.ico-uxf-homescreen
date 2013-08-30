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

#include <string>
#include "CicoLog.h"

CicoLog* CicoLog::ms_myInstance = NULL;
int  CicoLog::m_sTimeZone = 99*60*60;

CicoLog::CicoLog()
    : m_logLevel(0x7fffffff), m_flushMode(true), m_initialized(false),
      m_sDbgFd(NULL), m_sDbgLines(0)
{
    memset(&m_sDbgProg[0], 0, sizeof(m_sDbgProg));
}

CicoLog::~CicoLog()
{
    closeLog();
}

CicoLog*
CicoLog::getInstance(void)
{
    if (NULL == ms_myInstance) {
        ms_myInstance = new CicoLog();
    }
    return ms_myInstance;
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_log_print: printout log message
 *
 * @param[in]   fmt     message format(same as printf)
 * @param[in]   ...     arguments if need
 * @return      nothing
 */
/*--------------------------------------------------------------------------*/
void
CicoLog::printLog(int loglevel, const char *fmt, ...)
{
    if (m_logLevel < loglevel) {
        return;
    }
    va_list     list;

    if (! m_sDbgFd)   {
        openLog(NULL);
    }
#if ICO_APF_LOG_STDOUT == 0
    else if (m_sDbgLines >= (ICO_LOG_MAXLINES-2)) {
        if (m_sDbgLines >= ICO_LOG_MAXLINES)  {
            closeLog();
            openLog(m_sDbgProg);
        }
        else    {
            fflush(m_sDbgFd);
        }
    }
#endif /*ICO_APF_LOG_STDOUT*/
    if (m_sDbgFd) {
        va_start(list, fmt);
        vfprintf(m_sDbgFd, fmt, list);
        va_end(list);
        if (m_flushMode)  {
            fflush(m_sDbgFd);
        }
    }
    if (m_sDbgFd != stdout)   {
        m_sDbgLines ++;
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   open: open log file
 *
 * @param[in]   prog    program name
 * @return      nothing
 */
/*--------------------------------------------------------------------------*/
void
CicoLog::openLog(const char *prog)
{
#if ICO_LOG_STDOUT == 0
    int     idx;
    char    sPath[128];
    char    sPath2[128];
#endif /*ICO_LOG_STDOUT*/

    if (m_sDbgFd) {
        fflush(m_sDbgFd);
        if (m_sDbgFd != stdout)   {
            fclose(m_sDbgFd);
        }
    }

    m_sDbgLines = 0;

    if ((! prog) || (*prog == 0))   {
        m_sDbgFd = stdout;
        m_sDbgProg[0] = 0;
        return;
    }
    else    {
        strncpy(m_sDbgProg, prog, sizeof(m_sDbgProg)-1);
        m_sDbgProg[sizeof(m_sDbgProg)-1] = 0;
    }
#if ICO_LOG_STDOUT > 0
    m_sDbgFd = stdout;
#else  /*ICO_LOG_STDOUT*/
    snprintf(sPath, sizeof(sPath)-1, "%s/%s.log%d",
             ICO_LOG_DIR, m_sDbgProg, ICO_LOG_MAXFILES-1);
    (void) remove(sPath);

    for (idx = (ICO_LOG_MAXFILES-1); idx > 0; idx--)    {
        strcpy(sPath2, sPath);
        if (idx > 1)    {
            snprintf(sPath, sizeof(sPath)-1, "%s/%s.log%d",
                     ICO_LOG_DIR, m_sDbgProg, idx-1);
        }
        else    {
            snprintf(sPath, sizeof(sPath)-1, "%s/%s.log",
                     ICO_LOG_DIR, m_sDbgProg);
        }
        (void) rename(sPath, sPath2);
    }
    m_sDbgFd = fopen(sPath, "w");
    if (! m_sDbgFd)   {
        m_sDbgFd = stdout;
    }
    else if ((m_initialized == false) &&
             (m_sDbgFd != stdout) && (m_sDbgFd != stderr))  {
        m_initialized = true;
        fflush(stdout);
        fflush(stderr);
        stdout = m_sDbgFd;
        stderr = m_sDbgFd;
    }
#endif /*ICO_LOG_STDOUT*/
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   closeLog: close log file
 *
 * @param       nothing
 * @return      nothing
 */
/*--------------------------------------------------------------------------*/
void
CicoLog::closeLog(void)
{
#if ICO_LOG_STDOUT == 0
    if (m_sDbgFd) {
        fflush(m_sDbgFd);
        if (m_sDbgFd != stdout)   {
            fclose(m_sDbgFd);
        }
        m_sDbgFd = (FILE *)0;
    }
#endif /*ICO_LOG_STDOUT*/
}

/*--------------------------------------------------------------------------*/
/**
 * @brief  flushLog: flush log file
 *
 * @param       nothing
 * @return      nothing
 */
/*--------------------------------------------------------------------------*/
void
CicoLog::flushLog(void)
{
    if ((m_sDbgFd != NULL) && (m_flushMode == 0))   {
        fflush(m_sDbgFd);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   getStrCurtime: create current time string
 *
 * @param[in]   level   log level string(header of log message)
 * @return      current time string
 */
/*--------------------------------------------------------------------------*/
char *
CicoLog::getStrCurTime(const char *level)
{
    struct timeval  NowTime;
    extern long     timezone;
    static char     sBuf[28];

    gettimeofday(&NowTime, (struct timezone *)0);
    if (m_sTimeZone > (24*60*60)) {
        tzset();
        m_sTimeZone = timezone;
    }
    NowTime.tv_sec -= m_sTimeZone;

    sprintf(sBuf, "%02d:%02d:%02d.%03d[%s]@%d",
            (int)((NowTime.tv_sec/3600) % 24),
            (int)((NowTime.tv_sec/60) % 60),
            (int)(NowTime.tv_sec % 60),
            (int)NowTime.tv_usec/1000, level, getpid());

    return sBuf;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   setLogLevel: set log output level
 *
 * @param[in]   loglevel    log output level
 * @return      nothing
 */
/*--------------------------------------------------------------------------*/
void
CicoLog::setLogLevel(const int loglevel)
{
    m_logLevel = loglevel & (~(ICO_LOG_FLUSH|ICO_LOG_NOFLUSH));

    if (m_logLevel & (ICO_LOG_FLUSH|ICO_LOG_NOFLUSH))    {
        if (m_logLevel & ICO_LOG_FLUSH)  {
            m_flushMode = true;
        }
        else    {
            m_flushMode = false;
        }
    }
}
// vim:set expandtab ts=4 sw=4:
