/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Homescreen application aul handler multi support
 *
 * @date    Sep-23-2013
 */
#ifndef CICO_AUL_LISTEN_APP_H
#define CICO_AUL_LISTEN_APP_H

int aul_listen_app_launch_signal_add(int (*func) (int, void *), void *data);
int aul_listen_app_dead_signal_add(int (*func) (int, void *), void *data);
void initAulListenXSignal();

#endif
