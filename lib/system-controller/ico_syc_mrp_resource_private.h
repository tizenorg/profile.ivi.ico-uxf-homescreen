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
 *  @file   ico_syc_mrp_resource_private.h
 *
 *  @brief  This file is definition of murpy ressource control
 */
//==========================================================================

#ifndef __ICO_SYC_MRP_RESOURCE_PRIVATE_H__
#define __ICO_SYC_MRP_RESOURCE_PRIVATE_H__

#include <murphy/plugins/resource-native/libmurphy-resource/resource-api.h>
#ifdef __cplusplus
extern "C" {
#endif


#define ICO_APC_REQTYPE_REQUEST 0               /* Request from application         */
#define ICO_APC_REQTYPE_CREATE  1               /* Request automatically             */

// TODO 
#define REQTYPE_APP             0
#define REQTYPE_AUTO            1

#define RESID_KIND_DISPLAY      0x00000001  
#define RESID_KIND_SOUND        0x00000002  
#define RESID_KIND_INPUT        0x00000004  
#define RESID_KIND_MASK         0x000000FF  
                                            
#define RESID_TYPE_BASIC        0x00000100  
#define RESID_TYPE_INTERRUPT    0x00000200  
#define RESID_TYPE_ONSCREEN     0x00000400  
#define RESID_TYPE_MASK         0x00000F00  

#define RESID_CMD_ACQUIRE       0x00001000
#define RESID_CMD_RELEASE       0x00002000

#define RES_STATE_ACQUIRED      1
#define RES_STATE_DEPRIVED      2
#define RES_STATE_WAITING       3
#define RES_STATE_RELEASED      4

#define MURPHY_APPID_MAXIMUM_LENGTH 128

typedef struct {
    int  req_id;
    char appid[MURPHY_APPID_MAXIMUM_LENGTH];
    int  res_type;
    int  zone_idx;
    int  prio;
    mrp_list_hook_t hook;
} mrp_res_queue_item_t;

#define ICO_UXF_MAX_PROCESS_NAME 255
#define ICO_UXF_MAX_DEVICE_NAME  255

/* request information                          */
typedef struct  _resource_request {
    int                         id;             /* request object                   */
    int                         prio;           /* request priority                 */
    unsigned short              reqtype;        /* Request type                     */
    unsigned short              category;        /* Request category */

    int  resid;           /* resource id      */
    char *appid;          /* application id */
    int  appkind;         /* id of application kind */
    int  pid;             /* process id     */
    int  state;           /* request state */

    char *dispzone;       /* area of showing application window */
    int  dispzoneid;      /* area of showing application window */
    int  layerid;         /* layer of showing application window */
    char *winname;        /* window's surface name */
    int  surfaceid;       /* window id */
    char *animation;      /* name of animation */
    int  animationTime;   /* time of animation[ms] */
    bool  bEx;            /* Expansion flag */
    char *ECU;            /* name to identify ECU */
    char *display;        /* name to identify Display in ECU */
    char *layer;          /* name to identify Layer in Display */
    char *layout;         /* name to identify layout in Layer */
    char *area;           /* name to Output position in Layout */
    char *dispatchApp;    /* origin of application */
    char *role;           /* role of notice kind */
    int   role_stt;       /* state number change from role */
    int   resourceId;     /* ID number of resource */

    char *soundzone;      /* area of playing sound */
    int  soundzoneid;     /* area of playing sound */
    char *soundname;      /* sound stream name */
    int  soundid;         /* sound id */
    int  soundadjust;     /* adjust action */

    char *device;         /* input device name */
    int  input;           /* input event id */

    mrp_res_resource_set_t      *rset;          /* Murphy resource set */
    mrp_res_queue_item_t        *res_data;      /* identifier for the request */
    mrp_list_hook_t             hook;           /* keep track of requests */
    int                         released;

} resource_request_t;

typedef void (*ico_syc_mrp_enforce_sound_t)(unsigned short state,
                                            resource_request_t *req,
                                            void *user_data);


int ico_syc_mrp_init(ico_syc_mrp_enforce_sound_t soundcb,
                     void *user_data);
void ico_syc_mrp_term(void);

bool ico_syc_mrp_acquire_sound_resource(resource_request_t *req);
bool ico_syc_mrp_release_sound_resource(resource_request_t *req);
void ico_syc_mrp_active_app(const char *appid);

#ifdef __cplusplus
}
#endif
#endif  /* __ICO_SYC_MRP_RESOURCE_PRIVATE_H__ */
/* vim:set expandtab ts=4 sw=4: */
