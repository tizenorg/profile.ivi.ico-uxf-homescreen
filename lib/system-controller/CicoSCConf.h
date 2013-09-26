/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

//==========================================================================
/**
 *  @file   CicoSCConf.h
 *
 *  @brief  This file is definition of SystemConfigs
 */
//==========================================================================
#ifndef __CICO_SC_CONF_H__
#define __CICO_SC_CONF_H__

#include <string>
#include <vector>
#include <sstream>

#include "CicoLog.h"
   
//==========================================================================
/**
 *  @brief  enumeration of node type
 */
//==========================================================================
typedef enum _node_type {
    ICO_NODETYPE_CENTER     = 0,    ///< Center display
    ICO_NODETYPE_METER      = 1,    ///< Meter display
    ICO_NODETYPE_REMOTE     = 2,    ///< Information device(mobile device)
    ICO_NODETYPE_PASSENGER  = 3,    ///< Passenger display
    ICO_NODETYPE_REAR       = 4,    ///< Rear-seat entertainment
    ICO_NODETYPE_REARLEFT   = 5,    ///< Rear-seat entertainment(Left)
    ICO_NODETYPE_REARRIGHT  = 6,    ///< Rear-seat entertainment(right)
} ico_nodetype_e;

//==========================================================================
/**
 *  @brief  enumeration of display type
 */
//==========================================================================
typedef enum _display_type {
    ICO_DISPLAYTYPE_CENTER    = 0,  ///< Center display
    ICO_DISPLAYTYPE_METER     = 1,  ///< Meter display
    ICO_DISPLAYTYPE_REMOTE    = 2,  ///< Information device(mobile device)
    ICO_DISPLAYTYPE_PASSENGER = 3,  ///< Passenger display
    ICO_DISPLAYTYPE_REAR      = 4,  ///< Rear-seat display
    ICO_DISPLAYTYPE_REARLEFT  = 5,  ///< Rear-seat display(left)
    ICO_DISPLAYTYPE_REARRIGHT = 6,  ///< Rear-seat display(right)
} ico_display_type_e;

//==========================================================================
/**
 *  @brief  enumeration of policy type
 */
//==========================================================================
typedef enum _policy {
    ICO_POLICY_ALWAYS         = 0,  ///< No control
    ICO_POLICY_RUNNING        = 1,  ///< Running only
    ICO_POLICY_PARKED         = 2,  ///< Parked only
    ICO_POLICY_SHIFT_PARKING  = 3,  ///< Shift-position is park
    ICO_POLICY_SHIFT_REVERSES = 4,  ///< Shift-position is reverses
    ICO_POLICY_BLINKER_LEFT   = 5,  ///< Blinker is left
    ICO_POLICY_BLINKER_RIGHT  = 6,  ///< Blinker is right
} ico_policy_e;

//==========================================================================
/**
 *  @brief  enumeration of application privilege
 */
//==========================================================================
typedef enum _privilege {
    ICO_PRIVILEGE_ALMIGHTY       = 0,   ///< almighty privilege
    ICO_PRIVILEGE_SYSTEM         = 1,   ///< system level
    ICO_PRIVILEGE_SYSTEM_AUDIO   = 2,   ///< system level(audio only)
    ICO_PRIVILEGE_SYSTEM_VISIBLE = 3,   ///< system level(visible only)
    ICO_PRIVILEGE_MAKER          = 4,   ///< maker level
    ICO_PRIVILEGE_CERTIFICATE    = 5,   ///< certificated
    ICO_PRIVILEGE_NONE           = 6,   ///< no privilege
} ico_privilege_e;

#define ICO_SYC_CONFIG_APPATTR  "app_attr.conf" //TODO

//==========================================================================
/**
 *  @brief  This class holds display information of system config
 */
//==========================================================================
class CicoSCNodeConf
{
public:
    /// default constructor
    CicoSCNodeConf()
        : id(-1), name(""), address(""), type(-1) {}

    /// destructor
    virtual ~CicoSCNodeConf() {}

    /// dump log this class member variables
    void dumpConf(void)
    {
        ICO_DBG("node: id=%d name=%s address=%s type=%d",
                 id, name.c_str(), address.c_str(), type);
    }

public:
    int         id;      ///< node id
    std::string name;    ///< node name
    std::string address; ///< node ip address
    int         type;    ///< node type
};

//==========================================================================
/**
 *  @brief  This class holds layer information of system config
 */
//==========================================================================
class CicoSCLayerConf
{
public:
    /// default constructor
    CicoSCLayerConf()
        : id(-1), name(""), type(-1), menuoverlap(false) {}

    /// destructor
    virtual ~CicoSCLayerConf() {}

    /// dump log this class member variables
    void dumpConf(void)
    {
        ICO_DBG("  layer: id=%d name=%s type=%d menuoverlap=%s",
                id, name.c_str(), type, menuoverlap ? "true" : "false");
    }

public:
    int         id;          ///< layer id
    std::string name;        ///< layer name
    int         type;        ///< layer type
    bool        menuoverlap; ///< menu overlap flag
};

//==========================================================================
/**
 *  @brief  This class holds display zone information of system config
 */
//==========================================================================
class CicoSCDisplayZoneConf
{
public:
    /// default constructor
    CicoSCDisplayZoneConf()
        : id(-1), name(""), fullname(""), x(-1), y(-1), w(-1), h(-1) {}

    /// destructor
    virtual ~CicoSCDisplayZoneConf() {}

    /// dump log this class member variables
    void dumpConf(void)
    {
        ICO_DBG("  zone: id=%d name=%s fullname=%s x=%d y=%d "
                 "w=%d h=%d overlap=%s",
                 id, name.c_str(), fullname.c_str(),
                 x, y, w, h, overlapStr.c_str());
    }

public:
    int    id;              ///< display zone id
    std::string name;       ///< display zone name
    std::string fullname;   ///< display zone name
    int    x;               ///< display zone x position
    int    y;               ///< display zone y position
    int    w;               ///< display zone width
    int    h;               ///< display zone height
    std::string overlapStr; ///< display overlap
    std::vector<bool> overlap;  ///< display enable table

};

//==========================================================================
/**
 *  @brief  This class holds display information of system config
 */
//==========================================================================
class CicoSCDisplayConf
{
public:
    /// default constructor
    CicoSCDisplayConf()
        : id(-1), name(""), node(-1), no(-1),
          type(-1), width(-1), height(-1), inch(-1) {}

    /// destructor
    virtual ~CicoSCDisplayConf() {}

    /// dump log this class member variables
    void dumpConf(void)
    {
        ICO_DBG("display: id=%d name=%s node=%d no=%d "
                "type=%d width=%d height=%d inch=%d",
                id, name.c_str(), node, no, type, width, height, inch);
    }

public:
    int         id;      ///< display id
    std::string name;    ///< display name
    int         node;    ///< display node id
    int         no;      ///< display number
    int         type;    ///< display type
    int         width;   ///< display width resolution
    int         height;  ///< display height resolution
    int         inch;    ///< display inch size

    // layer config list
    std::vector<CicoSCLayerConf*> layerConfList;

    // zone config list
    std::vector<CicoSCDisplayZoneConf*> zoneConfList;
};

//==========================================================================
/**
 *  @brief  This class holds sound zone information of system config
 */
//==========================================================================
class CicoSCSoundZoneConf
{
public:
    /// default constructor
    CicoSCSoundZoneConf()
        : id(-1), name(""), fullname("") {}

    /// destructor
    virtual ~CicoSCSoundZoneConf() {}

    /// dump log this class member variables
    void dumpConf(void)
    {
        ICO_DBG(" zone: id=%d name=%s fullname=%s",
                id, name.c_str(), fullname.c_str());
    }

public:
    int         id;         ///< sound zone id
    std::string name;       ///< sound zone name
    std::string fullname;   ///< sound zone full name
};

//==========================================================================
/**
 *  @brief  This class holds sound information of system config
 */
//==========================================================================
class CicoSCSoundConf
{
public:
    /// default constructor
    CicoSCSoundConf()
        : id(-1), name(""), no(-1) {}

    /// destructor
    virtual ~CicoSCSoundConf() {}

    /// dump log this class member variables
    void dumpConf(void)
    {
        ICO_DBG("sound: id=%d name=%s no=%d", id, name.c_str(), no);
    }

public:
    int    id;          ///< sound id
    std::string name;   ///< sound name
    int    no;          ///< sound number
    std::vector<CicoSCSoundZoneConf*>  zoneConfList; ///< sound config list
};

//==========================================================================
/**
 *  @brief  This class holds category information of system config
 */
//==========================================================================
class CicoSCCategoryConf
{
public:
    /// default constructor
    CicoSCCategoryConf()
        : id(-1), name(""), type(""),
          view(-1), sound(-1), input(-1), priority(0), rctrl(-1) {}

    /// destructor
    virtual ~CicoSCCategoryConf() {}

    /// dump log this class member variables
    void dumpConf(void)
    {
        ICO_DBG("category: id=%d name=%s type=%s "
                 "view=%d sound=%d input=%d priority=%d",
                 id, name.c_str(), type.c_str(),
                 view, sound, input, priority);
    }

public:
    int         id;         ///< category id
    std::string name;       ///< category name
    std::string type;       ///< category type
    int         view;       ///< category view
    int         sound;      ///< category sound
    int         input;      ///< category input
    int         priority;   ///< category priority
    int         rctrl;      ///< resource control information
};

//==========================================================================
/**
 *  @brief  This class holds application kind information of system config
 */
//==========================================================================
class CicoSCAppKindConf
{
public:
    enum _privilege {
        PRIVILEGE_ALMIGHTY       = 0, ///< almighty privilege
        PRIVILEGE_SYSTEM         = 1, ///< system level
        PRIVILEGE_SYSTEM_AUDIO   = 2, ///< system level(audio only)
        PRIVILEGE_SYSTEM_VISIBLE = 3, ///< system level(visible only)
        PRIVILEGE_MAKER          = 4, ///< maker level
        PRIVILEGE_CERTIFICATE    = 5, ///< certificated
        PRIVILEGE_NONE           = 6, ///< no privilege
    };

    /// default constructor
    CicoSCAppKindConf()
        : id(-1), name(""), privilege(-1), priority(0){}

    /// destructor
    virtual ~CicoSCAppKindConf() {}

    /// dump log this class member variables
    void dumpConf(void)
    {
        ICO_DBG("appkind: id=%d name=%s priority=%d priority=%d",
                 id, name.c_str(), privilege, priority);
    }

public:
    int         id;         ///< application kind id
    std::string name;       ///< application kind name
    int         privilege;  ///< application privilege
    int         priority;   ///< application priority
};

//==========================================================================
/**
 *  @brief  This class holds input device switch information of system config
 */
//==========================================================================
class CicoSCSwitchConf
{
public:
    /// default constructor
    CicoSCSwitchConf()
        : id(-1), name(""), appid(""), keycode(0){}

    /// destructor
    virtual ~CicoSCSwitchConf() {}

    /// dump log this class member variables
    void dumpConf(void)
    {
        ICO_DBG("  switch: id=%d name=%s appid=%s",
                 id, name.c_str(), appid.c_str());
    }

public:
    int         id;         ///< input device switch id
    std::string name;       ///< input device switch name
    std::string appid;      ///< fixed assign apllication name
    int         keycode;    ///< keycode of input device switch
};

//==========================================================================
/**
 *  @brief  This class holds input device information of system config
 */
//==========================================================================
class CicoSCInputDevConf
{
public:
    /// default constructor
    CicoSCInputDevConf()
        : id(-1), name("") {}

    /// destructor
    virtual ~CicoSCInputDevConf() {}

    /// dump log this class member variables
    void dumpConf(void)
    {
        ICO_DBG("inputdev: id=%d name=%s", id, name.c_str());
    }

public:
    int         id;     ///< input device id
    std::string name;   ///< input device name
    std::vector<CicoSCSwitchConf*> switchConfList;  ///< switch list
};

//==========================================================================
/**
 *  @brief  This class holds default information of system config
 */
//==========================================================================
class CicoSCDefaultConf
{
public:
    /// default constructor
    CicoSCDefaultConf()
        : topdir(""), confdir(""),
          node(-1), appkind(-1), category(-1), display(-1), 
          layer(-1), displayzone(-1), sound(-1), soundzone(-1),
          inputdev(-1), inputsw(-1) {};

    /// destructor
    virtual ~CicoSCDefaultConf() {}

    /// dump log this class member variables
    void dumpConf(void)
    {
        ICO_DBG("default: topdir=%s confdir=%s "
                "node=%d appkind=%d category=%d "
                "display=%d layer=%d displayzone=%d "
                "sound=%d soundzone=%d inputdev=%d inputsw=%d",
                topdir.c_str(), confdir.c_str(),
                node, appkind, category, display, layer, displayzone,
                sound, soundzone, inputdev, inputsw);
    }

public:
    std::string topdir;         ///< top directory path
    std::string confdir;        ///< config directory path
    int         node;           ///< default application node id
    int         appkind;        ///< default application kind
    int         category;       ///< default application category
    int         display;        ///< default application display
    int         layer;          ///< default application display layer
    int         displayzone;    ///< default application display zone
    int         sound;          ///< default application sound
    int         soundzone;      ///< default application sound zone
    int         inputdev;       ///< default input device
    int         inputsw;        ///< default input device switch
};

//==========================================================================
/**
 *  @brief  This class holds cpu resource group information of system config
 */
//==========================================================================
class CicoSCCpuResourceGrp
{
public:
    /// default constructor
    CicoSCCpuResourceGrp()
        :m_id(-1), m_bDoIt(false), m_grpNm("") {}

    /// destructor
    virtual ~CicoSCCpuResourceGrp() {}

    /// dump log this class member variables
    void dumpConf(void)
    {
        std::stringstream hightlist;
        {
            std::vector<int>::iterator itr = m_hight.begin();
            for (; itr != m_hight.end(); ++itr) {
                hightlist << " " << *itr;
            }

        }

        std::stringstream lowlist;
        {
            std::vector<int>::iterator itr = m_low.begin();
            for (; itr != m_low.end(); ++itr) {
                lowlist << " " << *itr;
            }

        }

        ICO_DBG("cpu reousrce group; id=%d doIt=%s grpNm=%s, high=%d, low=%d",
                m_id, m_bDoIt? "true": "false", m_grpNm.c_str(),
                hightlist.str().c_str(), lowlist.str().c_str());
    };

public:
    int              m_id;      ///< cpu resource group id
    bool             m_bDoIt;   ///< cpu resouce control enable flag
    std::string      m_grpNm;   ///< cpu resource group name
    std::vector<int> m_hight;   ///< hight threshold list
    std::vector<int> m_low;     ///< low threshold list
};

//==========================================================================
/**
 *  @brief  This class holds system resource information of system config
 */
//==========================================================================
class CicoSCResourceConf
{
public:
    /// default constructor
    CicoSCResourceConf()
        :m_bDoIt(false), m_bLog(false), m_cpuCGRPPath(""), m_sampling(-1),
         m_retryCnt(5), m_lowLimitVal(25), m_highLimitVal(90) {}

    /// destructor
    virtual ~CicoSCResourceConf() {}

    /// dump log this class member variables
    void dumpConf(void)
    {
        ICO_DBG("do:%s, log:%s, dir:%s, smpl:%d r:%d l:%d h:%d sz:%d",
                m_bDoIt? "true": "false", m_bLog? "true": "false",
                m_cpuCGRPPath.c_str(), m_sampling, m_retryCnt, m_lowLimitVal,
                m_highLimitVal, m_cpuCtrl.size());
    }

public:
    bool        m_bDoIt;        ///< resource control enable flag
    bool        m_bLog;         ///< resource control log enable flag
    std::string m_cpuCGRPPath;  ///< cpu cgroup path
    int         m_sampling;     ///< sampling period
    int         m_retryCnt;     ///< retry count
    int         m_lowLimitVal;  ///< low limit value
    int         m_highLimitVal; ///< high limit value
    std::vector<CicoSCCpuResourceGrp*> m_cpuCtrl; ///< cpu control config list
};

//==========================================================================
/**
 *  @brief  This class holds user information of system config
 */
//==========================================================================
class CicoSCUserConf
{
public:
    /// default constructor
    CicoSCUserConf() :m_parent_dir("/home/app/ico") {}
    /// destructor
    virtual ~CicoSCUserConf() {}

    /// dump log this class member variables
    void dumpConf(void)
    {
        ICO_DBG("parent dir:%s", m_parent_dir.c_str());
    }

public:
    std::string m_parent_dir;  ///< parent dir
};

#endif  // __CICO_SC_CONF_H__
// vim:set expandtab ts=4 sw=4:
