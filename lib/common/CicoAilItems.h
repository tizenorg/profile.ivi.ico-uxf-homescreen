/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#ifndef CICOAILITEMS_H
#define CICOAILITEMS_H
#include <glib.h>
#include <string>
#include <vector>


#define DINITm_nodeID -1
#define DINITm_kindID -1
#define DINITm_categoryID -1
#define DINITm_display -1
#define DINITm_layer -1
#define DINITm_displayZone -1
#define DINITm_soundZone -1
#define DINITm_sound -1
#define DINITm_inputDev -1
#define DINITm_switch -1
#define DINITm_animationTime 0
#define DINITm_invisibleCPU 0

/**
 * @brief AIL information data class
 *
 */

class CicoAilItems {
public: // member method
    CicoAilItems();
    CicoAilItems(const char* pkg, const char* icon, const char* nam,
                   const char* ctgry, const char* typ, const char* exe,
                   std::string categorys, bool ndisp);
    CicoAilItems(const CicoAilItems& rAilItems);
    ~CicoAilItems();

    void init();
    void setup(const char* pkg, const char* icon, const char* nam,
               const char* ctgry, const char* typ, const char* exe,
               std::string categorys, bool ndisp);

    const CicoAilItems* p() const;

protected: // member method
    void categoryParse(const std::string categorys);
    bool categoryParseRun(const std::string& s);
    bool categoryParseKind(const std::string& s);
    bool categoryParseCategory(const std::string& s);
    bool categoryParseGetValue(const std::string& s, const std::string& k,
                               std::string & val);
    bool categoryGetDisplay(const std::string& disp, const std::string& layer,
                            const std::string& zone);
    bool categoryGetSound(const std::string& sound, const std::string& zone);
    bool categoryGetInput(const std::string& inputDev,
                          const std::string& sw);
    bool categoryParseNodisplay(const std::string& s);
    bool categoryParseAnimation(const std::string& s);
    bool categoryParseInvisiblecpu(const std::string& s);
    bool categoryParseNoconfigure(const std::string& s);
    bool categoryParseMenuoverlap(const std::string& s);
    bool categoryParseAuto(const std::string& s);
    bool categoryParseNoauto(const std::string& s);
    bool categoryParseMultiSurface(const std::string& s);
    bool categoryParseSingleSurface(const std::string& s);

private: // member method

public: // member aria
    std::string m_appid;      // PACKAGE NAME
    std::string m_icon;       // ICON
    std::string m_name;       // NAME
    std::string m_execPath;   // EXEC
    std::string m_type;       // TYPE

    std::string m_categories; // CATEGORIES

    int  m_nodeID;      // "run=xxx" xxx change to id
    int  m_kindID;      // "kind=xxx" xxx change to id
    int  m_categoryID;  // "category=xxx" xxx change to id

    int  m_display;   //
    int  m_layer;
    int  m_displayZone; // display zone
    int  m_soundZone;  // sound zone
    int  m_sound;
    int  m_inputDev;   // input device
    int  m_switch;
    bool m_noIcon;
    std::string m_animation;
    int  m_animationTime;
    int  m_invisibleCPU;
    bool m_memnuOverlap;
    bool m_autoStart; // auto start flag
    bool m_multiSurface;        // multi surface flag

    std::string m_group;
    std::string m_location;
    bool m_resumeShow;
};

inline const CicoAilItems* CicoAilItems::p() const
{
    return this;
}

#endif // CICOAILITEMS_H
// vim:set expandtab ts=4 sw=4:
