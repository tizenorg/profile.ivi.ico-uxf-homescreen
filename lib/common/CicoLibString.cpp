/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include <cstdlib>

using namespace std;

/**
 * @brief split string
 * @param s split target
 * @param d
 * @param e store split word
 */
void split(const std::string &s, char d, std::vector<std::string> &e)
{
    std::stringstream ss(s);
    std::string it;
    while(getline(ss, it, d)) {
        e.push_back(it);
    }
    return;
}

/**
 * @brief trim
 * @param s target string
 * @param trim charactor
 * @ret   trim string
 */
std::string trim(const std::string& s, const char* t)
{
    std::string r;
    std::string::size_type left = s.find_first_not_of(t);
    if (left != std::string::npos) {
        std::string::size_type right = s.find_last_not_of(t);
        r = s.substr(left, right - left + 1);
    }
    return r;
}

/**
 * @brief directory last slash add
 * @param s directory path string
 */
void chkAndAddSlash(std::string& s)
{
    int sz = s.size();
    const char* p = s.c_str();
    if ('/' != p[sz-1]) {
        s += "/";
    }
}

