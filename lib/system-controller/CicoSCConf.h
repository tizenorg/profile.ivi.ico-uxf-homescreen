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
 *  @brief  
 */
//==========================================================================
#ifndef __CICO_SC_CONF_H__
#define __CICO_SC_CONF_H__

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <cstdlib>
using namespace std;
using namespace boost;
using namespace boost::property_tree;

#include "CicoLog.h"
   
typedef enum _node_type {
    ICO_NODETYPE_CENTER     = 0,    //!< Center display
    ICO_NODETYPE_METER      = 1,    //!< Meter display
    ICO_NODETYPE_REMOTE     = 2,    //!< Information device(mobile device)
    ICO_NODETYPE_PASSENGER  = 3,    //!< Passenger display
    ICO_NODETYPE_REAR       = 4,    //!< Rear-seat entertainment
    ICO_NODETYPE_REARLEFT   = 5,    //!< Rear-seat entertainment(Left)
    ICO_NODETYPE_REARRIGHT  = 6,    //!< Rear-seat entertainment(right)
} ico_nodetype_e;

typedef enum _display_type {
    ICO_DISPLAYTYPE_CENTER    = 0,  //!< Center display           
    ICO_DISPLAYTYPE_METER     = 1,  //!< Meter display            
    ICO_DISPLAYTYPE_REMOTE    = 2,  //!< Information device(mobile device)
    ICO_DISPLAYTYPE_PASSENGER = 3,  //!< Passenger display        
    ICO_DISPLAYTYPE_REAR      = 4,  //!< Rear-seat display        
    ICO_DISPLAYTYPE_REARLEFT  = 5,  //!< Rear-seat display(left)  
    ICO_DISPLAYTYPE_REARRIGHT = 6,  //!< Rear-seat display(right) 
} ico_display_type_e;

typedef enum _policy {
    ICO_POLICY_ALWAYS         = 0,  //!< No control
    ICO_POLICY_RUNNING        = 1,  //!< Runnning only
    ICO_POLICY_PARKED         = 2,  //!< Parked only
    ICO_POLICY_SHIFT_PARKING  = 3,  //!< Shift-position is park
    ICO_POLICY_SHIFT_REVERSES = 4,  //!< Shift-position is reverses
    ICO_POLICY_BLINKER_LEFT   = 5,  //!< Blinker is left
    ICO_POLICY_BLINKER_RIGHT  = 6,  //!< Blinker is right
} ico_policy_e;

typedef enum _privilege {
    ICO_PRIVILEGE_ALMIGHTY       = 0,   //!< almighty privilege
    ICO_PRIVILEGE_SYSTEM         = 1,   //!< system level
    ICO_PRIVILEGE_SYSTEM_AUDIO   = 2,   //!< system level(audio only)
    ICO_PRIVILEGE_SYSTEM_VISIBLE = 3,   //!< system level(visible only)
    ICO_PRIVILEGE_MAKER          = 4,   //!< maker level
    ICO_PRIVILEGE_CERTIFICATE    = 5,   //!< certificated
    ICO_PRIVILEGE_NONE           = 6,   //!< no privilege
} ico_privilege_e;

#define ICO_SYC_CONFIG_APPATTR  "app_attr.conf"

//==========================================================================
/**
 *  @brief  
 */
//==========================================================================
class CicoSCNodeConf
{
public:
    CicoSCNodeConf()
        : id(-1), name(""), address(""), type(-1) { }

    void dumpConf(void)
    {
        ICO_DBG("node : id(%d) name(%s) address(%s) type(%d)",
                 id, name.c_str(), address.c_str(), type);
    }

    int    id;      //!< node id
    string name;    //!< node name
    string address; //!< node ip address
    int    type;    //!< node type
};

//==========================================================================
/**
 *  @brief  
 */
//==========================================================================
class CicoSCLayerConf
{
public:
    CicoSCLayerConf()
        : id(-1), name(""), type(-1), menuoverlap(false) {}
    void dumpConf(void)
    {
        ICO_DBG("\tlayer : id(%d) name(%s) type(%d) menuoverlap(%s)",
                id, name.c_str(), type,
                menuoverlap ? "true" : "false");
    }
    int    id;          //!< layer id
    string name;        //!< layer name
    int    type;        //!< layer type
    bool   menuoverlap; //!< menu overlap flag
};

//==========================================================================
/**
 *  @brief  
 */
//==========================================================================
class CicoSCDisplayZoneConf
{
public:
    CicoSCDisplayZoneConf()
        : id(-1), name(""), fullname(""), x(-1), y(-1), w(-1), h(-1){}
    void dumpConf(void)
    {
        ICO_DBG("\tdisplay zone : id(%d) name(%s) fullname(%s) x(%d) y(%d) "
                 "w(%d) h(%d) overlap(%s)",
                 id, name.c_str(), fullname.c_str(),
                 x, y, w, h, overlapStr.c_str());
    }
    int    id;          //!< display zone id
    string name;        //!< display zone name
    string fullname;    //!< display zone name
    int    x;           //!< display zone x position
    int    y;           //!< display zone y position
    int    w;           //!< display zone width
    int    h;           //!< display zone height
    string overlapStr;
    vector<bool> overlap;

};

//==========================================================================
/**
 *  @brief  
 */
//==========================================================================
class CicoSCDisplayConf
{
public:
    CicoSCDisplayConf()
        : id(-1), name(""), node(-1), no(-1),
          type(-1), width(-1), height(-1), inch(-1){}
    void dumpConf(void)
    {
        ICO_DBG("display : id(%d) name(%s) node(%d) no(%d)"
                "type(%d), width(%d) height(%d) inch(%d)",
                id, name.c_str(), node, no, type, width, height, inch);
    }
    int    id;
    string name;
    int    node;
    int    no;
    int    type;
    int    width;
    int    height;
    int    inch;
    vector<CicoSCLayerConf*> layerConfList;
    vector<CicoSCDisplayZoneConf*>  zoneConfList;
};

//==========================================================================
/**
 *  @brief  
 */
//==========================================================================
class CicoSCSoundZoneConf
{
public:
    CicoSCSoundZoneConf()
        : id(-1), name(""){}
    void dumpConf(void)
    {
        ICO_DBG("\tsound zone : id(%d) name(%s)", id, name.c_str());
    }
    int    id;
    string name;
};

//==========================================================================
/**
 *  @brief  
 */
//==========================================================================
class CicoSCSoundConf
{
public:
    CicoSCSoundConf()
        : id(-1), name(""), no(-1){}
    void dumpConf(void)
    {
        ICO_DBG("sound : id(%d) name(%s) no(%d)", id, name.c_str(), no);
    }
    int    id;
    string name;
    int    no;
    vector<CicoSCSoundZoneConf*>  zoneConfList;
};

//==========================================================================
/**
 *  @brief  
 */
//==========================================================================
class CicoSCCategoryConf
{
public:
    CicoSCCategoryConf()
        : id(-1), name(""), type(""),
          view(-1), sound(-1), input(-1), priority(0), rctrl(-1){}
    void dumpConf(void)
    {
        ICO_DBG("category : id(%d) name(%s) type(%s)"
                 "view(%d) sound(%d) input(%d) priority(%d)",
                 id, name.c_str(), type.c_str(),
                 view, sound, input, priority);
    }
    int id;
    string name;
    string type;
    int view;
    int sound;
    int input;
    int priority;
    int rctrl; /* resource control infomation */
};

//==========================================================================
/**
 *  @brief  
 */
//==========================================================================
class CicoSCAppKindConf
{
public:
    CicoSCAppKindConf()
        : id(-1), name(""), privilege(-1), priority(0){}
    void dumpConf(void)
    {
        ICO_DBG("appkind : id(%d) name(%s) priority(%d) priority(%d)",
                 id, name.c_str(), privilege, priority);
    }
    int id;
    string name;
    int privilege;
    int priority;
};

//==========================================================================
/**
 *  @brief  
 */
//==========================================================================
class CicoSCSwitchConf
{
public:
    CicoSCSwitchConf()
        : id(-1), name(""), appid(""), keycode(0){}
    void dumpConf(void)
    {
        ICO_DBG("\tswitch : id(%d) name(%s) appid(%s)",
                 id, name.c_str(), appid.c_str());
    }
    int    id;
    string name;
    string appid;
    int    keycode;
};

//==========================================================================
/**
 *  @brief  
 */
//==========================================================================
class CicoSCInputDevConf
{
public:
    CicoSCInputDevConf()
        : id(-1), name(""){}
    void dumpConf(void)
    {
        ICO_DBG("inputdev : id(%d) name(%s)", id, name.c_str());
    }
    int id;
    string name;
    vector<CicoSCSwitchConf*> switchConfList;
};

//==========================================================================
/**
 *  @brief  
 */
//==========================================================================
class CicoSCDefaultConf
{
public:
    CicoSCDefaultConf()
        : topdir(""), confdir(""),
          node(-1), appkind(-1), category(-1), display(-1), 
          layer(-1), displayzone(-1), sound(-1), soundzone(-1),
          inputdev(-1), inputsw(-1) {};
    void dumpConf(void)
    {
        ICO_DBG("default : topdir(%s) confdir(%s) "
                 "node(%d) appkind(%d) category(%d) "
                 "display(%d) layer(%d) displayzone(%d) "
                 "sound(%d)" "soundzone(%d) "
                 "inputdev(%d) inputsw(%d)",
                 topdir.c_str(), confdir.c_str(),
                 node, appkind, category,
                 display, layer, displayzone,
                 sound, soundzone,
                 inputdev, inputsw);
    }
    string topdir;
    string confdir;
    int node;
    int appkind;
    int category;
    int display;
    int layer;
    int displayzone;
    int sound;
    int soundzone;
    int inputdev;
    int inputsw;
};

//==========================================================================
/**
 *  @brief  CPU resouce control group data
 */
//==========================================================================
class CicoSCCpuResourceGrp
{
public:
    CicoSCCpuResourceGrp()
        :m_id(-1), m_bDoIt(false), m_grpNm(""){};
#if 0
    void getDumpStr(string& s)
    {
        ostringstream strm;
        strm << m_id << ", " << m_bDoIt << ", " << m_grpNm << ", H[";
        for (int i = 0; i < m_hight.size(); << i++) {
            if (i != 0) strm << ", ";
            strm << m_hight[i];
        }
        strm << "], L[";
        for (int i = 0; i < m_low.size(); << i++) {
            if (i != 0) strm << ", ";
            strm << m_low[i];
        }
        strm << "]";
        s = strm.str();
    };
#endif
    void dumpConf(void)
    {
#if 0
        string s;
        getDumpStr(s);
        ICO_DBG(s.c_str());
#else
        ICO_DBG("%d, %s, %s, H%d, L%d", m_id, m_bDoIt? "true": "false",
                m_grpNm.c_str(), m_hight.size(), m_low.size());
#endif
    };

public:
    int   m_id;
    bool  m_bDoIt;
    string m_grpNm;
    vector<int> m_hight;
    vector<int> m_low;
};


//==========================================================================
/**
 *  @brief  
 */
//==========================================================================
class CicoSCResourceConf
{
public:
    CicoSCResourceConf()
        :m_bDoIt(false), m_bLog(false), m_cpuCGRPPath(""), m_sampling(-1) {};
    void dumpConf(void)
    {
        ICO_DBG("monstt:%s, log:%s, dir:%s, smpl:%d sz:%d",
                m_bDoIt? "true": "false", m_bLog? "true": "false",
                m_cpuCGRPPath.c_str(), m_sampling, m_cpuCtrl.size());
    }
public:
    bool m_bDoIt;
    bool m_bLog;
    string m_cpuCGRPPath;
    int  m_sampling;
    vector<CicoSCCpuResourceGrp*> m_cpuCtrl;
};

#endif  // __CICO_SC_CONF_H__
// vim:set expandtab ts=4 sw=4:
