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

#ifndef __CICO_LOG_H__
#define __CICO_LOG_H__

#include    <stdio.h>
#include    <stdlib.h>
#include    <stdarg.h>
#include    <unistd.h>
#include    <string.h>
#include    <sys/time.h>
#include    <sys/types.h>
#include    <time.h>

#ifndef ICO_LOG_STDOUT
#define ICO_LOG_STDOUT      0       /* Log output to stdout(=1) or file(=0)     */
#endif /*ICO_APF_LOG_STDOUT*/

#define ICO_LOG_DIR         "/var/log/ico"
                                        /* Directory name of Log destination        */

/* Maximum lines/files          */
#define ICO_LOG_MAXLINES    10000   /* Maximum output lines of one log file     */
#define ICO_LOG_MAXFILES    6       /* Maximum number of the log file           */

/* Log output level             */
#define ICO_LOG_DEBUG       128     /* Debug write                              */
#define ICO_LOG_INFO        64      /* Information                              */
#define ICO_LOG_WARNING     16      /* Warning                                  */
#define ICO_LOG_CRITICAL    8       /* Critical                                 */
#define ICO_LOG_ERROR       4       /* Error                                    */

/* Log output flush             */
#define ICO_LOG_FLUSH       0x4000  /* Log outout with log flush                */
#define ICO_LOG_NOFLUSH     0x2000  /* Log output without log flush             */

#define ICO_TRA(fmt,...)                                            \
{                                                                   \
    CicoLog::getInstance()->printLog(ICO_LOG_DEBUG,                 \
                                     "%s> " fmt " (%s,%s:%d)\n",    \
                                     CicoLog::getStrCurTime("DBG"), \
                                     ##__VA_ARGS__,                 \
                                     __func__,                      \
                                     __FILE__,                      \
                                     __LINE__);                     \
}

#define ICO_DBG(fmt,...)                                            \
{                                                                   \
    CicoLog::getInstance()->printLog(ICO_LOG_DEBUG,                 \
                                     "%s> " fmt " (%s,%s:%d)\n",    \
                                     CicoLog::getStrCurTime("DBG"), \
                                     ##__VA_ARGS__,                 \
                                     __func__,                      \
                                     __FILE__,                      \
                                     __LINE__);                     \
}

#define ICO_INF(fmt,...)                                            \
{                                                                   \
    CicoLog::getInstance()->printLog(ICO_LOG_INFO,                  \
                                     "%s> " fmt " (%s,%s:%d)\n",    \
                                     CicoLog::getStrCurTime("INF"), \
                                     ##__VA_ARGS__,                 \
                                     __func__,                      \
                                     __FILE__,                      \
                                     __LINE__);                     \
}

#define ICO_WRN(fmt,...)                                            \
{                                                                   \
    CicoLog::getInstance()->printLog(ICO_LOG_WARNING,               \
                                     "%s> " fmt " (%s,%s:%d)\n",    \
                                     CicoLog::getStrCurTime("WRN"), \
                                     ##__VA_ARGS__,                 \
                                     __func__,                      \
                                     __FILE__,                      \
                                     __LINE__);                     \
}

#define ICO_CRI(fmt,...)                                            \
{                                                                   \
    CicoLog::getInstance()->printLog(ICO_LOG_CRITICAL,              \
                                     "%s> " fmt " (%s,%s:%d)\n",    \
                                     CicoLog::getStrCurTime("CRI"), \
                                     ##__VA_ARGS__,                 \
                                     __func__,                      \
                                     __FILE__,                      \
                                     __LINE__);                     \
}

#define ICO_ERR(fmt,...)                                            \
{                                                                   \
    CicoLog::getInstance()->printLog(ICO_LOG_ERROR,                 \
                                     "%s> " fmt " (%s,%s:%d)\n",    \
                                     CicoLog::getStrCurTime("ERR"), \
                                     ##__VA_ARGS__,                 \
                                     __func__,                      \
                                     __FILE__,                      \
                                     __LINE__);                     \
}

class CicoLog
{
public:
    CicoLog();

    ~CicoLog();

    static CicoLog* getInstance(void);

    void printLog(int loglevel, const char *fmt, ...);

    void openLog(const char *prog);

    void closeLog(void);

    void flushLog(void);

    void setLogLevel(const int loglevel);

    static char * getStrCurTime(const char *level);

private:
    static CicoLog* ms_myInstance;  //!< CicoLog Object
    static int  m_sTimeZone;               //!< local time difference(sec)
    int  m_logLevel;                //!< output level debug log
    bool m_flushMode;               //!< flush mode flag
    bool m_initialized;             //!< initialized flag
    FILE *m_sDbgFd;                 //!< file descriptor of output debug log
    int  m_sDbgLines;               //!< output lines
    char m_sDbgProg[32];            //!< name of output source module
};
#endif  // __CICO_LOG_H__
// vim:set expandtab ts=4 sw=4:
