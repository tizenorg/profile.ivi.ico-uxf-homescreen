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
 *  @file   CicoSCSystemConfig.h
 *
 *  @brief  
 */
//==========================================================================
#ifndef __CICO_SC_SYSTEM_CONFIG_H__
#define __CICO_SC_SYSTEM_CONFIG_H__

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

#include <string>
#include <map>
#include <vector>
using namespace std;
using namespace boost;
using namespace boost::property_tree;
    
class CicoSCNodeConf;
class CicoSCLayerConf;
class CicoSCDisplayZoneConf;
class CicoSCDisplayConf;
class CicoSCSoundZoneConf;
class CicoSCSoundConf;
class CicoSCCategoryConf;
class CicoSCAppKindConf;
class CicoSCSwitchConf;
class CicoSCInputDevConf;
class CicoSCDefaultConf;

class CicoSCSystemConfig 
{
public:
    static CicoSCSystemConfig* getInstance(void);

    int load(const string & confFile);

    //vector<CicoSCNodeConf*> m_nodeConfList;
    const vector<CicoSCDisplayConf*>& getDisplayConfList(void) const;
    //vector<CicoSCSoundConf*> m_soundConfList;
    //vector<CicoSCInputDevConf*> m_inputDevConfList;
    //vector<CicoSCCategoryConf*> m_categoryConfList;
    //vector<CicoSCAppKindConf*> m_appKindConfList;

    const CicoSCNodeConf* findNodeConfbyName(const string & name);
    const CicoSCDisplayConf* findDisplayConfbyName(const string & name);
    const CicoSCLayerConf* findLayerConfbyName(const string & displayName,
                                               const string & layerName);
    const CicoSCDisplayZoneConf* findDisplayZoneConfbyName(const string & displayName,
                                                       const string & zoneName);
    const CicoSCSoundConf* findSoundConfbyName(const string & name);
    const CicoSCSoundZoneConf* findSoundZoneConfbyName(const string & soundName,
                                                       const string & zoneName);
    const CicoSCInputDevConf* findInputDevConfbyName(const string & name);
    const CicoSCSwitchConf* findSwitchConfbyName(const string & inputDevName,
                                                 const string & switchName);

    const CicoSCAppKindConf* findAppKindConfbyName(const string & name);
    const CicoSCCategoryConf* findCategoryConfbyName(const string & name);

    const CicoSCDefaultConf* getDefaultConf(void);

    int getNodeIdbyName(const string & name);
    int getDisplayIdbyName(const string & name);
    int getLayerIdfbyName(const string & displayName,
                          const string & layerName);
    int getDizplayZoneIdbyName(const string & displayName,
                               const string & zoneName);
    int getSoundIdbyName(const string & name);
    
    int getSoundZoneIdbyName(const string & soundName,
                             const string & zoneName);
    int getInputDevIdbyName(const string & name);
    int getSwitchIdbyName(const string & inputDevName,
                          const string & switchName);
    int getAppKindIdbyName(const string & name);
    int getCategoryIdbyName(const string & name);


private:
    // default constructor
    CicoSCSystemConfig();

    // destructor
    ~CicoSCSystemConfig();

    // assignment operator
    CicoSCSystemConfig& operator=(const CicoSCSystemConfig &object);

    // copy constructor
    CicoSCSystemConfig(const CicoSCSystemConfig &object);

    void createNodeConfList(const ptree & root);
    void createDisplayConfList(const ptree & root);
    void createLayerConf(const ptree::value_type & child,
                         CicoSCDisplayConf* displayConf);
    void createDisplayZoneConf(const ptree::value_type & child,
                               CicoSCDisplayConf* displayConf);

    void createSoundConfList(const ptree & root);
    void createSoundZoneConf(const ptree::value_type & child,
                             CicoSCSoundConf* soundConf);
    void createPortConf(const ptree & root);
    void createCategoryConf(const ptree & root);
    void createAppKindConf(const ptree & root);
    void createInputDevList(const ptree & root);
    void createSwitchList(const ptree::value_type & child,
                          CicoSCInputDevConf* inputDevConf);
    void createDefaultConf(const ptree & root);
    void createLogConf(const ptree & root);

    int calcGeometryExpr(const string & expr, CicoSCDisplayConf* conf);

private:
    static CicoSCSystemConfig* ms_myInstance;
    string m_confFile;
    map<string,int> m_typeTable;
    map<string,int> m_displayTypeTable;
    map<string,int> m_categoryTalbe;
    map<string,int> m_privilegeTable;
    vector<CicoSCNodeConf*> m_nodeConfList;
    vector<CicoSCDisplayConf*> m_displayConfList;
    vector<CicoSCSoundConf*> m_soundConfList;
    vector<CicoSCInputDevConf*> m_inputDevConfList;
    vector<CicoSCCategoryConf*> m_categoryConfList;
    vector<CicoSCAppKindConf*> m_appKindConfList;
    int m_sysconPort;
    int m_soundPluginPort;
    int m_loglevel;
    bool  m_logflush;
    CicoSCDefaultConf *m_defaultConf;
};
#endif  // __CICO_SC_SYSTEM_CONFIG_H__
// vim:set expandtab ts=4 sw=4:
