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
#include <sstream>
#include <vector>
#include <utility>
#include <cstdlib>

#include <ico_log.h>
#include "CicoAilItems.h"
#include "CicoConf.h"
#include "CicoSystemConfig.h"
#include "CicoLibString.h"

using namespace std;

/**
 * @brief constructor
 */
CicoAilItems::CicoAilItems()
{
    ICO_TRA("start");
    init();
    ICO_TRA("end");
}

/**
 * @brief constructor
 * @param pkg package name(appid) by ail information data
 * @param icon icon path and file name by ail information data
 * @param nam name by ail information data
 * @param ctgry category by ail information data
 * @param exe exec path by ail information data
 * @param cateforys environ file data
 */
CicoAilItems::CicoAilItems(const char* pkg, const char* icon,
                               const char* nam, const char* ctgry,
                               const char* typ, const char* exe,
                               const std::string categorys, bool ndisp)
{
    ICO_TRA("start");
    init();
    setup(pkg, icon, nam, ctgry, typ, exe, categorys, ndisp);
    ICO_TRA("end");
}

CicoAilItems::CicoAilItems(const CicoAilItems &s)
{
    m_appid = s.m_appid;    // PACKAGE NAME
    m_icon = s.m_icon;  // ICON
    m_name = s.m_name;  // NAME
    m_execPath = s.m_execPath;  // EXEC
    m_type = s.m_type;  // TYPE

    m_categories = s.m_categories; // CATEGORIES

    m_nodeID = s.m_nodeID;  // "run=xxx" xxx change to id
    m_kindID = s.m_kindID;  // "kind=xxx" xxx change to id
    m_categoryID = s.m_categoryID;  // "category=xxx" xxx change to id

    m_display = s.m_display; //
    m_layer = s.m_layer;
    m_displayZone = s.m_displayZone; // display zone number
    m_soundZone = s.m_soundZone;  // sound zone number
    m_sound = s.m_sound;
    m_inputDev = s.m_inputDev;   // input device number
    m_switch = s.m_switch;
    m_noIcon = s.m_noIcon;
    m_animation = s.m_animation;
    m_animationTime = s.m_animationTime;
    m_invisibleCPU = s.m_invisibleCPU;
    m_memnuOverlap = s.m_memnuOverlap;
    m_autoStart = s.m_autoStart; // auto start flag

    m_group = s.m_group;
    m_location = s.m_location;
    m_resumeShow = s.m_resumeShow;
}

/**
 * @brief
 */
CicoAilItems::~CicoAilItems()
{
    ICO_TRA("start");
    ICO_TRA("end");
}
/**
 * @brief init member 
 */
void CicoAilItems::init()
{
    
    m_appid.clear();      // PACKAGE NAME
    m_icon.clear();       // ICON
    m_name.clear();       // NAME
    m_execPath.clear();   // EXEC
    m_type.clear();       // TYPE

    m_categories.clear(); // CATEGORIES

    const CicoSCDefaultConf* oCSCDC;
    oCSCDC = CicoSystemConfig::getInstance()->getDefaultConf();
    if (NULL != oCSCDC) {
        ICO_DBG("CicoAilItems::init base CicoSCDefaultConf");
        m_nodeID      = oCSCDC->node;
        m_kindID      = oCSCDC->appkind;
        m_categoryID  = oCSCDC->category;

        m_display     = oCSCDC->display;
        m_layer       = oCSCDC->layer;
        m_displayZone = oCSCDC->displayzone;

        m_sound       = oCSCDC->sound;
        m_soundZone   = oCSCDC->soundzone;

        m_inputDev    = oCSCDC->inputdev;
        m_switch      = oCSCDC->inputsw;
    }
    else {
        ICO_DBG("CicoAilItems::init base define");
        m_nodeID      = DINITm_nodeID; // "run=xxx" xxx change to id
        m_kindID      = DINITm_kindID; // "kind=xxx" xxx change to id
        m_categoryID  = DINITm_categoryID; // "category=xxx" xxx change to id

        m_display     = DINITm_display; //
        m_layer       = DINITm_layer;
        m_displayZone = DINITm_displayZone; // display zone

        m_soundZone   = DINITm_soundZone; // sound zone
        m_sound       = DINITm_sound;

        m_inputDev    = DINITm_inputDev; // input device
        m_switch      = DINITm_switch;
    }

    m_noIcon = false;
    m_animation.clear();
    m_animationTime = DINITm_animationTime;
    m_invisibleCPU = DINITm_invisibleCPU;
    m_memnuOverlap = false;
    m_autoStart = false; // auto start flag

    m_group.clear();
    m_location.clear();
    m_resumeShow = false;
}

/**
 * @brief constructor
 * @param pkg package name(appid) by ail information data
 * @param icon icon path and file name by ail information data
 * @param nam name by ail information data
 * @param ctgry category by ail information data
 * @param exe exec path by ail information data
 * @param cateforys environ file data
 */
void CicoAilItems::setup(const char* pkg, const char* icon,
                           const char* nam, const char* ctgry,
                           const char* typ, const char* exe,
                           std::string categorys, bool ndisp)
{
    ICO_TRA("start");
    m_appid.assign(pkg);
    ICO_DBG("package name=%s", pkg? pkg: "(NULL)");

    m_noIcon = ndisp;
    if (NULL != icon) {
        m_icon.assign(icon);
        ICO_DBG("icon path=%s", icon? icon: "(NULL)");
    }

    if (NULL != nam) {
        m_name.assign(nam);
        ICO_DBG("name=%s", nam? nam: "(NULL)");
    }

    if (NULL != exe) {
        m_execPath.assign(exe);
        ICO_DBG("exec path=%s", exe? exe: "(NULL)");
    }

    if (NULL != typ) {
        m_type.assign(typ);
        ICO_DBG("type=%s", typ? typ: "(NULL)");
    }

    categoryParse(categorys);
    ICO_TRA("end");
}

const string s_run("run");
const string s_kind("kind");
const string s_category("category");
const string s_display("display");
const string s_layer("layer");
const string s_dispzone("dispzone");
const string s_sound("sound");
const string s_input("input");
const string s_NoDisplay("NoDisplay");
const string s_Animation("Animation");
const string s_invisiblecpu("invisiblecpu");
const string s_noconfigure("noconfigure");
const string s_menuoverlap("menuoverlap");
const string s_auto("auto");
const string s_noauto("noauto");

const string s_soundzone("soundzone");
const string s_Animation_time("Animation_time");

/**
 * @brief category string parse
 * @parm category string data split code ";"
 */
void CicoAilItems::categoryParse(const std::string categorys)
{
    ICO_TRA("start");
    if (categorys.empty()) {
        ICO_TRA("end no category");
        return;
    }
    vector<string> x;
    split(categorys, ';', x);
    vector<string>::iterator it = x.begin(); // iterator
    string displayName;
    string layerName;
    string dispZoneName;
    string soundName;
    string soundZoneName;
    string inputDevName;
    string switchName;

    displayName.clear();
    layerName.clear();
    dispZoneName.clear();
    soundName.clear();
    soundZoneName.clear();
    inputDevName.clear();
    switchName.clear();

    CicoSystemConfig* CSCSC = CicoSystemConfig::getInstance();

    ICO_DBG("category def size = %d", x.size());
    for (it = x.begin(); it != x.end(); it++) {

        // run
        if (0 == strncasecmp(it->c_str(), s_run.c_str(), s_run.size())) {
            if (true == categoryParseRun(*it)) {
                ICO_DBG("Parse OK %s", it->c_str());
                continue;
            }
        }

        // kind
        if (0 == strncasecmp(it->c_str(), s_kind.c_str(), s_kind.size())) {
            if (true == categoryParseKind(*it)) {
                ICO_DBG("Parse OK %s", it->c_str());
                continue;
            }
        }

        // category
        if (0 == strncasecmp(it->c_str(), s_category.c_str(),
                             s_category.size())) {
            if (true == categoryParseCategory(*it)) {
                ICO_DBG("Parse OK %s", it->c_str());
                continue;
            }
        }

        // display
        if (0 == strncasecmp(it->c_str(), s_display.c_str(),
                             s_display.size())) {
            if (true == categoryParseGetValue(*it, s_display, displayName)) {
                ICO_DBG("Parse OK %s", it->c_str());
                continue;
            }
        }

        // layer
        if (0 == strncasecmp(it->c_str(), s_layer.c_str(), s_layer.size())) {
            if (true == categoryParseGetValue(*it, s_layer, layerName)) {
                ICO_DBG("Parse OK %s", it->c_str());
                continue;
            }
        }

        // dispzone
        if (0 == strncasecmp(it->c_str(), s_dispzone.c_str(),
                             s_dispzone.size())) {
            if (true == categoryParseGetValue(*it, s_dispzone, dispZoneName)) {
                ICO_DBG("Parse OK %s", it->c_str());
                continue;
            }
        }

        // sound / soundzone
        if (0 == strncasecmp(it->c_str(), s_soundzone.c_str(),
                             s_soundzone.size())) {
            if (true == categoryParseGetValue(*it, s_soundzone,
                                              soundZoneName)) {
                ICO_DBG("Parse OK %s", it->c_str());
                continue;
            }
        }
        if (0 == strncasecmp(it->c_str(), s_sound.c_str(), s_sound.size())) {
            if (true == categoryParseGetValue(*it, s_sound, soundName)) {
                ICO_DBG("Parse OK %s", it->c_str());
                continue;
            }
        }

        // input
        if (0 == strncasecmp(it->c_str(), s_input.c_str(), s_input.size())) {
            if (true == categoryParseGetValue(*it, s_input, inputDevName)) {
                ICO_DBG("Parse OK %s", it->c_str());
                continue;
            }
        }
// TODO mk_k not get switchName
//     switchName
// TODO mk_k not get switchName

        // NoDisplay
        if (0 == strncasecmp(it->c_str(), s_NoDisplay.c_str(),
                             s_NoDisplay.size())) {
            if (true == categoryParseNodisplay(*it)) {
                ICO_DBG("Parse OK %s", it->c_str());
                continue;
            }
        }

        // Animation / Animation_time
        if (0 == strncasecmp(it->c_str(), s_Animation.c_str(),
                             s_Animation.size())) {
            if (true == categoryParseAnimation(*it)) {
                ICO_DBG("Parse OK %s", it->c_str());
                continue;
            }
        }

        // invisiblecpu
        if (0 == strncasecmp(it->c_str(), s_invisiblecpu.c_str(),
                             s_invisiblecpu.size())) {
            if (true == categoryParseInvisiblecpu(*it)) {
                ICO_DBG("Parse OK %s", it->c_str());
                continue;
            }
        }

        // noconfigure
        if (0 == strncasecmp(it->c_str(), s_noconfigure.c_str(),
                             s_noconfigure.size())) {
            if (true == categoryParseNoconfigure(*it)) {
                ICO_DBG("Parse OK %s", it->c_str());
                continue;
            }
        }

        // menuoverlap
        if (0 == strncasecmp(it->c_str(), s_menuoverlap.c_str(),
                             s_menuoverlap.size())) {
            if (true == categoryParseMenuoverlap(*it)) {
                ICO_DBG("Parse OK %s", it->c_str());
                continue;
            }
        }

        // auto
        if (0 == strncasecmp(it->c_str(), s_auto.c_str(), s_auto.size())) {
            if (true == categoryParseAuto(*it)) {
                ICO_DBG("Parse OK %s", it->c_str());
                continue;
            }
        }

        // noauto
        if (0 == strncasecmp(it->c_str(), s_noauto.c_str(),
                             s_noauto.size())) {
            if (true == categoryParseNoauto(*it)) {
                ICO_DBG("Parse OK %s", it->c_str());
                continue;
            }
        }

        int categoryID = CSCSC->getCategoryIdbyName(*it);
        const char* ttt = it->c_str()? it->c_str(): "(NULL)";
        ICO_DBG("Category %s->%d", ttt, categoryID);
        if (-1 != categoryID) {
            ICO_DBG("Category %d -> %d(%s)", m_categoryID, categoryID, ttt);
            m_categoryID = categoryID;
            continue;
        }

        ICO_DBG("Parse UNKOWN %s", it->c_str());
    }
    categoryGetDisplay(displayName, layerName, dispZoneName);
    categoryGetSound(soundName, soundZoneName);
    categoryGetInput(inputDevName, switchName);
    ICO_TRA("end");
}

/**
 * @brief category string parse
 * @param category string
 * @retval true parse success
 * @retval false parse fail
 */
bool CicoAilItems::categoryParseRun(const string& s)
{
    ICO_TRA("start");
    const char* ps = s.c_str();
    int c = s_run.size();
    if ('=' != ps[c]) {
        ICO_TRA("end");
        return false;
    }
    // "run=xxx"
// TODO mk_k START getHostID(..) request order
    CicoSystemConfig* CSCSC = CicoSystemConfig::getInstance();
    int nodeID = CSCSC->getNodeIdbyName(&ps[c+1]);
// TODO mk_k int nodeID = -1;
// TODO mk_k int nodeID = CSCSC->getHostID(&ps[c+1]);
// TODO mk_k END getHostID(..) request order
    if (nodeID==-1) {
        ICO_TRA("end");
        return false;
    }
    m_nodeID = nodeID;
    ICO_TRA("end %d", m_nodeID);
    return true;
}

/**
 * @brief category string parse
 * @param category string
 * @retval true parse success
 * @retval false parse fail
 */
bool CicoAilItems::categoryParseKind(const string& s)
{
    ICO_TRA("start");
    const char* ps = s.c_str();
    int c = s_kind.size();
    if ('=' != ps[c]) {
        ICO_TRA("end");
        return false;
    }
    // "kind=xxx" xxx change to id
    CicoSystemConfig* CSCSC = CicoSystemConfig::getInstance();
    int kindID = CSCSC->getAppKindIdbyName(&ps[c+1]);
    if (kindID==-1) {
        ICO_TRA("end");
        return false;
    }
    m_kindID = kindID;
    ICO_TRA("end %d", m_kindID);
    return true;
}

/**
 * @brief category string parse
 * @param category string
 * @retval true parse success
 * @retval false parse fail
 */
bool CicoAilItems::categoryParseCategory(const string& s)
{
    ICO_TRA("start");
    const char* ps = s.c_str();
    int c = s_category.size();
    if ('=' != ps[c]) {
        ICO_TRA("end");
        return false;  // not "category=xxx"
    }
    // "category=xxx" xxx change to id
    CicoSystemConfig* CSCSC = CicoSystemConfig::getInstance();
    int categoryID = CSCSC->getCategoryIdbyName(&ps[c+1]);
    if (categoryID==-1) {
        ICO_TRA("end");
        return false; // unkown category xxx
    }
    m_categoryID = categoryID;
    ICO_TRA("end %d", m_categoryID);
    return true;
}

/**
 * @brief value get
 * @param s "key=value" string
 * @param k key string
 * @param val store value
 * @retval true get value success
 * @retval false get not good
 */
bool CicoAilItems::categoryParseGetValue(const std::string& s,
                                           const std::string& k,
                                           std::string & val)
{
    ICO_TRA("start, %s, %s", s.c_str(), k.c_str());
    const char* ps = s.c_str();
    int c = k.size();
    int c2;
#if 0 // TODO mk_k x!?
    int x = -1;
#endif // TODO mk_k x!?
    if ('=' == ps[c]) {
#if 0 // TODO mk_k x!?
        x = 0;
#endif // TODO mk_k x!?
        c2 = c+1;
    }
#if 0 // TODO mk_k .[0-9] !?
    else if (('.' == ps[c]) &&
             (0 != isdigit((int)ps[c+1])) &&
             ('=' == ps[c+2])) {
        x = (int) ps[c+1] - '0';
        c2 = c+3;

    }
#endif // TODO mk_k .[0-9] !?
    else {
        ICO_TRA("end NG");
        return false;
    }
    val = s.substr(c2);
    ICO_TRA("end %s", val.c_str());
    return true;
}
    
/**
 * @brief get
 * @param disp display key name
 * @param layer display layer key name
 * @param zone display zone key name
 * @retval true parse success
 * @retval false parse fail
 */
bool CicoAilItems::categoryGetDisplay(const string& disp,
                                        const string& layer,
                                        const string& zone)
{
    ICO_TRA("start");
    if (true == disp.empty()) {
        ICO_TRA("end not get display's data");
        return false;
    }

    CicoSystemConfig* CSCSC = CicoSystemConfig::getInstance();
    m_display = CSCSC->getDisplayIdbyName(disp);
    ICO_DBG("display %s->%d", disp.c_str(), m_display);
    if (false == layer.empty()) {
        m_layer = CSCSC->getLayerIdfbyName(disp, layer);
        ICO_DBG("layer %s, %s->%d", disp.c_str(), layer.c_str(), m_layer);
    }
    if (false == zone.empty()) {
        m_displayZone = CSCSC->getDizplayZoneIdbyName(disp, zone);
        ICO_DBG("zone %s, %s->%d", disp.c_str(), zone.c_str(), m_displayZone);
    }
    ICO_TRA("end d:%d l:%d z:%d", m_display, m_layer, m_displayZone);
    return true;
}

/**
 * @brief get sound config data
 * @param sound name key
 * @param soundZone name key
 * @retval true get data
 * @retval false param fail
 */
bool CicoAilItems::categoryGetSound(const string& sound, const string& zone)
{
    ICO_TRA("start");
    if (true == sound.empty()) {
        ICO_TRA("end not get sound's data");
        return false;
    }

    CicoSystemConfig* CSCSC = CicoSystemConfig::getInstance();
    m_sound = CSCSC->getSoundIdbyName(sound);
    ICO_DBG("sound %s->%d", sound.c_str(), m_sound);
    if (false == zone.empty()) {
        m_soundZone = CSCSC->getSoundZoneIdbyName(sound, zone);
        ICO_DBG("zone %s, %s->%d", sound.c_str(), zone.c_str(), m_soundZone);
    }
    ICO_TRA("end s:%d z:%d", m_sound, m_soundZone);
    return true;
}

bool CicoAilItems::categoryGetInput(const string& inputDev,
                                      const string& sw)
{
    ICO_TRA("start");
    if (true == inputDev.empty()) {
        ICO_TRA("end not get input Device's data");
        return false;
    }

    CicoSystemConfig* CSCSC = CicoSystemConfig::getInstance();
    m_sound = CSCSC->getInputDevIdbyName(inputDev);
    ICO_DBG("sound %s->%d", inputDev.c_str(), m_sound);
    if (false == sw.empty()) {
        m_switch = CSCSC->getSwitchIdbyName(inputDev, sw);
        ICO_DBG("switch %s, %s->%d", inputDev.c_str(), sw.c_str(), m_switch);
    }
    ICO_TRA("end idev:%d i:%d", m_inputDev, m_switch);
    return true;
}

/**
 * @brief category string parse
 * @param category string
 * @retval true parse success
 * @retval false parse fail
 */
bool CicoAilItems::categoryParseNodisplay(const string& s)
{
    ICO_TRA("start");
    const char* ps = s.c_str();
    int c = s_NoDisplay.size();
    if ('=' != ps[c]) {
        ICO_TRA("end");
        return false;  // not "NoDisplay=true/false"
    }
    if (0 == strcasecmp(&ps[c+1], "false")) {
        m_noIcon = false;
    }
    else {
        m_noIcon = true;
    }
    ICO_TRA("end %s", m_noIcon? "true": "false");
    return true;
}

/**
 * @brief category string parse
 * @param category string
 * @retval true parse success
 * @retval false parse fail
 */
bool CicoAilItems::categoryParseAnimation(const string& s)
{
    ICO_TRA("start");
    const char* ps = s.c_str();
    int c = s_Animation_time.size();
    bool b_at = false;  // animation time flag off
    if (0 == strncasecmp(ps, s_Animation_time.c_str(), c)) {
        b_at = true;    // flag on
    }
    else {
        c = s_Animation.size(); // not Animation_time
    }
    if ('=' != ps[c]) {
        ICO_TRA("end");
        return false;  // not "Animation=xxx" / "Animation_time=xxx"
    }
    if (true == b_at) {
        // Animation_time
        m_animationTime = (int)strtol(&ps[c+1], (char **)0, 0);
    }
    else {
        // Animation
        m_animation = &ps[c+1];
    }
    ICO_TRA("end");
    return true;
}

/**
 * @brief category string parse
 * @param category string
 * @retval true parse success
 * @retval false parse fail
 */
bool CicoAilItems::categoryParseInvisiblecpu(const string& s)
{
    ICO_TRA("start");
    const char* ps = s.c_str();
    int c = s_invisiblecpu.size();
    if ('=' != ps[c]) {
        ICO_TRA("end");
        return false;  // not "invisibluecpu=xxx"
    }
    if (0 == strcasecmp(&ps[c+1], "yes")) {
        m_invisibleCPU = 100;
    }
    else if (0 != strcasecmp(&ps[c+1], "no")) {
        m_invisibleCPU = (int)strtol(&ps[c+1], (char **)0, 0);
    }
    else { // "no"
        m_invisibleCPU = 0;
    }
    ICO_TRA("end %s,%d", &ps[c+1], m_invisibleCPU);
    return true;
}

/**
 * @brief category string parse
 * @param category string
 * @retval true parse success
 * @retval false parse fail
 */
bool CicoAilItems::categoryParseNoconfigure(const string&)
{
// TODO mk_k
    ICO_TRA("start");
//    m_noconfigure = true;
    ICO_TRA("end");
//    return true;
    return false;
}

/**
 * @brief category string parse
 * @param category string
 * @retval true parse success
 * @retval false parse fail
 */
bool CicoAilItems::categoryParseMenuoverlap(const string&)
{
    ICO_TRA("start");
    m_memnuOverlap = true;
    ICO_TRA("end");
    return true;
}

/**
 * @brief category string parse
 * @param category string
 * @retval true parse success
 * @retval false parse fail
 */
bool CicoAilItems::categoryParseAuto(const string&)
{
    ICO_TRA("start");
    m_autoStart = true;
    ICO_TRA("end");
    return true;
}

/**
 * @brief category string parse
 * @param category string
 * @retval true parse success
 * @retval false parse fail
 */
bool CicoAilItems::categoryParseNoauto(const string&)
{
    ICO_TRA("start");
    m_autoStart = false;
    ICO_TRA("end");
    return true;
}
// vim:set expandtab ts=4 sw=4:
