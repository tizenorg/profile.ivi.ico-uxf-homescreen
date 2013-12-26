/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#ifndef _CICOLIBSTRING_H_
#define _CICOLIBSTRING_H_
#include <string>
#include <vector>

/**
 * @brief split string
 * @param s split target
 * @param d
 * @param e store split word
 */
void split(const std::string &s, char d, std::vector<std::string> &e);

/**
 * @brief trim
 * @param s target string
 * @param trim charactor
 * @ret   trim string
 */
std::string trim(const std::string& s, const char* t = " \t\v\r\n");

/**
 * @brief directory last slash add
 * @param s directory path string
 */
void chkAndAddSlash(string& s);

#endif
