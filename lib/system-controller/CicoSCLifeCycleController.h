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
    int launch(const char* appid);
    int launch(const std::string& appid);
    // Stopping application
    int terminate(const char* appid);
    int terminate(const std::string& appid);
    int terminate(int pid);
    // suspend application
    int suspend(const char* appid);
    int suspend(const std::string& appid);
    int suspend(int pid);
    // run application check
    bool isRunning(const char* appid) const;
    bool isRunning(const std::string& appid) const;

    const std::vector<CicoSCAilItems>& getAilList() const;
    const CicoSCAilItems* findAIL(const char* appid) const;
    const CicoSCAilItems* findAIL(const std::string& appid) const;

    const std::vector<CicoSCAulItems>& getAulList() const;
// TODO mk_k Should I think about the multiple return values start
/*
    const CicoSCAulItems* findAUL(const char* appid) const;
    const CicoSCAulItems* findAUL(const std::string& appid) const;
*/
    const CicoSCAulItems* findAUL(int pid) const;
    bool getPIDs(const char* appid, std::vector<int>& pids) const;
    bool getPIDs(std::string& appid, std::vector<int>& pids) const;
    void enterAUL(const char* appid, int pid, const CicoSCWindow* obj=NULL);


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
};

/**
 * @brief AIL infomaton list
 * @return AIL infomation item list Container
 */
inline
const std::vector<CicoSCAilItems>& CicoSCLifeCycleController::getAilList() const
{
    return m_ail;
}


/**
 * @brief AUL infomaton list
 * @return AUL infomation item list Container
 */
inline
const std::vector<CicoSCAulItems>& CicoSCLifeCycleController::getAulList() const
{
    return m_aul;
}



#endif // CICOSCLIFECYCLECONTROLLER_H
