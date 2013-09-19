/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Homescreen application history sequence manager
 *
 * @date    Sep-16-2013
 */
#ifndef CICOHSAPPHISTORY_H
#define CICOHSAPPHISTORY_H

#include <string>
#include <list>

class CicoHSAppHistory
{
public:
    CicoHSAppHistory();
    CicoHSAppHistory(const char* user, const char* path);
    ~CicoHSAppHistory();

    // get
    const std::string& getUser() const;
    const std::string& getPath() const;
    const std::list<std::string>& getAppHistory() const;

    // operate
    void addAppHistory(const std::string& app);
    void delAppHistory(const std::string& app);
    void moveHistoryHead(const std::string& app);

protected:
    std::string         m_user;
    std::string         m_path;
    std::list<std::string>   m_appHistoryList;
};

/**
 * @brief get user name
 * @ret user name
 */
inline const std::string& CicoHSAppHistory::getUser() const
{
    return m_user;
}

/**
 * @breif get file path
 * @ret file path
 */
inline const std::string& CicoHSAppHistory::getPath() const
{
    return m_path;
}


/**
 * @brief get history appid list
 * @ret list<string> 
 */
inline const std::list<std::string>& CicoHSAppHistory::getAppHistory() const
{
    return m_appHistoryList;
}
#endif // CICOHSAPPHISTORY_H

