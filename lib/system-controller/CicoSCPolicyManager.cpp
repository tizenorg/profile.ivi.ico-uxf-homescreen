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
 *  @file   CicoSCPolicyManager.cpp
 *
 *  @brief  This file implementation of CicoSCPolicyManager class
 */
//==========================================================================

#include <string>
using namespace std;

#include "CicoSCPolicyManager.h"
#include "CicoStateMachine.h"
#include "CicoState.h"
#include "CicoHistoryState.h"
#include "CicoStateMachineCreator.h"
#include "CicoSCPolicyDef.h"
#include "CicoSCResourceManager.h"
#include "CicoLog.h"
#include "ico_syc_error.h"
#include "CicoSCServer.h"
#include "CicoSCMessage.h"
#include "ico_syc_msg_cmd_def.h"
#include "ico_syc_type.h"
#include "CicoSCCommonDef.h"

//==========================================================================
//  define
//==========================================================================

// Defines of D-Bus target.
#define DBUS_SERVICE    "org.automotive.message.broker"
#define DBUS_INTERFACE  "org.freedesktop.DBus.Properties"
#define DBUS_METHOD     "Get"

// Defines of maximum number.
#define AMB_MAX_VHCLINFO    10  /**< maximum number of vehicle info */

// Enumerations of Vehicle information key.
typedef enum {
    AMB_VHCL_SPEED  = 1,  /**< Vehicle speed */
    AMB_SHIFT_POSITION,   /**< Shift position */
    AMB_LIGHT_LEFT,       /**< Light left(Turn left) */
    AMB_LIGHT_RIGHT,      /**< Light right(Turn right) */
    AMB_NIGHT_MODE,       /**< Night mode */
    AMB_MAX_INFO          /**< Maximum number of type */
} _amb_vhcl_key_e;

// 
typedef struct _vehicle_info_property_t {
    int     key;        /* Vehicle Information key */
    const char    *property;  /* D-Bus property name     */
    const char    *path;      /* D-Bus path name         */
    const char    *interface; /* D-Bus interface name    */
} vhcl_info_prop_t;

// structure of Vehicle information data.
typedef struct _vehicle_info_data {
    int             key;    /* Vehicle Information key */
    DBusPendingCall *pending;
    E_DBus_Signal_Handler *handler;
    int             request;
    double          val;
} _vhcldata_t;

_vhcldata_t vhcl_data[AMB_MAX_INFO];

//*========================================================================
//  global variable
//========================================================================

static const vhcl_info_prop_t apf_vhcl_info[] = {
    {
        AMB_VHCL_SPEED,
        "VehicleSpeed",
        "/org/automotive/runningstatus/VehicleSpeed",
        "org.automotive.VehicleSpeed"
    },
    {
        AMB_SHIFT_POSITION,
        "ShiftPosition",
        "/org/automotive/runningstatus/Transmission",
        "org.automotive.Transmission"
    },
#if 0   // use LightStatus
/* use LightStatus, because AMB not support access of TurnSignal by D-Bus   */
    {
        ICO_SYC_VEHICLEINFO_TURN_SIGNAL,
        "TurnSignal",
        "/org/automotive/runningstatus/TurnSignal",
        "org.automotive.TurnSignal"
    },
#else   // use LightStatus
    {
        AMB_LIGHT_LEFT,
        "LeftTurn",
        "/org/automotive/runningstatus/LightStatus",
        "org.automotive.LightStatus"
    },
    {
        AMB_LIGHT_RIGHT,
        "RightTurn",
        "/org/automotive/runningstatus/LightStatus",
        "org.automotive.LightStatus"
    },
#endif   // use LightStatus
    {
        AMB_NIGHT_MODE,
        "NightMode",
        "/org/automotive/custom/NightMode",
        "org.automotive.NightMode"
    },
    { 0, "\0", "\0", "\0" }
};

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSCPolicyManager::CicoSCPolicyManager(CicoSCResourceManager* resourceMgr)
    : m_initialized(false),
      m_dbusConnection(NULL),
      m_ecoreTimer(NULL),
      m_stateMachine(NULL),
      m_resourceMgr(resourceMgr)
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSCPolicyManager::~CicoSCPolicyManager()
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  initialize policy manager
 */
//--------------------------------------------------------------------------
int
CicoSCPolicyManager::initialize(void)
{
    ICO_DBG("CicoSCPolicyManager::initialize Enter");

    int ret = ICO_SYC_EOK;
    ret = initAMB();
    if (ICO_SYC_EOK != ret) {
        return ret;
    }

    ret = initStateMachine();
    if (ICO_SYC_EOK != ret) {
        return ret;
    }

    ICO_DBG("CicoSCPolicyManager::initialize Leave");
    return ret;
}
  
//--------------------------------------------------------------------------
/**
 *  @brief  terminate policy manager
 */
//--------------------------------------------------------------------------
void
CicoSCPolicyManager::terminate(void)
{
    int idx = 0;

    ICO_DBG("CicoSCPolicyManager::terminate Enter");

    for (idx = 0; vhcl_data[idx].key; idx++) {
        if (vhcl_data[idx].handler != NULL) {
            e_dbus_signal_handler_del(m_dbusConnection, 
                                      vhcl_data[idx].handler);
        }
    }

    ICO_DBG("CicoSCPolicyManager::terminate Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  initialize amb connection
 */
//--------------------------------------------------------------------------
int
CicoSCPolicyManager::initAMB(void)
{
    ICO_DBG("CicoSCPolicyManager::initAMB Enter");

    int ret = 0;
    int idx;
    char signalname[64];

    if (true == m_initialized) {
        ICO_DBG("CicoSCPolicyManager::initAMB Leave(EOK)");
        return ICO_SYC_EOK;
    }

    /* Zero clear vhcl_data */
    memset(vhcl_data, 0, sizeof(vhcl_data));

    e_dbus_init();

    /* Get D-Bus connection */
    m_dbusConnection = e_dbus_bus_get(DBUS_BUS_SYSTEM);
    if (! m_dbusConnection) {
        ICO_ERR("dbus_bus_get failed.");
        ICO_ERR("CicoSCPolicyManager::initAMB Leave(EIO)");
        return ICO_SYC_EIO;
    }

    /* receive propertychanged request to AMB */
    for (idx = 0; apf_vhcl_info[idx].key; idx++) {
        memset(signalname, 0, sizeof(signalname));
        /* set vehicleinfo set key */
        vhcl_data[idx].key = apf_vhcl_info[idx].key;

        if (apf_vhcl_info[idx].path[0] == 0) {
            /* currently not support this vehicle information */
            continue;
        }

        strcpy(signalname, apf_vhcl_info[idx].property);
        strcat(signalname, "Changed");
        vhcl_data[idx].handler = e_dbus_signal_handler_add(m_dbusConnection, 
                                                           DBUS_SERVICE, NULL,
                                                           apf_vhcl_info[idx].interface,
                                                           signalname, 
                                                           AMBpropertyChanged, 
                                                           (void*)&vhcl_data[idx].key);

    }

    /* recv response from AMB timer start */
    ret = ecore_init();
    if (ret == 0) {
        ICO_ERR("ecore_init");
        ICO_ERR("CicoSCPolicyManager::initAMB Leave(ENOSYS)");
        return ICO_SYC_ENOSYS;
    }

    m_ecoreTimer = ecore_timer_add(0.2, //TODO
                                   CicoSCPolicyManager::ecoreTimerCB, this);
    if (! m_ecoreTimer)    {
        ICO_ERR("ecore_timer_add failed.");
        ICO_ERR("CicoSCPolicyManager::initAMB Leave(ENOSYS)");
        return ICO_SYC_ENOSYS;
    }

    m_initialized = true;

    ICO_DBG("CicoSCPolicyManager::initAMB Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  send AMB request
 */
//--------------------------------------------------------------------------
int
CicoSCPolicyManager::sendAMBRequest(void)
{
#if 0
//    ICO_DBG("CicoSCPolicyManager::sendAMBRequest Enter");

    DBusMessage *dbus_message = NULL;
    int     idx;
    int     ret = ICO_SYC_EOK;

    for (idx = 0; apf_vhcl_info[idx].key; idx++) {
        /* set vehicle info key */
        vhcl_data[idx].key = apf_vhcl_info[idx].key;

        /* status is pending ? */
        if (vhcl_data[idx].pending)  {
            ICO_WRN("(%s) not complete", apf_vhcl_info[idx].property);
            continue;
        }

        if (apf_vhcl_info[idx].path[0] == 0) {
            /* currently not support this vehicle information */
            continue;
        }

        /* Create send message */
        dbus_message = dbus_message_new_method_call(DBUS_SERVICE,
                                                    apf_vhcl_info[idx].path,
                                                    DBUS_INTERFACE,
                                                    DBUS_METHOD);
        if (! dbus_message) {
            ICO_ERR("dbus_message_new_method_call");
            ret = ICO_SYC_EIO;
        }
        /* Set parameters into message */
        else if (! dbus_message_append_args(dbus_message,
                                            DBUS_TYPE_STRING,
                                            &apf_vhcl_info[idx].interface,
                                            DBUS_TYPE_STRING,
                                            &apf_vhcl_info[idx].property,
                                            DBUS_TYPE_INVALID)) {
            ICO_ERR("dbus_message_append_args");
            ret = ICO_SYC_EIO;
        }
        /* Set destination */
        else if (! dbus_message_set_destination(dbus_message, 
                                                DBUS_SERVICE)) {
            ICO_ERR("dbus_message_set_destination");
            ret = ICO_SYC_EIO;
        }
        /* Send message */
        else if (! dbus_connection_send_with_reply(m_dbusConnection, 
                                                    dbus_message,
                                                    &vhcl_data[idx].pending,
                                                    200)) {
            ICO_ERR("dbus_connection_send");
            vhcl_data[idx].pending = NULL;
            ret = ICO_SYC_EIO;
        }
        else {
            //ICO_DBG("REQUEST req (%s)", apf_vhcl_info[idx].property);
        }

        if (dbus_message) {
            /* Release message */
            dbus_message_unref(dbus_message);
        }
    }

    /* dispatch if data queue exist */
    do  {
        dbus_connection_read_write_dispatch(m_dbusConnection, 0);
    } while (dbus_connection_get_dispatch_status(m_dbusConnection)
             == DBUS_DISPATCH_DATA_REMAINS);

    //ICO_DBG("CicoSCPolicyManager::sendAMBRequest Leave");
    return ret;
#endif
    return 0;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get vehicle information
 */
//--------------------------------------------------------------------------
int
CicoSCPolicyManager::getVehicleInfo(void)
{
    #if 0
    DBusMessage *dbus_message = NULL;
    DBusMessageIter iter_head;
    DBusMessageIter iter;
    int         idx;
    char        type;
    int32_t     i32;
    int16_t     i16;
    uint32_t    u32;
    uint16_t    u16;
    dbus_bool_t b;
    uint8_t     u8;
    double      d64;

    /* dispatch if data queue exist */
    do {
        dbus_connection_read_write_dispatch(m_dbusConnection, 0);
    } while (dbus_connection_get_dispatch_status(m_dbusConnection)
             == DBUS_DISPATCH_DATA_REMAINS);

    /* analyze reply data */
    for (idx = 0; apf_vhcl_info[idx].key; idx++) {
        if (! vhcl_data[idx].pending)    {
            continue;
        }
        if (! dbus_pending_call_get_completed(vhcl_data[idx].pending))   {
            //ICO_WRN("(%s) NOT complete", apf_vhcl_info[idx].property);
            continue;
        }

        dbus_message = dbus_pending_call_steal_reply(vhcl_data[idx].pending);
        if (! dbus_message) {
            //ICO_WRN("(%s) NO reply", apf_vhcl_info[idx].property);
            continue;
        }

        if (dbus_message_get_type(dbus_message) == DBUS_MESSAGE_TYPE_ERROR) {
            dbus_message_unref(dbus_message);
            dbus_pending_call_cancel(vhcl_data[idx].pending);
            vhcl_data[idx].pending = NULL;
            //ICO_ERR("(%s) reply error", apf_vhcl_info[idx].property);
            continue;
        }

        dbus_message_iter_init(dbus_message, &iter_head);
        dbus_message_iter_recurse(&iter_head, &iter);

        type = dbus_message_iter_get_arg_type(&iter);
        switch (type)   {
        case DBUS_TYPE_INT32:
            dbus_message_iter_get_basic(&iter, &i32);
            vhcl_data[idx].val = (double)i32;
            break;
        case DBUS_TYPE_INT16:
            dbus_message_iter_get_basic(&iter, &i16);
            vhcl_data[idx].val = (double)i16;
            break;
        case DBUS_TYPE_UINT32:
            dbus_message_iter_get_basic(&iter, &u32);
            break;
        case DBUS_TYPE_UINT16:
            dbus_message_iter_get_basic(&iter, &u16);
            vhcl_data[idx].val = (double)u16;
            break;
        case DBUS_TYPE_BOOLEAN:
            dbus_message_iter_get_basic(&iter, &b);
            if (b)      vhcl_data[idx].val = (double)1.0;
            else        vhcl_data[idx].val = (double)0.0;
            break;
        case DBUS_TYPE_BYTE:
            dbus_message_iter_get_basic(&iter, &u8);
            vhcl_data[idx].val = (double)u8;
            break;
        case DBUS_TYPE_DOUBLE:
            dbus_message_iter_get_basic(&iter, &d64);
            vhcl_data[idx].val = (double)d64;
            break;
        default:
            ICO_ERR("(%s) illegal data type(0x%02x)",
                    apf_vhcl_info[idx].property, ((int)type) & 0x0ff);
            break;
        }
        //ICO_DBG("REQUEST ans (%s) = %.2f",
        //        apf_vhcl_info[idx].property, vhcl_data[idx].val);

        /* free message and pending */
        dbus_message_unref(dbus_message);
        dbus_pending_call_cancel(vhcl_data[idx].pending);
        vhcl_data[idx].pending = NULL;
    };

    return ICO_SYC_EOK;
#endif
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  ecore timer callback
 */
//--------------------------------------------------------------------------
Eina_Bool
CicoSCPolicyManager::ecoreTimerCB(void *user_data)
{
    CicoSCPolicyManager *policyMgr =
        static_cast<CicoSCPolicyManager*>(user_data);
    policyMgr->recvAMBVehicleInfo();

    return ECORE_CALLBACK_RENEW;
}
//--------------------------------------------------------------------------
/**
 *  @brief  receive AMB vehicle information
 */
//--------------------------------------------------------------------------
void
CicoSCPolicyManager::AMBpropertyChanged(void *data, DBusMessage *msg) 
{
    DBusMessageIter iter, variant;
    int idx;
    char        type;
    int32_t     i32;
    int16_t     i16;
    uint32_t    u32;
    uint16_t    u16;
    dbus_bool_t b;
    uint8_t     u8;
    double      d64;

    int key;
    key = *((int *)data);

    if (!msg || !dbus_message_iter_init(msg, &iter)) {
        ICO_ERR("received illegal message.");
        return;
    }

    for (idx = 0; vhcl_data[idx].key; idx++) {
        if (vhcl_data[idx].key == key) {
            break;
        }
    }
    if (idx == AMB_MAX_INFO) {
        return;
    }

    dbus_message_iter_recurse(&iter, &variant);
    type = dbus_message_iter_get_arg_type(&variant);
    switch (type)   {
    case DBUS_TYPE_INT32:
        dbus_message_iter_get_basic(&variant, &i32);
        vhcl_data[idx].val = (double)i32;
        break;
    case DBUS_TYPE_INT16:
        dbus_message_iter_get_basic(&variant, &i16);
        vhcl_data[idx].val = (double)i16;
        break;
    case DBUS_TYPE_UINT32:
        dbus_message_iter_get_basic(&variant, &u32);
        break;
    case DBUS_TYPE_UINT16:
        dbus_message_iter_get_basic(&variant, &u16);
        vhcl_data[idx].val = (double)u16;
        break;
    case DBUS_TYPE_BOOLEAN:
        dbus_message_iter_get_basic(&variant, &b);
        if (b)      vhcl_data[idx].val = (double)1.0;
        else        vhcl_data[idx].val = (double)0.0;
        break;
    case DBUS_TYPE_BYTE:
        dbus_message_iter_get_basic(&variant, &u8);
        vhcl_data[idx].val = (double)u8;
        break;
    case DBUS_TYPE_DOUBLE:
        dbus_message_iter_get_basic(&variant, &d64);
        vhcl_data[idx].val = (double)d64;
        break;
    default:
        ICO_ERR("(%s) illegal data type(0x%02x)",
                apf_vhcl_info[idx].property, ((int)type) & 0x0ff);
        break;
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief  receive AMB vehicle information
 */
//--------------------------------------------------------------------------
void
CicoSCPolicyManager::recvAMBVehicleInfo(void)
{
//    ICO_DBG("CicoSCPolicyManager::recvAMBVehicleInfo Enter");

    int idx = 0;
    int key = 0;
    bool chgCamera     = false;
    bool chgRegulation = false;
    bool chgNightMode  = false;

    //getVehicleInfo();

    /* get vehicle info values  */
    for (idx = 0; idx < AMB_MAX_VHCLINFO; idx++)   {
        if (vhcl_data[idx].key == 0) break;
        key = vhcl_data[idx].key;
        switch (key) {
        case AMB_VHCL_SPEED:
            (void)sendSMEvent(EVID_VELOCTY, (int)vhcl_data[idx].val);
            if (true == sendSMEvent(EVID_DRVREGULATION)) {
                chgRegulation = true;
            }
            break;
        case AMB_SHIFT_POSITION:
            (void)sendSMEvent(EVID_SHIFTPOS, (int)vhcl_data[idx].val);
            if (true == sendSMEvent(EVID_CAMERA)) {
                chgCamera = true;
            }
            break;
        case AMB_LIGHT_LEFT:
            if (0.0 == vhcl_data[idx].val) {
                (void)sendSMEvent(EVID_TURN_OFF);
            }
            else {
                (void)sendSMEvent(EVID_TURN_LEFT);
            }
            if (true == sendSMEvent(EVID_CAMERA)) {
                chgCamera = true;
            }
            break;
        case AMB_LIGHT_RIGHT:
            if (0.0 == vhcl_data[idx].val) {
                (void)sendSMEvent(EVID_TURN_OFF);
            }
            else {
                (void)sendSMEvent(EVID_TURN_LEFT);
            }
            if (true == sendSMEvent(EVID_CAMERA)) {
                chgCamera = true;
            }
            break;
        case AMB_NIGHT_MODE:
            chgNightMode = sendSMEvent(EVID_NIGHTMODE, (int)vhcl_data[idx].val);
            break;
        default:
            ICO_WRN("not such key (%d)", key);
            break;
        }
    }

    if (true == chgRegulation) {
        ICO_DBG("true == chgRegulation");
        // notify changed state to resource manager
        CicoState* state =
                (CicoState*)m_stateMachine->getState(STID_DRVREGULATION);
        if (NULL != state) {
            vector<const CicoState*> currents;
            state->getCurrentState(currents, CicoStateCore::ELvlTop);
            if (0 != currents.size()) {
                ICO_DBG("current=%s", currents[0]->getName().c_str());
                notifyChangedState(currents[0]->getValue());
            }
        }

        // Notify regulation changed state
        CicoSCMessage *message = new CicoSCMessage();
        message->addRootObject("command", MSG_CMD_NOTIFY_CHANGED_STATE);
        message->addArgObject(MSG_PRMKEY_STATEID, ICO_SYC_STATE_REGULATION);
        if (true == m_policyStates[STID_DRVREGULATION_ON]->isActive()) {
            message->addArgObject(MSG_PRMKEY_STATE, ICO_SYC_STATE_ON);
        }
        else {
            message->addArgObject(MSG_PRMKEY_STATE, ICO_SYC_STATE_OFF);
        }
        CicoSCServer::getInstance()->sendMessageToHomeScreen(message);
    }

    if (true == chgCamera) {
        ICO_DBG("true == chgCamera");
        // notify changed state to resource manager
        CicoState* state = (CicoState*)m_stateMachine->getState(STID_CAMERA);
        if (NULL != state) {
            vector<const CicoState*> currents;
            state->getCurrentState(currents, CicoStateCore::ELvlTop);
            if (0 != currents.size()) {
                ICO_DBG("current=%s", currents[0]->getName().c_str());
                notifyChangedState(currents[0]->getValue());
            }
        }
    }

    if (true == chgNightMode) {
        ICO_DBG("true == chgNightMode");
        // Notify NightMode changed state
        CicoSCMessage *message = new CicoSCMessage();
        message->addRootObject("command", MSG_CMD_NOTIFY_CHANGED_STATE);
        message->addArgObject(MSG_PRMKEY_STATEID, ICO_SYC_STATE_NIGHTMODE);
        if (true == m_policyStates[STID_NIGHTMODE_ON]->isActive()) {
            message->addArgObject(MSG_PRMKEY_STATE, ICO_SYC_STATE_ON);
        }
        else {
            message->addArgObject(MSG_PRMKEY_STATE, ICO_SYC_STATE_OFF);
        }
        CicoSCServer::getInstance()->sendMessageToHomeScreen(message);
    }
        
    /* send request to AMB */
    //sendAMBRequest();

//    ICO_DBG("CicoSCPolicyManager::recvAMBVehicleInfo Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  get policy states
 */
//--------------------------------------------------------------------------
const std::map<int, const CicoState*>&
CicoSCPolicyManager::getPolicyStates(void)
{
    return m_policyStates;
}

//--------------------------------------------------------------------------
/**
 *  @brief  initialize state machine
 */
//--------------------------------------------------------------------------
int
CicoSCPolicyManager::initStateMachine(void)
{
    ICO_DBG("CicoSCPolicyManager::initStateMachine Enter");

    CicoStateMachineCreator creator;

    //TODO
    m_stateMachine = creator.createFile("/usr/apps/org.tizen.ico.system-controller/res/config/policy.json");
    if (NULL == m_stateMachine) {
        ICO_ERR("CicoStateMachineCreator::createFile failed.(reason:%s)",
                creator.getError().c_str());
        return ICO_SYC_ENOSYS;
    }

    int ret = m_stateMachine->start();
    if (ret == 0) {
        ICO_ERR("CicoStateMachine::start failed.");
        return ICO_SYC_ENOSYS;
    }

    vector<CicoStateCore*> objects;
    m_stateMachine->getObjects(objects);
    vector<CicoStateCore*>::iterator itr;
    itr = objects.begin();
    for (; itr != objects.end(); ++itr) {
        const CicoState* state = static_cast<const CicoState*>(*itr);
        m_policyStates[state->getValue()] = state;
#if 1   //-- { debug dump
        ICO_DBG("State=[%-45s] Active=%s",
                state->getName().c_str(),
                state->isActive() ? "true" : "false");
#endif  //-- } debug dump
    }

    {
        std::map<int, const CicoState*>::iterator itr;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE1);
        m_dispZoneStates[1] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE2);
        m_dispZoneStates[2] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE3);
        m_dispZoneStates[3] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE4);
        m_dispZoneStates[4] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE5);
        m_dispZoneStates[5] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE6);
        m_dispZoneStates[6] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE7);
        m_dispZoneStates[7] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE8);
        m_dispZoneStates[8] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE9);
        m_dispZoneStates[9] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE10);
        m_dispZoneStates[10] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE11);
        m_dispZoneStates[11] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE12);
        m_dispZoneStates[12] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE13);
        m_dispZoneStates[13] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE14);
        m_dispZoneStates[14] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE15);
        m_dispZoneStates[15] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE16);
        m_dispZoneStates[16] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE17);
        m_dispZoneStates[17] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE18);
        m_dispZoneStates[18] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE19);
        m_dispZoneStates[19] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE20);
        m_dispZoneStates[20] = itr != m_policyStates.end() ? itr->second : NULL;

        itr = m_policyStates.find(STID_DISPLAY1_ZONE1);
        m_dispZoneStates[21] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE2);
        m_dispZoneStates[22] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE3);
        m_dispZoneStates[23] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE4);
        m_dispZoneStates[24] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE5);
        m_dispZoneStates[25] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE6);
        m_dispZoneStates[26] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE7);
        m_dispZoneStates[27] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE8);
        m_dispZoneStates[28] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE9);
        m_dispZoneStates[29] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE10);
        m_dispZoneStates[30] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE11);
        m_dispZoneStates[31] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE12);
        m_dispZoneStates[32] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE13);
        m_dispZoneStates[33] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE14);
        m_dispZoneStates[34] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE15);
        m_dispZoneStates[35] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE16);
        m_dispZoneStates[36] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE17);
        m_dispZoneStates[37] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE18);
        m_dispZoneStates[38] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE19);
        m_dispZoneStates[39] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE20);
        m_dispZoneStates[40] = itr != m_policyStates.end() ? itr->second : NULL;
    }

    m_soundZoneStates.push_back(NULL);
    m_soundZoneStates.push_back(m_policyStates[STID_SOUND_ZONE1]);
    m_soundZoneStates.push_back(m_policyStates[STID_SOUND_ZONE2]);
    m_soundZoneStates.push_back(m_policyStates[STID_SOUND_ZONE3]);

    m_inputStates.push_back(NULL);
    m_inputStates.push_back(m_policyStates[STID_INPUT1_USING]);
    m_inputStates.push_back(m_policyStates[STID_INPUT2_USING]);

    ICO_DBG("CicoSCPolicyManager::initStateMachine Leave");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  query whether a state transition
 *
 *  @param [in] event_id    trigger event id
 *
 *  @return true on test success, false on test failed
 */
