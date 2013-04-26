/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Application Framework public header for log output
 *
 * @date    Feb-15-2013
 */

#ifndef _ICO_APF_LOG_H_
#define _ICO_APF_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Log output destination       */
#ifndef ICO_APF_LOG_STDOUT
#define ICO_APF_LOG_STDOUT      0       /* Log output to stdout(=1) or file(=0)     */
#endif /*ICO_APF_LOG_STDOUT*/
#define ICO_APF_LOG_DIR         "/var/log/ico"
                                        /* Directory name of Log destination        */

/* Maximum lines/files          */
#define ICO_APF_LOG_MAXLINES    10000   /* Maximum output lines of one log file     */
#define ICO_APF_LOG_MAXFILES    6       /* Maximum number of the log file           */

/* Log output level             */
#define ICO_APF_LOG_DEBUG       128     /* Debug write                              */
#define ICO_APF_LOG_INFO        64      /* Information                              */
#define ICO_APF_LOG_WARNING     16      /* Warning                                  */
#define ICO_APF_LOG_CRITICAL    8       /* Critical                                 */
#define ICO_APF_LOG_ERROR       4       /* Error                                    */

/* Log output flush             */
#define ICO_APF_LOG_FLUSH       0x4000  /* Log outout with log flush                */
#define ICO_APF_LOG_NOFLUSH     0x2000  /* Log output without log flush             */

/* Function prototypes          */
void ico_apf_log_print(const char *fmt, ...);
void ico_apf_log_open(const char *Prog);
void ico_apf_log_close(void);
void ico_apf_log_flush(void);
char *ico_apf_log_curtime(const char *level);
void ico_apf_log_setlevel(const int loglevel);

/* Current log output level     */
extern int  ico_apf_log_level;

/* Macros for log output        */
#define ICO_APF_DEBUG(fmt,...)  \
    {if( ico_apf_log_level >= ICO_APF_LOG_DEBUG) {ico_apf_log_print("%s> "fmt" (%s,%s:%d)\n",ico_apf_log_curtime("DBG"),##__VA_ARGS__,__func__,__FILE__,__LINE__);} }
#define ICO_APF_INFO(fmt,...)   \
    {if( ico_apf_log_level >= ICO_APF_LOG_INFO) {ico_apf_log_print("%s> "fmt" (%s,%s:%d)\n",ico_apf_log_curtime("INF"),##__VA_ARGS__,__func__,__FILE__,__LINE__);} }
#define ICO_APF_WARN(fmt,...)   \
    {if( ico_apf_log_level >= ICO_APF_LOG_WARNING) {ico_apf_log_print("%s> "fmt" (%s,%s:%d)\n",ico_apf_log_curtime("WRN"),##__VA_ARGS__,__func__,__FILE__,__LINE__);} }
#define ICO_APF_CRITICAL(fmt,...)   \
    {if( ico_apf_log_level >= ICO_APF_LOG_CRITICAL) {ico_apf_log_print("%s> "fmt" (%s,%s:%d)\n",ico_apf_log_curtime("CRI"),##__VA_ARGS__,__func__,__FILE__,__LINE__);} }
#define ICO_APF_ERROR(fmt,...)  \
    {if( ico_apf_log_level >= ICO_APF_LOG_ERROR) {ico_apf_log_print("%s> "fmt" (%s,%s:%d)\n",ico_apf_log_curtime("ERR"),##__VA_ARGS__,__func__,__FILE__,__LINE__);} }

/* Macros for compativility */
#ifndef apfw_trace
#define apfw_debug          ICO_APF_DEBUG
#define apfw_trace          ICO_APF_DEBUG
#define apfw_info           ICO_APF_INFO
#define apfw_warn           ICO_APF_WARN
#define apfw_critical       ICO_APF_CRITICAL
#define apfw_error          ICO_APF_ERROR
#define apfw_logflush       ico_apf_log_flush
#endif /*apfw_trace*/

#define uim_debug           ICO_APF_DEBUG
#define uifw_debug          ICO_APF_DEBUG
#define uifw_trace          ICO_APF_DEBUG
#define uifw_info           ICO_APF_INFO
#define uifw_warn           ICO_APF_WARN
#define uifw_critical       ICO_APF_CRITICAL
#define uifw_error          ICO_APF_ERROR
#define uifw_logflush       ico_apf_log_flush

#define ICO_UXF_DEBUG       ICO_APF_DEBUG
#define ICO_UXF_INFO        ICO_APF_INFO
#define ICO_UXF_WARN        ICO_APF_WARN
#define ICO_UXF_CRITICAL    ICO_APF_CRITICAL
#define ICO_UXF_ERROR       ICO_APF_ERROR

#define ico_uxf_log_open        ico_apf_log_open
#define ico_uxf_log_close       ico_apf_log_close
#define ico_uxf_log_flush       ico_apf_log_flush
#define ico_uxf_log_setlevel    ico_apf_log_setlevel

#ifdef __cplusplus
}
#endif
#endif  /* _ICO_APF_LOG_H_ */

