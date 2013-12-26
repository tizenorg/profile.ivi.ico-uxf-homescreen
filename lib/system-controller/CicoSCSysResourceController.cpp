/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#include <memory>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "CicoLog.h"
#include "CicoEvent.h"
#include "CicoEventInfo.h"
#include "CicoStateCore.h"
#include "CicoState.h"
#include "CicoHistoryState.h"
#include "CicoFinalState.h"
#include "CicoStateMachine.h"
#include "CicoStateMachineCreator.h"
#include "CicoSCSysResourceMonitor.h"
#include "CicoSCLifeCycleController.h"
#include "CicoSCSysResourceController.h"
#include "CicoConf.h"
#include "CicoSystemConfig.h"
#include <Ecore.h>
//#include <Ecore_Evas.h>

const int gEV1000 = 1000; // CPU event
const int gEV1001 = 1001; // MEMORY event

const string gstrSRCCPU_LOW  = "SRCCPU_LOW";
const string gstrSRCCPU_HIGH = "SRCCPU_HIGH";
const string gstrSRCMEM_LOW  = "SRCMEM_LOW";
const string gstrSRCMEM_HIGH = "SRCMEM_HIGH";

static const string gstrStateMachineFileName = "CicoSCSysResourceSM.json";

bool g_RC_LOG = false;
int  g_samplingWaitTimer = 200;

Ecore_Timer* g_SCResourceWatch = 0;
static Eina_Bool ico_SCResourceWatch(void *data)
{
    if (g_RC_LOG) ICO_TRA("start data(%x)", data);
    if ((NULL == data) || (0 == data)) {
        if (g_RC_LOG) ICO_TRA("end");
        return ECORE_CALLBACK_RENEW;
    }
    CicoSCSysResourceController* oCSCSRC = (CicoSCSysResourceController*)data;
    oCSCSRC->watch();
//    double tVal = (double)g_samplingWaitTimer / 100;
//    ecore_timer_add(g_samplingWaitTimer, ico_SCResourceWatch, data);
    if (g_RC_LOG) ICO_TRA("end");
    return ECORE_CALLBACK_RENEW;
}


/**
 * @brief constructor
 */
CicoSCSysResourceController::CicoSCSysResourceController()
{
    ICO_TRA("start");
    m_cgrpCPU = 0;
    m_cgrpMEM = 0;
    m_cpu = -1;
    m_mem = -1;
    m_bDoIt = false;
    m_rConf = CicoSystemConfig::getInstance()->getResourceConf();
    if (0 != m_rConf) {
        g_RC_LOG = m_rConf->m_bLog;
        if (-1 != m_rConf->m_sampling) {
            g_samplingWaitTimer = m_rConf->m_sampling;
        }
    }
    // cpu,cpuacct cgroup init
    ICO_TRA("end");
}

/**
 * @brief destructor
 */
CicoSCSysResourceController::~CicoSCSysResourceController()
{
    ICO_TRA("start");
    if (0 != g_SCResourceWatch) {
        ecore_timer_del(g_SCResourceWatch);
    }

    if ((CicoStateMachine*)0 != m_stt) {
        m_stt->stop();
        delete m_stt;
        m_stt = (CicoStateMachine*)0;
        ICO_TRA("delete CicoStateMachine");
    }

    if ((CicoSRCCPU_LOW*)0 != m_SRCCPU_LOW) {
        delete m_SRCCPU_LOW;
        m_SRCCPU_LOW = (CicoSRCCPU_LOW*)0;
        ICO_TRA("delete CicoSRCCPU_LOW");
    }

    if ((CicoSRCCPU_HIGH*)0 != m_SRCCPU_HIGH) {
        delete m_SRCCPU_HIGH;
        m_SRCCPU_HIGH = (CicoSRCCPU_HIGH*)0;
        ICO_TRA("delete CicoSRCCPU_HIGH");
    }

    if ((CicoSRCMEM_LOW*)0 != m_SRCMEM_LOW) {
        delete m_SRCMEM_LOW;
        m_SRCMEM_LOW = (CicoSRCMEM_LOW*)0;
        ICO_TRA("delete CicoSRCMEM_LOW");
    }

    if ((CicoSRCMEM_HIGH*)0 != m_SRCMEM_HIGH) {
        delete m_SRCMEM_HIGH;
        m_SRCMEM_HIGH = (CicoSRCMEM_HIGH*)0;
        ICO_TRA("delete CicoSRCMEM_HIGH");
    }

    if ((CicoSCSysResourceMonitor*)0 != m_monitor) {
        delete m_monitor;
        m_monitor = (CicoSCSysResourceMonitor*)0;
        ICO_TRA("delete CicoSCSysResourceMonitor");
    }
    ICO_TRA("end");
}

