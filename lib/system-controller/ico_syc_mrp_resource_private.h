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
 *  @brief 
 */
//==========================================================================

#ifndef __ICO_SYC_MRP_RESOURCE_PRIVATE_H__
#define __ICO_SYC_MRP_RESOURCE_PRIVATE_H__

#include <murphy/plugins/resource-native/libmurphy-resource/resource-api.h>
#ifdef __cplusplus
extern "C" {
#endif


#define ICO_APC_REQTYPE_REQUEST 0               /* Request from application         */
#define ICO_APC_REQTYPE_CREATE  1               /* Request automaticaly             */

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

/*
 *  id of resource
 */
typedef enum _resid {
    ICO_APF_RESID_BASIC_SCREEN  = 1, /* basic screen */
    ICO_APF_RESID_INT_SCREEN    = 2, /* interrupt screen */
    ICO_APF_RESID_ON_SCREEN     = 3, /* onscreeen */
    ICO_APF_RESID_BASIC_SOUND   = 4, /* basic sound */
    ICO_APF_RESID_INT_SOUND     = 5, /* interrupt sound */
    ICO_APF_RESID_INPUT_DEV     = 6, /* input device */
} ico_apf_resid_e;
#define ICO_APF_RESOURCE_RESID_MIN      ((int)(ICO_APF_RESID_BASIC_SCREEN))
#define ICO_APF_RESOURCE_RESID_MAX      ((int)(ICO_APF_RESID_INPUT_DEV))

/*
 *  state of resource
 */
typedef enum _resource_state {
    ICO_APF_RESOURCE_STATE_ACQUIRED     = 1,    /* acquired         */
    ICO_APF_RESOURCE_STATE_DEPRIVED     = 2,    /* deprived         */
    ICO_APF_RESOURCE_STATE_WAITTING     = 3,    /* waitting         */
    ICO_APF_RESOURCE_STATE_RELEASED     = 4,    /* released         */

    ICO_APF_RESOURCE_COMMAND_GET        = 5,    /* get command      */
    ICO_APF_RESOURCE_COMMAND_RELEASE    = 6,    /* release command  */
    ICO_APF_RESOURCE_COMMAND_ADD        = 7,    /* add command      */
    ICO_APF_RESOURCE_COMMAND_CHANGE     = 8,    /* change command   */
    ICO_APF_RESOURCE_COMMAND_DELETE     = 9,    /* delete command   */

    ICO_APF_RESOURCE_REPLY_OK           = 10,   /* OK reply         */
    ICO_APF_RESOURCE_REPLY_NG           = 11,   /* NG reply         */
    ICO_APF_RESOURCE_STATE_CONNECTED    = 12,   /* connected        */
    ICO_APF_RESOURCE_STATE_DISCONNECTED = 13,   /* disconnected     */
} ico_apf_resource_state_e;

/* Priority of resource                             */
#define ICO_UXF_PRIO_INTSCREEN   0x00000080 /* interrupt screen on basic screen     */
#define ICO_UXF_PRIO_CATEGORY    0x00000100 /* application category                 */
#define ICO_UXF_PRIO_ACTIVEAPP   0x0fff0000 /* active application count             */
#define ICO_UXF_PRIO_ACTIVECOUNT 0x00010000 /* active application                   */
#define ICO_UXF_PRIO_ONSCREEN    0x10000000 /* interrupt screen/sound               */
#define ICO_UXF_PRIO_REGULATION  0x40000000 /* no regulation controlled             */

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
typedef struct  _resource_request    {
//    struct _ico_apc_request     *next;          /* requestt list link               */
//    char                        appid[ICO_UXF_MAX_PROCESS_NAME+1];
                                                /* application id                   */
    //ico_apf_resid_e             resid;          /* resource id                      */
//    int resid;          /* resource id                      */
//    char                        device[ICO_UXF_MAX_DEVICE_NAME+1];
                                                /* request device                   */
    int                         id;             /* request object                   */
//    int                         bid;            /* request base object              */
//    int                         pid;            /* request client pid               */
    int                         prio;           /* request priority                 */
//    unsigned short              zoneidx;        /* request target zone index        */
//    unsigned short              timer;          /* Reply wait timer                 */
//    unsigned short              state;          /* status                           */
    unsigned short              reqtype;        /* Request type                     */
    unsigned short              category;        /* Request category */
#ifndef NO_MURPHY
//    mrp_res_resource_set_t      *rset;          /* Murphy resource set              */
//    mrp_res_queue_item_t        *res_data;      /* identifier for the request       */
//    mrp_list_hook_t             hook;           /* keep track of requests           */
#endif
    //int                         released;       /* Did the application release?     */

    int  resid;           /* resource id      */
    char *appid;          /* application id */
    int  pid;             /* process id     */
    int  state;           /* request state */

    char *dispzone;       /* area of showing application window */
    int  dispzoneid;      /* area of showing application window */
    char *winname;        /* window's surface name */
    int  surfaceid;       /* window id */

    char *soundzone;      /* area of playing sound */
    int  soundzoneid;     /* area of playing sound */
    char *sooudname;      /* sound stream name */
    int  soundid;         /* sound id */
    int  soundadjust;     /* adjust action */

    char *device;         /* input device name */
    int  input;           /* input event id */

    mrp_res_resource_set_t      *rset;          /* Murphy resource set */
    mrp_res_queue_item_t        *res_data;      /* identifier for the request */
    mrp_list_hook_t             hook;           /* keep track of requests */
    int                         released;

} resource_request_t;

typedef void (*ico_syc_mrp_enforce_display_t)(unsigned short state,
                                              const char *appid,
                                              unsigned int id,
                                              void *user_data);

typedef void (*ico_syc_mrp_enforce_sound_t)(unsigned short state,
                                            pid_t pid,
                                            void *user_data);

typedef void (*ico_syc_mrp_enforce_input_t)(unsigned short state,
                                            const char *appid,
                                            const char *device,
                                            void *user_data);

int ico_syc_mrp_init(ico_syc_mrp_enforce_display_t dispcb,
                     ico_syc_mrp_enforce_sound_t   soundcb,
                     ico_syc_mrp_enforce_input_t   inputcb,
                     void                          *user_data);
 

bool ico_syc_mrp_acquire_display_resource(resource_request_t *req, int addprio);
bool ico_syc_mrp_release_display_resource(resource_request_t *req);

bool ico_syc_mrp_acquire_sound_resource(resource_request_t *req, int addprio);
bool ico_syc_mrp_release_sound_resource(resource_request_t *req);

bool ico_syc_mrp_acquire_input_resource(resource_request_t *req, int addprio);
bool ico_syc_mrp_release_input_resource(resource_request_t *req);

#ifdef __cplusplus
}
#endif
#endif  /* __ICO_SYC_MRP_RESOURCE_PRIVATE_H__ */
/* vim:set expandtab ts=4 sw=4: */
