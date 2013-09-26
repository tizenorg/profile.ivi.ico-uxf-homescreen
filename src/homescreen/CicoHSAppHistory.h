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
#include <vector>
#include <utility>

class CicoHSAppHistory
{
public:
    CicoHSAppHistory();
    CicoHSAppHistory(const char* user, const char* path, const char* flagpath);
    CicoHSAppHistory(const std::string& user, const std::string& path,
                     const std::string& flagpath);
    virtual ~CicoHSAppHistory();

    // get
    const std::string& getUser() const;
    const std::string& getPath() const;
    const std::list<std::string>& getAppHistory() const;

    // operate
    virtual bool addAppHistory(const std::string& app);
    virtual bool delAppHistory(const std::string& app);
    virtual bool moveHistoryHead(const std::string& app);

    // swipe operate
    const std::string& prevSwipe();
    const std::string& nextSwipe();
    const std::string& getSwipeCurrentAppid() const;
    const std::string& homeSwipe();

    bool isFlag();
    bool force_flagoff();
    void setFilterManage(std::vector<std::string>& filter);
    void setFilterWrite(std::vector<std::string>& filter);
protected:
    bool writeAppHistory();
    bool filterChk(std::vector<std::string>& filter, const char* tgt) const;

protected:
    std::string         m_user;     // login-user name
    std::string         m_path;     // history application file path
    std::string         m_flagPath; // flag file path
    std::list<std::string>   m_appHistoryList;  // history application list
    std::vector<std::string> m_filterM;   // history manage filter string
    std::vector<std::string> m_filterW;   // history write filter string
    std::list<std::string>::iterator m_swipeCurr;
    std::string         m_swipeStr;
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
 * @brief get file path
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

/**
 * @brief set filter manage off
 * @param filter filter keywords
 */
inline void CicoHSAppHistory::setFilterManage(std::vector<std::string>& filter)
{
    m_filterM = filter;   // history manage filter string
}

/**
 * @brief set filter write off
 * @param filter filter keywords
 */
inline void CicoHSAppHistory::setFilterWrite(std::vector<std::string>& filter)
{
    m_filterW = filter;   // history write filter string
}

/**
 * @param get current swipe appid
 */
inline const std::string& CicoHSAppHistory::getSwipeCurrentAppid() const
{
    return m_swipeStr;
}

#endif // CICOHSAPPHISTORY_H
