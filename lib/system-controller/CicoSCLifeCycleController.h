/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#ifndef CICOSCLIFECYCLECONTROLLER_H
#define CICOSCLIFECYCLECONTROLLER_H
#include <glib.h>
#include <vector>
#include <map>
#include <ail.h>
#include <aul/aul.h>
#include <package-manager.h>

#include "CicoSCAilItems.h"
#include "CicoSCAulItems.h"


#ifndef __CICO_SC_WINDOW_H__
class CicoSCWindow;
#endif

#ifndef CICOSCSYSRESOURCECONTROLLER_H
class CicoSCSysResourceController;
#endif

class CicoSCLifeCycleController {
public:
    CicoSCLifeCycleController();
    ~CicoSCLifeCycleController();

    static CicoSCLifeCycleController* getInstance(void);

    // Starting application
    int launch(const char* appid, bundle* b = NULL);
    int launch(const std::string& appid, bundle* b = NULL);
    // Stopping application
    int terminate(const char* appid);
    int terminate(const std::string& appid);
    int terminate(int pid);
    // suspend application
    int suspend(const char* appid);
    int suspend(const std::string& appid);
    int suspend(int pid);
    // run application check
    bool isRunning(const char* appid);
    bool isRunning(const std::string& appid);

    const std::vector<CicoSCAilItems>& getAilList() const;
    const CicoSCAilItems* findAIL(const char* appid) const;
    const CicoSCAilItems* findAIL(const std::string& appid) const;
    bool isAilRenew() const;
    void ailRenewFlagOff();

    const std::vector<CicoSCAulItems>& getAulList();
// TODO mk_k Should I think about the multiple return values start
/*
    const CicoSCAulItems* findAUL(const char* appid) const;
    const CicoSCAulItems* findAUL(const std::string& appid) const;
*/
    const CicoSCAulItems* findAUL(int pid);
    bool getPIDs(const char* appid, std::vector<int>& pids) const;
    bool getPIDs(std::string& appid, std::vector<int>& pids) const;
    void enterAUL(const char* appid, int pid, const CicoSCWindow* obj = NULL,
                  int aulstt = AUL_R_OK);

protected:
    void initAIL();
    friend ail_cb_ret_e CSCLCCail_list_appinfo_cbX(const ail_appinfo_h appinfo,
                                                   CicoSCLifeCycleController* x);
    bool createAilItems();

    friend 
        int CSCLCCpkgmgr_handlerX(int req_id, const char *pkg_type,
                                 const char *pkg_name, const char *key,
                                 const char *val, const void *pmsg,
                                 CicoSCLifeCycleController *x);
    void renewAIL();
    void ailRenewFlagOn();
    void initAUL();
    friend int CSCLCCapp_launch_handlerX(int pid,
                                         CicoSCLifeCycleController *x);
    friend int CSCLCCapp_dead_handlerX(int pid, CicoSCLifeCycleController *x);

    int terminateR(int pid);
    void getCategory(const char* sPkg, const char* sNm, const char* sCtg,
                     std::string& category);
    bool addAIL(const char* sPkg, const char* sIco, const char* sNm,
                const char* sCtg, const char* sTyp, const char* sExe,
                bool bndsp);
    bool removeAUL(int pid);
private:
    static CicoSCLifeCycleController* ms_myInstance;

protected:
    std::vector<CicoSCAilItems> m_ail;
    std::vector<CicoSCAulItems> m_aul;
    GKeyFile* m_gconf;
    pkgmgr_client* m_pc;
    CicoSCSysResourceController* m_RC;
    bool m_ailRenew;
};

/**
 * @brief AIL infomaton list
 * @return AIL information item list Container
 */
inline
const std::vector<CicoSCAilItems>& CicoSCLifeCycleController::getAilList() const
{
    return m_ail;
}


/**
 * @brief AIL change flag is on?
 * @ret bool
 * @retval true change flag on
 * @retval false change flag off
 */
inline bool CicoSCLifeCycleController::isAilRenew() const
{
    return m_ailRenew;
}

/**
 * @brief AIL change flag off set
 */
inline void CicoSCLifeCycleController::ailRenewFlagOff()
{
    m_ailRenew = false;
}

/**
 * @brief AIL change flag on set
 */
inline void CicoSCLifeCycleController::ailRenewFlagOn()
{
    m_ailRenew = true;
}

#endif // CICOSCLIFECYCLECONTROLLER_H
