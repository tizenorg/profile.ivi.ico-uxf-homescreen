/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

/*========================================================================*/
/**
 *  @file   CicoSystemConfig.cpp
 *
 *  @brief  This file implementation of CicoSystemConfig class
 */
/*========================================================================*/

#include <ico_log.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>

#include "CicoSystemConfig.h"
#include "CicoConf.h"

using namespace boost::property_tree;
//==========================================================================
//  private static variable
//==========================================================================
CicoSystemConfig* CicoSystemConfig::ms_myInstance = NULL;

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSystemConfig::CicoSystemConfig()
{
    m_typeTable[""]          = ICO_NODETYPE_CENTER;
    m_typeTable["center"]    = ICO_NODETYPE_CENTER;
    m_typeTable["meter"]     = ICO_NODETYPE_METER;
    m_typeTable["remote"]    = ICO_NODETYPE_REMOTE;
    m_typeTable["passenger"] = ICO_NODETYPE_PASSENGER;
    m_typeTable["rear"]      = ICO_NODETYPE_REAR;
    m_typeTable["rearleft"]  = ICO_NODETYPE_REARLEFT;
    m_typeTable["rearright"] = ICO_NODETYPE_REARRIGHT;

    m_displayTypeTable[""]          = ICO_DISPLAYTYPE_CENTER;
    m_displayTypeTable["center"]    = ICO_DISPLAYTYPE_CENTER;
    m_displayTypeTable["meter"]     = ICO_DISPLAYTYPE_METER;
    m_displayTypeTable["remote"]    = ICO_DISPLAYTYPE_REMOTE;
    m_displayTypeTable["passenger"] = ICO_DISPLAYTYPE_PASSENGER;
    m_displayTypeTable["rear"]      = ICO_DISPLAYTYPE_REAR;
    m_displayTypeTable["rearleft"]  = ICO_DISPLAYTYPE_REARLEFT;
    m_displayTypeTable["rearright"] = ICO_DISPLAYTYPE_REARRIGHT;

    //
    m_categoryTalbe[""]              = ICO_POLICY_ALWAYS;
    m_categoryTalbe["always"]        = ICO_POLICY_ALWAYS;
    m_categoryTalbe["run"]           = ICO_POLICY_RUNNING;
    m_categoryTalbe["park"]          = ICO_POLICY_PARKED;
    m_categoryTalbe["shift_park"]    = ICO_POLICY_SHIFT_PARKING;
    m_categoryTalbe["shift_back"]    = ICO_POLICY_SHIFT_REVERSES;
    m_categoryTalbe["shift_rev"]     = ICO_POLICY_SHIFT_REVERSES;
    m_categoryTalbe["blinker_left"]  = ICO_POLICY_BLINKER_LEFT;
    m_categoryTalbe["blinker_right"] = ICO_POLICY_BLINKER_RIGHT;

    //
    m_privilegeTable["almighty"]       = ICO_PRIVILEGE_ALMIGHTY;
    m_privilegeTable["system"]         = ICO_PRIVILEGE_SYSTEM;
    m_privilegeTable["system.audio"]   = ICO_PRIVILEGE_SYSTEM_AUDIO;
    m_privilegeTable["system.visible"] = ICO_PRIVILEGE_SYSTEM_VISIBLE;
    m_privilegeTable["maker"]          = ICO_PRIVILEGE_MAKER;
    m_privilegeTable["certificate"]    = ICO_PRIVILEGE_CERTIFICATE;
    m_privilegeTable["none"]           = ICO_PRIVILEGE_NONE;
    m_privilegeTable[""]               = ICO_PRIVILEGE_NONE;
    m_resourceConf = NULL;
    m_userConf = NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSystemConfig::~CicoSystemConfig()
{
    // TODO
}

//--------------------------------------------------------------------------
/**
 *  @brief  Get instance of CicoSystemConfig
 *
 *  @return  pointer of CicoSystemConfig object
 */
//--------------------------------------------------------------------------
CicoSystemConfig*
CicoSystemConfig::getInstance(void)
{
    if (NULL == ms_myInstance) {
        ms_myInstance = new CicoSystemConfig();
        ms_myInstance->load(ICO_SYSTEM_CONFIG_PATH);
    }
    return ms_myInstance;
}

//--------------------------------------------------------------------------
/**
 *  @brief  Get instance of CicoSystemConfig
 *
 *  @param  [in]    confFile    config file name
 *  @return true on success, false on error
 */
//--------------------------------------------------------------------------
bool
CicoSystemConfig::load(const string & confFile)
{
    try {
        ptree root;
        read_xml(confFile, root);

        createNodeConfList(root);
        createDisplayConfList(root);
        createSoundConfList(root);
        createPortConf(root);
        createCategoryConf(root);
        createAppKindConf(root);
        createInputDevList(root);
        createDefaultConf(root);
        createResourceConf(root);
        createUserConf(root);
        createVehicleInfoConf(root);
        createRoleConf(root);
        createPositionOSConf(root);
    }
    catch (...) {
        ICO_ERR("catch exception!");
        return false;
    }
    return true;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const vector<CicoSCNodeConf*>&
CicoSystemConfig::getNodeConfList(void) const
{
    return m_nodeConfList;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const vector<CicoSCDisplayConf*>&
CicoSystemConfig::getDisplayConfList(void) const
{
    return m_displayConfList;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const vector<CicoSCSoundConf*>&
CicoSystemConfig::getSoundConfList(void) const
{
    return m_soundConfList;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const vector<CicoSCInputDevConf*> &
CicoSystemConfig::getInputDevConfList(void) const
{
    return m_inputDevConfList;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const vector<CicoSCCategoryConf*> &
CicoSystemConfig::getCategoryConfList(void) const
{
    return m_categoryConfList;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const vector<CicoSCAppKindConf*> &
CicoSystemConfig::getAppKindConfList(void) const
{
    return m_appKindConfList;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
void
CicoSystemConfig::createNodeConfList(const ptree & root)
{
    //<nodes>
//    if (root.not_found() == root.find("systemconfig")) {
//        ICO_ERR("nodes element not found");
//        return;
//    }
    ptree nodes = root.get_child("systemconfig.nodes");

    BOOST_FOREACH (const ptree::value_type& child, nodes) {
        if (0 != strcmp(child.first.data(),"node")) {
            ICO_ERR("unknown element(%s)", child.first.data());
        }
        optional<int> id = optional<int>(-1);
        optional<string> name;
        optional<string> type;
        optional<string> address;

        id = child.second.get_optional<int>("<xmlattr>.id");
        if (false == id.is_initialized()) {
            ICO_ERR("node.id attr not found");
            continue;
        }
        name = child.second.get_optional<string>("<xmlattr>.name");
        if (false == name.is_initialized()) {
            ICO_ERR("node.name attr not found");
            continue;
        }
        type = child.second.get_optional<string>("type");
        if (false == type.is_initialized()) {
            ICO_ERR("node.type element not found");
            continue;
        }
        address = child.second.get_optional<string>("ipaddress");
        if (false == address.is_initialized()) {
            ICO_ERR("node.address element not found");
            continue;
        }

        CicoSCNodeConf* nodeConf = new CicoSCNodeConf;
        nodeConf->id      = id.get();
        nodeConf->name    = name.get();
        nodeConf->type    = m_typeTable[type.get()];
        nodeConf->address = address.get();
        m_nodeConfList.push_back(nodeConf);

        nodeConf->dumpConf();
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
void
CicoSystemConfig::createDisplayConfList(const ptree & root)
{
    //<displays>
//    if (root.not_found() != root.find("systemconfi.displays")) {
//        ICO_ERR("displays element not found");
//        return;
//    }

    numDisplay = 0;
    ptree displays = root.get_child("systemconfig.displays");
    BOOST_FOREACH (const ptree::value_type& child, displays) {
        optional<int> id = optional<int>(-1);
        optional<string> name;
        optional<string> node;
        optional<int> no = optional<int>(-1);
        optional<string> type;
        optional<int> width = optional<int>(-1);
        optional<int> height = optional<int>(-1);

        id = child.second.get_optional<int>("<xmlattr>.id");
        if (false == id.is_initialized()) {
            ICO_ERR("display.id attr not found");
            continue;
        }
        name = child.second.get_optional<string>("<xmlattr>.name");
        if (false == name.is_initialized()) {
            ICO_ERR("display.name attr not found");
            continue;
        }
        node = child.second.get_optional<string>("node");
        if (false == node.is_initialized()) {
            ICO_ERR("display.node attr not found");
            continue;
        }
        no = child.second.get_optional<int>("no");
        if (false == no.is_initialized()) {
            ICO_ERR("display.no element not found");
            continue;
        }
        type = child.second.get_optional<string>("type");
        if (false == type.is_initialized()) {
            ICO_ERR("display.type element not found");
            continue;
        }
        width = child.second.get_optional<int>("width");
        if (false == width.is_initialized()) {
            ICO_ERR("display.width element not found");
            continue;
        }
        height = child.second.get_optional<int>("height");
        if (false == height.is_initialized()) {
            ICO_ERR("display.height element not found");
            continue;
        }

        CicoSCDisplayConf* displayConf = new CicoSCDisplayConf();
        displayConf->id     = id.get();
        displayConf->name   = name.get();
        displayConf->node   = getNodeIdbyName(node.get());
        displayConf->no     = no.get();
        displayConf->type   = m_displayTypeTable[type.get()];
        displayConf->width  = width.get();
        displayConf->height = height.get();

        displayConf->dumpConf();

        createLayerConf(child, displayConf);
        createDisplayZoneConf(child, displayConf);

        m_displayConfList.push_back(displayConf);
        numDisplay ++;
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
void
CicoSystemConfig::createLayoutConfContainedArea(
    const ptree::value_type& layout,
    const optional<int>& layer_id, const optional<string>& layer_name,
    const optional<int>& layout_id, const optional<string>& layout_name,
    CicoSCDisplayConf* displayConf)
{
    ptree areas = layout.second.get_child("areas");
    BOOST_FOREACH (const ptree::value_type& area, areas) {
        optional<int> id = optional<int>(-1);
        optional<string> name;
        optional<int> type = optional<int>(-1);
        optional<bool> overlap = optional<bool>(false);

        id = area.second.get_optional<int>("<xmlattr>.id");
        if (false == id.is_initialized()) {
            ICO_ERR("disply.layer.layout.area.id attr not found");
            continue;
        }
        name = area.second.get_optional<string>("<xmlattr>.name");
        if (false == name.is_initialized()) {
            ICO_ERR("disply.layer.layout.area.name attr not found");
            continue;
        }
        type = area.second.get_optional<int>("type");
        if (false == type.is_initialized()) {
            ICO_ERR("disply.layer.layout.area.type element not found");
            continue;
        }
        overlap = area.second.get_optional<bool>("menuoverlap");
        if (false == overlap.is_initialized()) {
            ICO_ERR("disply.layer.layout.area.overlap element not found");
            continue;
        }

        CicoSCLayerConf* layerConf = new CicoSCLayerConf();
        layerConf->id          = layer_id.get();
        layerConf->name        = layer_name.get();
        layerConf->type        = type.get();
        layerConf->menuoverlap = overlap.get();
        layerConf->layout_id   = layout_id.get();
        layerConf->layout_name = layout_name.get();
        layerConf->area_id     = id.get();
        layerConf->area_name   = name.get();

        displayConf->layerConfList.push_back(layerConf);

        layerConf->dumpConf();
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
void
CicoSystemConfig::createLayerConfContainedLayout(
    const ptree::value_type& layer,
    const optional<int>& layer_id, const optional<string>& layer_name,
    CicoSCDisplayConf* displayConf)
{
    ptree layouts = layer.second.get_child("layouts");
    BOOST_FOREACH (const ptree::value_type& layout, layouts) {
        optional<int> id = optional<int>(-1);
        optional<string> name;

        id = layout.second.get_optional<int>("<xmlattr>.id");
        if (false == id.is_initialized()) {
            ICO_ERR("disply.layer.layout.id attr not found");
            continue;
        }
        name = layer.second.get_optional<string>("<xmlattr>.name");
        if (false == name.is_initialized()) {
            ICO_ERR("disply.layer.layout.name attr not found");
            continue;
        }
        createLayoutConfContainedArea(layout, layer_id, layer_name, id, name,
                                      displayConf);
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
void
CicoSystemConfig::createLayerConf(const ptree::value_type & child,
                                    CicoSCDisplayConf* displayConf)
{
    ptree layers = child.second.get_child("layers");
    BOOST_FOREACH (const ptree::value_type& layer, layers) {
        optional<int> id = optional<int>(-1);
        optional<string> name;
        optional<int> type = optional<int>(-1);
        optional<bool> overlap = optional<bool>(false);

        id = layer.second.get_optional<int>("<xmlattr>.id");
        if (false == id.is_initialized()) {
            ICO_ERR("disply.layer.id attr not found");
            continue;
        }
        name = layer.second.get_optional<string>("<xmlattr>.name");
        if (false == name.is_initialized()) {
            ICO_ERR("disply.layer.name attr not found");
            continue;
        }
        type = layer.second.get_optional<int>("type");
        if (false == type.is_initialized()) {
//             ICO_ERR("disply.layer.type element not found");
            createLayerConfContainedLayout(layer, id, name, displayConf);
            continue;
        }
        overlap = layer.second.get_optional<bool>("menuoverlap");
        if (false == overlap.is_initialized()) {
            ICO_ERR("disply.layer.overlap element not found");
            continue;
        }

        CicoSCLayerConf* layerConf = new CicoSCLayerConf();
        layerConf->id          = id.get();
        layerConf->name        = name.get();
        layerConf->type        = type.get();
        layerConf->menuoverlap = overlap.get();

        displayConf->layerConfList.push_back(layerConf);

        layerConf->dumpConf();
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
void
CicoSystemConfig::createDisplayZoneConf(const ptree::value_type & child,
                                          CicoSCDisplayConf* displayConf)
{
    ptree zones = child.second.get_child("zones");
    BOOST_FOREACH (const ptree::value_type& zone, zones) {
        optional<int>    id = optional<int>(-1);
        optional<string> name;
        optional<string> x;
        optional<string> y;
        optional<string> w;
        optional<string> h;
        optional<bool>   fixed;
        optional<bool>   l;
        optional<bool>   r;
        optional<bool>   t;
        optional<bool>   b;

        id = zone.second.get_optional<int>("<xmlattr>.id");
        if (false == id.is_initialized()) {
            ICO_WRN("zone.id.attr not found");
            continue;
        }
        name = zone.second.get_optional<string>("<xmlattr>.name");
        if (false == name.is_initialized()) {
            ICO_WRN("zone.name.attr not found");
            continue;
        }
        x = zone.second.get_optional<string>("geometry.<xmlattr>.x");
        if (false == x.is_initialized()) {
            ICO_WRN("zone.geometry.x attr not found");
            continue;
        }
        y = zone.second.get_optional<string>("geometry.<xmlattr>.y");
        if (false == y.is_initialized()) {
            ICO_WRN("zone.geometry.y attr not found");
            continue;
        }
        w = zone.second.get_optional<string>("geometry.<xmlattr>.w");
        if (false == w.is_initialized()) {
            ICO_WRN("zone.geometry.w attr not found");
            continue;
        }
        h = zone.second.get_optional<string>("geometry.<xmlattr>.h");
        if (false == h.is_initialized()) {
            ICO_WRN("zone.geometry.h attr not found");
            continue;
        }
        fixed = zone.second.get_optional<bool>("aspect.<xmlattr>.Fixed");
        if (false == fixed.is_initialized()) {
            fixed = optional<bool>(false);
        }
        l = zone.second.get_optional<bool>("aspect.<xmlattr>.AlignLeft");
        if (false == l.is_initialized()) {
            l = optional<bool>(false);
        }
        r = zone.second.get_optional<bool>("aspect.<xmlattr>.AlignRight");
        if (false == r.is_initialized()) {
            r = optional<bool>(false);
        }
        t = zone.second.get_optional<bool>("aspect.<xmlattr>.AlignTop");
        if (false == t.is_initialized()) {
            t = optional<bool>(false);
        }
        b = zone.second.get_optional<bool>("aspect.<xmlattr>.AlignBottom");
        if (false == b.is_initialized()) {
            b = optional<bool>(false);
        }

        CicoSCDisplayZoneConf* zoneConf = new CicoSCDisplayZoneConf();
        zoneConf->id         = id.get();
        zoneConf->name       = name.get();
        zoneConf->fullname   = displayConf->name + "." + name.get();
        zoneConf->x          = calcGeometryExpr(x.get(), displayConf);
        zoneConf->y          = calcGeometryExpr(y.get(), displayConf);
        zoneConf->w          = calcGeometryExpr(w.get(), displayConf);
        zoneConf->h          = calcGeometryExpr(h.get(), displayConf);
        zoneConf->aspectFixed       = fixed.get();
        zoneConf->aspectAlignLeft   = l.get();
        zoneConf->aspectAlignRight  = r.get();
        zoneConf->aspectAlignTop    = t.get();
        zoneConf->aspectAlignBottom = b.get();
        displayConf->zoneConfList.push_back(zoneConf);

        zoneConf->dumpConf();
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
void
CicoSystemConfig::createSoundConfList(const ptree & root)
{
    //<sound>
    ptree sounds = root.get_child("systemconfig.sounds");
    BOOST_FOREACH (const ptree::value_type& child, sounds) {
        optional<int> id = optional<int>(-1);
        optional<string> name;
        optional<int> no = optional<int>(-1);

        id = child.second.get_optional<int>("<xmlattr>.id");
        if (false == id.is_initialized()) {
            continue;
        }

        name = child.second.get_optional<string>("<xmlattr>.name");
        if (false == name.is_initialized()) {
            continue;
        }

        no = child.second.get_optional<int>("no");
        if (false == no.is_initialized()) {
            continue;
        }

        CicoSCSoundConf* soundConf = new CicoSCSoundConf();
        soundConf->id   = id.get();
        soundConf->name = name.get();
        soundConf->no   = no.get();

        soundConf->dumpConf();

        createSoundZoneConf(child, soundConf);

        m_soundConfList.push_back(soundConf);
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
void
CicoSystemConfig::createSoundZoneConf(const ptree::value_type & child,
                                        CicoSCSoundConf* soundConf)
{
    //<sound>
    //  <zone>
    //      ...
    ptree zones = child.second.get_child("zones");
    BOOST_FOREACH (const ptree::value_type& zone, zones) {
        optional<int>    id = optional<int>(-1);
        optional<string> name;

        id = zone.second.get_optional<int>("<xmlattr>.id");
        if (false == id.is_initialized()) {
            continue;
        }
        name = zone.second.get_optional<string>("<xmlattr>.name");
        if (false == name.is_initialized()) {
            continue;
        }

        CicoSCSoundZoneConf* zoneConf = new CicoSCSoundZoneConf();
        zoneConf->id       = id.get();
        zoneConf->name     = name.get();
        zoneConf->fullname = soundConf->name + "." + name.get();
        soundConf->zoneConfList.push_back(zoneConf);

        zoneConf->dumpConf();
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
void
CicoSystemConfig::createPortConf(const ptree & root)
{
    // <ports>
    ptree ports = root.get_child("systemconfig.ports");
    BOOST_FOREACH (const ptree::value_type& child, ports) {
        optional<int> id = optional<int>(-1);
        optional<string> name;

        id = child.second.get_optional<int>("<xmlattr>.id");
        if (false == id.is_initialized()) {
            continue;
        }
        name = child.second.get_optional<string>("<xmlattr>.name");
        if (false == name.is_initialized()) {
            continue;
        }

        switch (id.get()) {
        case 0:
            // TODO
            m_sysconPort = atoi(child.second.data().c_str());
            break;
        case 1:
            m_soundPluginPort = atoi(child.second.data().c_str());
            break;
        default:
            break;
        }
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
void
CicoSystemConfig::createCategoryConf(const ptree & root)
{
    //<category>
    ptree categorys = root.get_child("systemconfig.categorys");
    BOOST_FOREACH (const ptree::value_type& child, categorys) {
        optional<int> id = optional<int>(-1);
        optional<string> name;
        optional<string> type;
        optional<string> view;
        optional<string> sound;
        optional<string> input;
        optional<int> priority = optional<int>(-1);
        optional<int> r_ctrl = optional<int>(-1);

        id = child.second.get_optional<int>("<xmlattr>.id");
        if (false == id.is_initialized()) {
            continue;
        }
        name = child.second.get_optional<string>("<xmlattr>.name");
        if (false == name.is_initialized()) {
            continue;
        }
        type = child.second.get_optional<string>("type");
        if (false == type.is_initialized()) {
            continue;
        }
        view = child.second.get_optional<string>("view");
        if (false == view.is_initialized()) {
            continue;
        }
        sound = child.second.get_optional<string>("sound");
        if (false == sound.is_initialized()) {
            continue;
        }
        input= child.second.get_optional<string>("input");
        if (false == sound.is_initialized()) {
            continue;
        }
        priority = child.second.get_optional<int>("priority");
        if (false == priority.is_initialized()) {
            continue;
        }
        r_ctrl = child.second.get_optional<int>("r_ctrl");
        if (false == r_ctrl.is_initialized()) {
            continue;
        }

        CicoSCCategoryConf* categoryConf = new CicoSCCategoryConf();
        categoryConf->id       = id.get();
        categoryConf->name     = name.get();
        categoryConf->type     = type.get();
        categoryConf->view     = m_categoryTalbe[view.get()];
        categoryConf->sound    = m_categoryTalbe[sound.get()];
        categoryConf->input    = m_categoryTalbe[input.get()];
        categoryConf->priority = priority.get();
        categoryConf->rctrl    = r_ctrl.get();
        m_categoryConfList.push_back(categoryConf);
        categoryConf->dumpConf();
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
void
CicoSystemConfig::createAppKindConf(const ptree & root)
{
    // <appkinds>
    ptree appkinds = root.get_child("systemconfig.appkinds");
    BOOST_FOREACH (const ptree::value_type& child, appkinds) {
        optional<int> id = optional<int>(-1);
        optional<string> name;
        optional<string> privilege;
        optional<int> priority = optional<int>(-1);

        id = child.second.get_optional<int>("<xmlattr>.id");
        if (false == id.is_initialized()) {
            continue;
        }

        name = child.second.get_optional<string>("<xmlattr>.name");
        if (false == name.is_initialized()) {
            continue;
        }

        privilege = child.second.get_optional<string>("privilege");
        if (false == name.is_initialized()) {
            continue;
        }

        priority = child.second.get_optional<int>("priority");
        if (false == priority.is_initialized()) {
            continue;
        }

        CicoSCAppKindConf* appKindConf = new CicoSCAppKindConf();
        appKindConf->id        = id.get();
        appKindConf->name      = name.get();
        appKindConf->privilege = m_privilegeTable[privilege.get()];
        appKindConf->priority  = priority.get();
        m_appKindConfList.push_back(appKindConf);
        appKindConf->dumpConf();
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
void
CicoSystemConfig::createInputDevList(const ptree & root)
{
    //<inputs>
    ptree inputs = root.get_child("systemconfig.inputs");
    BOOST_FOREACH (const ptree::value_type& child, inputs) {
        optional<int> id = optional<int>(-1);
        optional<string> name;

        id = child.second.get_optional<int>("<xmlattr>.id");
        if (false == id.is_initialized()) {
            continue;
        }

        name = child.second.get_optional<string>("<xmlattr>.name");
        if (false == name.is_initialized()) {
            continue;
        }

        CicoSCInputDevConf* inputDevConf = new CicoSCInputDevConf();
        inputDevConf->id   = id.get();
        inputDevConf->name = name.get();
        inputDevConf->dumpConf();

        createSwitchList(child, inputDevConf);
        m_inputDevConfList.push_back(inputDevConf);
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
void
CicoSystemConfig::createSwitchList(const ptree::value_type & child,
                                     CicoSCInputDevConf* inputDevConf)
{
    ptree switchs = child.second.get_child("switchs");
    BOOST_FOREACH (const ptree::value_type& zone, switchs) {
        optional<int> id = optional<int>(-1);
        optional<string> name;
        optional<string> appid;

        id = zone.second.get_optional<int>("<xmlattr>.id");
        if (false == id.is_initialized()) {
            continue;
        }

        name = zone.second.get_optional<string>("<xmlattr>.name");
        if (false == name.is_initialized()) {
            continue;
        }

        appid = zone.second.get_optional<string>("<xmlattr>.appid");
        if (false == appid.is_initialized()) {
            continue;
        }

        CicoSCSwitchConf* switchConf = new CicoSCSwitchConf();
        switchConf->id    = id.get();
        switchConf->name  = name.get();
        switchConf->appid = appid.get();
        switchConf->dumpConf();
        inputDevConf->switchConfList.push_back(switchConf);
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
void
CicoSystemConfig::createDefaultConf(const ptree & root)
{
    // <default>
    ptree defaults = root.get_child("systemconfig.default");

    optional<string> node;
    optional<string> appkind;
    optional<string> category;
    optional<string> display;
    optional<string> layer;
    optional<string> displayzone;
    optional<string> sound;
    optional<string> soundzone;
    optional<string> inputdev;
    optional<string> inputsw;

    node = defaults.get_optional<string>("node");
    if (false == node.is_initialized()) {
        ICO_WRN("default.node element not found");
    }

    appkind = defaults.get_optional<string>("appkind");
    if (false == appkind.is_initialized()) {
        ICO_WRN("default.appkind element not found");
    }

    category = defaults.get_optional<string>("category");
    if (false == category.is_initialized()) {
        ICO_WRN("default.category element not found");
    }

    display = defaults.get_optional<string>("display");
    if (false == display.is_initialized()) {
        ICO_WRN("default.display element not found");
    }

    layer = defaults.get_optional<string>("layer");
    if (false == layer.is_initialized()) {
        ICO_WRN("default.layer element not found");
    }

    displayzone = defaults.get_optional<string>("displayzone");
    if (false == displayzone.is_initialized()) {
        ICO_WRN("default.displayzone element not found");
    }

    sound = defaults.get_optional<string>("sound");
    if (false == sound.is_initialized()) {
        ICO_WRN("default.sound element not found");
    }

    soundzone = defaults.get_optional<string>("soundzone");
    if (false == soundzone.is_initialized()) {
        ICO_WRN("default.soundzone element not found");
    }

    inputdev = defaults.get_optional<string>("inputdev");
    if (false == inputdev.is_initialized()) {
        ICO_WRN("default.inputdev element not found");
    }

    inputsw = defaults.get_optional<string>("inputsw");
    if (false == inputsw.is_initialized()) {
        ICO_WRN("default.inputdsw element not found");
    }

    m_defaultConf = new CicoSCDefaultConf();

    m_defaultConf->node        = getNodeIdbyName(node.get());
    m_defaultConf->appkind     = getAppKindIdbyName(appkind.get());
    m_defaultConf->category    = getCategoryIdbyName(category.get());
    m_defaultConf->display     = getDisplayIdbyName(display.get());
    m_defaultConf->layer       = getLayerIdfbyName(display.get(), layer.get());
    m_defaultConf->displayzone = getDizplayZoneIdbyName(display.get(),
                                                      displayzone.get());
    m_defaultConf->sound       = getSoundIdbyName(sound.get());
    m_defaultConf->soundzone   = getSoundZoneIdbyName(sound.get(),
                                                    soundzone.get());
    m_defaultConf->inputdev    = getInputDevIdbyName(inputdev.get());
    m_defaultConf->inputsw     = getSwitchIdbyName(inputdev.get(),
                                                 inputsw.get());

//TODO
#define ICO_SYC_TOP_EVN     (char*)"SYSCON_TOPDIR"
#define ICO_SYC_TOP_DIR     (char*)"/usr/apps/org.tizen.ico.system-controller"
    /* decide top directory in all configurations       */
    char *topdir = getenv(ICO_SYC_TOP_EVN);
    if (NULL ==  topdir) {
        topdir = ICO_SYC_TOP_DIR;
    }
    m_defaultConf->topdir = topdir;

//TODO
#define ICO_SYC_CONFIG_ENV  (char*)"SYSCON_CONFDIR"
#define ICO_SYC_CONFIG_DIR  (char*)"res/config"
    /* decide top directory in configuration file's     */
    char *confdir = getenv(ICO_SYC_CONFIG_ENV);
    if (NULL != confdir) {
        m_defaultConf->confdir = confdir;
    }
    else {
        m_defaultConf->confdir = m_defaultConf->topdir;
        m_defaultConf->confdir.append("/");
        m_defaultConf->confdir.append(ICO_SYC_CONFIG_DIR);
    }

    m_defaultConf->dumpConf();
}

//--------------------------------------------------------------------------
/**
 *  @brief  resource config class object create
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
void getArray(ptree& t, vector<int>& vec);
static const char* g_resource_cpu = "systemconfig.resource_cpu_control";
void
CicoSystemConfig::createResourceConf(const ptree & root)
{
    m_resourceConf = new CicoSCResourceConf;
    ptree rc = root.get_child(g_resource_cpu);
    bool b = false;
    bool bApp = false;

    ptree lcl_pt;
    string fPath;
    bool bF = false;
    if ((CicoSCDefaultConf*)0 != m_defaultConf) {
        fPath = m_defaultConf->confdir;
        fPath += string("/");
        fPath += string("system_sysres.json");
        struct stat buff;
        /* file check */
        memset(&buff, 0, sizeof(buff));
        if (0 == stat(fPath.c_str(), &buff)) {
            read_json(fPath.c_str(), lcl_pt);
            bF = true;
        }
    }
    if (true==bF) {
        try{
            if (boost::optional<std::string> function =
                lcl_pt.get_optional<std::string>("function")) {
                string v = function.get();
                if (0 == v.compare("yes")) {
                    b = true;
                }
                else if (0 == v.compare("app")) {
                    bApp = true;
                }
            }
        }
        catch (...) {
            ICO_WRN("NG json file(%s)", fPath.c_str());
            b = false;
            bApp = false;
        }
    }
    else {
        optional<string> opts = rc.get_optional<string>("do_it");
        if (true == opts.is_initialized()) {
            string v = opts.get();
            if (0 == v.compare("yes")) {
                b = true;
            }
            else if (0 == v.compare("app")) {
                bApp = true;
            }
        }
    }
    m_resourceConf->m_bDoIt = b;
    m_resourceConf->m_bDoItApp = bApp;

    string dirnm;
    if (true == bF) {
        try {
            if (boost::optional<std::string> ctrl_dir_path =
                lcl_pt.get_optional<std::string>("ctrl_dir_path")) {
                dirnm = ctrl_dir_path.get();
            }
        }
        catch (...) {
            ICO_WRN("NG json file(%s)", fPath.c_str());
        }
    }
    else {
        optional<string> opts2 = rc.get_optional<string>("ctrl_dir_path");
        if (true == opts2.is_initialized()) {
            dirnm = opts2.get();
        }
    }
    if (true == dirnm.empty()) {
        dirnm = "/sys/fs/cgroup/cpu,cpuacct/SCprivate";
    }
    m_resourceConf->m_cpuCGRPPath = dirnm;
    if (false == b) {
        return;
    }
    optional<string> opts;

    opts = rc.get_optional<string>("sampling_wait");
    if (true == opts.is_initialized()) {
        m_resourceConf->m_sampling = atoi(opts.get().c_str());
    }

    opts = rc.get_optional<string>("log");
    if (true == opts.is_initialized()) {
        string v = opts.get();
        if (0 == v.compare("true")) {
            m_resourceConf->m_bLog = true;
        }
    }

    opts = rc.get_optional<string>("retry_cnt");
    if (true == opts.is_initialized()) {
        m_resourceConf->m_retryCnt = atoi(opts.get().c_str());
    }

    opts = rc.get_optional<string>("low_limit_value");
    if (true == opts.is_initialized()) {
        m_resourceConf->m_lowLimitVal = atoi(opts.get().c_str());
    }

    opts = rc.get_optional<string>("high_limit_value");
    if (true == opts.is_initialized()) {
        m_resourceConf->m_highLimitVal = atoi(opts.get().c_str());
    }

    BOOST_FOREACH(ptree::value_type& child, rc) {
        optional<int> id = optional<int>(-1);
        optional<string> name;
        id = child.second.get_optional<int>("<xmlattr>.id");
        if (false == id.is_initialized()) {
            continue;
        }
        name = child.second.get_optional<string>("<xmlattr>.name");
        if (false == name.is_initialized()) {
            continue;
        }
        ptree pth = child.second.get_child("hight_array");
        ptree ptl = child.second.get_child("low_array");

        CicoSCCpuResourceGrp* obj = new CicoSCCpuResourceGrp;
        obj->m_id    = id.get();
        obj->m_bDoIt = b;
        obj->m_grpNm = name.get();
        getArray(pth, obj->m_hight);
        getArray(ptl, obj->m_low);
        m_resourceConf->m_cpuCtrl.push_back(obj);
    }
    m_resourceConf->dumpConf();
}

//--------------------------------------------------------------------------
/**
 *  @brief  vechicle information config class object create
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
void
CicoSystemConfig::createVehicleInfoConf(const ptree & root)
{
    m_vehicleInfoConf = new CicoSCVehicleInfoConf();

    ptree child = root.get_child("systemconfig.vehicle_info");

    optional<int> retryCnt;
    optional<int> waitTime;

    retryCnt = child.get_optional<int>("retryCnt");
    if (false == retryCnt.is_initialized()) {
        ICO_WRN("vehicle_info.retryCnt element not found");
        retryCnt = optional<int>(0);
    }

    waitTime = child.get_optional<int>("waitTime");
    if (false == waitTime.is_initialized()) {
        ICO_WRN("vehicle_info.waitTime element not found");
        waitTime = optional<int>(0);
    }

    m_vehicleInfoConf->retryCnt = retryCnt.get();
    m_vehicleInfoConf->waitTime = waitTime.get();

    BOOST_FOREACH (const ptree::value_type& vinfo, child) {
        optional<int> id;
        optional<string> name;
        optional<string> objname;
        optional<string> property;
        optional<int> zone;
        optional<string> type;

        id = vinfo.second.get_optional<int>("<xmlattr>.id");
        if (false == id.is_initialized()) {
            continue;
        }

        name = vinfo.second.get_optional<string>("<xmlattr>.name");
        if (false == name.is_initialized()) {
            continue;
        }

        property = vinfo.second.get_optional<string>("property");
        if (false == property.is_initialized()) {
            continue;
        }

        objname = vinfo.second.get_optional<string>("objname");
        if (false == objname.is_initialized()) {
            continue;
        }

        zone = vinfo.second.get_optional<int>("zone");
        if (false == zone.is_initialized()) {
            continue;
        }

        type = vinfo.second.get_optional<string>("type");
        if (false == type.is_initialized()) {
            continue;
        }

        CicoSCVIPropertyConf* viprop = new CicoSCVIPropertyConf();
        viprop->id       = id.get();
        viprop->name     = name.get();
        viprop->objname  = objname.get();
        viprop->property = property.get();
        viprop->zone     = zone.get();
        viprop->typestr  = type.get();
        viprop->type     = viprop->typeStrToInt(viprop->typestr);
        m_vehicleInfoConf->properties[id.get()] = viprop;
    }

    m_vehicleInfoConf->dumpConf();
}

//--------------------------------------------------------------------------
/**
 *  @brief  user config class object create
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
static const char* g_login_user_conf = "systemconfig.login_user";
void
CicoSystemConfig::createUserConf(const ptree & root)
{
    m_userConf = new CicoSCUserConf;
    ptree rc = root.get_child(g_login_user_conf);
    optional<string> opts = rc.get_optional<string>("parent_dir");
    if (true == opts.is_initialized()) {
        string v = opts.get();
        if (v.empty()) {
            m_userConf->m_parent_dir = v;
        }
    }
    m_userConf->dumpConf();
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
int
CicoSystemConfig::calcGeometryExpr(const string & expr,
                                     CicoSCDisplayConf* conf)
{
    int     val = 0;
    int     wval;
    int     i, j, sign;
    char    word[32];

    j = 0;
    sign = 0;
    for (i = 0; ; i++)  {
        if (expr[i] == ' ')  continue;
        if ((expr[i] == 0) || (expr[i] == '+') || (expr[i] == '-') ||
            (expr[i] == '*') || (expr[i] == '-') || (expr[i] == '/'))  {
            if (j > 0)  {
                word[j] = 0;
                if ((strcasecmp(word, "dispw") == 0) ||
                    (strcasecmp(word, "width") == 0))   {
                    wval = conf->width;
                }
                else if ((strcasecmp(word, "disph") == 0) ||
                         (strcasecmp(word, "heigh") == 0))  {
                    wval = conf->height;
                }
                else    {
                    wval = strtol(word, (char **)0, 0);
                }
                j = 0;
                if (sign >= 10) {
                    wval = 0 - wval;
                }
                switch (sign % 10)  {
                case 0:                     /* assign       */
                    val = wval;
                    break;
                case 1:                     /* '+'          */
                    val += wval;
                    break;
                case 2:                     /* '-'          */
                    val -= wval;
                    break;
                case 3:                     /* '*'          */
                    val *= wval;
                    break;
                case 4:                     /* '/'          */
                    val /= wval;
                    break;
                default:
                    break;
                }
                sign = 0;
                if (expr[i] == '+')      sign = 1;
                else if (expr[i] == '-') sign = 2;
                else if (expr[i] == '*') sign = 3;
                else if (expr[i] == '/') sign = 4;
                else                    sign = 0;
            }
            else    {
                if ((sign > 0) && (expr[i] == '-'))  {
                    sign += 10;
                }
                else    {
                    if (expr[i] == '+')      sign = 1;
                    else if (expr[i] == '-') sign = 2;
                    else if (expr[i] == '*') sign = 3;
                    else if (expr[i] == '/') sign = 4;
                    else                     sign = 0;
                }
            }
            if (expr[i] == 0)    break;
        }
        else    {
            if (j < ((int)sizeof(word)-1))  {
                word[j++] = expr[i];
            }
        }
    }
    return val;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const CicoSCNodeConf*
CicoSystemConfig::findNodeConfbyName(const string & name)
{
    vector<CicoSCNodeConf*>::const_iterator itr;
    itr = m_nodeConfList.begin();
    for (; itr != m_nodeConfList.end(); ++itr) {
        const CicoSCNodeConf* conf = (*itr);
        if (name == conf->name) {
            return conf;
        }
    }

    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const CicoSCDisplayConf*
CicoSystemConfig::findDisplayConfbyName(const string & name)
{
    vector<CicoSCDisplayConf*>::const_iterator itr;
    itr = m_displayConfList.begin();
    for (; itr != m_displayConfList.end(); ++itr) {
        const CicoSCDisplayConf* conf = (*itr);
        if (name == conf->name) {
            return conf;
        }
    }

    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const CicoSCDisplayConf*
CicoSystemConfig::findDisplayConfbyName(const string& ECU, const string& name)
{
    int nodeId = getNodeIdbyName(ECU);
    vector<CicoSCDisplayConf*>::const_iterator itr;
    itr = m_displayConfList.begin();
    for (; itr != m_displayConfList.end(); ++itr) {
        const CicoSCDisplayConf* conf = (*itr);
        if ((name == conf->name)&&(nodeId == conf->node)) {
            return conf;
        }
    }

    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const CicoSCDisplayConf*
CicoSystemConfig::findDisplayConfbyId(int id)
{
    vector<CicoSCDisplayConf*>::const_iterator itr;
    itr = m_displayConfList.begin();
    for (; itr != m_displayConfList.end(); ++itr) {
        const CicoSCDisplayConf* conf = (*itr);
        if (id == conf->id) {
            return conf;
        }
    }
    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const CicoSCLayerConf*
CicoSystemConfig::findLayerConfbyName(const string & displayName,
                                        const string & layerName)
{
    const CicoSCDisplayConf* displayConf = NULL;
    displayConf = findDisplayConfbyName(displayName);
    if (NULL == displayConf) {
        return NULL;
    }

    vector<CicoSCLayerConf*>::const_iterator itr;
    itr = displayConf->layerConfList.begin();
    for (; itr != displayConf->layerConfList.end(); ++itr) {
        const CicoSCLayerConf* conf = (*itr);
//        if (layerName == conf->name) {
        if ((layerName == conf->name) && (-1 == conf->layout_id)) {
            return conf;
        }
    }

    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const CicoSCLayerConf*
CicoSystemConfig::findLayerConfbyName(const string& ECU,
                                      const string& display,
                                      const string& layer,
                                      const string& layout,
                                      const string& area)
{
    const CicoSCDisplayConf* displayConf = NULL;
    displayConf = findDisplayConfbyName(ECU, display);
    if (NULL == displayConf) {
        return NULL;
    }

    vector<CicoSCLayerConf*>::const_iterator itr;
    itr = displayConf->layerConfList.begin();
    for (; itr != displayConf->layerConfList.end(); ++itr) {
        const CicoSCLayerConf* conf = (*itr);
        if ((layer == conf->name)&&(layout == conf->layout_name)&&
            (area == conf->area_name)) {
            return conf;
        }
    }

    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const CicoSCLayerConf*
CicoSystemConfig::findLayerConfbyIdx(int displayid, int idx)
{
    const CicoSCDisplayConf* displayConf = NULL;
    displayConf = findDisplayConfbyId(displayid);
    if (NULL == displayConf) {
        return NULL;
    }

    vector<CicoSCLayerConf*>::const_iterator itr;
    itr = displayConf->layerConfList.begin();
    for (; itr != displayConf->layerConfList.end(); ++itr) {
        const CicoSCLayerConf* conf = (*itr);
        if (idx <= 0)   {
            return conf;
        }
        idx --;
    }
    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const CicoSCDisplayZoneConf*
CicoSystemConfig::findDisplayZoneConfbyName(const string & displayName,
                                              const string & zoneName)
{
//    ICO_TRA("CicoSystemConfig::findDisplayZoneConfbyName Enter"
//            "(displayName=%s zoneNmae=%s)",
//            displayName.c_str(), zoneName.c_str());

    const CicoSCDisplayConf* displayConf = NULL;
    displayConf = findDisplayConfbyName(displayName);
    if (NULL == displayConf) {
//        ICO_TRA("CicoSystemConfig::findDisplayZoneConfbyName Leave(NULL)");
        return NULL;
    }

    vector<CicoSCDisplayZoneConf*>::const_iterator itr;
    itr = displayConf->zoneConfList.begin();
    for (; itr != displayConf->zoneConfList.end(); ++itr) {
        const CicoSCDisplayZoneConf* conf = (*itr);
        if (zoneName == conf->name) {
//            ICO_TRA("CicoSystemConfig::findDisplayZoneConfbyName Leave"
//                    "(0x%08x)", conf);
            return conf;
        }
    }

//    ICO_TRA("CicoSystemConfig::findDisplayZoneConfbyName Leave(NULL)");
    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const CicoSCSoundConf*
CicoSystemConfig::findSoundConfbyName(const string & name)
{
    vector<CicoSCSoundConf*>::const_iterator itr;
    itr = m_soundConfList.begin();
    for (; itr != m_soundConfList.end(); ++itr) {
        const CicoSCSoundConf* conf = (*itr);
        if (name == conf->name) {
            return conf;
        }
    }

    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const CicoSCSoundZoneConf*
CicoSystemConfig::findSoundZoneConfbyName(const string & soundName,
                                            const string & zoneName)
{
//    ICO_TRA("CicoSystemConfig::findSoundZoneConfbyName Enter"
//            "(soundName=%s zoneNmae=%s)",
//            soundName.c_str(), zoneName.c_str());

    const CicoSCSoundConf* soundConf = NULL;
    soundConf = findSoundConfbyName(soundName);
    if (NULL == soundConf) {
        return NULL;
    }

    vector<CicoSCSoundZoneConf*>::const_iterator itr;
    itr = soundConf->zoneConfList.begin();
    for (; itr != soundConf->zoneConfList.end(); ++itr) {
        const CicoSCSoundZoneConf* conf = (*itr);
        if (zoneName == conf->name) {
            return conf;
        }
    }

    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const CicoSCInputDevConf*
CicoSystemConfig::findInputDevConfbyName(const string & name)
{
    vector<CicoSCInputDevConf*>::const_iterator itr;
    itr = m_inputDevConfList.begin();
    for (; itr != m_inputDevConfList.end(); ++itr) {
        const CicoSCInputDevConf* conf = (*itr);
        if (name == conf->name) {
            return conf;
        }
    }

    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const CicoSCSwitchConf*
CicoSystemConfig::findSwitchConfbyName(const string & inputDevName,
                                         const string & switchName)
{
    const CicoSCInputDevConf* inputDevConf = NULL;
    inputDevConf = findInputDevConfbyName(inputDevName);
    if (NULL == inputDevConf) {
        ICO_WRN("name(%s) input device config not found.",
                 inputDevName.c_str());
        return NULL;
    }

    vector<CicoSCSwitchConf*>::const_iterator itr;
    itr = inputDevConf->switchConfList.begin();
    for (; itr != inputDevConf->switchConfList.end(); ++itr) {
        const CicoSCSwitchConf* conf = (*itr);
        if (switchName == conf->name) {
            return conf;
        }
    }

    ICO_WRN("name(%s) switch config not found.", switchName.c_str());
    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const CicoSCAppKindConf*
CicoSystemConfig::findAppKindConfbyName(const string & name)
{
    vector<CicoSCAppKindConf*>::const_iterator itr;
    itr = m_appKindConfList.begin();
    for (; itr != m_appKindConfList.end(); ++itr) {
        const CicoSCAppKindConf* conf = (*itr);
        if (name == conf->name) {
            return conf;
        }
    }

    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const CicoSCDisplayZoneConf*
CicoSystemConfig::findDisplayZoneConfbyId(int id)
{
    vector<CicoSCDisplayConf*>::const_iterator itr;
    itr = m_displayConfList.begin();
    for (; itr != m_displayConfList.end(); ++itr) {
        vector<CicoSCDisplayZoneConf*>::const_iterator itr2;
        itr2 = (*itr)->zoneConfList.begin();
        for (; itr2 != (*itr)->zoneConfList.end(); ++itr2) {
            if (id == (*itr2)->id) {
                return *itr2;
            }
        }
    }

    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const CicoSCSoundZoneConf*
CicoSystemConfig::findSoundZoneConfbyId(int id)
{
    vector<CicoSCSoundConf*>::const_iterator itr;
    itr = m_soundConfList.begin();
    for (; itr != m_soundConfList.end(); ++itr) {
        vector<CicoSCSoundZoneConf*>::const_iterator itr2;
        itr2 = (*itr)->zoneConfList.begin();
        for (; itr2 != (*itr)->zoneConfList.end(); ++itr2) {
            if (id == (*itr2)->id) {
                return *itr2;
            }
        }
    }

    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const CicoSCCategoryConf*
CicoSystemConfig::findCategoryConfbyName(const string & name)
{
    vector<CicoSCCategoryConf*>::const_iterator itr;
    itr = m_categoryConfList.begin();
    for (; itr != m_categoryConfList.end(); ++itr) {
        const CicoSCCategoryConf* conf = (*itr);
        if (name == conf->name) {
            //return const_cast<CicoSCCategoryConf*>(itr->pointer);
            return conf;
        }
    }

    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const CicoSCCategoryConf*
CicoSystemConfig::findCategoryConfbyId(int id)
{
    vector<CicoSCCategoryConf*>::const_iterator itr;
    itr = m_categoryConfList.begin();
    for (; itr != m_categoryConfList.end(); ++itr) {
        const CicoSCCategoryConf* conf = (*itr);
        if (id == conf->id) {
            return conf;
        }
    }

    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const CicoSCAppKindConf *
CicoSystemConfig::findAppKindConfbyId(int id)
{
    vector<CicoSCAppKindConf*>::const_iterator itr;
    itr = m_appKindConfList.begin();
    for (; itr != m_appKindConfList.end(); ++itr) {
        if (id == (*itr)->id) {
            return *itr;
        }
    }

    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const CicoSCDefaultConf*
CicoSystemConfig::getDefaultConf(void)
{
    return m_defaultConf;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
int
CicoSystemConfig::getNodeIdbyName(const string & name)
{
    const CicoSCNodeConf* conf = NULL;
    conf = findNodeConfbyName(name);
    if (NULL == conf) {
        return -1;
    }

    return conf->id;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
int
CicoSystemConfig::getDisplayIdbyName(const string & name)
{
    const CicoSCDisplayConf* conf = NULL;
    conf = findDisplayConfbyName(name);
    if (NULL == conf) {
        return -1;
    }

    return conf->id;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
int
CicoSystemConfig::getDisplayIdbyNo(int no)
{
    vector<CicoSCDisplayConf*>::const_iterator itr;
    itr = m_displayConfList.begin();
    for (; itr != m_displayConfList.end(); ++itr) {
        const CicoSCDisplayConf* conf = (*itr);
        if (no == conf->no) {
            return conf->id;
        }
    }
    return -1;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
int
CicoSystemConfig::getDisplayIdbyType(int type)
{
    vector<CicoSCDisplayConf*>::const_iterator itr;
    itr = m_displayConfList.begin();
    for (; itr != m_displayConfList.end(); ++itr) {
        const CicoSCDisplayConf* conf = (*itr);
        if (type == conf->type) {
            return conf->id;
        }
    }
    return -1;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
int
CicoSystemConfig::getNumberofDisplay(void)
{
    return numDisplay;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
int
CicoSystemConfig::getLayerIdfbyName(const string & displayName,
                                      const string & layerName)
{
    const CicoSCLayerConf* conf = NULL;
    conf = findLayerConfbyName(displayName, layerName);
    if (NULL == conf) {
        return -1;
    }

    return conf->id;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
int
CicoSystemConfig::getLayerIdfbyName(const string& ECU,
                                    const string& display,
                                    const string& layer,
                                    const string& layout,
                                    const string& area)
{
    const CicoSCLayerConf* conf = NULL;
    conf = findLayerConfbyName(ECU, display, layer, layout, area);
    if (NULL == conf) {
        return -1;
    }

    return conf->id;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
int
CicoSystemConfig::getDizplayZoneIdbyName(const string & displayName,
                                           const string & zoneName)
{
    const CicoSCDisplayZoneConf* conf = NULL;
    conf = findDisplayZoneConfbyName(displayName, zoneName);
    if (NULL == conf) {
        return -1;
    }

    return conf->id;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
int
CicoSystemConfig::getDizplayZoneIdbyFullName(const string & zoneFullName)
{
    string::size_type index = zoneFullName.find(".", 0);
    if (string::npos == index) {
        return -1;
    }
    string displayName = zoneFullName.substr(0, index);
    string zoneName    = zoneFullName.substr(index+1);

    const CicoSCDisplayZoneConf* conf = NULL;
    conf = findDisplayZoneConfbyName(displayName, zoneName);
    if (NULL == conf) {
        return -1;
    }

    return conf->id;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
int
CicoSystemConfig::getSoundIdbyName(const string & name)
{
    const CicoSCSoundConf* conf = NULL;
    conf = findSoundConfbyName(name);
    if (NULL == conf) {
        return -1;
    }

    return conf->id;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
int
CicoSystemConfig::getSoundZoneIdbyName(const string & soundName,
                                         const string & zoneName)
{
    const CicoSCSoundZoneConf* conf = NULL;
    conf = findSoundZoneConfbyName(soundName, zoneName);
    if (NULL == conf) {
        return -1;
    }

    return conf->id;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
int
CicoSystemConfig::getSoundZoneIdbyFullName(const string & zoneFullName)
{
    string::size_type index = zoneFullName.find(".", 0);
    if (string::npos == index) {
        return -1;
    }
    string soundName = zoneFullName.substr(0, index);
    string zoneName  = zoneFullName.substr(index+1);

    const CicoSCSoundZoneConf* conf = NULL;
    conf = findSoundZoneConfbyName(soundName, zoneName);
    if (NULL == conf) {
        return -1;
    }

    return conf->id;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
int
CicoSystemConfig::getInputDevIdbyName(const string & name)
{
    const CicoSCInputDevConf* conf = NULL;
    conf = findInputDevConfbyName(name);
    if (NULL == conf) {
        return -1;
    }

    return conf->id;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
int
CicoSystemConfig::getSwitchIdbyName(const string & inputDevName,
                                      const string & switchName)
{
    const CicoSCSwitchConf* conf = NULL;
    conf = findSwitchConfbyName(inputDevName, switchName);
    if (NULL == conf) {
        return -1;
    }

    return conf->id;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
int
CicoSystemConfig::getAppKindIdbyName(const string & name)
{
    const CicoSCAppKindConf* conf = NULL;
    conf = findAppKindConfbyName(name);
    if (NULL == conf) {
        return -1;
    }

    return conf->id;
}

//--------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
int
CicoSystemConfig::getCategoryIdbyName(const string & name)
{
    const CicoSCCategoryConf* conf = NULL;
    conf = findCategoryConfbyName(name);
    if (NULL == conf) {
        return -1;
    }

    return conf->id;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get category config object class
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
const CicoSCCategoryConf*
CicoSystemConfig::getCategoryObjbyCaategoryID(int id)
{
    if (-1 == id) {
        return NULL;
    }
    vector<CicoSCCategoryConf*>::const_iterator itr;
    itr = m_categoryConfList.begin();
    for (; itr != m_categoryConfList.end(); ++itr) {
        const CicoSCCategoryConf* conf = (*itr);
        if (id == conf->id) {
            //return const_cast<CicoSCCategoryConf*>(itr->pointer);
            return conf;
        }
    }
    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief  array xml tree to vector<int>
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
CicoSCVehicleInfoConf*
CicoSystemConfig::getVehicleInfoConf(void)
{
    return m_vehicleInfoConf;
}

//--------------------------------------------------------------------------
/**
 *  @brief  array xml tree to vector<int>
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
void getArray(ptree& t, vector<int>& vec)
{
    vec.clear();
    BOOST_FOREACH (const ptree::value_type& child, t) {
        const int value = lexical_cast<int>(child.second.data());
        vec.push_back(value);
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief  get state number from id key
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
short CicoSystemConfig::getRoleStt(const string& key)
{
    if ((CicoSCRoleConf*)NULL == m_roleConf) {
        return ICO_SYC_ROLE_CONF_DEF;
    }
    short r;
    map<string, short>::iterator p;
    p = m_roleConf->m_stt.find(key);
    if (p == m_roleConf->m_stt.end()) {
        r = m_roleConf->m_def;
    }
    else {
        r = p->second;
    }
    return r;
}

//--------------------------------------------------------------------------
/**
 *  @brief  array xml tree to vector<int>
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
void CicoSystemConfig::createRoleConf(const ptree& root)
{
    m_roleConf = new CicoSCRoleConf();

    ptree roles = root.get_child("systemconfig.on_screen_roles");
    BOOST_FOREACH (const ptree::value_type& role, roles) {
        optional<string>  id;
        optional<int>     stt;
        id = role.second.get_optional<string>("<xmlattr>.id");
        if (false == id.is_initialized()) {
            continue;
        }
        stt =  role.second.get_optional<int>("<xmlattr>.state");
        if (false == stt.is_initialized()) {
            continue;
        }
        string v_key = id.get();
        int v_stt = stt.get();
        if (0 == v_key.compare(ICO_SYC_ROLE_CONF_DEF_STR)) { // default define
            m_roleConf->m_def = (short)v_stt;
        }
        else if (0 == v_key.compare(ICO_SYC_ROLE_CONF_RST_STR)) { // reset event number
            m_roleConf->m_rst = (short)v_stt;
        }
        else {
            m_roleConf->m_stt.insert(pair<string, short>(v_key, (short)v_stt));
        }
    }
    m_roleConf->dumpConf();
}

//--------------------------------------------------------------------------
/**
 *  @brief  position OnScreen Config
 *
 *  @param  [in]
 */
//--------------------------------------------------------------------------
void CicoSystemConfig::createPositionOSConf(const ptree& root)
{
    ptree posc = root.get_child("systemconfig.on_screen_position");

    int x = ICO_SYC_ONSCREEN_WINDOW_START_X;
    int y = ICO_SYC_ONSCREEN_WINDOW_START_Y;
    int w = ICO_SYC_ONSCREEN_WINDOW_WIDTH;
    int h = ICO_SYC_ONSCREEN_WINDOW_HEIGHT;

    optional<string> optsx = posc.get_optional<string>("x");
    if (true == optsx.is_initialized()) {
        x = atoi(optsx.get().c_str());
    }

    optional<string> optsy = posc.get_optional<string>("y");
    if (true == optsy.is_initialized()) {
        y = atoi(optsy.get().c_str());
    }

    optional<string> optsw = posc.get_optional<string>("w");
    if (true == optsw.is_initialized()) {
        w = atoi(optsw.get().c_str());
    }

    optional<string> optsh = posc.get_optional<string>("h");
    if (true == optsh.is_initialized()) {
        h = atoi(optsh.get().c_str());
    }

    m_positionOSConf = new CicoSCPositionOSConf(x, y, w, h);
    m_positionOSConf->dumpConf();
}

// vim:set expandtab ts=4 sw=4:
