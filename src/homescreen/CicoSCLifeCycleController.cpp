/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <glib.h>
#include <ail.h>
#include <aul/aul.h>
#include <sys/stat.h>

#include "ico_syc_error.h"
#include "CicoLog.h"
#include "CicoSCAulItems.h"
#include "CicoSCAilItems.h"
#include "CicoSCLifeCycleController.h"
#include "CicoSCConf.h"
#include "CicoSCSystemConfig.h"

using namespace std;


#define ICO_SYC_EOK              0  /* OK                           */
#define ICO_SYC_EPERM           -1  /* Operation not permitted      */
#define ICO_SYC_ENOENT          -2  /* No such object               */
#define ICO_SYC_ESRCH           -3  /* Not initialized              */
#define ICO_SYC_EIO             -5  /* I/O(send/receive) error      */
#define ICO_SYC_ENXIO           -6  /* Not exist                    */
#define ICO_SYC_E2BIG           -7  /* Buffer size too smale        */
#define ICO_SYC_EBADF           -9  /* Illegal data type            */
#define ICO_SYC_EAGAIN          -11 /* Try again                    */
#define ICO_SYC_ENOMEM          -12 /* Out of memory                */
#define ICO_SYC_EFAULT          -14 /* Bad address                  */
#define ICO_SYC_EBUSY           -16 /* Not available now            */
#define ICO_SYC_EEXIST          -17 /* Multiple define              */
#define ICO_SYC_EINVAL          -22 /* Invalid argument             */
#define ICO_SYC_ENOSYS          -38 /* System error                 */


CicoSCLifeCycleController* CicoSCLifeCycleController::ms_myInstance = NULL;

const char* DAilTypeFilPrm_Menu="menu";
const char* DAilTypeFilPrm_App="Application";
const char* DNil = "(nil)";
const char* sectAppAttr = "app-attributes";
const char* DNull = "(null)";

#define APP_CONF_AIL_NULL_STR   DNull

/**
 * @brief
 * @param req_id
 * @param pkg_type
 * @param pkg_name package name
 * @param key "start"/"end"/"install_percent"/"command" strings
 * @param val "ok"/"error"/"0.000000"-"100.000000" strings
 * @param pmsg pkgmgr message 
 * @param data user data
 */
static int CSCLCCpkgmgr_handler(int req_id, const char *pkg_type,
                                const char *pkg_name, const char *key,
                                const char *val, const void *pmsg, void *data)
{
    CicoSCLifeCycleController* x = (CicoSCLifeCycleController*)data;
    return
        CSCLCCpkgmgr_handlerX(req_id, pkg_type, pkg_name, key, val, pmsg, x);
}

/**
 * @brief ail_filter_list_appinfo_foreach callback function
 * @param appinfo appinfo_h the appinfo's handle
 * @param data user data
 * @retval AIL_CB_RET_CONTINUE return if you continue iteration
 * @retval AIL_CB_RET_CANCEL return if you cancel iteration
 */
static ail_cb_ret_e CSCLCCail_list_appinfo_cb(const ail_appinfo_h appinfo,
                                              void *data)
{
    CicoSCLifeCycleController* x = (CicoSCLifeCycleController*)data;
    return CSCLCCail_list_appinfo_cbX(appinfo, x);
}

/**
 * @brief applications are launched. callback function
 * @param pid
 * @param data user data
 */
static int CSCLCCapp_launch_handler(int pid, void *data)
{
    CicoSCLifeCycleController* x = (CicoSCLifeCycleController*)data;
    return CSCLCCapp_launch_handlerX(pid, x);
}

/**
 * @brief applications die. callback function
 * @param pid
 * @param data user data
 */
static int CSCLCCapp_dead_handler(int pid, void *data)
{
    CicoSCLifeCycleController* x = (CicoSCLifeCycleController*)data;
    return CSCLCCapp_dead_handlerX(pid, x);
}


/**
 * @brief contractor
 */
CicoSCLifeCycleController::CicoSCLifeCycleController()
{
    m_gconf = (GKeyFile*)NULL;
    m_pc = (pkgmgr_client*)NULL;
    initAIL();
    initAUL();
}

/**
 * @brief destructor
 */
CicoSCLifeCycleController::~CicoSCLifeCycleController()
{

}

CicoSCLifeCycleController*
CicoSCLifeCycleController::getInstance(void)
{
    if (NULL == ms_myInstance) {
        ms_myInstance = new CicoSCLifeCycleController();
    }
    return ms_myInstance;
}

/**
 * @brief application launch
 * @param appid application id
 * @retval ICO_SYC_EOK   success
 * @retval ICO_SYC_ESRCH error(not initialized)
 * @retval ICO_SYC_ENOENT error(does not exist)
 * @retval ICO_SYC_EBUSY error(already launch)
 * @retval ICO_SYC_EPERM error(no authority)
 */
int CicoSCLifeCycleController::launch(const char* appid)
{
    ICO_TRA("start %s", appid);
    // appid check AIL table exist
    if (NULL == findAIL(appid)) {
        ICO_TRA("not find");
        return ICO_SYC_ENOSYS;
    }
    int r = aul_launch_app(appid, NULL);
    if (0 > r) {
        ICO_TRA("aul_launch_app NG %d", r);
        return ICO_SYC_ENOSYS;
    }
    ICO_TRA("launch success");
    return ICO_SYC_EOK;
}

/**
 * @brief application launch
 * @param appid application id
 * @retval ICO_SYC_EOK   success
 * @retval ICO_SYC_ESRCH error(not initialized)
 * @retval ICO_SYC_ENOENT error(does not exist)
 * @retval ICO_SYC_EBUSY error(already launch)
 * @retval ICO_SYC_EPERM error(no authority)
 */
int CicoSCLifeCycleController::launch(const std::string& appid)
{
    return launch((const char*)appid.c_str());
}

/**
 * @brief application terminate
 * @param appid application id
 * @retval ICO_SYC_EOK   success
 * @retval ICO_SYC_ESRCH error(not initialized)
 * @retval ICO_SYC_ENOENT error(does not exist)
 * @retval ICO_SYC_EBUSY error(already launch)
 * @retval ICO_SYC_EPERM error(no authority)
 */
int CicoSCLifeCycleController::terminate(const char* appid)
{
    ICO_TRA("CicoSCLifeCycleController::terminate %s", appid);
    int r = ICO_SYC_EOK;
    vector<int> pids;
    if (true == getPIDs(appid, pids)) {
        vector<int>::iterator it;
        for(it = pids.begin(); it != pids.end(); ++it) {
            int tr = terminateR(*it);
            if (ICO_SYC_EOK != tr) {
                r = tr;
            }
        }
    }
    else {
        ICO_TRA("end ICO_SYC_EPERM");
        return ICO_SYC_EPERM;
    }
    ICO_TRA("end %d", r);
    return r;
}

/**
 * @brief application terminate
 * @param appid application id
 * @retval ICO_SYC_EOK   success
 * @retval ICO_SYC_ESRCH error(not initialized)
 * @retval ICO_SYC_ENOENT error(does not exist)
 * @retval ICO_SYC_EBUSY error(already launch)
 * @retval ICO_SYC_EPERM error(no authority)
 */
int CicoSCLifeCycleController::terminate(const std::string& appid)
{
    return terminate((const char*)appid.c_str());
}

/**
 * @brief application terminate
 * @param pid
 * @retval ICO_SYC_EOK   success
 * @retval ICO_SYC_ESRCH error(not initialized)
 * @retval ICO_SYC_ENOENT error(does not exist)
 * @retval ICO_SYC_EBUSY error(already launch)
 * @retval ICO_SYC_EPERM error(no authority)
 */
int CicoSCLifeCycleController::terminate(int pid)
{
    ICO_TRA("CicoSCLifeCycleController::terminate %d", pid);
    const CicoSCAulItems* t = findAUL(pid);
    if ((NULL == t) || (0 == t)) {
        ICO_TRA("not find");
        return ICO_SYC_EPERM;
    }
    return terminateR(pid);
}

/**
 * @brief application terminate
 * @param pid
 * @retval ICO_SYC_EOK   success
 * @retval ICO_SYC_ENOSYS error
 */
int CicoSCLifeCycleController::terminateR(int pid)
{
    ICO_TRA("CicoSCLifeCycleController::terminateR %d", pid);
    int r = aul_terminate_pid(pid);
    if (r < 0) {
        ICO_DBG("terminate error pid:%d, r:%d", pid, r);
        return ICO_SYC_ENOSYS;
    }
    return ICO_SYC_EOK;
}

/**
 * @brief application suspend
 * @param appid application id
 * @retval ICO_SYC_EOK   success
 * @retval ICO_SYC_ESRCH error(not initialized)
 * @retval ICO_SYC_ENOENT error(does not exist)
 * @retval ICO_SYC_EBUSY error(already launch)
 * @retval ICO_SYC_EPERM error(no authority)
 */
int CicoSCLifeCycleController::suspend(const char* appid)
{
    int r = ICO_SYC_EOK;
    vector<int> pids;
    if (true == getPIDs(appid, pids)) {
        vector<int>::iterator it;
        for(it = pids.begin(); it != pids.end(); ++it) {
            int tr = suspend(*it);
            if (ICO_SYC_EOK != tr) {
                r = tr;
            }
        }
    }
    else {
        ICO_TRA("end ICO_SYC_EPERM");
        return ICO_SYC_EPERM;
    }
    ICO_TRA("end %d", r);
    return r;
}

/**
 * @brief application suspend
 * @param appid application id
 * @retval ICO_SYC_EOK   success
 * @retval ICO_SYC_ESRCH error(not initialized)
 * @retval ICO_SYC_ENOENT error(does not exist)
 * @retval ICO_SYC_EBUSY error(already launch)
 * @retval ICO_SYC_EPERM error(no authority)
 */
int CicoSCLifeCycleController::suspend(const std::string& appid)
{
    return suspend((const char*)appid.c_str());
}

/**
 * @brief application suspend
 * @param pid
 * @retval ICO_SYC_EOK   success
 * @retval ICO_SYC_ESRCH error(not initialized)
 * @retval ICO_SYC_ENOENT error(does not exist)
 * @retval ICO_SYC_EBUSY error(already launch)
 * @retval ICO_SYC_EPERM error(no authority)
 */
int CicoSCLifeCycleController::suspend(int pid)
{
// TODO mk_k
// TODO mk_k
    return ICO_SYC_EPERM;
}

/**
 * @brief appid is runnning check
 * @param appid package
 * @return running status
 * @retval true is runnning
 * @retval false not runnning
 */
bool CicoSCLifeCycleController::isRunning(const char* appid) const
{
    ICO_TRA("start %s", appid);
    bool r = false;
#if 0 // TODO mk_k
    vector<CicoSCAulItems>::iterator it;
    for (it = m_aul.begin(); it != m_aul.end(); ++it) {
        if (0 == strcasecmp(it->m_appid.c_str(), appid)) {
            r = true;
            break;
        }
    }
#else
    int sz = m_aul.size();
    for (int i = 0; i < sz; i++) {
        if (0 == strcasecmp(m_aul[i].m_appid.c_str(), appid)) {
            r = true;
            break;
        }
    }
#endif
    ICO_TRA("end %s", r? "true": "false");
    return r;
}

/**
 * @brief appid is runnning check
 * @param appid package
 * @return running status
 * @retval true is runnning
 * @retval false not runnning
 */
bool CicoSCLifeCycleController::isRunning(const std::string& appid) const
{
    return isRunning((const char*)appid.c_str());
}

/**
 * @brief find ail data
 * @param appid find key
 * @return CicoSCAilItems class pointer
 * @retval NULL is not find
 */
const CicoSCAilItems* CicoSCLifeCycleController::findAIL(const char* appid) const
{
    ICO_TRA("start");
    const CicoSCAilItems* r = NULL;
    int sz = m_ail.size();
    for (int i = 0; i < sz; i++) {
        if (0 == strcasecmp(m_ail[i].m_appid.c_str(), appid)) {
            r = m_ail[i].p();
            break; // break of for
        }
    }
    ICO_TRA("end %x", r);
    return r;
}

/**
 * @brief find ail data
 * @param appid find key
 * @return CicoSCAilItems class pointer
 * @retval NULL is not find
 */
const CicoSCAilItems* CicoSCLifeCycleController::findAIL(const std::string& appid) const
{
    ICO_TRA("call findAIL(const chr*)");
    return findAIL((const char*)appid.c_str());
}

/**
 * @brief get launched apprication pid
 * @param appid target application appid
 * @return pid
 */
bool CicoSCLifeCycleController::getPIDs(const char* appid, std::vector<int>& pids) const
{
    ICO_TRA("start %s", appid);
    bool r = false;
#if 0 // TODO mk_k
    vector<CicoSCAulItems>::iterator it;
    for (it = m_aul.begin(); it != m_aul.end(); ++it) {
        if (0 == strcasecmp(it->m_appid.c_str(), appid)) {
            pids.push_back(it->m_pid);
            r = true;
        }
    }
#else
    int sz = m_aul.size();
    for (int i = 0; i < sz; i++) {
        const CicoSCAulItems* t = m_aul[i].p();
        if (0 == strcasecmp(t->m_appid.c_str(), appid)) {
            pids.push_back(t->m_pid);
            r = true;
        }
    }
#endif
    ICO_TRA("end %d %s", pids.size(), r? "true": "false");
    return r;
}

/**
 * @brief get launched apprication pid
 * @param appid target application appid
 * @return pid
 */
bool CicoSCLifeCycleController::getPIDs(std::string& appid, std::vector<int>& pids) const
{
    return getPIDs((const char*)appid.c_str(), pids);
}

/**
 * @brief get launched apprication pid
 * @param appid target application appid
 * @return pid
 */
const CicoSCAulItems* CicoSCLifeCycleController::findAUL(int pid) const
{
    ICO_TRA("start %d", pid);
    const CicoSCAulItems* r = NULL;
    int sz = m_aul.size();
    for (int i = 0; i < sz; i++) {
        const CicoSCAulItems* t = m_aul[i].p();
        if (t->m_pid == pid) {
            r = t;
            ICO_DBG("find %d -> %s", pid, t->m_appid.c_str());
            break; // break of for
        }
    }
    ICO_TRA("end %x", r);
    return r;
}


/**
 * @brief ail infomation data initialization
 */
void CicoSCLifeCycleController::initAIL()
{
    ICO_TRA("start");
    createAilItems();
    if ((pkgmgr_client*)NULL == m_pc) {
        m_pc = pkgmgr_client_new(PC_LISTENING);
        int r = pkgmgr_client_listen_status(m_pc, CSCLCCpkgmgr_handler,
                                            (void*)this);
        if (PKGMGR_R_OK != r) {
            pkgmgr_client_free(m_pc);
            m_pc = NULL;
        }
    }
    ICO_TRA("end");
}

/**
 * @brief
 */
int CSCLCCpkgmgr_handlerX(int req_id, const char *pkg_type, const char *pkg_name, 
                         const char *key, const char *val, const void *pmsg,
                         CicoSCLifeCycleController* x)
{
// TODO mk_k
    ICO_TRA("pkgmgr t:%s, n:%s, k:%s, v:%s, m:%s",pkg_type, pkg_name, key,
            val, pmsg);
    if ((NULL == x) || (0 == x)) {
        ICO_TRA("end user data none");
        return -1;
    }
    if (strcasecmp(key, "end") == 0) {
        if (strcasecmp(val, "ok") == 0) {
            ICO_TRA("end::ok");
            x->renewAIL();
        }
    }
    return 0;
// TODO mk_k
}

/**
 * @brief AIL list renew
 */
void CicoSCLifeCycleController::renewAIL()
{
    vector<CicoSCAilItems> old = m_ail;
    m_ail.clear();
    int cnt =0;
    while (true == createAilItems()) {
        if (cnt > 500) {
            m_ail.clear();
            m_ail = old;
            break;
        }
        cnt++;
        usleep(10000);
    }
    if (cnt > 500) {
        ICO_TRA("error AIL I/F");
        return ;
    }
#if 0 // TODO mk_k
    vector<string> add_appid;
    vector<string> remove_appid;

    vector<CicoSCAulItems>::iterator it_n;
    vector<CicoSCAulItems>::iterator it_o;
    for(it_n = m_aul.begin(); it_n != m_aul.end(); ++it_n) {
        bool bingo = false;
        for(it_o = old.begin(); it_o != old.end(); ++it_o) {
            if (it_n->m_appid.compaire(it_o->m_appid)) {
                bingo = true;
            }
        }
        if (false == bingo) {
            add_appid.push_back(it_n->m_appid);
        }
    }
    for(it_o = old.begin(); it_o != old.end(); ++it_o) {
        bool bingo = false;
        for(it_n = m_aul.begin(); it_n != m_aul.end(); ++it_n) {
            if (it_o->m_appid.compaire(it_n->m_appid)) {
                bingo = true;
            }
        }
        if (false == bingo) {
            remove_appid.push_back(it_o->m_appid);
        }
    }

    if (0 != add_appid.size()) {
// TODO mk_k install
    }
    if (0 != remove_appid.size()) {
// TODO mk_k uninstall
    }
#endif
    old.clear();
    return;
}

/**
 * @brief ail_filter_list_appinfo_foreach callback function
 * @param appinfo appinfo_h the appinfo's handle
 * @param x CicoSCLifeCycleController class pointer
 * @retval AIL_CB_RET_CONTINUE return if you continue iteration
 * @retval AIL_CB_RET_CANCEL return if you cancel iteration
 *
 */
ail_cb_ret_e CSCLCCail_list_appinfo_cbX(const ail_appinfo_h appinfo,
                                        CicoSCLifeCycleController* x)
{
    ICO_TRA("start");
    if ((NULL == x) || (0 == x)) {
        ICO_TRA("end bad end")
        return AIL_CB_RET_CANCEL;
    }
    char   *pkg;
    char   *icn;
    char   *nm;
    char   *ctgry;
    char   *typ;
    char   *exe;
    bool   bndsp = false;
    /* get package name for appid */
    ail_appinfo_get_str(appinfo, AIL_PROP_PACKAGE_STR, &pkg);
    if (strcmp(pkg, APP_CONF_AIL_NULL_STR) == 0) {
        pkg = NULL;
    }
    /* get icon path */
    ail_appinfo_get_str(appinfo, AIL_PROP_ICON_STR, &icn);
    if (strcmp(icn, APP_CONF_AIL_NULL_STR) == 0) {
        icn = NULL;
    }
    else if (icn != NULL) {
        struct stat buff;
        /* file check */
        memset(&buff, 0, sizeof(buff));
        if (stat(icn, &buff) == 0) {
            if (S_ISDIR(buff.st_mode)) {
                /* is directory */
                ICO_DBG("%s is directory", icn);
                icn = NULL;
            }
        }
        else {
            /* is not exist */
            icn = NULL;
        }
    }
    /* get name */
    ail_appinfo_get_str(appinfo, AIL_PROP_NAME_STR, &nm);
    if (strcmp(nm, APP_CONF_AIL_NULL_STR) == 0) {
        nm = NULL;
    }
    /* get category */
    ail_appinfo_get_str(appinfo, AIL_PROP_CATEGORIES_STR, &ctgry);
    if (strcmp(ctgry, APP_CONF_AIL_NULL_STR) == 0) {
        ctgry = NULL;
    }
    /* get type */
    ail_appinfo_get_str(appinfo, AIL_PROP_TYPE_STR, &typ);
    if (strcmp(typ, APP_CONF_AIL_NULL_STR) == 0) {
        typ = NULL;
    }
    /* get exec */
    ail_appinfo_get_str(appinfo, AIL_PROP_EXEC_STR, &exe);
    if (strcmp(exe, APP_CONF_AIL_NULL_STR) == 0) {
        exe = NULL;
    }
    ail_appinfo_get_bool(appinfo, AIL_PROP_NODISPLAY_BOOL, &bndsp);
    ICO_DBG("pkg:%s icn:%s nm:%s ctg:%s ty:%s exe:%s ds:%s", pkg? pkg:DNil,
            icn? icn:DNil, nm? nm:DNil, ctgry? ctgry:DNil, typ? typ:DNil,
            exe? exe:DNil, bndsp? "true":"false");
    if (false == x->addAIL(pkg, icn, nm, ctgry, typ, exe, bndsp)) {
        ICO_TRA("end bad end")
        return AIL_CB_RET_CANCEL;
    }
    ICO_TRA("end")
    return AIL_CB_RET_CONTINUE;
}

/**
 * @brief create infomation ail data
 * @retval true success
 * @retval false fail create
 */
bool CicoSCLifeCycleController::createAilItems()
{
    ICO_TRA("start");
    m_ail.clear(); // clear items
    if (NULL == m_gconf) {
        m_gconf = g_key_file_new();
        GString* gsfp = g_string_new("xx");
        CicoSCSystemConfig* CSCSC = CicoSCSystemConfig::getInstance();
        g_string_printf(gsfp, "%s/%s", CSCSC->getDefaultConf()->confdir.c_str(),
                        ICO_SYC_CONFIG_APPATTR);
        GError  *gerr = NULL;
        int flg;
        flg = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;
        if (!g_key_file_load_from_file(m_gconf, gsfp->str, (GKeyFileFlags)flg, &gerr)) {
            ICO_ERR("load error conf:%s %s", (char*)gsfp->str, gerr->message);
            g_key_file_free(m_gconf);
            m_gconf = NULL;
            g_string_free(gsfp, TRUE);
            return false;
        }
        g_string_free(gsfp, TRUE);
    }

    int r;
    ail_filter_h fil;
    ail_filter_new(&fil);
    r = ail_filter_add_str(fil, AIL_PROP_TYPE_STR, DAilTypeFilPrm_Menu);
    r = ail_filter_list_appinfo_foreach(fil, CSCLCCail_list_appinfo_cb,
                                        (void*)this);
    ail_filter_destroy(fil);
    if (r != AIL_ERROR_OK) {
        if (m_gconf) {
            g_key_file_free(m_gconf);
            m_gconf = NULL;
        }
        ICO_TRA("end menu read NG(AIL)=%d", r);
        return false;
    }

    ail_filter_new(&fil);
    r = ail_filter_add_str(fil, AIL_PROP_TYPE_STR, DAilTypeFilPrm_App);
    r = ail_filter_list_appinfo_foreach(fil, CSCLCCail_list_appinfo_cb,
                                        (void*)this);
    ail_filter_destroy(fil);
    if (r != AIL_ERROR_OK) {
        if (m_gconf) {
            g_key_file_free(m_gconf);
            m_gconf = NULL;
        }
        ICO_TRA("end Application read NG(AIL)=%d", r);
        return false;
    }
    if (m_gconf) {
        g_key_file_free(m_gconf);
        m_gconf = NULL;
    }
    ICO_TRA("end");
    return true;
}

/**
 * @brief ail infomation data add one item
 * @param sPkg string package (appid)
 * @param sIco string icon full path
 * @param sNm string name
 * @param sCtg string categoies
 * @param sTyp string type
 * @param sExe string exec name
 * @param bndsp bool nodisplay value
 * @return method status
 * @retval true success added item
 * @retval false fail added item
 */
bool CicoSCLifeCycleController::addAIL(const char* sPkg, const char* sIco,
                                       const char* sNm, const char* sCtg,
                                       const char* sTyp, const char* sExe,
                                       bool bndsp)
{
    string category;
    getCategory(sPkg, sNm, sCtg, category);
    CicoSCAilItems CSCAI(sPkg, sIco, sNm, sCtg, sTyp, sExe, category, bndsp);
    m_ail.push_back(CSCAI);
    return true;
}

/**
 * @brief get category data
 * @param sPkg ail package(appid) (get key)
 * @param sNm ail name (get key)
 * @param sCtg ail categories (get key)
 * @param category read config file category string define
 */
void CicoSCLifeCycleController::getCategory(const char* sPkg, const char* sNm,
                                            const char* sCtg,
                                            std::string& category)
{
    category.clear();
    GError *gerr;
    char *appCtgry;
    int  appCtgryType;
    char addCtgry[400];
    int  addCtgryLen;
    char work[80];

    /* get default category of this application */
    addCtgry[0] = 0;
    gerr = NULL;
    appCtgry = g_key_file_get_string(m_gconf, sectAppAttr, sPkg, &gerr);
    if (gerr == NULL)  {
        appCtgryType = 0;
    }
    else {
        g_clear_error(&gerr);
        gerr = NULL;
        appCtgry = g_key_file_get_string(m_gconf, sectAppAttr, sNm, &gerr);
        if (gerr == NULL) {
            appCtgryType = 1;
        }
    }
    addCtgryLen = 0;
    if (gerr != NULL) {
        g_clear_error(&gerr);
    }
    else {
        for (int i = 1;; i++) {
            strncpy(&addCtgry[addCtgryLen],
            appCtgry, sizeof(addCtgry)-addCtgryLen-2);
            addCtgry[sizeof(addCtgry)-2] = 0;
            addCtgryLen = strlen(addCtgry);
            if (addCtgryLen > 0) {
                if (addCtgry[addCtgryLen-1] != ';') {
                    strcpy(&addCtgry[addCtgryLen++], ";");
                }
            }
            snprintf(work, sizeof(work)-1, "%s.%d",
                     appCtgryType == 0 ? sPkg : sNm, i);
            gerr = NULL;
            appCtgry = g_key_file_get_string(m_gconf, sectAppAttr, work, &gerr);
            if (gerr != NULL) {
                g_clear_error(&gerr);
                break;
            }
        }
    }
    g_clear_error(&gerr);

    /* get category */
    if (NULL != sCtg) {
        strncpy(&addCtgry[addCtgryLen], sCtg, sizeof(addCtgry)-addCtgryLen-1);
        addCtgry[sizeof(addCtgry)-1] = 0;
    }
    if (addCtgry[0]) {
         category.assign(addCtgry);
    }
}

/**
 * @brief aul infomation data initialization
 */
void CicoSCLifeCycleController::initAUL()
{
    ICO_TRA("start");
    aul_listen_app_launch_signal(CSCLCCapp_launch_handler, (void*)this);
    aul_listen_app_dead_signal(CSCLCCapp_dead_handler, (void*)this);
    ICO_TRA("end");
}

/**
 * @brief
 */
void CicoSCLifeCycleController::enterAUL(const char* appid, int pid,
                                         const CicoSCWindow* obj)
{
    ICO_TRA("start");
    bool bPushBack = true;
    int sz = m_aul.size();
    for (int i = 0; i < sz; i++) {
        CicoSCAulItems* t = (CicoSCAulItems*)m_aul[i].p();
        if (t->m_pid == pid) {
            bPushBack = false;  // push_back flag off
            if (NULL == obj) {
                // Do not replace to NULL
                ICO_DBG("Do not replace to NULL %d", t->m_CSCWptrs.size());
                break; // break of for
            }
            else {
                ICO_DBG("add window info. %d, %x", t->m_CSCWptrs.size(), obj);
                t->enterWindow(obj);
                break; // break of for
            }
        }
    }
    if (true == bPushBack) { // push back flag on?
        // AUL list Unregistered
        ICO_DBG("push_back(AUL(%s, %d, %x))", appid, pid, obj);
        m_aul.push_back(CicoSCAulItems(appid, pid, obj));
    }
    ICO_TRA("end");
}

/**
 * @brief
 */
bool CicoSCLifeCycleController::removeAUL(int pid)
{
    ICO_TRA("start %d", pid);
    bool r = false;
    vector<CicoSCAulItems>::iterator it;
    for(it = m_aul.begin(); it != m_aul.end(); ++it) {
        if (pid == it->m_pid) {
            ICO_DBG("erase appid=%s", it->m_appid.c_str());
            m_aul.erase(it);
            r = true;
            break;
        }
    }
    ICO_TRA("end %s", r? "true":"false");
    return r;
}

/**
 * @brief 
 * @param x CicoSCLifeCycleController class pointer
 */
int CSCLCCapp_launch_handlerX(int pid, CicoSCLifeCycleController* x)
{
    ICO_TRA("start");
    if ((NULL == x) || (0 == x)) {
        ICO_TRA("end user data is NULL");
        return -1;
    }
    char appid[255];
    aul_app_get_appid_bypid(pid, appid, sizeof(appid));
    ICO_DBG("added %s, %d", appid, pid);
    x->enterAUL(appid,pid);
    ICO_TRA("end %s %d", appid, pid);
    return 0;
}


/**
 * @brief applications die. callback function
 * @param pid
 * @param data user data
 * @param x CicoSCLifeCycleController class pointer
 */
int CSCLCCapp_dead_handlerX(int pid, CicoSCLifeCycleController* x)
{
    ICO_TRA("start");
    if ((NULL == x) || (0 == x)) {
        ICO_TRA("end user data is NULL");
        return -1;
    }
    if (false == x->removeAUL(pid)) {
        ICO_TRA("end dead %d fail", pid);
        return -1;
    }
    ICO_TRA("end dead %d success", pid);
    return 0;
}