//--------------------------------------------------------------------------
bool
CicoSCPolicyManager::testSMEvent(unsigned short event_id)
{
    CicoEvent event(event_id);
    return m_stateMachine->eventTest(event);
}

//--------------------------------------------------------------------------
/**
 *  @brief  query whether a state transition
 *
 *  @param [in] event_id    trigger event id
 *  @param [in] value       trigger optional integer value
 *
 *  @return true on test success, false on test failed
 */
//--------------------------------------------------------------------------
bool
CicoSCPolicyManager::testSMEvent(unsigned short event_id, int value)
{
    CicoEvent event(event_id, value);
    return m_stateMachine->eventTest(event);
}

//--------------------------------------------------------------------------
/**
 *  @brief  send tigger event
 *
 *  @param [in] event_id    trigger event id
 *
 *  @return true on state transition, false on not state transition
 */
//--------------------------------------------------------------------------
bool
CicoSCPolicyManager::sendSMEvent(unsigned short event_id)
{
    CicoEvent event(event_id);
    return m_stateMachine->eventEntry(event);
}

//--------------------------------------------------------------------------
/**
 *  @brief  send tigger event
 *
 *  @param [in] event_id    trigger event id
 *  @param [in] value       trigger optional integer value
 *
 *  @return true on state transition, false on not state transition
 */
//--------------------------------------------------------------------------
bool
CicoSCPolicyManager::sendSMEvent(unsigned short event_id, int value)
{
    CicoEvent event(event_id, value);
    return m_stateMachine->eventEntry(event);
}

bool
CicoSCPolicyManager::acquireDisplayResource(int type, int zoneid, int priority)
{
    ICO_DBG("CicoSCPolicyManager::acquireDisplayResource Enter"
            "(type=0x%08X zoneid=%d priority=%d)", type, zoneid, priority);

    bool chg = false;

    if (RESID_TYPE_BASIC == type) {
        bool zoneChg = testSMEvent(EVID_DISPLAY_ZONE_ACQUIRE, zoneid);
        bool cateChg = testSMEvent(EVID_DISPLAY0_CATEGORY, priority);
        ICO_DBG("zoneChg=%d cateChg=%d", zoneChg, cateChg);
        if ((true == zoneChg) && (true == cateChg)) {
            sendSMEvent(EVID_DISPLAY_ZONE_ACQUIRE, zoneid);
            sendSMEvent(EVID_DISPLAY0_CATEGORY, priority);
            chg = true;
        }
#if 0   //-- { debug dump
        else {
            std::map<int, const CicoState*>::iterator itr;
            itr = m_policyStates.begin();
            for (; itr != m_policyStates.end(); ++itr) {
                ICO_DBG("State=[%-45s] Active=%s",
                        itr->second->getName().c_str(),
                        itr->second->isActive() ? "true" : "false");
            }
        }
#endif  //-- } debug dump
    }
    else if (RESID_TYPE_INTERRUPT == type) {
        if (1 == zoneid) {
            chg = sendSMEvent(EVID_INTTERPUT_D0_Z1, priority);
        }
        else if (2 == zoneid) {
            chg = sendSMEvent(EVID_INTTERPUT_D0_Z2, priority);
        }
        else if (3 == zoneid) {
            chg = sendSMEvent(EVID_INTTERPUT_D0_Z3, priority);
        }
    }
    else if (RESID_TYPE_ONSCREEN == type) {
        chg = sendSMEvent(EVID_ONSCREEN, priority);
    }
    ICO_DBG("CicoSCPolicyManager::acquireDisplayResource Leave(%s)",
            chg ? "true" : "false");
    return chg;
}

bool
CicoSCPolicyManager::releaseDisplayResource(int zoneid, int priority)
{
    return sendSMEvent(EVID_DISPLAY_ZONE_RELEASE, zoneid);
}

bool
CicoSCPolicyManager::acquireSoundResource(int type, int zoneid, int priority)
{
    ICO_DBG("CicoSCPolicyManager::acquireSoundResource Enter"
            "(type=0x%08X zoneid=%d priority=%d)", type, zoneid, priority);

    bool chg = false;

    if (RESID_TYPE_BASIC == type) {
        bool zoneChg = testSMEvent(EVID_SOUND_ZONE, zoneid);
        bool cateChg = testSMEvent(EVID_SOUND_CATEGORY, priority);
        ICO_DBG("zoneChg=%d cateChg=%d", zoneChg, cateChg);
        if ((true == zoneChg) && (true == cateChg)) {
            sendSMEvent(EVID_SOUND_ZONE, zoneid);
            sendSMEvent(EVID_SOUND_CATEGORY, priority);
            chg = true;
        }
    }
    else if (RESID_TYPE_INTERRUPT == type) {
        if (1 == zoneid) {
            chg = sendSMEvent(EVID_INTTERPUT_S_Z1, priority);
        }
        else if (2 == zoneid) {
            chg = sendSMEvent(EVID_INTTERPUT_S_Z2, priority);
        }
        else if (3 == zoneid) {
            chg = sendSMEvent(EVID_INTTERPUT_S_Z3, priority);
        }
    }

    ICO_DBG("CicoSCPolicyManager::acquireSoundResource Leave(%s)",
            chg ? "true" : "false");
    return chg;
}

bool
CicoSCPolicyManager::releaseSoundResource(int type, int zoneid)
{
    ICO_DBG("CicoSCPolicyManager::acquireSoundResource Enter"
            "(type=%d zoneid=%d)", type, zoneid);

    bool chg = false;
    if (RESID_TYPE_BASIC == type) {
        chg = sendSMEvent(EVID_SOUND_ZONE_NOUSE);
        chg = sendSMEvent(EVID_SOUND_CATEGORY_UNKNOWN);
    }
    else if (RESID_TYPE_INTERRUPT == type) {
        if (1 == zoneid) {
            chg = sendSMEvent(EVID_INTTERPUT_S_Z1_NOOUTPUT);
        }
        else if (2 == zoneid) {
            chg = sendSMEvent(EVID_INTTERPUT_S_Z2_NOOUTPUT);
        }
        else if (3 == zoneid) {
            chg = sendSMEvent(EVID_INTTERPUT_S_Z3_NOOUTPUT);
        }
    }

    ICO_DBG("CicoSCPolicyManager::acquireSoundResource Leave(%s)",
            chg ? "true" : "false");

    return true;
}

bool
CicoSCPolicyManager::acquireInputResource(int input, int priority)
{
    ICO_DBG("CicoSCPolicyManager::acquireInputResource Enter"
            "input=%d priority=%d", input, priority);

    bool chg = false;

    if (1 == input) {
        chg = sendSMEvent(EVID_INPUT1_ACQUIRE, input);
    }
    else if (2 == input) {
        chg = sendSMEvent(EVID_INPUT2_ACQUIRE, input);
    }

    ICO_DBG("CicoSCPolicyManager::acquireInputResource Leave(%s)",
            chg ? "true" : "false");
    return chg;
}

bool
CicoSCPolicyManager::releaseInputResource(int input)
{
    if (1 == input) {
        (void)sendSMEvent(EVID_INPUT1_RELEASE, input);
    }
    else if (2 == input) {
        (void)sendSMEvent(EVID_INPUT2_RELEASE, input);
    }
    return true;
}

void
CicoSCPolicyManager::notifyChangedState(int state)
{
    m_resourceMgr->receiveChangedState(state);
}

bool
CicoSCPolicyManager::getDispZoneState(int zoneid)
{
    if (0 >= zoneid) {
        return false;
    }

    // find state instance
    std::map<int, const CicoState*>::iterator itr;
    itr = m_dispZoneStates.find(zoneid);
    if (itr == m_dispZoneStates.end()) {
        return false;
    }

    // if state instance is null
    if (NULL == itr->second) {
        return false;
    }
        
    return itr->second->isActive();
}

bool
CicoSCPolicyManager::getSoundZoneState(int zoneid) const
{
    if ((0 < zoneid) && ((int)m_soundZoneStates.size()-1 > zoneid)) {
        return m_soundZoneStates[zoneid]->isActive();
    }
    return false;
}

bool
CicoSCPolicyManager::getInputState(int input) const
{
    if ((0 < input) && ((int)m_inputStates.size()-1 > input)) {
        return m_inputStates[input]->isActive();
    }
    return false;
}

bool
CicoSCPolicyManager::getRegulation(void)
{
    return m_policyStates[STID_DRVREGULATION_ON]->isActive();
}

bool
CicoSCPolicyManager::isExistDisplayZoneOwer(int zoneid)
{
    if ((zoneid >= ICO_DISPLAY0_ZONEID_MIN) &&
        (zoneid <= ICO_DISPLAY0_ZONEID_MAX)) {
        return !m_policyStates[STID_DISPLAY0_NOOWER]->isActive();
    }
    if ((zoneid >= ICO_DISPLAY1_ZONEID_MIN) &&
        (zoneid <= ICO_DISPLAY1_ZONEID_MAX)) {
        return !m_policyStates[STID_DISPLAY1_NOOWER]->isActive();
    }
    return true;
}

//--------------------------------------------------------------------------
/**
 *  @brief  notify connected
 */
//--------------------------------------------------------------------------
void
CicoSCPolicyManager::notifyConnected(const string & appid)
{
    // Notify regulation changed state
    CicoSCMessage *message = new CicoSCMessage();
    message->addRootObject("command", MSG_CMD_NOTIFY_CHANGED_STATE);
    message->addArgObject(MSG_PRMKEY_STATEID, ICO_SYC_STATE_REGULATION);
    if (true == m_policyStates[STID_DRVREGULATION_ON]->isActive()) {
        message->addArgObject(MSG_PRMKEY_STATE, ICO_SYC_STATE_ON);
    }
    else {
        message->addArgObject(MSG_PRMKEY_STATE, ICO_SYC_STATE_OFF);
    }
    CicoSCServer::getInstance()->sendMessageToHomeScreen(message);

    // Notify NightMode changed state
    message = new CicoSCMessage();
    message->addRootObject("command", MSG_CMD_NOTIFY_CHANGED_STATE);
    message->addArgObject(MSG_PRMKEY_STATEID, ICO_SYC_STATE_NIGHTMODE);
    if (true == m_policyStates[STID_NIGHTMODE_ON]->isActive()) {
        message->addArgObject(MSG_PRMKEY_STATE, ICO_SYC_STATE_ON);
    }
    else {
        message->addArgObject(MSG_PRMKEY_STATE, ICO_SYC_STATE_OFF);
    }
    CicoSCServer::getInstance()->sendMessageToHomeScreen(message);
}
// vim:set expandtab ts=4 sw=4:
