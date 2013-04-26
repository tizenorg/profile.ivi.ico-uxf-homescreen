/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Regulation controller
 *
 * @date    Feb-28-2013
 */

#include    <stdio.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    <stdbool.h>
#include    <string.h>
#include    <errno.h>
#include    <pthread.h>
#include    <sys/ioctl.h>
#include    <sys/time.h>
#include    <fcntl.h>

#include    <wayland-client.h>
#include    <ico_window_mgr-client-protocol.h>
#include    <dbus/dbus.h>
#include    <Ecore.h>
#include    "ico_apf.h"
#include    "ico_syc_apc.h"
#include    "ico_syc_apc_private.h"

/*==============================================================================*/
/* static tables                                                                */
/*==============================================================================*/
/* callback function            */
static ico_apc_regulation_cb_t  regulation_cb = NULL;
static void                     *regulation_cb_user_data = NULL;

/* Ecore/D-Bus static valiables */
static Ecore_Timer *vehicle_timer = NULL;
static DBusConnection *dbus_connection = NULL;
static ico_apc_reguration_control_t control[ICO_UXF_CATEGORY_MAX];

/* vehicle information          */
static const struct _vehicle_info_property {
    int     key;                        /* Vehicle Information key                  */
    char    *property;                  /* D-Bus property name                      */
    char    *path;                      /* D-Bus path name                          */
    char    *interface;                 /* D-Bus interface name                     */
}                   vehicle_info[] = {
    { ICO_SYC_VEHICLEINFO_VEHICLE_SPEED, "VehicleSpeed",
      "/org/automotive/runningstatus/vehicleSpeed", "org.automotive.vehicleSpeed" },
    { ICO_SYC_VEHICLEINFO_SHIFT_POSITION, "ShiftPosition",
      "/org/automotive/runningstatus/transmission", "org.automotive.transmission" },
    { ICO_SYC_VEHICLEINFO_BLINKER, "Blinker",
      "\0", "\0" },
    { 0, "\0", "\0", "\0" }
};

/* Vehicle information data     */
static struct _vehicle_info_data    {
    int             key;                /* Vehicle Information key                  */
    DBusPendingCall *pending;
    int             request;
    int             errcount;
    double          val;
}                   vehicle_data[ICO_UXF_REGULATION_VIC_MAX];

/* system configuration         */
static Ico_Uxf_Sys_Config       *confsys = NULL;
static int      ncategory;
static Ico_Uxf_conf_category    *category;

/*==============================================================================*/
/* define static function prototype                                             */
/*==============================================================================*/
static int request_vehicle_info(void);
static int get_vehicle_info(void);
static Eina_Bool rule_engine_wake(void *user_data);

/*--------------------------------------------------------------------------*/
/**
 * @brief   request_vehicle_info: request to AMB(static function)
 *
 * @param       none
 * @return      result
 * @retval      ICO_SYC_EOK     success
 * @retval      ICO_SYC_EIO     error(D-Bus send error)
 */
/*--------------------------------------------------------------------------*/
static int
request_vehicle_info(void)
{
    DBusMessage *dbus_message = NULL;
    int     idx;
    int     ret = ICO_SYC_EOK;

    for (idx = 0; vehicle_info[idx].key; idx++) {

        /* set vehicle info key     */
        vehicle_data[idx].key = vehicle_info[idx].key;

        if (vehicle_data[idx].pending)  {
            apfw_trace("request_vehicle_info: (%s) not complite",
                       vehicle_info[idx].property);
            continue;
        }

        if (vehicle_info[idx].path[0] == 0) {
            /* currently not support this vehicle information   */
            continue;
        }

        /* Create send message      */
        dbus_message = dbus_message_new_method_call(DBUS_SERVICE, vehicle_info[idx].path,
                                                    DBUS_INTERFACE, DBUS_METHOD);
        if (! dbus_message) {
            apfw_warn("request_vehicle_info: ERROR dbus_message_new_method_call" );
            ret = ICO_SYC_EIO;
        }
        /* Set parameters into message  */
        else if (! dbus_message_append_args(
                            dbus_message,
                            DBUS_TYPE_STRING, &vehicle_info[idx].interface,
                            DBUS_TYPE_STRING, &vehicle_info[idx].property,
                            DBUS_TYPE_INVALID)) {
            apfw_warn("request_vehicle_info: ERROR dbus_message_append_args" );
            ret = ICO_SYC_EIO;
        }
        /* Set destination              */
        else if (! dbus_message_set_destination(dbus_message, DBUS_SERVICE))    {
            apfw_warn("request_vehicle_info: ERROR dbus_message_set_destination" );
            ret = ICO_SYC_EIO;
        }
        /* Send message                 */
        else if (! dbus_connection_send_with_reply(
                            dbus_connection, dbus_message,
                            &vehicle_data[idx].pending, 200))    {
            apfw_warn("request_vehicle_info: ERROR dbus_connection_send" );
            vehicle_data[idx].pending = NULL;
            ret = ICO_SYC_EIO;
        }
        if (dbus_message)   {
            /* Release message                  */
            dbus_message_unref(dbus_message);
        }
    }

    /* dispatch if data queue exist         */
    do  {
        dbus_connection_read_write_dispatch(dbus_connection, 0);
    } while (dbus_connection_get_dispatch_status(dbus_connection)
             == DBUS_DISPATCH_DATA_REMAINS);

    return(ret);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   get_vehicle_info: get vercle information from AMB(static function)
 *
 * @param       none
 * @return      always ICO_SYC_EOK(success)
 */
/*--------------------------------------------------------------------------*/
static int
get_vehicle_info(void)
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

    /* dispatch if data queue exist         */
    do {
        dbus_connection_read_write_dispatch(dbus_connection, 0);
    } while (dbus_connection_get_dispatch_status(dbus_connection)
             == DBUS_DISPATCH_DATA_REMAINS);

    /* analize reply datas                  */
    for (idx = 0; vehicle_info[idx].key; idx++) {
        if (! vehicle_data[idx].pending)    {
            continue;
        }
        if (! dbus_pending_call_get_completed(vehicle_data[idx].pending))   {
            apfw_trace("get_vehicle_info: (%s) NOT complite",
                       vehicle_info[idx].property);
            continue;
        }

        dbus_message = dbus_pending_call_steal_reply(vehicle_data[idx].pending);
        if (! dbus_message) {
            apfw_trace("get_vehicle_info: (%s) NO reply", vehicle_info[idx].property);
            continue;
        }

        if (dbus_message_get_type(dbus_message) == DBUS_MESSAGE_TYPE_ERROR) {
            dbus_message_unref(dbus_message);
            dbus_pending_call_cancel(vehicle_data[idx].pending);
            vehicle_data[idx].pending = NULL;
            vehicle_data[idx].errcount ++;
            if (vehicle_data[idx].errcount <= 5)    {
                apfw_warn("get_vehicle_info: (%s) reply error", vehicle_info[idx].property);
            }
            continue;
        }

        dbus_message_iter_init(dbus_message, &iter_head);
        dbus_message_iter_recurse(&iter_head, &iter);

        type = dbus_message_iter_get_arg_type(&iter);
        switch (type)   {
        case DBUS_TYPE_INT32:
            dbus_message_iter_get_basic(&iter, &i32);
            vehicle_data[idx].val = (double)i32;
            break;
        case DBUS_TYPE_INT16:
            dbus_message_iter_get_basic(&iter, &i16);
            vehicle_data[idx].val = (double)i16;
            break;
        case DBUS_TYPE_UINT32:
            dbus_message_iter_get_basic(&iter, &u32);
            vehicle_data[idx].val = (double)u32;
            break;
        case DBUS_TYPE_UINT16:
            dbus_message_iter_get_basic(&iter, &u16);
            vehicle_data[idx].val = (double)u16;
            break;
        case DBUS_TYPE_BOOLEAN:
            dbus_message_iter_get_basic(&iter, &b);
            if (b)      vehicle_data[idx].val = (double)1.0;
            else        vehicle_data[idx].val = (double)0.0;
            break;
        case DBUS_TYPE_BYTE:
            dbus_message_iter_get_basic(&iter, &u8);
            vehicle_data[idx].val = (double)u8;
            break;
        case DBUS_TYPE_DOUBLE:
            dbus_message_iter_get_basic(&iter, &d64);
            vehicle_data[idx].val = (double)d64;
            break;
        default:
            apfw_warn("get_vehicle_info: (%s) illegal data type(0x%02x)",
                      vehicle_info[idx].property, ((int)type) & 0x0ff);
            break;
        }
        /* free message and pending     */
        dbus_message_unref(dbus_message);
        dbus_pending_call_cancel(vehicle_data[idx].pending);
        vehicle_data[idx].pending = NULL;
    };
    return ICO_SYC_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   rule_engine_wake: judge a run regulation state(static function)
 *
 * @param[in]   user_data       user data(unused)
 * @return      always ECORE_CALLBACK_RENEW(periodic timer)
 */
/*--------------------------------------------------------------------------*/
static Eina_Bool
rule_engine_wake(void *user_data)
{
    int     idx;
    ico_apc_reguration_control_t    wkcontrol[ICO_UXF_CATEGORY_MAX];
    ico_apc_reguration_control_t    change;
    double  VehicleSpeed = 0.0;
    int     ShiftPosition = 0;
    int     Blinker = 0;

    memset(wkcontrol, 0, sizeof(ico_apc_reguration_control_t) * ncategory);

    /* get reply (vehicle ifno) */
    get_vehicle_info();

    /* get vehicle info values  */
    for (idx = 0; vehicle_data[idx].key; idx++)   {
        if (vehicle_data[idx].key == ICO_SYC_VEHICLEINFO_VEHICLE_SPEED)   {
            VehicleSpeed = (double)vehicle_data[idx].val;
        }
        else if (vehicle_data[idx].key == ICO_SYC_VEHICLEINFO_SHIFT_POSITION) {
            ShiftPosition = (int)vehicle_data[idx].val;
        }
        else if (vehicle_data[idx].key == ICO_SYC_VEHICLEINFO_BLINKER) {
            Blinker = (int)vehicle_data[idx].val;
        }
    }

    /* Make control code            */
    memcpy(wkcontrol, control, sizeof(ico_apc_reguration_control_t) * ncategory);

    /* Check Vehicle Speed          */
    for (idx = 0; idx < ncategory; idx++)   {
        switch (category[idx].view) {
        case ICO_UXF_POLICY_ALWAYS:
            wkcontrol[idx].display = ICO_SYC_APC_REGULATION_NOREGULATION;
            break;
        case ICO_UXF_POLICY_RUNNING:
            if (VehicleSpeed >= ICO_SYC_APC_REGULATION_SPEED_RUNNING)
                wkcontrol[idx].display = ICO_SYC_APC_REGULATION_NOREGULATION;
            else
                wkcontrol[idx].display = ICO_SYC_APC_REGULATION_REGULATION;
            break;
        case ICO_UXF_POLICY_PARKED:
            if (VehicleSpeed >= ICO_SYC_APC_REGULATION_SPEED_RUNNING)
                wkcontrol[idx].display = ICO_SYC_APC_REGULATION_REGULATION;
            else
                wkcontrol[idx].display = ICO_SYC_APC_REGULATION_NOREGULATION;
            break;
        case ICO_UXF_POLICY_SHIFT_PARKING:
            if ((VehicleSpeed < ICO_SYC_APC_REGULATION_SPEED_RUNNING) &&
                (ShiftPosition == ICO_SYC_APC_REGULATION_SHIFT_PARKING))
                wkcontrol[idx].display = ICO_SYC_APC_REGULATION_REGULATION;
            else
                wkcontrol[idx].display = ICO_SYC_APC_REGULATION_NOREGULATION;
            break;
        case ICO_UXF_POLICY_SHIFT_REVERSES:
            if (ShiftPosition == ICO_SYC_APC_REGULATION_SHIFT_REVERSES)
                wkcontrol[idx].display = ICO_SYC_APC_REGULATION_NOREGULATION;
            else
                wkcontrol[idx].display = ICO_SYC_APC_REGULATION_REGULATION;
            break;
        case ICO_UXF_POLICY_BLINKER_LEFT:
            if ((Blinker != ICO_SYC_APC_REGULATION_BLINKER_LEFT) ||
                (ShiftPosition == ICO_SYC_APC_REGULATION_SHIFT_PARKING) ||
                (ShiftPosition == ICO_SYC_APC_REGULATION_SHIFT_REVERSES))
                wkcontrol[idx].display = ICO_SYC_APC_REGULATION_REGULATION;
            else
                wkcontrol[idx].display = ICO_SYC_APC_REGULATION_NOREGULATION;
            break;
        case ICO_UXF_POLICY_BLINKER_RIGHT:
            if ((Blinker != ICO_SYC_APC_REGULATION_BLINKER_RIGHT) ||
                (ShiftPosition == ICO_SYC_APC_REGULATION_SHIFT_PARKING) ||
                (ShiftPosition == ICO_SYC_APC_REGULATION_SHIFT_REVERSES))
                wkcontrol[idx].display = ICO_SYC_APC_REGULATION_REGULATION;
            else
                wkcontrol[idx].display = ICO_SYC_APC_REGULATION_NOREGULATION;
            break;
        default:
            apfw_trace("rule_engine_wake: category(%d) has unknown view(%d)",
                       idx, category[idx].view);
            break;
        }

        switch (category[idx].sound)    {
        case ICO_UXF_POLICY_ALWAYS:
            wkcontrol[idx].sound = ICO_SYC_APC_REGULATION_NOREGULATION;
            break;
        case ICO_UXF_POLICY_RUNNING:
            if (VehicleSpeed >= ICO_SYC_APC_REGULATION_SPEED_RUNNING)
                wkcontrol[idx].sound = ICO_SYC_APC_REGULATION_NOREGULATION;
            else
                wkcontrol[idx].sound = ICO_SYC_APC_REGULATION_REGULATION;
            break;
        case ICO_UXF_POLICY_PARKED:
            if (VehicleSpeed >= ICO_SYC_APC_REGULATION_SPEED_RUNNING)
                wkcontrol[idx].sound = ICO_SYC_APC_REGULATION_REGULATION;
            else
                wkcontrol[idx].sound = ICO_SYC_APC_REGULATION_NOREGULATION;
            break;
        case ICO_UXF_POLICY_SHIFT_PARKING:
            if (ShiftPosition == ICO_SYC_APC_REGULATION_SHIFT_PARKING)
                wkcontrol[idx].sound = ICO_SYC_APC_REGULATION_NOREGULATION;
            else
                wkcontrol[idx].sound = ICO_SYC_APC_REGULATION_REGULATION;
            break;
        case ICO_UXF_POLICY_SHIFT_REVERSES:
            if (ShiftPosition == ICO_SYC_APC_REGULATION_SHIFT_REVERSES)
                wkcontrol[idx].sound = ICO_SYC_APC_REGULATION_NOREGULATION;
            else
                wkcontrol[idx].sound = ICO_SYC_APC_REGULATION_REGULATION;
            break;
        case ICO_UXF_POLICY_BLINKER_LEFT:
            if ((Blinker != ICO_SYC_APC_REGULATION_BLINKER_LEFT) ||
                (ShiftPosition == ICO_SYC_APC_REGULATION_SHIFT_PARKING) ||
                (ShiftPosition == ICO_SYC_APC_REGULATION_SHIFT_REVERSES))
                wkcontrol[idx].sound = ICO_SYC_APC_REGULATION_REGULATION;
            else
                wkcontrol[idx].sound = ICO_SYC_APC_REGULATION_NOREGULATION;
            break;
        case ICO_UXF_POLICY_BLINKER_RIGHT:
            if ((Blinker != ICO_SYC_APC_REGULATION_BLINKER_RIGHT) ||
                (ShiftPosition == ICO_SYC_APC_REGULATION_SHIFT_PARKING) ||
                (ShiftPosition == ICO_SYC_APC_REGULATION_SHIFT_REVERSES))
                wkcontrol[idx].sound = ICO_SYC_APC_REGULATION_REGULATION;
            else
                wkcontrol[idx].sound = ICO_SYC_APC_REGULATION_NOREGULATION;
            break;
        default:
            apfw_trace("rule_engine_wake: category(%d) has unknown sound(%d)",
                       idx, category[idx].sound);
            break;
        }

        switch (category[idx].input)    {
        case ICO_UXF_POLICY_ALWAYS:
            wkcontrol[idx].input = ICO_SYC_APC_REGULATION_NOREGULATION;
            break;
        case ICO_UXF_POLICY_RUNNING:
            if (VehicleSpeed >= ICO_SYC_APC_REGULATION_SPEED_RUNNING)
                wkcontrol[idx].input = ICO_SYC_APC_REGULATION_NOREGULATION;
            else
                wkcontrol[idx].input = ICO_SYC_APC_REGULATION_REGULATION;
            break;
        case ICO_UXF_POLICY_PARKED:
            if (VehicleSpeed >= ICO_SYC_APC_REGULATION_SPEED_RUNNING)
                wkcontrol[idx].input = ICO_SYC_APC_REGULATION_REGULATION;
            else
                wkcontrol[idx].input = ICO_SYC_APC_REGULATION_NOREGULATION;
            break;
        case ICO_UXF_POLICY_SHIFT_PARKING:
            if (ShiftPosition == ICO_SYC_APC_REGULATION_SHIFT_PARKING)
                wkcontrol[idx].input = ICO_SYC_APC_REGULATION_NOREGULATION;
            else
                wkcontrol[idx].input = ICO_SYC_APC_REGULATION_REGULATION;
            break;
        case ICO_UXF_POLICY_SHIFT_REVERSES:
            if (ShiftPosition == ICO_SYC_APC_REGULATION_SHIFT_REVERSES)
                wkcontrol[idx].input = ICO_SYC_APC_REGULATION_NOREGULATION;
            else
                wkcontrol[idx].input = ICO_SYC_APC_REGULATION_REGULATION;
            break;
        case ICO_UXF_POLICY_BLINKER_LEFT:
            if ((Blinker != ICO_SYC_APC_REGULATION_BLINKER_LEFT) ||
                (ShiftPosition == ICO_SYC_APC_REGULATION_SHIFT_PARKING) ||
                (ShiftPosition == ICO_SYC_APC_REGULATION_SHIFT_REVERSES))
                wkcontrol[idx].input = ICO_SYC_APC_REGULATION_REGULATION;
            else
                wkcontrol[idx].input = ICO_SYC_APC_REGULATION_NOREGULATION;
            break;
        case ICO_UXF_POLICY_BLINKER_RIGHT:
            if ((Blinker != ICO_SYC_APC_REGULATION_BLINKER_RIGHT) ||
                (ShiftPosition == ICO_SYC_APC_REGULATION_SHIFT_PARKING) ||
                (ShiftPosition == ICO_SYC_APC_REGULATION_SHIFT_REVERSES))
                wkcontrol[idx].input = ICO_SYC_APC_REGULATION_REGULATION;
            else
                wkcontrol[idx].input = ICO_SYC_APC_REGULATION_NOREGULATION;
            break;
        default:
            apfw_trace("rule_engine_wake: category(%d) has unknown input(%d)",
                       idx, category[idx].input);
            break;
        }
    }

    for (idx = 0; idx < ncategory; idx++) {
        if ((control[idx].display != wkcontrol[idx].display) ||
            (control[idx].sound != wkcontrol[idx].sound) ||
            (control[idx].input != wkcontrol[idx].input))   {
            apfw_trace("rule_engine_wake: Category.%d view.%d>%d sound.%d>%d inp.%d>%d",
                       idx, control[idx].display, wkcontrol[idx].display,
                       control[idx].sound, wkcontrol[idx].sound,
                       control[idx].input, wkcontrol[idx].input);

            if (regulation_cb)  {
                if (control[idx].display != wkcontrol[idx].display)
                    change.display = wkcontrol[idx].display;
                else
                    change.display = ICO_SYC_APC_REGULATION_NOCHANGE;
                if (control[idx].sound != wkcontrol[idx].sound)
                    change.sound = wkcontrol[idx].sound;
                else
                    change.sound = ICO_SYC_APC_REGULATION_NOCHANGE;
                if (control[idx].input != wkcontrol[idx].input)
                    change.input = wkcontrol[idx].input;
                else
                    change.input = ICO_SYC_APC_REGULATION_NOCHANGE;

                (*regulation_cb)(idx, change, regulation_cb_user_data);
            }
            control[idx].display = wkcontrol[idx].display;
            control[idx].sound = wkcontrol[idx].sound;
            control[idx].input = wkcontrol[idx].input;
        }
    }
    /* send request to AMB                  */
    request_vehicle_info();

    return ECORE_CALLBACK_RENEW;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_apc_regulation_init: initialize regulation control
 *
 * @param       none
 * @return      result
 * @retval      ICO_SYC_EOK     success
 * @retval      ICO_SYC_EIO     error(D-Bus initialize error)
 */
/*--------------------------------------------------------------------------*/
int
ico_syc_apc_regulation_init(void)
{
    int     i;
    DBusError dbus_error;

    apfw_trace("ico_syc_apc_regulation_init: Enter");

    /* get configurations                           */
    confsys = (Ico_Uxf_Sys_Config *)ico_uxf_getSysConfig();

    if (! confsys) {
        apfw_trace("ico_syc_apc_regulation_init: Leave(can not read configuration)");
        return ICO_SYC_EIO;
    }
    ncategory = confsys->categoryNum;
    category = confsys->category;

    memset(vehicle_data, 0, sizeof(vehicle_data));
    memset(control, 0, sizeof(control));
    for (i = 0; i <ncategory; i++)  {
        control[i].display = ICO_SYC_APC_REGULATION_NOREGULATION;
        control[i].sound = ICO_SYC_APC_REGULATION_NOREGULATION;
        control[i].input = ICO_SYC_APC_REGULATION_NOREGULATION;
    }

    /* Reset D-Bus error        */
    dbus_error_init(&dbus_error);

    /* Get D-Bus connection     */
    dbus_connection = dbus_bus_get(DBUS_BUS_SYSTEM, &dbus_error);
    if (! dbus_connection){
        apfw_warn("ico_syc_apc_regulation_init: Leave(ERROR dbus_bus_get)" );
        return ICO_SYC_EIO;
    }

    /* send request to AMB                  */
    request_vehicle_info();

    vehicle_timer = ecore_timer_add(0.1, rule_engine_wake, NULL);
    if (! vehicle_timer)    {
        apfw_error("ico_syc_apc_regulation_init: Leave(Can not create Ecore timer)");
    }

    apfw_trace("ico_syc_apc_regulation_init: Leave(EOK)");

    return ICO_SYC_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_apc_regulation_term: terminate regulation control
 *
 * @param       none
 * @return      always ICO_SYC_EOK(success)
 */
/*--------------------------------------------------------------------------*/
int
ico_syc_apc_regulation_term(void)
{
    apfw_trace("ico_syc_apc_regulation_term: Enter");

    if (! confsys) {
        apfw_trace("ico_syc_apc_regulation_term: Leave(not initialized)");
        return ICO_SYC_EOK;
    }
    ecore_timer_del(vehicle_timer);

    confsys = NULL;

    apfw_trace("ico_syc_apc_regulation_term: Leave(EOK)");
    return ICO_SYC_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_apc_regulation_listener: set regulation control listener
 *
 * @param[in]   func            listener function
 * @param[in]   user_data       user data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
ico_syc_apc_regulation_listener(ico_apc_regulation_cb_t func, void *user_data)
{
    int     idx;

    regulation_cb = func;
    regulation_cb_user_data = user_data;

    if (regulation_cb)  {
        for (idx = 0; idx < ncategory; idx++) {
            (*regulation_cb)(idx, control[idx], regulation_cb_user_data);
        }
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_apc_regulation_app_visible: get display regulation status
 *
 * @param[in]   category    category Id
 * @return      result
 * @retval      TRUE        The application of this category can use the display
 * @retval      FALSE       The application of this category can not use the display
 */
/*--------------------------------------------------------------------------*/
int
ico_syc_apc_regulation_app_visible(const int category)
{
    if ((category < 0) || (category >= ICO_UXF_CATEGORY_MAX))    {
        apfw_warn("ico_syc_apc_regulation_app_visible: Illegal category(%d)", category);
        return TRUE;
    }
    if (control[category].display == ICO_SYC_APC_REGULATION_NOREGULATION)   {
        return TRUE;
    }
    return FALSE;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_apc_regulation_app_sound: get sound reguration status
 *
 * @param[in]   category    category Id
 * @return      result
 * @retval      TRUE        The application of this category can use the sound output
 * @retval      FALSE       The application of this category can not use the sound output
 */
/*--------------------------------------------------------------------------*/
int
ico_syc_apc_regulation_app_sound(const int category)
{
    if ((category < 0) || (category >= ICO_UXF_CATEGORY_MAX))    {
        apfw_warn("ico_syc_apc_regulation_app_sound: Illegal category(%d)", category);
        return TRUE;
    }
    if (control[category].sound == ICO_SYC_APC_REGULATION_NOREGULATION) {
        return TRUE;
    }
    return FALSE;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_apc_regulation_app_input: get input switch reguration status
 *
 * @param[in]   category    category Id
 * @return      result
 * @retval      TRUE        The application of this category is available with an input
 * @retval      FALSE       The application of this category is not available with an input
 */
/*--------------------------------------------------------------------------*/
int
ico_syc_apc_regulation_app_input(const int category)
{
    if ((category < 0) || (category >= ICO_UXF_CATEGORY_MAX))    {
        apfw_warn("ico_syc_apc_regulation_app_input: Illegal category(%d)", category);
        return TRUE;
    }
    if (control[category].input == ICO_SYC_APC_REGULATION_NOREGULATION) {
        return TRUE;
    }
    return FALSE;
}

