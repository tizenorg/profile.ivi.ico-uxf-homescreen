/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   CicoSCSysResourceMonitor
 *          SystemController resource monitor
 *          CPU/FreeMemory monitoring
 *
 * @date    Jul-02-2013 create start
 */

#ifndef CICOSCSYSRESOURCEMONITOR_H
#define CICOSCSYSRESOURCEMONITOR_H

#include <sys/types.h>
#include <vector>


/**
 * /proc/stat cpu/cpuX data struct type
 */
typedef struct t_stat_data {
    int user;    // 1) Time spent in user mode.
    int nice;    // 2) Time spent in user mode with low priority (nice).
    int system;  // 3) Time spent in system mode.
    int idle;    // 4) Time spent in the idle task.
                 // This value should be USER_HZ times the second entry
                 // in the /proc/uptime pseudo-file.
    int iowait;  // 5) Time waiting for I/O to complete.
    int irq;     // 6) Time servicing interrupts.
    int softirq; // 7) Time servicing softirqs.
    int steal;   // 8) Time stolen
                 // 8) stolen time
    int guest;   // 9) Time guest OS
    int guest_nice; // 10) Time guest OS(nice)
} stat_data_t;

class CicoSCSysResourceMonitorP {
public: // member method
    CicoSCSysResourceMonitorP(pid_t pid);
    CicoSCSysResourceMonitorP(const CicoSCSysResourceMonitorP& s);





public: // member Variable
    pid_t m_pid;
    int m_utime_o;
    int m_stime_o;
    int m_vsize_o;
/*

  /proc/[pid]/stat
  01)pid, 02)comm, 03)state, 04)ppid, 05)pgrp, 06)session, 07)tty_nr, 08)tpgid
  09)flags, 10)minflt, 11)cminflt, 12)majflt, 13)cmajflt, 14)utime, 15)stime
  16)cutime, 17)cstime, 18)priority, 19)nice, 20)num_threads, 21)itrealvalue
  22)starttime, 23)vsize, 24)rss, 25)rsslim, 26)startcode, 27)endcode
  28)startstack, 29)kstkesp, 30)kstkeip, 31)signal, 32)blocked, 33)sigignore
  34)sigcatch, 35)wchan, 36)nswap, 37)cnswap, 38)exit_signal, 39)processor
  40)rt_priority, 41)policy, 42)delayacct_blkio_ticks, 43)guest_time
  44)cguest_time
*/
};





/**
 * @brief Resource monitoring
 */
class CicoSCSysResourceMonitor {
public:
    CicoSCSysResourceMonitor();
    ~CicoSCSysResourceMonitor();

    enum E_USAGETYPE{
        E_user = 0,  //  1) Time spent in user mode.
        E_nice,      //  2) Time spent in user mode with low priority (nice).
        E_system,    //  3) Time spent in system mode.
        E_idle,      //  4) Time spent in the idle task.
        E_iowait,    //  5) Time waiting for I/O to complete.
        E_irq,       //  6) Time servicing interrupts.
        E_softirq,   //  7) Time servicing softirqs.
        E_steal,     //  8) Time stolen
        E_guest,     //  9) Time Guest OS
        E_guest_nice // 10) Time guest OS(nice)
    };
    bool isAllReady() const;
    // cpu
    int  getCPUCount() const; // get number of cpu cores

                   // @param cpu -1:total 0:cpu0 1:cpu1 n:cpuN
                   // @retval 0 - 10000(retval/100 = 0.00 - 100.00%)
    int getCPUuser(int cpu=-1) const;
    int getCPUsystem(int cpu=-1) const;
    int getCPUtime(E_USAGETYPE e, int cpu=-1) const;

    // memory
    unsigned int getFreeMemSize() const; // free memory size maybe kB
    unsigned int getTotalMemSize() const; // total memory size maybe kB
    int getUsageFreeMem() const; // usage free memory

    // controller
    void monitoring();

    // process monitoring entry and remove
    void addTarget(pid_t pid);
    void removeTarget(pid_t pid);
//    std::vector<Xxxxx>& get
protected:
    void getResource();
    void getResourceCPU();
    void getResourceMEM();
private:
    int createCurrentCPU();
    void readCurrentCPU();
    int readProcStatCPU(stat_data_t* sdtp, const int cpucnt);
    bool usageCalcCPU(const stat_data_t& old, const stat_data_t& current,
                      stat_data_t& calc);
    void readProcMeminfo(bool ttlMem=false);
protected:
    int m_CPUcnt; // CPU core count
    stat_data_t* m_old; // CPU time of previous value
    stat_data_t* m_current; // CPU time of this time
    stat_data_t* m_calc; // usage CPU time
    unsigned int m_totalMem; // total memory size maybe kB
    unsigned int m_freeMem; // free memory size maybe kB
    int m_usageFreeMem;  // usage free memory
private:
    bool m_allready;
};

/**
 * @brief monitoring all ready
 * @return true:ok false:not good
 */
inline bool CicoSCSysResourceMonitor::isAllReady() const
{
    return m_allready;
}

/**
 * @brief cpu core count
 * @return cpum_usageFreeMem core count
 */
inline int CicoSCSysResourceMonitor::getCPUCount() const
{
    return m_CPUcnt;
}

/**
 * @brief user usage
 * @param cpu core cpu position -1:all 0-n:cpu position
 * @return cpu usage
 * @retval 0 - 10000 (value/100 = 0.00 - 100.00%)
 * @retval -1:get fail
 */
inline int CicoSCSysResourceMonitor::getCPUuser(int cpu) const
{
    return getCPUtime(E_user,cpu);
}

/**
 * @brief system usage
 * @param cpu core cpu position -1:all 0-n:cpu position
 * @return cpu usage
 * @retval 0 - 10000 (value/100 = 0.00 - 100.00%)
 * @retval -1:get fail
 */
inline int CicoSCSysResourceMonitor::getCPUsystem(int cpu) const
{
    return getCPUtime(E_system,cpu);
}

/**
 * @brief free memory size
 * @return free memory size maybe(kB)
 */
inline unsigned int CicoSCSysResourceMonitor::getFreeMemSize() const
{
    return m_freeMem;
}

/**
 * @brief total memory size
 * @return free memory size maybe(kB)
 */
inline unsigned int CicoSCSysResourceMonitor::getTotalMemSize() const
{
    return m_totalMem;
}

/**
 * @brief usage free memory
 * @return usage
 * @retval -1:during measurement
 * @retval 0 - 10000(value / 100 = 0.00% - 100.00%)
 */
inline int CicoSCSysResourceMonitor::getUsageFreeMem() const // usage free memory
{
    return m_usageFreeMem;
}

#endif // CICOSCSYSRESOURCEMONITOR_H
