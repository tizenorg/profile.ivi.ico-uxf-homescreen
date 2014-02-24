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
 *  @file   CicoSystemConfig.h
 *
 *  @brief  This file is definition of CicoSystemConfig class
 */
//==========================================================================
#ifndef __CICO_SYSTEM_CONFIG_H__
#define __CICO_SYSTEM_CONFIG_H__

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <string>
#include <map>
#include <vector>
using namespace std;
using namespace boost;
using namespace boost::property_tree;
    
//==========================================================================
//  Forward declaration
//==========================================================================
#ifndef __CICO_SC_CONF_H__
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
class CicoSCResourceConf;
class CicoSCUserConf;
class CicoSCVehicleInfoConf;
#endif

//==========================================================================
/**
 *  @brief  This class has function of access to system config information
 */
//==========================================================================
class CicoSystemConfig 
{
public:
    static CicoSystemConfig* getInstance(void);

    bool load(const string & confFile);

    const vector<CicoSCNodeConf*> &    getNodeConfList(void) const;
    const vector<CicoSCDisplayConf*> & getDisplayConfList(void) const;
    const vector<CicoSCSoundConf*> &   getSoundConfList(void) const;
    const vector<CicoSCInputDevConf*> &  getInputDevConfList(void) const;
    const vector<CicoSCCategoryConf*> & getCategoryConfList(void) const;
    const vector<CicoSCAppKindConf*> &  getAppKindConfList(void) const;

    const CicoSCNodeConf* findNodeConfbyName(const string & name);
    const CicoSCDisplayConf* findDisplayConfbyName(const string & name);
    const CicoSCDisplayConf* findDisplayConfbyId(int id);
    const CicoSCLayerConf* findLayerConfbyName(const string & displayName,
                                               const string & layerName);
	const CicoSCLayerConf*  findLayerConfbyIdx(int displayid, int idx);
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

    const CicoSCDisplayZoneConf* findDisplayZoneConfbyId(int id);

    const CicoSCSoundZoneConf* findSoundZoneConfbyId(int id);
    const CicoSCCategoryConf* findCategoryConfbyId(int id);
    const CicoSCAppKindConf* findAppKindConfbyId(int id);

    const CicoSCDefaultConf* getDefaultConf(void);


    int getNodeIdbyName(const string & name);
    int getDisplayIdbyName(const string & name);
    int getLayerIdfbyName(const string & displayName,
                          const string & layerName);
    int getDizplayZoneIdbyName(const string & displayName,
                               const string & zoneName);
    int getDizplayZoneIdbyFullName(const string & zoneFullName);
    int getSoundIdbyName(const string & name);

    int getSoundZoneIdbyName(const string & soundName,
                             const string & zoneName);
    int getSoundZoneIdbyFullName(const string & zoneFullName);
    int getInputDevIdbyName(const string & name);
    int getSwitchIdbyName(const string & inputDevName,
                          const string & switchName);
    int getAppKindIdbyName(const string & name);
    int getCategoryIdbyName(const string & name);
    int getDisplayIdbyNo(int no);
	void setDisplaySize(int id, int width, int height);

    const CicoSCResourceConf* getResourceConf() const
    {
        return m_resourceConf;
    };
    const CicoSCCategoryConf* getCategoryObjbyCaategoryID(int id);

    const CicoSCUserConf* getUserConf() const 
    {
        return m_userConf;
    };

    CicoSCVehicleInfoConf* getVehicleInfoConf(void);

private:
    // default constructor
    CicoSystemConfig();

    // destructor
    ~CicoSystemConfig();

    // assignment operator
    CicoSystemConfig& operator=(const CicoSystemConfig &object);

    // copy constructor
    CicoSystemConfig(const CicoSystemConfig &object);

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
    void createResourceConf(const ptree & root);
    void createUserConf(const ptree & root);
    void createVehicleInfoConf(const ptree & root);

    int calcGeometryExpr(const string & expr, CicoSCDisplayConf* conf);

private:
    static CicoSystemConfig* ms_myInstance;
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
    CicoSCDefaultConf *m_defaultConf;
    CicoSCResourceConf *m_resourceConf;
    CicoSCUserConf *m_userConf;
    CicoSCVehicleInfoConf *m_vehicleInfoConf;
};
#endif  // __CICO_SYSTEM_CONFIG_H__
// vim:set expandtab ts=4 sw=4:
