/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <cstdlib>
#include <cstring>
#include "CicoLog.h"
#include "CicoSCSysResourceMonitor.h"
using namespace std;
extern bool g_RC_LOG;

const char* rdTgtCPU = "/proc/stat";
const char sP1C = 'c';
const char sP2P = 'p';
const char sP3U = 'u';
const int nDmyCpuCnt = 17;   // CPU(16) + Total(1)

const double dCarryUp = 10000;

const char* rdTgtMEM = "/proc/meminfo";
const string rdMEMTag1 = "MemFree:";
const string rdMEMTag2 = "MemTotal:";

/**
 * @brief Constructor
 */
CicoSCSysResourceMonitor::CicoSCSysResourceMonitor()
{
    ICO_TRA("start constructor");
    m_CPUcnt = 0;
    m_old = (stat_data_t*)0;
    m_current = (stat_data_t*)0;
    m_calc = (stat_data_t*)0;
    m_totalMem = 0;
    m_freeMem = 0;
    m_usageFreeMem = -1;
    m_allready = false;
    ICO_TRA("end constructor");
}

/**
 * @brief destructor
 */
CicoSCSysResourceMonitor::~CicoSCSysResourceMonitor()
{
    ICO_TRA("start destructor");
    if ((stat_data_t*)0 != m_old) {
        delete[] m_old;
        m_old = (stat_data_t*)0;
    }
    if ((stat_data_t*)0 != m_current) {
        delete[] m_current;
        m_current = (stat_data_t*)0;
    }
    if ((stat_data_t*)0 != m_calc) {
        delete[] m_calc;
        m_calc = (stat_data_t*)0;
    }
    ICO_TRA("end destructor");
}

/**
 * @brief system usage
 * @param cpu core cpu position -1:all 0-n:cpu position
 * @return cpu usage(return value/100 = 0.00 - 100.00%)
 *         -1:get fail
 */
int CicoSCSysResourceMonitor::getCPUtime(E_USAGETYPE e, int cpu) const
{
    if (g_RC_LOG) ICO_TRA("start %d, %d", e, cpu);
    if (false == isAllReady()) {
        ICO_TRA("is no ready!");
        return -1;
    }
    int n = cpu + 1;
    if ((0 > n) || (m_CPUcnt <= n)) {
        if (g_RC_LOG) ICO_TRA("end cpu count error %d, %d", n, m_CPUcnt);
        return -1;
    }
    int r = -1;
    switch(e) {
    case E_user:       // 1) Time spent in user mode.
        r = m_calc[n].user;
        break;
    case E_nice:       // 2) Time spent in user mode with low priority (nice).
        r = m_calc[n].nice;
        break;
    case E_system:     // 3) Time spent in system mode.
        r = m_calc[n].system;
        break;
    case E_idle:       // 4) Time spent in the idle task.
        r = m_calc[n].idle;
        break;
    case E_iowait:     // 5) Time waiting for I/O to complete.
        r = m_calc[n].iowait;
        break;
    case E_irq:        // 6) Time servicing interrupts.
        r = m_calc[n].irq;
        break;
    case E_softirq:    // 7) Time servicing softirqs.
        r = m_calc[n].softirq;
        break;
    case E_steal:      // 8) Time stolen
        r = m_calc[n].steal;
        break;
    case E_guest:      // 9) Time Guest OS
        r = m_calc[n].guest;
        break;
    case E_guest_nice: // 10) Time guest OS(nice)
        r = m_calc[n].guest_nice;
        break;
    default:
        break;
    }
    if (g_RC_LOG) ICO_TRA("end success ret(%d)", r);
    return r;
}

/**
 * @brief monitoring
 */
void CicoSCSysResourceMonitor::monitoring()
{
    if (g_RC_LOG) ICO_TRA("call getResource");
    getResource();
}

/**
 * @brief getResource
 *        CPU usage and free memory size get
 */
void CicoSCSysResourceMonitor::getResource()
{
    if (g_RC_LOG) ICO_TRA("call get resource CPU,MEM");
    getResourceCPU();
    getResourceMEM();
}

/**
 * @brief getResourceCPU
 */
void CicoSCSysResourceMonitor::getResourceCPU()
{
    if (0 == m_CPUcnt) {
        m_CPUcnt = createCurrentCPU();
        size_t sz = m_CPUcnt + 1;
        if (0 == m_old) {
            m_old = new stat_data_t[sz];
        }
        if (0 == m_calc) {
            m_calc = new stat_data_t[sz];
        }
        ICO_DBG("cpu count is %d", m_CPUcnt);
        return;
    }
    if (((stat_data_t*)0 == m_old) || ((stat_data_t*)0 == m_current) || 
        ((stat_data_t*)0 == m_calc)) {
        ICO_DBG("error none new area");
        return;
    }
    // set old value
    size_t sz = m_CPUcnt + 1;
    memcpy(m_old, m_current, sizeof(stat_data_t)*sz);
    // read current value
    readCurrentCPU();
    // calc CPU usage
    for (size_t i=0; i < sz; i++) {
        usageCalcCPU(m_old[i], m_current[i], m_calc[i]);
    }
    m_allready = true;
    if (g_RC_LOG) ICO_TRA("calc");
}

/**
 * @brief CPU usage first read
 *        area allocation and read
 *        CPU core count
 * @return CPU core count
 */
int CicoSCSysResourceMonitor::createCurrentCPU()
{
    if (0 != m_current) {
        ICO_TRA("ret(0) current is zero!");
        return 0;
    }
    int cnt = 0;
    stat_data_t tmp[nDmyCpuCnt];
    memset(tmp, 0, sizeof(tmp));
    cnt = readProcStatCPU(tmp, nDmyCpuCnt-1);
    if (0 == cnt) {
        ICO_TRA("ret(0) cnt is zero!");
        return 0;
    }
    size_t sz = cnt + 1;
    m_current = new stat_data_t[sz];
    memcpy(m_current, tmp, sizeof(stat_data_t)*sz);
    ICO_TRA("ret cpu core count(%d)", cnt);
    return cnt;
}

/**
 * @brief CPU usage read
 */
void CicoSCSysResourceMonitor::readCurrentCPU()
{
    readProcStatCPU(m_current, m_CPUcnt);
}

/**
 * @brief read /proc/stat CPU information
 * @param sdtp destination read value
 * @param cpucnt CPU core count
 * @return read CPU count
 */
int CicoSCSysResourceMonitor::readProcStatCPU(stat_data_t* sdtp, const int cpucnt)
{
    int r = 0;
    int sztmp = cpucnt;
    string tag;
    ifstream ifs(rdTgtCPU);
    while(ifs >> tag) {
        int tgt=-1;
        const char* tags = tag.c_str();
        if ((sP1C == tags[0]) && (sP2P == tags[1]) && (sP3U == tags[2])) {
            if ('\0' == tags[3]) { // "cpu"
                tgt = 0;
            }
            else { // "cpuX" X = 0 - 9
                if (0 != isdigit(tags[3])) {
                    int tmp_tgt = atoi(&tags[3]);
                    if ((0 <= tmp_tgt) && (tmp_tgt < cpucnt)) {
                        tgt = tmp_tgt + 1;
                    }
                }
            }
        }
        if (-1 != tgt) {
            // get now value
            ifs >> sdtp[tgt].user;
            ifs >> sdtp[tgt].nice;
            ifs >> sdtp[tgt].system;
            ifs >> sdtp[tgt].idle;
            ifs >> sdtp[tgt].iowait;
            ifs >> sdtp[tgt].irq;
            ifs >> sdtp[tgt].softirq;
            ifs >> sdtp[tgt].steal;
            ifs >> sdtp[tgt].guest;
            ifs >> sdtp[tgt].guest_nice;
            if (0 != tgt) {
                sztmp--;
                r++;
            }
        }
        if (0==sztmp) {
            break;    // break of while
        }
    }
    ifs.close();
    if (g_RC_LOG) ICO_TRA("ret(%d)", r);
    return r;
}

