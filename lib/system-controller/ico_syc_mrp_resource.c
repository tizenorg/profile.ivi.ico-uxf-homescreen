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
 *  @file   ico_syc_mrp_resource.c
 *
 *  @brief 
 */
//==========================================================================

#include <murphy/common.h>
#include <murphy/common/ecore-glue.h>
#include <murphy/domain-control/client.h>

#include <ico_log.h>
#include "ico_syc_error.h"
#include "ico_syc_mrp_resource_private.h"


static void ico_syc_mrp_resource_cb(mrp_res_context_t *ctx,
                                    const mrp_res_resource_set_t *rs,
                                    void *userdata);

static void ico_syc_mrp_state_cb(mrp_res_context_t *ctx,
                                 mrp_res_error_t err,
                                 void *userdata);

static void ico_syc_mrp_connect_notify(mrp_domctl_t *dc,int connected,
                                       int errcode, const char *errmsg,
                                       void *user_data);

static void ico_syc_mrp_data_notify(mrp_domctl_t *dc,
                                    mrp_domctl_data_t *tables,
                                    int ntable, void *user_data);

static mrp_res_queue_item_t *
ico_syc_mrp_create_queue_item(resource_request_t *req);
static resource_request_t *
ico_syc_mrp_find_request(int req_id, char *appid);
#if 0
static void ico_syc_mrp_process_input_queue(void);
static void ico_syc_mrp_process_display_queue(void);
#endif
static void ico_syc_mrp_process_sound_queue(void);
            
/* Murphy resource library */ 
static bool                     mrp_res_connected = FALSE; 
static mrp_res_context_t        *mrp_ctx = NULL; 
static mrp_mainloop_t           *mrp_ml = NULL; 
static mrp_domctl_t             *mrp_dc = NULL; 
 
static mrp_list_hook_t          mrp_sound_wait_queue; 
 
static mrp_list_hook_t          mrp_reqs; 


/*
 * tables for inserting and reading data from Murphy
 */

#define ACTIVE_APP_COLUMNS      \
    "zone       varchar(128),"  \
    "appid      varchar(128)"

#define ACTIVE_APP_INDEX "zone, appid"

#define DISPLAY_OWNER_SELECT "*"
#define DISPLAY_OWNER_WHERE  NULL

mrp_domctl_table_t ico_tables[] = {
    MRP_DOMCTL_TABLE("active_screen",
                     ACTIVE_APP_COLUMNS,
                     ACTIVE_APP_INDEX),
};

mrp_domctl_watch_t ico_watches[] = {
    MRP_DOMCTL_WATCH("display-owner",
                     DISPLAY_OWNER_SELECT,
                     DISPLAY_OWNER_WHERE, 0),
};

static ico_syc_mrp_enforce_sound_t ico_syc_mrp_enforce_soundCB = NULL;
static void *cb_user_data = NULL;


/*-------------------------------------------------------------------------*/
/*
 *  public functions
 */
/*-------------------------------------------------------------------------*/
int ico_syc_mrp_init(ico_syc_mrp_enforce_sound_t soundcb, void *user_data)

{

    ICO_DBG("ico_syc_mrp_init: Enter");

    ico_syc_mrp_enforce_soundCB = soundcb;
    cb_user_data = user_data;

    mrp_list_init(&mrp_sound_wait_queue);

    mrp_list_init(&mrp_reqs);

    ICO_DBG("mrp_mainloop_ecore_get() called.");
    mrp_ml = mrp_mainloop_ecore_get();

    ICO_DBG("mrp_res_connecte() called.");
    mrp_ctx = mrp_res_create(mrp_ml, ico_syc_mrp_state_cb, NULL);

    if (!mrp_ctx) {
        ICO_ERR("ico_syc_mrp_init: Leave(mrp_res_create Error)");
        return ICO_SYC_EIO;
    }

    ICO_DBG("mrp_domctl_create() called.");
    mrp_dc = mrp_domctl_create("ico-homescreen", mrp_ml,
                               ico_tables, MRP_ARRAY_SIZE(ico_tables),
                               ico_watches, MRP_ARRAY_SIZE(ico_watches),
                               ico_syc_mrp_connect_notify,
                               ico_syc_mrp_data_notify, NULL);

    if (mrp_dc == NULL) {
        ICO_ERR("ico_syc_mrp_init: Leave(mrp_domctl_create Error)");
        return ICO_SYC_EIO;
    } 

    ICO_DBG("mrp_domctl_connect() called.");
    if (!mrp_domctl_connect(mrp_dc, MRP_DEFAULT_DOMCTL_ADDRESS, 0)) {
        ICO_ERR("ico_syc_mrp_init: Leave(mrp_domctl_connect Error)");
        return ICO_SYC_EIO;
    } 

    return ICO_SYC_EOK;
}

void
ico_syc_mrp_add_request(resource_request_t *req)
{
    /* store the request for Murphy */
    ICO_DBG("ico_syc_mrp_add_request: "
            "add request %p to mrp_reqs list", req);
    mrp_list_init(&req->hook);
    mrp_list_append(&mrp_reqs, &req->hook);

}

void
ico_syc_mrp_del_request(resource_request_t *req)
{
    if (NULL == req) {
        return;
    }

    ICO_DBG("ico_syc_mrp_del_request: "
            "del request %p to mrp_reqs list", req);
    mrp_list_delete(&req->hook);

    if (mrp_ctx && req->rset) {
        mrp_res_release_resource_set(mrp_ctx, req->rset);
        mrp_res_delete_resource_set(mrp_ctx, req->rset);
        req->rset = NULL;
    }
}

bool
ico_syc_mrp_acquire_sound_resource(resource_request_t *newreq)
{
    mrp_res_resource_set_t      *rs;

    ICO_DBG("ico_syc_mrp_acquire_sound_resource: Enter");

    resource_request_t *req = ico_syc_mrp_find_request(newreq->id,
                                                       newreq->appid);
    if (NULL == req) {
        ico_syc_mrp_add_request(newreq);
        req = newreq;
    }

    if (mrp_ctx) {
        if (!mrp_res_connected) {
            mrp_res_queue_item_t *item;

            ICO_DBG("ico_syc_mrp_acquire_sound_resource: "
                    "resource context is not connected yet, queueing");

            item = ico_syc_mrp_create_queue_item(req);
            mrp_list_append(&mrp_sound_wait_queue, &item->hook);

            ICO_DBG("ico_syc_mrp_acquire_sound_resource: Leave(true)");
            return true;
        }

        if (!req->rset) {
            int addprio = req->resid & RESID_TYPE_MASK;
            const char *resource_class =
                (addprio == RESID_TYPE_BASIC) ? "basic" : "emergency";
            mrp_res_resource_t *res;
            mrp_res_attribute_t *attr;

            ICO_DBG("ico_syc_mrp_acquire_sound_resource: "
                    "could not create resource for sound");

            ICO_DBG("mrp_res_create_resource_set called.");
            rs = mrp_res_create_resource_set(mrp_ctx,
                                             resource_class,
                                             ico_syc_mrp_resource_cb,
                                             req);

            if (!rs) {
                ICO_ERR("ico_syc_mrp_acquire_sound_resource: "
                        "could not create resource set for audio");
                return false;
            }

            ICO_DBG("mrp_res_set_autorelease called.");
            if (!mrp_res_set_autorelease(mrp_ctx, FALSE, rs)) {
                ICO_ERR("ico_syc_mrp_acquire_sound_resource: "
                        "failed to set auto release mode");
                ICO_DBG("ico_syc_mrp_acquire_sound_resource: Leave(false)");
                return false;
            }

            ICO_DBG("mrp_res_create_resource called.");
            res = mrp_res_create_resource(mrp_ctx, rs, "audio_playback", TRUE, FALSE);

            if (!res) {
                ICO_ERR("ico_syc_mrp_acquire_sound_resource: "
                        "could not create resource for audio");
                ICO_DBG("ico_syc_mrp_acquire_sound_resource: Leave(false)");
                return false;
            }

            ICO_DBG("pid=%d category=%d appid=%s priority=%d",
                    req->pid, req->category, req->appid, req->prio);
            attr = mrp_res_get_attribute_by_name(mrp_ctx, res, "pid");
            if (attr && attr->type == mrp_string) {
                char numbuf[16];
                int ret;

                ret = snprintf(numbuf, sizeof(numbuf), "%d", req->pid);

                if (ret > 0 && (size_t)ret < sizeof(numbuf)) {
                    mrp_res_set_attribute_string(mrp_ctx, attr, numbuf);
                }
            }
            attr = mrp_res_get_attribute_by_name(mrp_ctx, res, "category");
            if (attr && attr->type == mrp_int32) {
                mrp_res_set_attribute_int(mrp_ctx, attr, req->category);
            }

            attr = mrp_res_get_attribute_by_name(mrp_ctx, res, "appid");
            if (attr && attr->type == mrp_string) {
                mrp_res_set_attribute_string(mrp_ctx, attr, req->appid);
            }

            attr = mrp_res_get_attribute_by_name(mrp_ctx, res, "priority");
            if (attr && attr->type == mrp_int32) {
               mrp_res_set_attribute_int(mrp_ctx, attr, 0/*req->prio*/);
            }

            req->rset = rs;
            req->released = 0;
        }

        ICO_DBG("mrp_res_acquire_resource_set called.");
        mrp_res_acquire_resource_set(mrp_ctx, req->rset);
    }

    ICO_DBG("ico_syc_mrp_acquire_sound_resource: Leave(true)");
    return true;
}

bool
ico_syc_mrp_release_sound_resource(resource_request_t *req)
{
    ICO_DBG("ico_syc_mrp_release_sound_resource: Enter");

    ico_syc_mrp_del_request(req);
#if 0
    if (mrp_ctx && req->rset) {
        if (REQTYPE_APP == req->reqtype) {
            mrp_res_release_resource_set(mrp_ctx, req->rset);
            req->released = 1;
        }
        else {
            mrp_res_release_resource_set(mrp_ctx, req->rset);
            mrp_res_delete_resource_set(mrp_ctx, req->rset);
            req->rset = NULL;
        }
    }
#endif
    ICO_DBG("ico_syc_mrp_release_sound_resource: Leave(true)");
    return true;
}

/*-------------------------------------------------------------------------*/
/*
 *  private functions
 */
/*-------------------------------------------------------------------------*/
static mrp_res_queue_item_t *
ico_syc_mrp_create_queue_item(resource_request_t *req)
{
    mrp_res_queue_item_t *item = mrp_allocz(sizeof(mrp_res_queue_item_t));
    int ret = 0;

    ICO_DBG("ico_syc_mrp_create_queue_item: Enter");

    if (NULL == item) {
        ICO_ERR("mrp_allocz failed.");
        return NULL;
    }

    mrp_list_init(&item->hook);

    if (req->resid | RESID_KIND_SOUND) {
        item->req_id   = req->soundid;
        item->zone_idx = req->soundzoneid;
    }

    item->prio     = req->prio;
    item->res_type = req->resid;

    ret = snprintf(item->appid, MURPHY_APPID_MAXIMUM_LENGTH, "%s", req->appid);

    if (ret < 0 || ret >= MURPHY_APPID_MAXIMUM_LENGTH) {
        mrp_free(item);
        ICO_DBG("ico_syc_mrp_create_queue_item: Leave(NULL)");
        return NULL;
    }

    ICO_DBG("murphy: created queue item (id: %d, zone: %d, prio: %d, type: %d)",
            item->req_id, item->zone_idx, item->prio, item->res_type);

    ICO_DBG("ico_syc_mrp_create_queue_item: Leave(0x%08x)", item);
    return item;
}

static resource_request_t *
ico_syc_mrp_find_request(int req_id, char *appid)
{
    mrp_list_hook_t *p, *n;
    resource_request_t *req;

    ICO_DBG("ico_syc_mrp_find_request: Enter");

    /* go through all mrp_res_queue_item_t structs in the list */

    mrp_list_foreach(&mrp_reqs, p, n) {
        req = mrp_list_entry(p, typeof(*req), hook);

        ICO_DBG("ico_syc_mrp_find_request: req(id=%d appid=%s)",
                req->id, req->appid);
        if (req->id == req_id && strcmp(req->appid, appid) == 0) {
            ICO_DBG("ico_syc_mrp_find_request: Leave(0x%08x)", req);
            return req;
        }
    }

    ICO_DBG("ico_syc_mrp_find_request: Leave(NULL)");
    return NULL;
}

static void
ico_syc_mrp_process_sound_queue(void)
{
    mrp_list_hook_t *p, *n;
    mrp_res_queue_item_t *item;

    ICO_DBG("ico_syc_mrp_process_sound_queue: Enter");

    /* go through all mrp_res_queue_item_t structs in the list */

    mrp_list_foreach(&mrp_sound_wait_queue, p, n) {
        resource_request_t *req;
        item = mrp_list_entry(p, typeof(*item), hook);

        req = ico_syc_mrp_find_request(item->req_id, item->appid);

        if (req) {
            ico_syc_mrp_acquire_sound_resource(req);
        }
        else  {
            ICO_WRN("murphy: application (appid: %s, id: %d, zone: %d)"
                    " went away before resource handling",
                    item->appid, item->req_id, item->zone_idx);
        }

        mrp_list_delete(&item->hook);
        mrp_free(item);
    }

    ICO_DBG("ico_syc_mrp_process_sound_queue: Leave");
}
                       
static void
ico_syc_mrp_resource_cb(mrp_res_context_t *ctx,
                        const mrp_res_resource_set_t *rs,
                        void *userdata)
{
    resource_request_t *req = NULL;

    //mrp_res_queue_item_t *item = userdata;
    resource_request_t *item = userdata;

    unsigned short state = RES_STATE_ACQUIRED;

    ICO_DBG("ico_syc_mrp_resource_cb: Enter"
            "(req_id=%d appid=%s)", item->id, item->appid);
            //"(req_id=%d appid=%s)", item->req_id, item->appid);

    if (NULL == item) {
        return;
    }

    //if (item->res_type | RESID_KIND_SOUND) {
    if (item->resid & RESID_KIND_SOUND) {
        //req = ico_syc_mrp_find_request(item->req_id, item->appid);
        req = ico_syc_mrp_find_request(item->id, item->appid);
        ICO_DBG("RESID_KIND_SOUND req=0x%08x", req);
    }

    if (NULL == req) {
        ICO_ERR("murphy: the request wasn't found!");
        ICO_DBG("ico_syc_mrp_resource_cb: Leave");
        return;
    }

    if (!mrp_res_equal_resource_set(rs, req->rset)) {
        ICO_DBG("mrp_res_equal_resource_set : not match");
        ICO_DBG("ico_syc_mrp_resource_cb: Leave");
        return;
    }

    /* delete the previous set */
    ICO_DBG("called: mrp_res_delete_resource");
    mrp_res_delete_resource_set(ctx, req->rset);

    /* copy the new set into place */
    ICO_DBG("called: mrp_res_copy_resource_set");
    req->rset = mrp_res_copy_resource_set(ctx, rs);

    switch(req->rset->state) {
    case MRP_RES_RESOURCE_ACQUIRED:
        ICO_DBG("ico_syc_mrp_resource_cb: state=MRP_RES_RESOURCE_ACQUIRED");
        state = RES_STATE_ACQUIRED;
        break;
    case MRP_RES_RESOURCE_LOST:
        ICO_DBG("ico_syc_mrp_resource_cb: state=MRP_RES_RESOURCE_LOST");
    case MRP_RES_RESOURCE_AVAILABLE:
        ICO_DBG("ico_syc_mrp_resource_cb: state=MRP_RES_RESOURCE_AVAILABLE");
        if (req->released) {
            state = RES_STATE_RELEASED;
        }
        else {
            state = RES_STATE_WAITING;
        }
        break;
    default:
        ICO_WRN("ico_syc_mrp_resource_cb: state=UNKNOWN");
        state = RES_STATE_RELEASED;
        break;
    }

    /* send decision to client, if needed */
    if (req->reqtype == REQTYPE_APP) {
        if (req->state != state) {
// TODO CicoSCServer::sendMessage();
//            if (ico_apf_resource_send_to_client(
//                req->appid, state, req->resid, req->device, req->id)
//                        != ICO_APF_RESOURCE_E_NONE) {
//                ICO_WRN("murphy: failed to send decision to client (%s)",
//                        req->appid);
//            }
            req->state = state;
        }
    }

    /* do the enforcing */
    if (req->resid & RESID_KIND_SOUND) {
        if (NULL != ico_syc_mrp_enforce_soundCB) {
            ico_syc_mrp_enforce_soundCB(state, req, cb_user_data);
        }
    }
    else {
        ICO_WRN("unknown resource id");
    }

    ICO_DBG("ico_syc_mrp_resource_cb: Leave");
}
                       
static void
ico_syc_mrp_state_cb(mrp_res_context_t *ctx,
                     mrp_res_error_t err,
                     void *userdata)
{
    ICO_DBG("ico_syc_mrp_state_cb: Enter");
    if (err != MRP_RES_ERROR_NONE) {
        ICO_DBG("ico_syc_mrp_state_cb: state callback error(%d)", err);
        return;
    }

    switch (ctx->state)
    {
        case MRP_RES_CONNECTED:
            ICO_DBG("ico_syc_mrp_state_cb: connected to murphyd");
            mrp_res_connected = TRUE;

            /* process the queued resources */
            ico_syc_mrp_process_sound_queue();
            break;
        case MRP_RES_DISCONNECTED:
            ICO_DBG("ico_syc_mrp_state_cb: disconnected from murphyd");
            if (ctx) {
                mrp_res_destroy(ctx);
                mrp_ctx = NULL;
            }
            mrp_res_connected = FALSE;
            break;
        default:
            ICO_ERR("ico_syc_mrp_state_cb: state error");
            break;
    }

    ICO_DBG("ico_syc_mrp_state_cb: Leave");
}

static void
ico_syc_mrp_connect_notify(mrp_domctl_t *dc,int connected,
                           int errcode, const char *errmsg,
                           void *user_data)
{
    ICO_DBG("ico_syc_mrp_connect_notify: Enter");
    ICO_DBG("ico_syc_mrp_connect_notify: Leave");
}

static void
ico_syc_mrp_data_notify(mrp_domctl_t *dc,
                        mrp_domctl_data_t *tables,
                        int ntable, void *user_data)
{
    ICO_DBG("ico_syc_mrp_data_notify: Enter");
    ICO_DBG("ico_syc_mrp_data_notify: Leave");
}

static void
ico_sys_mrp_export_notify(mrp_domctl_t *dc, int errcode,
                          const char *errmsg, void *user_data)
{
    MRP_UNUSED(dc);
    MRP_UNUSED(user_data);

    if (errcode != 0) {
        ICO_ERR("murphy: exporting data to Murphy failed "
                "(%d: %s).", errcode, errmsg);
    }
}

void
ico_syc_mrp_active_app(const char *appid)
{
    mrp_domctl_data_t tables[1];
    mrp_domctl_value_t values[2];

    ICO_DBG("ico_syc_mrp_active_app: active app '%s'", appid);

    memset(tables, 0, sizeof(tables));
    memset(values, 0, sizeof(values));

    tables[0].id = 0;
    tables[0].ncolumn = 2;
    tables[0].nrow = 1;
    tables[0].rows = mrp_allocz(sizeof(*tables[0].rows));
    tables[0].rows[0] = values;

    values[0].type = MRP_DOMCTL_STRING;
    values[0].str = "driver";
    values[1].type = MRP_DOMCTL_STRING;
    values[1].str = appid;

    if (!mrp_domctl_set_data(mrp_dc, tables, 1,
                             ico_sys_mrp_export_notify, NULL)) {
        ICO_DBG("ico_syc_mrp_active_app: "
                "failed to export active app data to Murphy");
    }

    mrp_free(tables[0].rows);
}
/* vim:set expandtab ts=4 sw=4: */
