/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   CicoSCAppResourceController
 *          SystemController resource
 *          Application Resouce Controller
 *
 * @date    Aug-05-2013 create start
 */

#ifndef CICOSCAPPRESOURCECONTROLLER_H
#define CICOSCAPPRESOURCECONTROLLER_H
#include <cstddef>
#include <string>
#include <vector>

#include "CicoAilItems.h"
#include "CicoAulItems.h"
#include "CicoSCSysResourceController.h"

#define D_STRappResource "appResource.json"
#define DSTRcpu_shares "cpu_shares"
#define DSTRshare "share"
#define DSTRappid "appid"
/**
 * @brief System Controller Application Resource Controller class
 */
class CicoSCAppResourceController : public CicoSCSysResourceController
{
public:
    CicoSCAppResourceController();
    ~CicoSCAppResourceController();

    bool initAppResource();
    bool startAppResource(const std::string& un);
    bool clearApps();

    bool isAppResource();

    bool entryApp(const std::string& appid, int pid);
    bool entryApps(std::vector<CicoAulItems>& vAulItem);

    void getAppResourceFilePath(const std::string& un, std::string& fp) const;
    void createAppResourceFile(const std::string& filepath);
protected:
    bool orderApps(const std::string& filepath);
    bool readApps(const std::string& filepath);
    void init_cgroup_app_dir();
    void init_cgroup_app_cpushare();

protected:
    bool m_bDoItApp;
private:
    
    std::vector<std::string> m_vAppid;
    std::vector<int> m_vShares;
    std::string m_filepath;
};

inline bool CicoSCAppResourceController::isAppResource()
{
    return m_bDoItApp;
}

#endif // CICOSCAPPRESOURCECONTROLLER_H
