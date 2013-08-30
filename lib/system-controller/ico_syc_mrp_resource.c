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

#include "ico_log.h"
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
ico_syc_mrp_create_queue_item(resource_request_t *req, int prio);
static resource_request_t *
ico_syc_mrp_find_request(int req_id, char *appid);
static void ico_syc_mrp_process_input_queue(void);
static void ico_syc_mrp_process_display_queue(void);
static void ico_syc_mrp_process_sound_queue(void);
            
/* Murphy resource library */ 
static bool                     mrp_res_connected = FALSE; 
static mrp_res_context_t        *mrp_ctx = NULL; 
static mrp_mainloop_t           *mrp_ml = NULL; 
static mrp_domctl_t             *mrp_dc = NULL; 
 
static mrp_list_hook_t          mrp_disp_wait_queue; 
static mrp_list_hook_t          mrp_sound_wait_queue; 
static mrp_list_hook_t          mrp_input_wait_queue; 
 
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

static ico_syc_mrp_enforce_display_t ico_syc_mrp_enforce_displayCB = NULL;
static ico_syc_mrp_enforce_sound_t   ico_syc_mrp_enforce_soundCB   = NULL;
static ico_syc_mrp_enforce_input_t   ico_syc_mrp_enforce_inputCB   = NULL;
static void *cb_user_data = NULL;


/*-------------------------------------------------------------------------*/
/*
 *  pablic functions
 */
/*-------------------------------------------------------------------------*/
int ico_syc_mrp_init(ico_syc_mrp_enforce_display_t dispcb,
                     ico_syc_mrp_enforce_sound_t   soundcb,
                     ico_syc_mrp_enforce_input_t   inputcb,
                     void *user_data)

{

    ICO_DBG("ico_syc_mrp_init: Enter");

    ico_syc_mrp_enforce_displayCB = dispcb;
    ico_syc_mrp_enforce_soundCB   = soundcb;
    ico_syc_mrp_enforce_inputCB   = inputcb;
    cb_user_data = user_data;

    mrp_list_init(&mrp_disp_wait_queue);
    mrp_list_init(&mrp_sound_wait_queue);
    mrp_list_init(&mrp_input_wait_queue);

    mrp_list_init(&mrp_reqs);

    mrp_ml = mrp_mainloop_ecore_get();

    ICO_DBG("mrp_res_connecte() called.");
    mrp_ctx = mrp_res_create(mrp_ml, ico_syc_mrp_state_cb, NULL);

    if (!mrp_ctx) {
        ICO_ERR("ico_syc_mrp_init: Leave(mrp_res_create Error)");
        return ICO_SYC_ENOSYS;
    }

    ICO_DBG("mrp_domctl_create() called.");
    mrp_dc = mrp_domctl_create("ico-homescreen", mrp_ml,
                               ico_tables, MRP_ARRAY_SIZE(ico_tables),
                               ico_watches, MRP_ARRAY_SIZE(ico_watches),
                               ico_syc_mrp_connect_notify,
                               ico_syc_mrp_data_notify, NULL);

    if (mrp_dc == NULL) {
        ICO_ERR("ico_syc_mrp_init: Leave(mrp_domctl_create Error)");
        return ICO_SYC_ENOSYS;
    } 

    ICO_DBG("mrp_domctl_connect() called.");
    if (!mrp_domctl_connect(mrp_dc, MRP_DEFAULT_DOMCTL_ADDRESS, 0)) {
        ICO_ERR("ico_syc_mrp_init: Leave(mrp_domctl_connect Error)");
        return ICO_SYC_ENOSYS;
    } 
    return ICO_SYC_EOK;
}

bool
ico_syc_mrp_acquire_display_resource(resource_request_t *req, int addprio)
{
    mrp_res_resource_set_t *rs;

    ICO_DBG("ico_syc_mrp_acquire_display_resource: Enter");

    if (NULL == mrp_ctx) {
        ICO_DBG("ico_syc_mrp_acquire_display_resource: Leave(false)");
        return false;
    }

    if (false == mrp_res_connected) {
        mrp_res_queue_item_t *item;

        ICO_ERR("murphy: resource context is not connected yet, queueing");

        item = ico_syc_mrp_create_queue_item(req, addprio);
        mrp_list_append(&mrp_disp_wait_queue, &item->hook);

        ICO_DBG("ico_syc_mrp_acquire_display_resource: Leave(true)");
        return true;
    }
    else {
        /* store the request for Murphy */
        mrp_list_init(&req->hook);
        mrp_list_append(&mrp_reqs, &req->hook);
    }

    if (!req->rset) {
        const char *resource_class = (addprio == ICO_UXF_PRIO_ONSCREEN) ? "emergency" : "basic";
        mrp_res_resource_t   *res;
        mrp_res_attribute_t  *attr;
        mrp_res_queue_item_t *item;

        ICO_DBG("murphy: going to create new resource set for screen");

        item = ico_syc_mrp_create_queue_item(req, addprio);

        if (NULL == item) {
            return false;
        }

        rs = mrp_res_create_resource_set(mrp_ctx,
                                         resource_class,
                                         ico_syc_mrp_resource_cb,
                                         item);

        if (NULL == rs) {
            ICO_ERR("murphy: could not create resource set for screen");
            return false;
        }

        if (!mrp_res_set_autorelease(mrp_ctx, FALSE, rs)) {
            ICO_ERR("murphy: failed to set autorelease mode");
            return false;
        }

        res = mrp_res_create_resource(mrp_ctx, rs, "screen", TRUE, FALSE);

        if (!res) {
            ICO_ERR("murphy: could not create resource for screen");
            return false;
        }

        attr = mrp_res_get_attribute_by_name(mrp_ctx, res, "pid");
        ICO_DBG("murphy: attr=%x", attr);
        if (attr) {
            char numbuf[16];
            unsigned int ret;

            ret = snprintf(numbuf, sizeof(numbuf), "%d", req->pid);

            if (ret > 0 && ret < sizeof(numbuf)) {
                mrp_res_set_attribute_string(mrp_ctx, attr, numbuf);
            }      
        }

        attr = mrp_res_get_attribute_by_name(mrp_ctx, res, "category");
        ICO_DBG("murphy: attr=%x", attr);
        if (attr && attr->type == mrp_int32) {
            mrp_res_set_attribute_int(mrp_ctx, attr, req->category);
        }

        attr = mrp_res_get_attribute_by_name(mrp_ctx, res, "appid");
        ICO_DBG("murphy: attr=%x", attr);
        if (attr && attr->type == mrp_string) {
            mrp_res_set_attribute_string(mrp_ctx, attr, req->appid);
        }

        attr = mrp_res_get_attribute_by_name(mrp_ctx, res, "priority");
        ICO_DBG("murphy: attr=%x", attr);
        if (attr && attr->type == mrp_int32) {
            mrp_res_set_attribute_int(mrp_ctx, attr, addprio);
        }

        req->rset = rs;
        req->released = 0;
    }

    ICO_DBG("murphy: acquiring the resource set");
    mrp_res_acquire_resource_set(mrp_ctx, req->rset);

    ICO_DBG("ico_syc_mrp_acquire_display_resource: Leave(true)");
    return true;
}

bool
ico_syc_mrp_release_display_resource(resource_request_t *req)
{
    ICO_DBG("murphy: removed request %p from mrp_reqs list", req);
    mrp_list_delete(&req->hook);

    if (mrp_ctx && req->rset) {
        mrp_res_release_resource_set(mrp_ctx, req->rset);
        mrp_res_delete_resource_set(mrp_ctx, req->rset);
        mrp_free(req->res_data);
        req->rset = NULL;
    }

    return true;
}

bool
ico_syc_mrp_acquire_sound_resource(resource_request_t *req, int addprio)
{
    mrp_res_resource_set_t      *rs;

    if (mrp_ctx) {
        if (!mrp_res_connected) {
            mrp_res_queue_item_t *item;

            ICO_DBG("murphy: resource context is not connected yet, queueing");

            item = ico_syc_mrp_create_queue_item(req, addprio);
            mrp_list_append(&mrp_sound_wait_queue, &item->hook);

            return TRUE;
        }

        if (!req->rset) {
            const char *resource_class = "basic";
            mrp_res_resource_t *res;
            ICO_DBG("murphy: going to create new resource set for input");
            mrp_res_attribute_t *attr;

            ICO_DBG("murphy: going to create new resource set for input");

            rs = mrp_res_create_resource_set(mrp_ctx,
                                             resource_class,
                                             ico_syc_mrp_resource_cb,
                                             req);

            if (!rs) {
                ICO_DBG("murphy: could not create resource set for input");
                return FALSE;
            }

            if (!mrp_res_set_autorelease(mrp_ctx, FALSE, rs)) {
                ICO_DBG("murphy: failed to set autorelease mode");
                return FALSE;
            }

            res = mrp_res_create_resource(mrp_ctx, rs, "input", TRUE, FALSE);

            if (!res) {
                ICO_DBG("murphy: could not create resource for input");
                return FALSE;
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
               mrp_res_set_attribute_int(mrp_ctx, attr, addprio);
            }

            req->rset = rs;
            req->released = 0;
        }

        ICO_DBG("murphy: acquiring the resource set");
        mrp_res_acquire_resource_set(mrp_ctx, req->rset);
    }

    return TRUE;
}

bool
ico_syc_mrp_release_sound_resource(resource_request_t *req)
{
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

    return true;
}

bool
ico_syc_mrp_acquire_input_resource(resource_request_t *req, int addprio)
{
    mrp_res_resource_set_t      *rs;
    //Ico_Uxf_conf_application    *conf = (Ico_Uxf_conf_application *)get_appconf(req->appid);

    if (mrp_ctx) {

        if (!mrp_res_connected) {
            mrp_res_queue_item_t *item;

            ICO_ERR("murphy: resource context is not connected yet, queueing");

            item = ico_syc_mrp_create_queue_item(req, addprio);
            mrp_list_append(&mrp_input_wait_queue, &item->hook);

            return TRUE;
        }

        if (!req->rset) {
            const char *resource_class = "basic";
            mrp_res_resource_t *res;
            ICO_DBG("murphy: going to create new resource set for input");
            mrp_res_attribute_t *attr;

            ICO_DBG("murphy: going to create new resource set for input");


            rs = mrp_res_create_resource_set(mrp_ctx,
                                             resource_class,
                                             ico_syc_mrp_resource_cb,
                                             req);

            if (!rs) {
                ICO_DBG("murphy: could not create resource set for input");
                return FALSE;
            }

            if (!mrp_res_set_autorelease(mrp_ctx, FALSE, rs)) {
                ICO_DBG("murphy: failed to set autorelease mode");
                return FALSE;
            }

            res = mrp_res_create_resource(mrp_ctx, rs, "input", TRUE, FALSE);

            if (!res) {
                ICO_DBG("murphy: could not create resource for input");
                return FALSE;
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
               mrp_res_set_attribute_int(mrp_ctx, attr, addprio);
            }

            req->rset = rs;
            req->released = 0;
        }

        ICO_DBG("murphy: acquiring the resource set");
        mrp_res_acquire_resource_set(mrp_ctx, req->rset);
    }

    return false;
}

bool
ico_syc_mrp_release_input_resource(resource_request_t *req)
{
    if (mrp_ctx && req->rset) {
        mrp_res_release_resource_set(mrp_ctx, req->rset);
        mrp_res_delete_resource_set(mrp_ctx, req->rset);
        req->rset = NULL;
    }

    return true;
}

/*-------------------------------------------------------------------------*/
/*
 *  private functions
 */
/*-------------------------------------------------------------------------*/
static mrp_res_queue_item_t *
ico_syc_mrp_create_queue_item(resource_request_t *req, int prio)
{
    mrp_res_queue_item_t *item = mrp_allocz(sizeof(mrp_res_queue_item_t));
    int ret;

    if (NULL == item) {
        ICO_ERR("mrp_allocz failaed.");
        return NULL;
    }

    mrp_list_init(&item->hook);

    if (req->resid | RESID_KIND_DISPLAY) {
        item->req_id   = req->surfaceid;
        item->zone_idx = req->dispzoneid;
    }
    else if (req->resid | RESID_KIND_SOUND) {
        item->req_id   = req->soundid;
        item->zone_idx = req->soundzoneid;
    }
    else if (req->resid | RESID_KIND_INPUT) {
        item->req_id = req->input;
    }
    item->prio     = prio;
    item->res_type = req->resid;

    ret = snprintf(item->appid, MURPHY_APPID_MAXIMUM_LENGTH, "%s", req->appid);

    if (ret < 0 || ret >= MURPHY_APPID_MAXIMUM_LENGTH) {
        mrp_free(item);
        return NULL;
    }

    ICO_DBG("murphy: created queue item (id: %d, zone: %d, prio: %d, type: %d)",
            item->req_id, item->zone_idx, item->prio, item->res_type);

    return item;
}

static resource_request_t *
ico_syc_mrp_find_request(int req_id, char *appid)
{
    mrp_list_hook_t *p, *n;
    resource_request_t *req;

    /* go through all mrp_res_queue_item_t structs in the list */

    mrp_list_foreach(&mrp_reqs, p, n) {
        req = mrp_list_entry(p, typeof(*req), hook);

        ICO_DBG("ico_syc_mrp_find_request: req(id=%d appid=%s)",
                req->id, req->appid);
        if (req->id == req_id && strcmp(req->appid, appid) == 0) {
            return req;
        }
    }

    return NULL;
}

       
static void
ico_syc_mrp_process_display_queue(void)
{
    mrp_list_hook_t *p, *n;
    mrp_res_queue_item_t *item;

    /* go through all mrp_res_queue_item_t structs in the list */

    mrp_list_foreach(&mrp_disp_wait_queue, p, n) {
        resource_request_t *req;
        item = mrp_list_entry(p, typeof(*item), hook);

        req = ico_syc_mrp_find_request(item->req_id, item->appid);

        if (req) {
            ico_syc_mrp_acquire_display_resource(req, item->prio);
        }
        else  {
            ICO_WRN("murphy: application (appid: %s, id: %d, zone: %d)"
                    " went away before resource handling",
                    item->appid, item->req_id, item->zone_idx);
        }

        mrp_list_delete(&item->hook);
        mrp_free(item);
    }
}
        
static void
ico_syc_mrp_process_sound_queue(void)
{
    mrp_list_hook_t *p, *n;
    mrp_res_queue_item_t *item;

    /* go through all mrp_res_queue_item_t structs in the list */

    mrp_list_foreach(&mrp_sound_wait_queue, p, n) {
        resource_request_t *req;
        item = mrp_list_entry(p, typeof(*item), hook);

        req = ico_syc_mrp_find_request(item->req_id, item->appid);

        if (req) {
            ico_syc_mrp_acquire_sound_resource(req, item->prio);
        }
        else  {
            ICO_WRN("murphy: application (appid: %s, id: %d, zone: %d)"
                    " went away before resource handling",
                    item->appid, item->req_id, item->zone_idx);
        }

        mrp_list_delete(&item->hook);
        mrp_free(item);
    }
}
                       
static void
ico_syc_mrp_process_input_queue(void)
{
    mrp_list_hook_t *p, *n;
    mrp_res_queue_item_t *item;

    /* go through all mrp_res_queue_item_t structs in the list */

    mrp_list_foreach(&mrp_input_wait_queue, p, n) {
        resource_request_t *req;
        item = mrp_list_entry(p, typeof(*item), hook);

        req = ico_syc_mrp_find_request(item->req_id, item->appid);

        if (req) {
            ico_syc_mrp_acquire_input_resource(req, item->prio);
        }
        else  {
            ICO_WRN("murphy: application (appid: %s, id: %d, zone: %d)"
                    " went away before resource handling",
                    item->appid, item->req_id, item->zone_idx);
        }

        mrp_list_delete(&item->hook);
        mrp_free(item);
    }

}
           
static void
ico_syc_mrp_resource_cb(mrp_res_context_t *ctx,
                        const mrp_res_resource_set_t *rs,
                        void *userdata)
{
    resource_request_t *req = NULL;

    mrp_res_queue_item_t *item = userdata;

    unsigned short state = ICO_APF_RESOURCE_STATE_ACQUIRED;

    ICO_DBG("ico_syc_mrp_resource_cb: Enter"
            "(req_id=%d appid=%s)", item->req_id, item->appid);

    if (NULL == item) {
        return;
    }

    if (item->res_type | RESID_KIND_DISPLAY) {
        req = ico_syc_mrp_find_request(item->req_id, item->appid);
        ICO_DBG("RESID_KIND_DISPLAY req=0x%08x", req);
    }
    else if (item->res_type | RESID_KIND_SOUND) {
        req = ico_syc_mrp_find_request(item->req_id, item->appid);
        ICO_DBG("RESID_KIND_SOUND req=0x%08x", req);
    }
    else if (item->res_type | RESID_KIND_INPUT) {
        req = ico_syc_mrp_find_request(item->req_id, item->appid);
        ICO_DBG("RESID_KIND_INPUT req=0x%08x", req);
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
    mrp_res_delete_resource_set(ctx, req->rset);

    /* copy the new set into place */
    req->rset = mrp_res_copy_resource_set(ctx, rs);

    switch(req->rset->state) {
    case MRP_RES_RESOURCE_ACQUIRED:
        ICO_DBG("ico_syc_mrp_resource_cb: state=MRP_RES_RESOURCE_ACQUIRED");
        state = ICO_APF_RESOURCE_STATE_ACQUIRED;
        break;
    case MRP_RES_RESOURCE_LOST:
        ICO_DBG("ico_syc_mrp_resource_cb: state=MRP_RES_RESOURCE_LOST");
    case MRP_RES_RESOURCE_AVAILABLE:
        ICO_DBG("ico_syc_mrp_resource_cb: state=MRP_RES_RESOURCE_AVAILABLE");
        if (req->released) {
            state = ICO_APF_RESOURCE_STATE_RELEASED;
        }
        else {
            state = ICO_APF_RESOURCE_STATE_WAITTING;
        }
        break;
    default:
        ICO_WRN("ico_syc_mrp_resource_cb: state=UNKNOWN");
        state = ICO_APF_RESOURCE_STATE_RELEASED;
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

    if (req->resid | RESID_KIND_DISPLAY) {
        if (NULL != ico_syc_mrp_enforce_displayCB) {
            ico_syc_mrp_enforce_displayCB(state, req->appid,
                                          req->id, cb_user_data);
        }
    }
    else if (req->resid | RESID_KIND_SOUND) {
        if (NULL != ico_syc_mrp_enforce_soundCB) {
            ico_syc_mrp_enforce_soundCB(state, req->pid, cb_user_data);
        }
    }
    else if (req->resid | RESID_KIND_INPUT) {
        if (NULL != ico_syc_mrp_enforce_inputCB) {
            ico_syc_mrp_enforce_inputCB(state, req->appid,
                                        req->device, cb_user_data);
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
    if (err != MRP_RES_ERROR_NONE) {
        ICO_DBG("murphy: state callback error");
        return;
    }

    switch (ctx->state)
    {
        case MRP_RES_CONNECTED:
            ICO_DBG("murphy: connected to murphyd");
            mrp_res_connected = TRUE;

            /* process the queued resources */
            ico_syc_mrp_process_display_queue();
            ico_syc_mrp_process_sound_queue();
            ico_syc_mrp_process_input_queue();
            break;
        case MRP_RES_DISCONNECTED:
            ICO_DBG("murphy: disconnected from murphyd");
            if (ctx) {
                mrp_res_destroy(ctx);
                mrp_ctx = NULL;
            }
            mrp_res_connected = FALSE;
            break;
        default:
            ICO_DBG("murphy: state error");
            break;
    }
}

static void
ico_syc_mrp_connect_notify(mrp_domctl_t *dc,int connected,
                           int errcode, const char *errmsg,
                           void *user_data)
{
}

static void
ico_syc_mrp_data_notify(mrp_domctl_t *dc,
                        mrp_domctl_data_t *tables,
                        int ntable, void *user_data)
{
}
/* vim:set expandtab ts=4 sw=4: */