/**
 * @brief CPU usage calc
 * @param old previous value
 * @param current value of this time
 * @param calc distnation calculated value
 * @return true:calc success false:calc fail
 */
bool CicoSCSysResourceMonitor::usageCalcCPU(const stat_data_t& old,
                                     const stat_data_t& current,
                                     stat_data_t& calc)
{
    if (g_RC_LOG) ICO_TRA("start");
    double ttl = 0;
    double duser = current.user - old.user;
    ttl += duser;
    double dnice = current.nice - old.nice;
    ttl += dnice;
    double dsystem = current.system - old.system;
    ttl += dsystem;
    double didle = current.idle - old.idle;
    ttl += didle;
    double diowait = current.iowait - old.iowait;
    ttl += diowait;
    double dirq = current.irq - old.irq;
    ttl += dirq;
    double dsoftirq = current.softirq - old.softirq;
    ttl += dsoftirq;
    double dsteal = current.steal - old.steal;
    ttl += dsteal;
    double dguest = current.guest - old.guest;
    ttl += dguest;
    double dguest_nice = current.guest_nice - old.guest_nice;
    ttl += dguest_nice;
    if (0 == ttl) { // must not be divided by ZERO
        if (g_RC_LOG) ICO_TRA("end divide zero!?");
        return false;
    }
    calc.user = (int)(duser / ttl * dCarryUp);
    calc.nice = (int)(dnice / ttl * dCarryUp);
    calc.system = (int)(dsystem / ttl * dCarryUp);
    calc.idle = (int)(didle / ttl * dCarryUp);
    calc.iowait = (int)(diowait / ttl * dCarryUp);
    calc.irq = (int)(dirq / ttl * dCarryUp);
    calc.softirq = (int)(dsoftirq / ttl * dCarryUp);
    calc.steal = (int)(dsteal / ttl * dCarryUp);
    calc.guest = (int)(dguest / ttl * dCarryUp);
    calc.guest_nice = (int)(dguest_nice / ttl * dCarryUp);
    if (g_RC_LOG) ICO_TRA("end");
    return true;
}

/**
 * @brief get resource free memory
 */
void CicoSCSysResourceMonitor::getResourceMEM()
{
    if (g_RC_LOG) ICO_TRA("start");
    if (0 >= m_totalMem) {
        readProcMeminfo(true);
    }
    else {
        readProcMeminfo();
    }
    if (g_RC_LOG) ICO_TRA("end");
}

/**
 * @brief read /proc/meminfo
 * @param ttlMem total memory read flag
 */
void CicoSCSysResourceMonitor::readProcMeminfo(bool ttlMem)
{
#if 0
    if (g_RC_LOG) ICO_TRA("start %s", ttlMem? "true": "false");
    string tag;
    ifstream ifs(rdTgtMEM);
    bool tgt1 = false;
    bool tgt2 = ttlMem;       // total memory get flag set
    while(ifs >> tag) {
        if (true == tgt2) {   // flag on
            if (rdMEMTag2 == tag) { // is total memory tag?
                ifs >> m_totalMem; // total memory size get
                tgt2 = false; // flag off
            }
        }
        if (rdMEMTag1 == tag) { // is free memory tag
            ifs >> m_freeMem; // get free memory size
            tgt1 = true;  // get flag on
        }
        if ((true == tgt1) && (false == tgt2)) {
            break;   // break of while
        }
    }
    ifs.close();
    if (0 != m_totalMem) {
        if (true == tgt1) {
            m_usageFreeMem = (int)
                (((double)m_freeMem / (double)m_totalMem) * 10000);
        }
    }
    else {
        m_usageFreeMem = -1;
    }
    if (g_RC_LOG) ICO_TRA("end usage free mem st.(%d)", m_usageFreeMem);
#else 
    m_usageFreeMem = -1;
    return;
#endif
}
