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
 *  @file   ico_syc_msg_cmd_def.h
 *
 *  @brief  
 */
/*========================================================================*/
#ifndef __ICO_SYC_MSG_CMD_DEF_H__
#define __ICO_SYC_MSG_CMD_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/* definition                                                                 */
/*============================================================================*/
/*
 * uri and protocol
 */
#define ICO_SYC_URI             (char *)"ws://127.0.0.1:18081"
#define ICO_SYC_PROTOCOL        (char *)"ico_syc_protocol"

/*
 * message primary key name
 */
#define MSG_PRMKEY_APPID        (char *)"appid"
#define MSG_PRMKEY_CMD          (char *)"command"
#define MSG_PRMKEY_PID          (char *)"pid"

/* for privilege API */
#define MSG_PRMKEY_ARG          (char *)"arg"
/* window controller and input controller */
#define MSG_PRMKEY_WINNAME      (char *)"winname"
#define MSG_PRMKEY_SURFACE      (char *)"surface"
#define MSG_PRMKEY_LAYER        (char *)"layer"
#define MSG_PRMKEY_NODE         (char *)"node"
#define MSG_PRMKEY_ZONE         (char *)"zone"
#define MSG_PRMKEY_ANIM_NAME    (char *)"anim_name"
#define MSG_PRMKEY_ANIM_TIME    (char *)"anim_time"
#define MSG_PRMKEY_ATTR         (char *)"attr"
#define MSG_PRMKEY_DEV          (char *)"device"
#define MSG_PRMKEY_INPUT        (char *)"input_num"
#define MSG_PRMKEY_ALLOC        (char *)"alloc_type"
#define MSG_PRMKEY_KEY          (char *)"keycode"
#define MSG_PRMKEY_DEVICE       (char *)"deviceno"
#define MSG_PRMKEY_EV_TYPE      (char *)"ev_type"
#define MSG_PRMKEY_EV_TIME      (char *)"ev_time"
#define MSG_PRMKEY_EV_CODE      (char *)"ev_code"
#define MSG_PRMKEY_EV_VAL       (char *)"ev_value"
#define MSG_PRMKEY_POS_X        (char *)"pos_x"
#define MSG_PRMKEY_POS_Y        (char *)"pos_y"
#define MSG_PRMKEY_WIDTH        (char *)"width"
#define MSG_PRMKEY_HEIGHT       (char *)"height"
#define MSG_PRMKEY_RAISE        (char *)"raise"
#define MSG_PRMKEY_VISIBLE      (char *)"visible"
#define MSG_PRMKEY_ACTIVE       (char *)"active"
#define MSG_PRMKEY_STRIDE       (char *)"stride"
#define MSG_PRMKEY_FORMAT       (char *)"format"
#define MSG_PRMKEY_RATE         (char *)"framerate"
/* user controller */
#define MSG_PRMKEY_USER         (char *)"user"
#define MSG_PRMKEY_PASSWD       (char *)"password"
#define MSG_PRMKEY_USER_NUM     (char *)"user_num"
#define MSG_PRMKEY_USER_LIST    (char *)"user_list"
#define MSG_PRMKEY_USER_LOGIN   (char *)"user_login"

/* for general API */
#define MSG_PRMKEY_RES          (char *)"res"
#define MSG_PRMKEY_REGION       (char *)"region"
/* app resource controller */
#define MSG_PRMKEY_RES_WINDOW   (char *)"window"
#define MSG_PRMKEY_RES_SOUND    (char *)"sound"
#define MSG_PRMKEY_RES_INPUT    (char *)"input"
#define MSG_PRMKEY_RES_ZONE     (char *)"zone"
#define MSG_PRMKEY_RES_NAME     (char *)"name"
#define MSG_PRMKEY_RES_SURFACE  (char *)"surface"
#define MSG_PRMKEY_RES_ID       (char *)"id"
#define MSG_PRMKEY_RES_ADJUST   (char *)"adjust"
#define MSG_PRMKEY_RES_EV       (char *)"event"
#define MSG_PRMKEY_RES_TYPE     (char *)"type"
#define MSG_PRMKEY_RES_POS_X    (char *)"pos_x"
#define MSG_PRMKEY_RES_POS_Y    (char *)"pos_y"
#define MSG_PRMKEY_RES_WIDTH    (char *)"width"
#define MSG_PRMKEY_RES_HEIGHT   (char *)"height"
#define MSG_PRMKEY_RES_HOT_X    (char *)"hotspot_x"
#define MSG_PRMKEY_RES_HOT_Y    (char *)"hotspot_y"
#define MSG_PRMKEY_RES_CUR_X    (char *)"cursor_x"
#define MSG_PRMKEY_RES_CUR_Y    (char *)"cursor_x"
#define MSG_PRMKEY_RES_CUR_WIDTH  (char *)"cursor_width"
#define MSG_PRMKEY_RES_CUR_HEIGHT (char *)"cursor_height"
#define MSG_PRMKEY_RES_ATTR     (char *)"attr"
/* user controller */
#define MSG_PRMKEY_LASTINFO     (char *)"lastinfo"

/* notify changed system state */
#define MSG_PRMKEY_STATEID      (char *)"stateid"
#define MSG_PRMKEY_STATE        (char *)"state"

/* commnad type mask */
#define MSG_CMD_TYPE_WINCTRL            0x00010000
#define MSG_CMD_TYPE_INPUTCTRL          0x00020000
#define MSG_CMD_TYPE_USERMGR            0x00030000
#define MSG_CMD_TYPE_RESOURCEMGR        0x00040000
#define MSG_CMD_TYPE_INPUTDEVSETTING    0x00050000
#define MSG_CMD_TYPE_MASK               0xFFFF0000

/*
 * message command name
 */
/* common */
#define MSG_CMD_SEND_APPID          0x00000001
/* window controller */
#define MSG_CMD_CREATE              0x00010001
#define MSG_CMD_DESTROY             0x00010002
#define MSG_CMD_SHOW                0x00010003
#define MSG_CMD_HIDE                0x00010004
#define MSG_CMD_MOVE                0x00010005
#define MSG_CMD_CHANGE_ACTIVE       0x00010006
#define MSG_CMD_CHANGE_LAYER        0x00010007
#define MSG_CMD_CHANGE_ATTR         0x00010008
#define MSG_CMD_NAME                0x00010009
#define MSG_CMD_MAP_THUMB           0x00010011
#define MSG_CMD_UNMAP_THUMB         0x00010012
#define MSG_CMD_SHOW_LAYER          0x00010020
#define MSG_CMD_HIDE_LAYER          0x00010021
#define MSG_CMD_CHANGE_LAYER_ATTR   0x00010022
/* input controller */
#define MSG_CMD_ADD_INPUT           0x00020001
#define MSG_CMD_DEL_INPUT           0x00020002
#define MSG_CMD_SEND_INPUT          0x00020003
/* user controller */
#define MSG_CMD_CHANGE_USER         0x00030001
#define MSG_CMD_GET_USERLIST        0x00030002
#define MSG_CMD_GET_LASTINFO        0x00030003
#define MSG_CMD_SET_LASTINFO        0x00030004
/* app resource controller */
#define MSG_CMD_ACQUIRE_RES         0x00040001
#define MSG_CMD_RELEASE_RES         0x00040002
#define MSG_CMD_DEPRIVE_RES         0x00040003
#define MSG_CMD_WAITING_RES         0x00040004
#define MSG_CMD_REVERT_RES          0x00040005
/* input device setting */
#define MSG_CMD_SET_REGION          0x00050001
#define MSG_CMD_UNSET_REGION        0x00050002

/* app resource controller */
#define MSG_CMD_CREATE_RES          0x00040011
#define MSG_CMD_DESTORY_RES         0x00040012

/* notify state changed to homescreen */
#define MSG_CMD_NOTIFY_CHANGED_STATE    0x00060001
#ifdef __cplusplus
}
#endif
#endif  /* __ICO_SYC_MSG_CMD_DEF_H__ */
/* vim:set expandtab ts=4 sw=4: */
