/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#ifndef CICOAULITEMS_H
#define CICOAULITEMS_H

#include <string>
#include <vector>

/**
 * @brief System controller unit
 *        application life cycle information data
 *
 */

class CicoAulItems {
public: // member method
    CicoAulItems();
    CicoAulItems(const char* appid, int pid, int ctgry, int aulstt,
                   const void* obj=NULL);
    CicoAulItems(const CicoAulItems& raul);
    ~CicoAulItems();

    const CicoAulItems* p() const;
    void enterWindow(const void* obj);
    void rmWindow(const void* obj);
    void update_appid();
protected: // member method
    bool getPidCgroupInfo(int pid, std::string& m, std::string& c);
private: // member method

public: // member aria
    std::string m_appid; // appid
    int m_pid; // pid
    int m_category; //
    int m_cpucgroup;
    int m_memcgroup;
    std::vector<const void*> m_CSCWptrs;

protected: // member aria
    std::string m_defCgrpCpu;
    std::string m_defCgrpMem;
    int m_aulstt;

};

inline const CicoAulItems* CicoAulItems::p() const
{
    return this;
}

#endif // CICOSCAULITEMS_H
// vim:set expandtab ts=4 sw=4:
