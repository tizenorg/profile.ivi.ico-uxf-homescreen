/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   CicoSCSysResourceController
 *          SystemController resource watcher
 *          CPU/FreeMemory monitoring
 *
 * @date    Aug-05-2013 create start
 */

#ifndef CICOSCSYSRESOURCECONTROLLER_H
#define CICOSCSYSRESOURCECONTROLLER_H
#include <cstddef>

#include "CicoStateAction.h"

#ifndef CICOSTATEMACHINE_H
class CicoStateMachine;
#endif

#ifndef CICOSCSYSRESOURCEMONITOR_H
class CicoSCSysResourceMonitor;
#endif

#ifndef __CICO_SC_CONF_H__
class CicoSCResourceConf;
#endif

#define D_STRtasks      "tasks"
#define D_STRcpushares  "cpu.shares"

class CicoSCSysResourceController;

/**
 * @brief Class ico System Resource Controller CPU LOW level
 */
class CicoSRCCPU_LOW : public CicoStateAction
{
public: // member method
    CicoSRCCPU_LOW(CicoSCSysResourceController* obj);
    virtual ~CicoSRCCPU_LOW();

    virtual void onEntry(const CicoEvent& ev, const CicoState* stt, int addval);
    virtual void onDo(const CicoEvent& ev, const CicoState* stt, int addval);
    virtual void onExit(const CicoEvent& ev, const CicoState* stt,
                        int addval);
protected: // member Variable
    int m_val;      // cpu or memory usage value
    int m_baseVal;  // cpu or memory base usage value
    int m_cnt;
    CicoSCSysResourceController* m_cntlr;
    const CicoSCResourceConf* m_rConf;
};

/**
 * @brief Class ico System Resource Controller CPU HIGH level
 */
class CicoSRCCPU_HIGH : public CicoSRCCPU_LOW
{
public: // member method
    CicoSRCCPU_HIGH(CicoSCSysResourceController* obj);
    virtual ~CicoSRCCPU_HIGH();
    void onDo(const CicoEvent& ev, const CicoState* stt, int addval);

protected: // member Variable
};

/**
 * @brief Class ico System Resource Controller MEMory LOW level
 */
class CicoSRCMEM_LOW : public CicoSRCCPU_LOW
{
public: // member method
    CicoSRCMEM_LOW(CicoSCSysResourceController* obj);
    virtual ~CicoSRCMEM_LOW();
    void onDo(const CicoEvent& ev, const CicoState* stt, int addval);
protected: // member Variable
};

/**
 * @brief Class ico System Resource Controller MEMory HIGH level
 */
class CicoSRCMEM_HIGH : public CicoSRCCPU_LOW
{
public: // member method
    CicoSRCMEM_HIGH(CicoSCSysResourceController* obj);
    virtual ~CicoSRCMEM_HIGH();
    void onDo(const CicoEvent& ev, const CicoState* stt, int addval);
protected: // member Variable
};

/**
 * @brief System Controller System Resource Controller class
 */
class CicoSCSysResourceController
{
public:
    CicoSCSysResourceController();
    ~CicoSCSysResourceController();

    bool isWactchAllReady() const;

    bool startSysResource(); // start cgroup controll

    bool watch();   // watch request (please interval call)
    const CicoSCSysResourceMonitor& getMonitor() const;
    int getCpuUsge() const;
    int getMemoryUsge() const;

    void resourceControlCPUShares(bool bHight);
    void resourceControlMemLimit(bool bHight);
    bool entryCgroupCPU(int pid, int grpNo);
protected:
    void init_cgroup();
    void init_cgroup_cpu();
    void init_cgroup_memory();
    void make_directorys(std::vector<std::string> dir);
    bool replaceCgroupFile(const std::string& tgt, int val);
    bool replaceCgroupFile(const std::string& tgt, const char* val);

protected:
    CicoSCSysResourceMonitor* m_monitor;
    const CicoSCResourceConf* m_rConf;
    std::vector<std::string> m_RCCpuTasks;
    std::vector<std::string> m_RCCpuShares;
    bool m_bDoIt;
private:
    CicoStateMachine* m_stt;

    CicoSRCCPU_LOW* m_SRCCPU_LOW;
    CicoSRCCPU_HIGH* m_SRCCPU_HIGH;
    CicoSRCMEM_LOW* m_SRCMEM_LOW;
    CicoSRCMEM_HIGH* m_SRCMEM_HIGH;

    int m_cpu;
    int m_mem;
    int m_cgrpCPU;
    int m_cgrpMEM;
};

/**
 * @brief
 * @return
 */
inline
bool CicoSCSysResourceController::isWactchAllReady() const
{
    if ((-1 != m_cpu) && (-1 != m_mem)) {
        return true;
    }
    return false;
}

/**
 * @brief get monitor class
 * return monitor class
 */
inline const CicoSCSysResourceMonitor&
    CicoSCSysResourceController::getMonitor() const
{
    return *m_monitor;
}

/**
 * @brief get CPU usage
 * @return CPU usage
 */
inline int CicoSCSysResourceController::getCpuUsge() const
{
    return m_cpu;
}

/**
 * @brief get CPU usage
 * @return CPU usage
 */
inline int CicoSCSysResourceController::getMemoryUsge() const
{
    return m_mem;
}

#endif // CICOSCSYSRESOURCECONTROLLER_H