/**
 * @brief startWatch
 */
bool CicoSCSysResourceController::startSysResource()
{
    ICO_TRA("start");
    if (0 == m_rConf) {
        ICO_TRA("end");
        return false;
    }
    m_bDoIt = m_rConf->m_bDoIt;
    if (false == m_bDoIt) {
        ICO_TRA("end");
        return false;
    }
    // cpu,cpuacct cgroup init
    init_cgroup();  // cgroup 
    m_monitor = new CicoSCSysResourceMonitor();
    CicoSystemConfig* CSCSC = CicoSystemConfig::getInstance();
    string filePath(CSCSC->getDefaultConf()->confdir);
    filePath += string("/");
    filePath += gstrStateMachineFileName;

    CicoStateMachineCreator csmc;
    m_stt = csmc.createFile(filePath);
    if ((CicoStateMachine*)0 == m_stt) {
        ICO_ERR("state machine create error(%s)", filePath.c_str());
        ICO_TRA("end");
        return false;
    }

    m_SRCCPU_LOW  = new CicoSRCCPU_LOW(this);
    m_SRCCPU_HIGH = new CicoSRCCPU_HIGH(this);
    m_SRCMEM_LOW  = new CicoSRCMEM_LOW(this);
    m_SRCMEM_HIGH = new CicoSRCMEM_HIGH(this);

    CicoState* s = (CicoState*)m_stt->getState(gstrSRCCPU_LOW);
    if ((CicoState*)0 != s) {
        s->addEntryAction(m_SRCCPU_LOW);
        s->addDoAction(m_SRCCPU_LOW);
    }
    s = (CicoState*)m_stt->getState(gstrSRCCPU_HIGH);
    if ((CicoState*)0 != s) {
        s->addEntryAction(m_SRCCPU_HIGH);
        s->addDoAction(m_SRCCPU_HIGH);
    }
    s = (CicoState*)m_stt->getState(gstrSRCMEM_LOW);
    if ((CicoState*)0 != s) {
        s->addEntryAction(m_SRCMEM_LOW);
        s->addDoAction(m_SRCMEM_LOW);
    }
    s = (CicoState*)m_stt->getState(gstrSRCMEM_HIGH);
    if ((CicoState*)0 != s) {
        s->addEntryAction(m_SRCMEM_HIGH);
        s->addDoAction(m_SRCMEM_HIGH);
    }
    m_stt->start();
    double tVal = (double)g_samplingWaitTimer / 100;
    ICO_TRA("ECORE_TIMER(%f)", tVal);
    g_SCResourceWatch = ecore_timer_add(tVal, ico_SCResourceWatch, this);
    ICO_TRA("end");
    return true;
}

/**
 * @brief watch
 * @return true:change level false:no change
 */
bool CicoSCSysResourceController::watch()
{
    if (g_RC_LOG) ICO_TRA("start");
    if ((CicoSCSysResourceMonitor*)0 == m_monitor) {
        if (g_RC_LOG) ICO_TRA("end false");
        return false;
    }
    bool r = false;
    m_monitor->monitoring();
    int cpu = m_monitor->getCPUuser(); // application cpu
    if (-1 != cpu) {
        cpu /= 100;
        CicoEvent ev(gEV1000, cpu);
        bool b = m_stt->eventEntry(ev);
        if (g_RC_LOG) ICO_TRA("cpu %c, %d->%d", b? 't': 'f', m_cpu, cpu);
        m_cpu = cpu;
        r = true;
    }
    int mem = m_monitor->getUsageFreeMem(); // free memory
    if (-1 != mem) {
        mem = (10000 - mem)/100;  // free->used memory calc
        CicoEvent ev(gEV1001, mem);
        m_stt->eventEntry(ev);
        m_mem = mem;
        r = true;
    }
    if (g_RC_LOG) ICO_TRA("end %s", r? "true": "false");
    return r;
}

/**
 * @brief CPU share control request
 * @param pulus true:level up / false level down
 */
void CicoSCSysResourceController::resourceControlCPUShares(bool bHight)
{
    ICO_TRA("start %s", bHight? "true": "false");
    if ((NULL == m_rConf) || (false == m_rConf->m_bDoIt)) {
        ICO_TRA("end");
        return;
    }
    if (true == m_rConf->m_cpuCtrl.empty()) {
        ICO_TRA("end");
        return;
    }
    int newCgrpCPU = -1;
    int sz = m_rConf->m_cpuCtrl.size();
    for (int i = 0; i < sz; i++) {
        if (true == m_RCCpuShares[i].empty()) { // write target none
            continue; // next target
        }
        int sharesVal = -1; // initialize cpu.shares value
        if (true == bHight) {
            if (m_cgrpCPU < (int)m_rConf->m_cpuCtrl[i]->m_hight.size()) {
                sharesVal = m_rConf->m_cpuCtrl[i]->m_hight[m_cgrpCPU];
                newCgrpCPU = m_cgrpCPU + 1;
                ICO_DBG("m_hight[%d]=%d", m_cgrpCPU, sharesVal);
            }
        }
        else {
            if (0 != m_cgrpCPU) {
                int tmp = m_cgrpCPU - 1;
                if (tmp < (int)m_rConf->m_cpuCtrl[i]->m_low.size()) {
                    sharesVal = m_rConf->m_cpuCtrl[i]->m_low[tmp];
                    newCgrpCPU = tmp;
                    ICO_DBG("m_low[%d]=%d", tmp, sharesVal);
                }
            }
        }
        if (0 <= sharesVal) {
            replaceCgroupFile(m_RCCpuShares[i], sharesVal);
        }
//        else if (-9 == sharesVal) {
//        }
    }
    if (-1 != newCgrpCPU) { // control level value check
        ICO_DBG("control level %d -> %d", m_cgrpCPU, newCgrpCPU);
        m_cgrpCPU = newCgrpCPU; // set next control level
    }
    ICO_TRA("end");
}

bool CicoSCSysResourceController::replaceCgroupFile(const std::string& tgt,
                                                    int val)
{
    char sv[64];
    sprintf(sv, "%d", val);
    return replaceCgroupFile(tgt, sv);
}

bool CicoSCSysResourceController::replaceCgroupFile(const std::string& tgt,
                                                    const char* val)
{
    ICO_TRA("start");
    if (true == tgt.empty()) {
        ICO_TRA("end");
        return false;
    }
    FILE *fp = fopen(tgt.c_str(), "wt");
    if (0 == fp) {
        ICO_TRA("end open error %s", tgt.c_str());
        return false;
    }
    fprintf(fp, val);
    fclose(fp);
    ICO_TRA("end Cmd(echo %s > %s", val, tgt.c_str());
    return true;
}

/**
 * @brief memory limit control request
 * @param pulus true:level up / false level down
 */
void CicoSCSysResourceController::resourceControlMemLimit(bool bHight)
{
    ICO_TRA("start");
    if ((NULL == m_rConf) || (false == m_rConf->m_bDoIt)) {
        ICO_TRA("end");
        return;
    }
    // TODO mk_k coming soon
    if (true == bHight) {
        // coming soon
        ICO_TRA("hight (true)");
    }
    else {
        // coming soon
        ICO_TRA("low (false)");
    }
//    m_cgrpMEM++;
    ICO_TRA("end");
}



/**
 * @brief
 *
 */
bool CicoSCSysResourceController::entryCgroupCPU(int pid, int grpNo)
{
    ICO_TRA("start pid(%d), cgrpNo(%d)", pid, grpNo);
    if (((0 > grpNo) || ((int)m_RCCpuTasks.size() <= grpNo)) ||
        ((NULL == m_rConf) || (false == m_rConf->m_bDoIt))) {
        ICO_TRA("end %d", m_RCCpuTasks.size());
        return false;
    }
#if 1
    struct stat statbuf;
    char procdir[128];
    sprintf(procdir, "/proc/%d/task", pid);
    if (0 != stat(procdir, &statbuf)) {
        ICO_TRA("end %s ng dir", procdir);
        return false;
    }
    DIR *dp;
    dp = opendir(procdir);
    if(dp == NULL) {
        ICO_TRA("end opendir error");
        return false;
    }
    struct dirent *ent;
    while((ent = readdir(dp)) != NULL){
        if (0 != isdigit(ent->d_name[0])) {
            ICO_DBG("echo %s > %s", ent->d_name, m_RCCpuTasks[grpNo].c_str());
            replaceCgroupFile(m_RCCpuTasks[grpNo], ent->d_name);
        }
    }
    closedir(dp);
    ICO_TRA("end");
    return true;
#else
    return replaceCgroupFile(m_RCCpuTasks[grpNo], pid);
#endif
}

/**
 * @brief initialize cgroup
 */
void CicoSCSysResourceController::init_cgroup()
{
    ICO_TRA("start");
    init_cgroup_cpu();
    // memory cgroup directory get
    init_cgroup_memory();
    ICO_TRA("end");
}

/**
 * @brief initialize cgroup cpu
 */
void CicoSCSysResourceController::init_cgroup_cpu()
{
    ICO_TRA("start");
    if ((NULL == m_rConf) || (false == m_rConf->m_bDoIt)) {
        ICO_TRA("end");
        return;
    }
    string ss("/");
    string sTasks(D_STRtasks);
    string sShares(D_STRcpushares);
    string t1(m_rConf->m_cpuCGRPPath);
    if ('/' != t1[t1.size()-1]) {
        t1 += ss;
    }
    vector<string> d;
    d.push_back(m_rConf->m_cpuCGRPPath);
    int sz = m_rConf->m_cpuCtrl.size();
    for (int i = 0; i < sz; i++) {
        CicoSCCpuResourceGrp* obj = m_rConf->m_cpuCtrl[i];
        string t2;
        if (0 != obj) {
            t2 = t1;
            t2 += obj->m_grpNm;
        }
        else {
            t2.clear();
        }
        d.push_back(t2);
        string t3(t2);
        string t4(t2);
        if (false == t2.empty()) {
            if ('/' != t2[t2.size()-1]) {
                t3 += ss;
                t4 += ss;
            }
            t3 += sTasks;
            t4 += sShares;
        }
        m_RCCpuTasks.push_back(t3);
        m_RCCpuShares.push_back(t4);
    }
    make_directorys(d);
    ICO_TRA("end");
    return;
}

/**
 * @brief initialize cgroup memory
 */
void CicoSCSysResourceController::init_cgroup_memory()
{
    ICO_TRA("start");
    if ((NULL == m_rConf) || (false == m_rConf->m_bDoIt)) {
        ICO_TRA("end error config class");
        return;
    }
    // Coming Soon
    ICO_TRA("end");
    return;
}

/**
 * @brief make directory
 * @param d create directory target
 */
void CicoSCSysResourceController::make_directorys(std::vector<std::string> dir)
{
    ICO_TRA("start");
    int sz = dir.size();
    for (int i = 0; i < sz; i++) {
        if (true == dir[i].empty()) {
            continue;
        }
        const char* dpath = dir[i].c_str();
        struct stat stat_buf;
        if (0 != stat(dpath, &stat_buf)) {
            // drwxr-xr-x
            int r = mkdir(dpath, S_IRUSR| S_IWUSR| S_IXUSR| S_IRGRP|
                          S_IXGRP| S_IROTH| S_IXOTH);
            ICO_DBG("%s create status(%d)", dpath, r);
        }
    }
    ICO_TRA("end");
}

/**
 * @brief Class ico System Resource Controller CPU LOW level
 *        constructor
 */
CicoSRCCPU_LOW::CicoSRCCPU_LOW(CicoSCSysResourceController* obj):
    m_cntlr(obj)
{
    ICO_TRA("constructor");
    m_val = -1;      // cpu or memory usage value
    m_baseVal = -1;  // cpu or memory base usage value
    m_cnt = 0;
    m_rConf = CicoSystemConfig::getInstance()->getResourceConf();
}

/**
 * @brief Class ico System Resource Controller CPU LOW level
 *        destructor
 */
CicoSRCCPU_LOW::~CicoSRCCPU_LOW()
{
    ICO_TRA("destructor");
    m_cntlr = NULL;
}

/**
 * @brief resource control entry state
 * @param ev event data
 * @param stt state object
 */
void CicoSRCCPU_LOW::onEntry(const CicoEvent& ev, const CicoState* stt, int)
{
    int v1 = ev.getEV();
    int v2 = ev.getGCVi();
    if (g_RC_LOG) ICO_TRA("start %s,%d,%d CicoSRCCPU_LOW::", stt->getName().c_str(), v1, v2);
    m_baseVal = v2;
    m_val = m_baseVal;
    m_cnt = m_rConf->m_retryCnt;
    if (g_RC_LOG) ICO_TRA("end cnt(%d) CicoSRCCPU_LOW::", m_cnt);
}

/**
 * @brief cpu usage low resource control
 * @param ev event data
 * @param stt state object
 */
void CicoSRCCPU_LOW::onDo(const CicoEvent& ev, const CicoState* stt, int)
{
    int valN = ev.getGCVi();
    if (g_RC_LOG) ICO_TRA("start o(%d), n(%d)CicoSRCCPU_LOW::", m_val, valN);
#if 0
    if (valN > m_val) {
        // When the value is greater than the previous
        if (g_RC_LOG) ICO_TRA("end n(%d)->o(%d) CicoSRCCPU_LOW::", m_val, valN);
        return;
    }
    // When the value is less than the previous
    int t = ((double)valN / (double)m_baseVal)*100;
    if (50 <= t) {
        m_cntlr->resourceControlCPUShares(false);
        if (g_RC_LOG) ICO_DBG("CHG %d->%d CicoSRCCPU_LOW::",m_baseVal, valN);
        m_baseVal = valN;
    }
#else
    if (valN < m_rConf->m_lowLimitVal) {
        m_cnt--;
        if (0 >= m_cnt) {
            m_cntlr->resourceControlCPUShares(false);
            if (g_RC_LOG) ICO_DBG("CHG %d->%d CicoSRCCPU_LOW::",m_baseVal, valN);
            m_cnt = m_rConf->m_retryCnt;
            m_baseVal = valN;
        }
    }
    else {
        if (g_RC_LOG) ICO_DBG("cnt rst V(%d) CicoSRCCPU_LOW::", valN);
        m_cnt = m_rConf->m_retryCnt;
    }
#endif
    m_val = valN;
    if (g_RC_LOG) ICO_TRA("end CicoSRCCPU_LOW::");
}


/**
 * @brief resource control exit state
 * @param ev event data
 * @param stt state object
 */
void CicoSRCCPU_LOW::onExit(const CicoEvent&, const CicoState*, int)
{
    if (g_RC_LOG) ICO_TRA("start CicoSRCCPU_LOW::");
    m_baseVal = -1;
    m_val = -1;
    if (g_RC_LOG) ICO_TRA("end CicoSRCCPU_LOW::");
}

/**
 * @brief Class ico System Resource Controller CPU HIGH level
 *        constructor
 */
CicoSRCCPU_HIGH::CicoSRCCPU_HIGH(CicoSCSysResourceController* obj):
        CicoSRCCPU_LOW(obj)
{
    ICO_TRA("constructor");
}

/**
 * @brief Class ico System Resource Controller CPU HIGH level
 *        destructor
 */
CicoSRCCPU_HIGH::~CicoSRCCPU_HIGH()
{
    ICO_TRA("destructor");
}

/**
 * @brief cpu usage high resource control
 * @param ev event data
 * @param stt state object
 */
void CicoSRCCPU_HIGH::onDo(const CicoEvent& ev, const CicoState* stt, int)
{
    int valN = ev.getGCVi();
    if (g_RC_LOG) ICO_TRA("start o(%d), n(%d) CicoSRCCPU_HIGH::", m_val, valN);
#if 0
    if (valN <= m_val) {
        // When the value is less than the previous
        if (g_RC_LOG) ICO_TRA("n(%d)<=o(%d) CicoSRCCPU_HIGH::", valN, m_val);
        return;
    }
    // When the value is greater than the previous
    int t = ((double)(100-valN) / (double)(100-m_baseVal))*100;
    if (50 <= t) {
        m_cntlr->resourceControlCPUShares(true);
        if (g_RC_LOG) ICO_DBG("%d->%d CicoSRCCPU_HIGH::", m_baseVal, valN);
        m_baseVal = valN;
    }
#else
    if (valN > m_rConf->m_highLimitVal) {
        m_cnt--;
        if (g_RC_LOG) ICO_DBG("cnt(%d) CicoSRCCPU_HIGH::", m_cnt);
        if (0 >= m_cnt) {
            m_cntlr->resourceControlCPUShares(true);
            if (g_RC_LOG) ICO_DBG("CicoSRCCPU_HIGH:: %d->%d",m_baseVal, valN);
            m_cnt = m_rConf->m_retryCnt;
            m_baseVal = valN;
        }
    }
    else {
        if (g_RC_LOG) ICO_DBG("CicoSRCCPU_HIGH:: (%d)", valN);
        m_cnt = m_rConf->m_retryCnt;
    }
#endif
    m_val = valN;
    if (g_RC_LOG) ICO_TRA("end CicoSRCCPU_HIGH::");
}

/**
 * @brief Class ico System Resource Controller MEMory LOW level
 *        constructor
 */
CicoSRCMEM_LOW::CicoSRCMEM_LOW(CicoSCSysResourceController* obj):
        CicoSRCCPU_LOW(obj)
{
    ICO_TRA("constructor CicoSRCMEM_LOW::");
}

/**
 * @brief Class ico System Resource Controller MEMory LOW level
 *        constructor
 */
CicoSRCMEM_LOW::~CicoSRCMEM_LOW()
{
    ICO_TRA("destructor CicoSRCMEM_LOW::");
}

/**
 * @brief memory usage low resource control
 * @param ev event data
 * @param stt state object
 */
void CicoSRCMEM_LOW::onDo(const CicoEvent& ev, const CicoState* stt, int)
{
    int valN = ev.getGCVi();
    if (g_RC_LOG) ICO_TRA("start o(%d), n(%d)", m_val, valN);
    if (valN >= m_val) {
        // When the value is greater than the previous
        if (g_RC_LOG) ICO_TRA("n(%d)>=o(%d)", valN, m_val);
        return;
    }
    // When the value is less than the previous
    int t = ((double)valN / (double)m_baseVal)*100;
    if (50 <= t) {
        m_cntlr->resourceControlMemLimit(false);
        if (g_RC_LOG) ICO_TRA("BASE CHG %d->%d",m_baseVal, valN);
        m_baseVal = valN;
    }
    m_val = valN;
    if (g_RC_LOG) ICO_TRA("end");

}

/**
 * @brief Class ico System Resource Controller MEMory HIGH level
 *        constructor
 */
CicoSRCMEM_HIGH::CicoSRCMEM_HIGH(CicoSCSysResourceController* obj):
    CicoSRCCPU_LOW(obj)
{
    ICO_TRA("constructor");
}

/**
 * @brief Class ico System Resource Controller MEMory HIGH level
 *        destructor
 */
CicoSRCMEM_HIGH::~CicoSRCMEM_HIGH()
{
    ICO_TRA("destructor");
}

/**
 * @brief memory usage high resource control
 * @param ev event data
 * @param stt state object
 */
void CicoSRCMEM_HIGH::onDo(const CicoEvent& ev, const CicoState* stt, int)
{
    int valN = ev.getGCVi();
    if (g_RC_LOG) ICO_TRA("start o(%d), n(%d)", m_val, valN);
    if (valN <= m_val) {
        // When the value is less than the previous
        ICO_TRA("end");
        return;
    }
    // When the value is greater than the previous
    int t = ((double)(100-valN) / (double)(100-m_baseVal))*100;
    if (50 <= t) {
        m_cntlr->resourceControlMemLimit(true);
        if (g_RC_LOG) ICO_TRA("BASE CHG %d -> %d", m_baseVal, valN);
        m_baseVal = valN;
    }
    m_val = valN;
    if (g_RC_LOG) ICO_TRA("end");
}
