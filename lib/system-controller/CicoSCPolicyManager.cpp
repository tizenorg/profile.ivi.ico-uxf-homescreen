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
 *  @brief 
 */
//==========================================================================
#include <Ecore.h>

#include "CicoStateMachine.h"
#include "CicoState.h"
#include "CicoHistoryState.h"
#include "CicoStateMachineCreator.h"
#include "CicoSCPolicyManager.h"
#include "CicoSCResourceManager.h"
#include "CicoLog.h"
#include "ico_syc_error.h"

//==========================================================================
//  define
//==========================================================================

// Defines of D-Bus target.
#define DBUS_SERVICE    "org.automotive.message.broker"
#define DBUS_INTERFACE  "org.freedesktop.DBus.Properties"
#define DBUS_METHOD     "Get"

// Defines of Muximum number.
#define AMB_MAX_VHCLINFO    10  /**< maximum number of vehicle info */

// Enumerations of Vehicle information key.
typedef enum {
    AMB_VHCL_SPEED  = 1,  /**< Vehicle speed */
    AMB_SHIFT_POSITION,   /**< Shift position */
    AMB_WINKER,           /**< Winker (not implement in AMB) */
    AMB_MAX_INFO          /**< Maximum number of type */
} _amb_vhcl_key_e;


/**
 *
 */
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
    int             request;
    double          val;
} _vhcldata_t;

_vhcldata_t vhcl_data[AMB_MAX_INFO];

// running state
#define STID_DRIVING           1000
#define STID_DRIVING_STOPPING  1001
#define STID_DRIVING_RUNNING   1002

// shift position state
#define STID_SHIFTPOS_PARKING  2001
#define STID_SHIFTPOS_REVERSE  2002
#define STID_SHIFTPOS_OTHER    2003

// winker state
#define STID_WINKER_OFF        2001
#define STID_WINKER_RIGHT      2002
#define STID_WINKER_LEFT       2003

// driving regulation state
#define STID_DRVREGULATION     9000
#define STID_DRVREGULATION_OFF 9001
#define STID_DRVREGULATION_ON  9002

// camera state
#define STID_CAMERA        10600 //TODO

#define EVID_VELOCTY        1000
#define EVID_SHIFTPOS       2000
#define EVID_WINKER         3000
#define EVID_DRVREGULATION  9000
#define EVID_CAMERA        10600

#define EVID_ZONE1_ACQUIRE 10000
#define EVID_ZONE1_RELEASE 10002

#define EVID_ZONE2_ACQUIRE 10003
#define EVID_ZONE2_RELEASE 10004

#define EVID_ZONE3_ACQUIRE 10005
#define EVID_ZONE3_RELEASE 10006

#define EVID_SCR1_RLS      10001
#define EVID_SCR1_CAMERA   10600
#define EVID_SCR2_GET      12000
#define EVID_SCR2_RLS      12001
#define EVID_ON_SCR_GET    14000
#define EVID_ON_SCR_RLS    14001
#define EVID_INT_SCR1_GET  16000
#define EVID_INT_SCR1_RLS  16001
#define EVID_INT_SCR2_GET  18000
#define EVID_INT_SCR2_RLS  18001
#define EVID_SND_GET       20000
#define EVID_SND_RLS       20001
#define EVID_INT_SND_R_GET 22000
#define EVID_INT_SND_R_RLS 22001
#define EVID_INT_SND_L_GET 24000
#define EVID_INT_SND_L_RLS 24001
#define EVID_INT_STSW_GET  30000
#define EVID_INT_STSW_RLS  30001
#define EVID_INT_ESW_GET   32000
#define EVID_INT_ESW_RLS   32001

/*========================================================================*/    
/*
 *  private global variable
 */
/*========================================================================*/    
/*
 *
 */
static const vhcl_info_prop_t apf_vhcl_info[] = {
    { AMB_VHCL_SPEED,
      "VehicleSpeed",
      "/org/automotive/runningstatus/VehicleSpeed",
      "org.automotive.VehicleSpeed"
    },
    { AMB_SHIFT_POSITION,
      "ShiftPosition",
      "/org/automotive/runningstatus/Transmission",
      "org.automotive.Transmission"
    },
    { AMB_WINKER,
      "Winker",
      "\0",
      "\0" },
    { 0, "\0", "\0", "\0" }
};

/*
 *  Ecore/D-Bus static variables
 */
static DBusConnection *dbus_connection = NULL;

static Ecore_Timer *vehicle_timer = NULL;

//static int amb_initialized = 0;

CicoSCPolicyManager::CicoSCPolicyManager(CicoSCResourceManager* resourceMgr)
    : m_initialized(false), m_resourceMgr(resourceMgr)
{
    m_acquireDispResEventTable.push_back(0);
    m_acquireDispResEventTable.push_back(EVID_ZONE1_ACQUIRE);
    m_acquireDispResEventTable.push_back(EVID_ZONE2_ACQUIRE);
    m_acquireDispResEventTable.push_back(EVID_ZONE3_ACQUIRE);

    m_releaseDispResEventTable.push_back(0);
    m_releaseDispResEventTable.push_back(EVID_ZONE1_RELEASE);
    m_releaseDispResEventTable.push_back(EVID_ZONE2_RELEASE);
    m_releaseDispResEventTable.push_back(EVID_ZONE3_RELEASE);
}

CicoSCPolicyManager::~CicoSCPolicyManager()
{
}

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
  
void
CicoSCPolicyManager::terminate(void)
{
    ICO_DBG("CicoSCPolicyManager::terminate Enter");
    ICO_DBG("CicoSCPolicyManager::terminate Leave");
}

int
CicoSCPolicyManager::initAMB(void)
{
    ICO_DBG("CicoSCPolicyManager::initAMB Enter");

    int ret = 0;
    DBusError dbus_error;

    if (true == m_initialized) {
        ICO_DBG("CicoSCPolicyManager::initAMB Leave(EOK)");
        return ICO_SYC_EOK;
    }

    /* Zero clear vhcl_data */
    memset(vhcl_data, 0, sizeof(vhcl_data));

    /* Reset D-Bus error */
    dbus_error_init(&dbus_error);

    /* Get D-Bus connection */
    dbus_connection = dbus_bus_get(DBUS_BUS_SYSTEM, &dbus_error);
    if (! dbus_connection) {
        ICO_ERR("dbus_bus_get failed.");
        ICO_ERR("CicoSCPolicyManager::initAMB Leave(EIO)");
        return ICO_SYC_EIO;
    }

    /* send request to AMB */
    sendAMBRequest();

    /* recv response from AMB timer start */
    ret = ecore_init();
    if (ret == 0) {
        ICO_ERR("ecore_init");
        ICO_ERR("CicoSCPolicyManager::initAMB Leave(ENOSYS)");
        return ICO_SYC_ENOSYS;
    }

    vehicle_timer = ecore_timer_add(1,//TODO
                                    CicoSCPolicyManager::ecoreTimerCB,
                                    this);
    if (! vehicle_timer)    {
        ICO_ERR("ecore_timer_add failed.");
        ICO_ERR("CicoSCPolicyManager::initAMB Leave(ENOSYS)");
        return ICO_SYC_ENOSYS;
    }

    m_initialized = true;

    ICO_DBG("CicoSCPolicyManager::initAMB Leave(EOK)");
    return ICO_SYC_EOK;
}

int
CicoSCPolicyManager::sendAMBRequest(void)
{
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
        else if (! dbus_connection_send_with_reply(dbus_connection, 
                                                   dbus_message,
                                                   &vhcl_data[idx].pending,
                                                   200)) {
            ICO_ERR("dbus_connection_send");
            vhcl_data[idx].pending = NULL;
            ret = ICO_SYC_EIO;
        }
        else {
//            ICO_DBG("REQUEST req (%s)", apf_vhcl_info[idx].property);
        }

        if (dbus_message) {
            /* Release message */
            dbus_message_unref(dbus_message);
        }
    }

    /* dispatch if data queue exist */
    do  {
        dbus_connection_read_write_dispatch(dbus_connection, 0);
    } while (dbus_connection_get_dispatch_status(dbus_connection)
             == DBUS_DISPATCH_DATA_REMAINS);

//    ICO_DBG("CicoSCPolicyManager::sendAMBRequest Leave");
    return ret;
}

int
CicoSCPolicyManager::getVehicleInfo(void)
{
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
        dbus_connection_read_write_dispatch(dbus_connection, 0);
    } while (dbus_connection_get_dispatch_status(dbus_connection)
             == DBUS_DISPATCH_DATA_REMAINS);

    /* analize reply datas */
    for (idx = 0; apf_vhcl_info[idx].key; idx++) {
        if (! vhcl_data[idx].pending)    {
            continue;
        }
        if (! dbus_pending_call_get_completed(vhcl_data[idx].pending))   {
            ICO_WRN("(%s) NOT complete", apf_vhcl_info[idx].property);
            continue;
        }

        dbus_message = dbus_pending_call_steal_reply(vhcl_data[idx].pending);
        if (! dbus_message) {
            ICO_WRN("(%s) NO reply", apf_vhcl_info[idx].property);
            continue;
        }

        if (dbus_message_get_type(dbus_message) == DBUS_MESSAGE_TYPE_ERROR) {
            dbus_message_unref(dbus_message);
            dbus_pending_call_cancel(vhcl_data[idx].pending);
            vhcl_data[idx].pending = NULL;
            ICO_ERR("(%s) reply error", apf_vhcl_info[idx].property);
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
        ICO_DBG("REQUEST ans (%s) = %.2f",
                apf_vhcl_info[idx].property, vhcl_data[idx].val);

        /* free message and pending */
        dbus_message_unref(dbus_message);
        dbus_pending_call_cancel(vhcl_data[idx].pending);
        vhcl_data[idx].pending = NULL;
    };

    return ICO_SYC_EOK;
}

Eina_Bool
CicoSCPolicyManager::ecoreTimerCB(void *user_data)
{
    CicoSCPolicyManager *policyMgr =
        static_cast<CicoSCPolicyManager*>(user_data);
    policyMgr->recvAMBVehicleInfo();

    return ECORE_CALLBACK_RENEW;
}

void
CicoSCPolicyManager::recvAMBVehicleInfo(void)
{
//    ICO_DBG("CicoSCPolicyManager::recvAMBVehicleInfo Enter");

    int idx = 0;
    int key = 0;
    bool chgDrvState = false;
    bool chgShiftPosState = false;
    bool chgWinkerState = false;

    getVehicleInfo();

    /* get vehicle info values  */
    for (idx = 0; idx < AMB_MAX_VHCLINFO; idx++)   {
        if (vhcl_data[idx].key == 0) break;
        key = vhcl_data[idx].key;
        switch (key) {
        case AMB_VHCL_SPEED:
            ICO_DBG("AMB_VHCL_SPEED : %d",(int)vhcl_data[idx].val);
            chgDrvState = sendSMEvent(EVID_VELOCTY,
                                      (int)vhcl_data[idx].val);
            break;
        case AMB_SHIFT_POSITION:
            chgShiftPosState = sendSMEvent(EVID_SHIFTPOS,
                                           (int)vhcl_data[idx].val);
            break;
        case AMB_WINKER:
            chgWinkerState = sendSMEvent(EVID_WINKER,
                                         (int)vhcl_data[idx].val);
            break;
        default:
            ICO_WRN("not such key (%d)", key);
            break;
        }
    }

    if (true == chgDrvState) {
        bool chg = sendSMEvent(EVID_DRVREGULATION);
        if (true == chg) {
            // notify changed state to resource manager
            CicoState* state =
                (CicoState*)m_stateMachine->getState(STID_DRVREGULATION);
            vector<const CicoState*> currents;
            if ((NULL != state) && (0 != currents.size())) {
                state->getCurrentState(currents, CicoStateCore::ELvlTop);
                ICO_DBG("current=%s", currents[0]->getName().c_str());
                notifyChangedState(currents[0]->getValue());
            }
        }
    }
    if (true == chgShiftPosState || true == chgWinkerState) {
        bool chg = sendSMEvent(EVID_CAMERA);
        if (true == chg) {
            // notify changed state to resource manager
            CicoState* state =
                (CicoState*)m_stateMachine->getState(STID_CAMERA);
            vector<const CicoState*> currents;
            if ((NULL != state) && (0 != currents.size())) {
                state->getCurrentState(currents, CicoStateCore::ELvlTop);
                ICO_DBG("current=%s", currents[0]->getName().c_str());
                notifyChangedState(currents[0]->getValue());
            }
        }
    }
        
    /* send request to AMB */
    sendAMBRequest();

//    ICO_DBG("CicoSCPolicyManager::recvAMBVehicleInfo Leave");
}

int
CicoSCPolicyManager::initStateMachine(void)
{
    ICO_DBG("CicoSCPolicyManager::initStateMachine Enter");

    CicoStateMachineCreator creator;

    m_stateMachine = creator.createFile("/usr/apps/org.tizen.ico.system-controller/res/config/policy.json");
    if (NULL == m_stateMachine) {
        ICO_ERR("CicoStateMachineCreator::createFile failed.");
        return ICO_SYC_ENOSYS;
    }

    int ret = m_stateMachine->start();
    if (ret == 0) {
        ICO_ERR("CicoStateMachine::start faile.");
        return ICO_SYC_ENOSYS;
    }

    ICO_DBG("CicoSCPolicyManager::initStateMachine Leave");
    return ICO_SYC_EOK;
}

bool
CicoSCPolicyManager::testSMEvent(unsigned short event_id)
{
    CicoEvent event(event_id);
    return m_stateMachine->eventTest(event);
}

bool
CicoSCPolicyManager::testSMEvent(unsigned short event_id, int value)
{
    CicoEvent event(event_id, value);
    return m_stateMachine->eventTest(event);
}

bool
CicoSCPolicyManager::sendSMEvent(unsigned short event_id)
{
    CicoEvent event(event_id);
    return m_stateMachine->eventEntry(event);
}

bool
CicoSCPolicyManager::sendSMEvent(unsigned short event_id, int value)
{
    CicoEvent event(event_id, value);
    return m_stateMachine->eventEntry(event);
}

bool
CicoSCPolicyManager::acquireDisplayResource(int zoneid, int category)
{
    ICO_DBG("CicoSCPolicyManager::acquireDisplayResource Enter"
            "(zoneid=%d category=%d)", zoneid, category);
    bool ret = sendSMEvent(m_acquireDispResEventTable[zoneid], category);
    ICO_DBG("CicoSCPolicyManager::acquireDisplayResource Leave(%s)",
            ret ? "true" : "false");
    return ret;
}

bool
CicoSCPolicyManager::releaseDisplayResource(int zoneid, int category)
{
    return true;
}

bool
CicoSCPolicyManager::acquireInputResource(int zoneid, int category)
{
    return sendSMEvent(m_acquireDispResEventTable[zoneid], category);
}

bool
CicoSCPolicyManager::releaseInputResource(int zoneid, int category)
{
    return true;
}

void
CicoSCPolicyManager::notifyChangedState(int state)
{
    m_resourceMgr->receiveChangedState(state);
}
// vim:set expandtab ts=4 sw=4:
