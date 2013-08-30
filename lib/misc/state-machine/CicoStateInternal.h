/*
 * Copyright (c) 2013 TOYOTA MOTOR CORPORATION.
 *
 * Contact: 
 *
 * Licensed under the Apache License, Version 2.0 (the License){}
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. 
 */
#ifndef CICOSTATEINTERNAL_H
#define CICOSTATEINTERNAL_H

/**
 * state machine internal
 * log define
 *    LOG_CICO, LOG_CONSOLE,
 */

#include <cstdio>
#include <cstdlib>

#define LOG_TAG "STT_MCN"

#ifdef LOG_CICO
#   include "ico_log.h"
#   define _ERR(fmt, ...)  ICO_ERR(fmt, ##__VA_ARGS__)
#   define _INFO(fmt, ...) ICO_INF(fmt, ##__VA_ARGS__)
#   define _DBG(fmt, ...)  ICO_DBG(fmt, ##__VA_ARGS__)
#else // LOG_ICO
#   ifdef LOG_CONSOLE

#       define _ERR(fmt, arg...) \
            do {fprintf(stderr, "SttM: " fmt "\n", ##arg);}while (0)

#       define _INFO(fmt, arg...) \
            do {fprintf(stdout, fmt "\n", ##arg);} while (0)

#       define _DBG(fmt, arg...) \
            do { \
                if (getenv("SttM_DEBUG")) { \
                fprintf(stdout, fmt "\n",##arg); \
            }while (0)
#   else // LOG_CONSOLE
#       include <dlog.h>
#       ifdef _DLOG_H_

#           define _ERR(fmt, arg...) \
                do { \
                    fprintf(stderr, "SttM: " fmt "\n", ##arg); \
                    LOGE(fmt, ##arg); \
                }while (0)

#           define _INFO(...) LOGI(__VA_ARGS__)

#           define _DBG(...) LOGD(__VA_ARGS__)
#       else // _DLOG_H_
#           define _ERR(fmt, arg...)

#           define _INFO(fmt, arg...)

#           define _DBG(fmt, arg...)

#       endif // _DLOG_H_
#   endif // LOG_CONSOLE
#endif // LOG_ICO

#endif // CICOSTATEINTERNAL_H
