/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#ifndef CICOSCAULITEMS_H
#define CICOSCAULITEMS_H

#include <string>
#include <vector>

#ifndef __CICO_SC_WINDOW_H__
class CicoSCWindow;
#endif


/**
 * @brief System controller unit
 *        application life cycle information data
 *
 */

class CicoSCAulItems {
public: // member method
    CicoSCAulItems();
    CicoSCAulItems(const char* appid, int pid, int ctgry,
                   const CicoSCWindow* obj=NULL);
    CicoSCAulItems(const CicoSCAulItems& raul);
    ~CicoSCAulItems();

    const CicoSCAulItems* p() const;
    void enterWindow(const CicoSCWindow* obj);
    void rmWindow(const CicoSCWindow* obj);
protected: // member method
    bool getPidCgroupInfo(int pid, std::string& m, std::string& c);
private: // member method

public: // member aria
    std::string m_appid; // appid
    int m_pid; // pid
    int m_category; //
    int m_cpucgroup;
    int m_memcgroup;
    std::vector<const CicoSCWindow*> m_CSCWptrs;

protected: // member aria
    std::string m_defCgrpCpu;
    std::string m_defCgrpMem;

#if 0 // TODO mk_k
//    std::string m_icon; // icon file path
    bool m_active; //
    bool m_supend; //
    std::string m_group;
    int  m_nodeID;
    std::string m_location;
    std::string m_animation;
    int m_animationTime;
    bool m_autoStart; // auto start flag
    bool m_memnuOverlap;
    int  m_invisibleCPU;
    int  m_kindID;
    int  m_categoryID;
    int  m_displayZoneNum; // display zone number
    int  m_soundZoneNum;  // sound zone number
    int  m_inputDevNum;   // input device number
    int  m_display;
    int  m_sound;
    int  m_input;



    std::string m_name;
    std::string m_execPath;
    std::string m_type;
    bool m_resumeShow
#endif
};

inline const CicoSCAulItems* CicoSCAulItems::p() const
{
    return this;
}

#endif // CICOSCAULITEMS_H
