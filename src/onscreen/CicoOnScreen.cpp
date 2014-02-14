/*
 * Copyright (c) 2014, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   On Screen
 *
 * @date    Jan-07-2014
 */
#include "CicoOnScreen.h"
#include <Ecore.h>
#include <Ecore_Wayland.h>

using namespace std;

//==========================================================================
// static members
//==========================================================================
CicoOnScreen * CicoOnScreen::os_instance=NULL;
struct popup_data CicoOnScreen::ico_appdata;

//==========================================================================
// functions
//==========================================================================

//--------------------------------------------------------------------------
/**
 * @brief   CicoOnScreen::CicoOnScreen
 *          Constractor
 *
 * @param[in]   none
 * @return      none
 */
//--------------------------------------------------------------------------
CicoOnScreen::CicoOnScreen(void)
{
    ICO_TRA("CicoOnScreen::CicoOnScreen Enter");

    InitializePopupData();

    ICO_TRA("CicoOnScreen::CicoOnScreen Leave");
}

//--------------------------------------------------------------------------
/**
 * @brief   CicoOnScreen::~CicoOnScreen
 *          Destractor
 *
 * @param[in]   none
 * @return      none
 */
//--------------------------------------------------------------------------
CicoOnScreen::~CicoOnScreen(void)
{
//    ICO_TRA("CicoOnScreen::~CicoOnScreen Enter");
//    ICO_TRA("CicoOnScreen::~CicoOnScreen Leave");
}

//--------------------------------------------------------------------------
/**
 * @brief   CicoOnScreen::InitializePopupData
 *          Initialize popup struct data
 *
 * @param[in]   none
 * @return      none
 */
//--------------------------------------------------------------------------
void
CicoOnScreen::InitializePopupData(void)
{
    ICO_TRA("CicoOnScreen::InitializePopupData Enter");

    ico_appdata.window = NULL;
    ico_appdata.icon = NULL;
    ico_appdata.noti = NULL;
    ico_appdata.show_flag = FALSE;
    ico_appdata.noti = NULL;

    ICO_TRA("CicoOnScreen::InitializePopupData Leave");
}

//--------------------------------------------------------------------------
/**
 * @brief   CicoOnScreen::StartOnScreen
 *          Start on screen
 *
 * @param[in]   none
 * @return      none
 * @return true on success, false on error
 */
//--------------------------------------------------------------------------
bool
CicoOnScreen::StartOnScreen(void)
{
    ICO_TRA("CicoOnScreen::StartOnScreen Enter");

    ico_syc_connect(EventCallBack, NULL);

    // save instance pointer
    os_instance = this;

    // Initialize
    ecore_evas_init();
    InitializePopup();

    // set notification callback function
    notiservice_.SetCallback(NotificationCallback, this);

    ICO_TRA("CicoOnScreen::StartOnScreen Leave(true)");
    return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   CicoOnScreen::Finalize
 *          Finalize
 *
 * @param[in]   none
 * @return      none
 */
//--------------------------------------------------------------------------
void
CicoOnScreen::Finalize(void)
{
    ICO_TRA("CicoOnScreen::Finalize Enter");

    // Free window handle
    if (ico_appdata.window != NULL) {
        ecore_evas_free(ico_appdata.window);
    }

    ICO_TRA("CicoOnScreen::Finalize Leave");
}

//--------------------------------------------------------------------------
/**
 * @brief   CicoOnScreen::ShowPopup
 *          Show popup window
 *
 * @param[in]   none
 * @return      none
 */
//--------------------------------------------------------------------------
void
CicoOnScreen::ShowPopup(CicoNotification &noti)
{
    ICO_TRA("CicoOnScreen::ShowPopup Enter");

	ico_appdata.noti = new CicoNotification(noti.GetNotiHandle());

	// get caller pachage name
	const char *pkgname = noti.GetPkgname();

	// get priv_id
	int priv_id = noti.GetPrivId();

	// Get title
	const char *title = noti.GetTitle();

	// Get content
	const char *content = noti.GetContent();

	// Get execute option
	const char *text = NULL;
	bundle *service_handle = NULL;
	(void)noti.GetExecuteOption(NOTIFICATION_EXECUTE_TYPE_SINGLE_LAUNCH,
								&text, &service_handle);

	// Get icon path
	const char *icon = noti.GetIconPath();
	ICO_DBG("Received: %s : %i : %s : %s : %s : %x",
			pkgname, priv_id, title, content,
			text, (int)service_handle);

    if (icon) {
        if (NULL != ico_appdata.icon) {
            evas_object_image_file_set(ico_appdata.icon, icon, NULL);
            evas_object_show(ico_appdata.icon);
        }
    }

    if (title) {
        if (!edje_object_part_text_set(ico_appdata.theme, "title_text", title)) {
            ICO_WRN("could not set the text. Maybe part 'text' does not exist?");
        }
    }

    if (content) {
        if (strlen(content) <= 25) {
            ICO_DBG("content text center");
            if (!edje_object_part_text_set(ico_appdata.theme, "content_text_center", content)) {
                ICO_WRN("could not set the text. Maybe part 'text' does not exist?");
            }
            if (!edje_object_part_text_set(ico_appdata.theme, "content_text_left", "")) {
                ICO_WRN("could not set the text. Maybe part 'text' does not exist?");
            }
        }
        else {
            ICO_DBG("content text left");
            if (!edje_object_part_text_set(ico_appdata.theme, "content_text_left", content)) {
                ICO_WRN("could not set the text. Maybe part 'text' does not exist?");
            }
            if (!edje_object_part_text_set(ico_appdata.theme, "content_text_center", "")) {
                ICO_WRN("could not set the text. Maybe part 'text' does not exist?");
            }
        }
    }

    if (text) {
        if (!edje_object_part_text_set(ico_appdata.theme, "button_text", text)) {
            ICO_WRN("could not set the text. Maybe part 'text' does not exist?");
        }
    }
    else {
        if (!edje_object_part_text_set(ico_appdata.theme, "button_text", "OK")) {
            ICO_WRN("could not set the text. Maybe part 'text' does not exist?");
        }
    }

    evas_object_show(ico_appdata.theme);

    // TODO show onscreen layer
    ico_syc_show_layer(6);

    ecore_evas_show(ico_appdata.window);

    ICO_TRA("CicoOnScreen::ShowPopup Leave");
}

//--------------------------------------------------------------------------
/**
 * @brief   CicoOnScreen::HidePopup
 *          Hide popup window
 *
 * @param[in]   data        The user data passed from the callback
 *                          registration function
 * @param[in]   e           The handle to the popup window
 * @param[in]   obj         The handle to the Evas object
 * @param[in]   event_info  Event information
 * @return      none
 */
//--------------------------------------------------------------------------
void
CicoOnScreen::HidePopup(void)
{
    ICO_TRA("CicoOnScreen::HidePopup Enter");

    // TODO hide onscreen layer
    ico_syc_hide_layer(6);

    // Background(popup frame) hide
    evas_object_hide(ico_appdata.theme);

	if (NULL != ico_appdata.noti) {
        notification_error_e err = NOTIFICATION_ERROR_NONE;
		err = notification_delete(ico_appdata.noti->GetNotiHandle());
        if (NOTIFICATION_ERROR_NONE != err) {
            ICO_ERR("notification_delete faile(%d).");
        }
		delete ico_appdata.noti;
		ico_appdata.noti = NULL;
	}

    ico_appdata.show_flag = FALSE;

    ICO_TRA("CicoOnScreen::HidePopup Leave");
}

//--------------------------------------------------------------------------
/**
 * @brief   CicoOnScreen::InitializePopup
 *          Initialize popup window
 *
 * @param[in]   none
 * @return      none
 */
//--------------------------------------------------------------------------
void
CicoOnScreen::InitializePopup(void)
{
    ICO_TRA("CicoOnScreen::InitializePopup Enter");

    // Window setup
    ico_appdata.window = ecore_evas_new(NULL, 0, 0, WIDTH, HEIGHT, "frame=0");
    if (ico_appdata.window == NULL) {
        ICO_ERR("ecore_evas_new() error");
        Finalize();
        ICO_TRA("CicoOnScreen::InitializePopup Leave(ERR)");
        return;
    }
    ecore_evas_show(ico_appdata.window);

    ico_appdata.theme = edje_object_add(ecore_evas_get(ico_appdata.window));
    if (NULL == ico_appdata.theme) {
        ICO_ERR("could not create edje object!");
        return;
    }

    if (!edje_object_file_set(ico_appdata.theme, "/usr/apps/org.tizen.ico.onscreen/res/themes/onscreen.edj", "main")) {
        Edje_Load_Error err = edje_object_load_error_get(ico_appdata.theme);
        const char *errmsg = edje_load_error_str(err);
        ICO_ERR("could not load 'main' from onscreen.edj: %s",
                errmsg);
        evas_object_del(ico_appdata.theme);
        ico_appdata.theme = NULL;
        return;
    }

    // icon setup
    ico_appdata.icon = evas_object_image_filled_add(ecore_evas_get(ico_appdata.window));
    edje_object_part_swallow(ico_appdata.theme, "icon", ico_appdata.icon);

    /* getting size of screen */
    /* home screen size is full of display*/
    int display_width  = 0;
    int display_height = 0;
    ecore_wl_screen_size_get(&display_width, &display_height);

    ICO_DBG("display size w/h=%d/%d", display_width, display_height);
    int popup_x = (display_width / 2) - (POPUP_FRAME_WIDTH / 2);
    int popup_y = (display_height / 2) - (POPUP_FRAME_HEIGHT/ 2);
    ICO_DBG("popup postion x/y=%d/%d", popup_x, popup_y);
    evas_object_move(ico_appdata.theme, popup_x, popup_y);
    evas_object_resize(ico_appdata.theme, POPUP_FRAME_WIDTH, POPUP_FRAME_HEIGHT);

    Evas_Object* obj = NULL;
    obj = (Evas_Object*)edje_object_part_object_get(ico_appdata.theme,
                                                    "button_text");
    if (NULL != obj) {
        evas_object_event_callback_add(obj,
                                       EVAS_CALLBACK_MOUSE_UP,
                                       evasMouseUpCB,
                                       this);
    }

    obj = (Evas_Object*)edje_object_part_object_get(ico_appdata.theme,
                                                    "button");
    if (NULL != obj) {
        evas_object_event_callback_add(obj,
                                       EVAS_CALLBACK_MOUSE_UP,
                                       evasMouseUpCB,
                                       this);
    }

    evas_object_show(ico_appdata.theme);

    ICO_TRA("CicoOnScreen::InitializePopup Leave(OK)");
}

//--------------------------------------------------------------------------
/**
 * @brief   CicoOnScreen::NotificationCallback
 *          Notification callback function
 *
 * @param[in]  data        The user data passed from the callback
 *                         registration function
 * @param[in]  type        The type of notification
 * @return     none
 */
//--------------------------------------------------------------------------
void
CicoOnScreen::NotificationCallback(void *data, notification_type_e type,
                                   notification_op *op_list, int num_op)
{
    ICO_TRA("CicoOnScreen::NotificationCallback Enter");

    for (int i = 0; i < num_op; i++) {
        notification_op_type_e op_type = op_list[i].type;
        int priv_id = op_list[i].priv_id;
        notification_h noti_h = op_list[i].noti;
        ICO_DBG("RECV notification op_type=%d priv_id=%d noti=0x%08x",
                op_type, priv_id, noti_h);

        switch (op_type) {
        case NOTIFICATION_OP_INSERT :
        {
            ICO_DBG("NOTIFICATION_OP_INSERT(%d)", op_type);

            CicoNotification noti(noti_h);
            if (noti.Empty()) {
                break;
            }

            // check notification type
            // TODO config
            notification_type_e type = noti.GetType();
            if (NOTIFICATION_TYPE_NOTI != type) {
                break;
            }

            // Show popup window
            if (ico_appdata.show_flag == FALSE) {
                ico_appdata.show_flag = TRUE;
                os_instance->ShowPopup(noti);
            }
            break;
        }
        case NOTIFICATION_OP_UPDATE :
            ICO_DBG("NOTIFICATION_OP_UPDATE(%d)", op_type);
            break;
        case NOTIFICATION_OP_DELETE:
        {
            ICO_DBG("NOTIFICATION_OP_DELETE(%d)", op_type);

            if (NULL == ico_appdata.noti) {
                break;
            }

            if (priv_id == ico_appdata.noti->GetPrivId()) {
                delete ico_appdata.noti;
                ico_appdata.noti = NULL;
                os_instance->HidePopup();
            }
            break;
        }
        case NOTIFICATION_OP_DELETE_ALL:
            ICO_DBG("NOTIFICATION_OP_DELETE_ALL(%d)", op_type);
            break;
        case NOTIFICATION_OP_REFRESH:
            ICO_DBG("NOTIFICATION_OP_REFRESH(%d)", op_type);
            break;
        case NOTIFICATION_OP_SERVICE_READY:
            ICO_DBG("NOTIFICATION_OP_SERVICE_READY(%d)", op_type);
            break;
        default :
            break;
        }
    }

    ICO_TRA("CicoOnScreen::NotificationCallback Leave");
}

//--------------------------------------------------------------------------
/**
 * @brief   callback function of evas mouse up event
 *
 * @param [in] data         The user data passed from the callback
 *                          registration function
 * @param [in] e            The handle to the popup window
 * @param [in] obj          The handle to the Evas object
 * @param [in] event_info   Event information
 *
 */
//--------------------------------------------------------------------------
void
CicoOnScreen::evasMouseUpCB(void *data, Evas *e,
                            Evas_Object *obj, void *event_info)
{
    ICO_TRA("CicoOnScreen::evasMouseUpCB Enter");
    ICO_DBG("obj name=%s", evas_object_name_get(obj));

    static_cast<CicoOnScreen*>(data)->HidePopup();

    ICO_TRA("CicoOnScreen::evasMouseUpCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief   callback for system controller
 *
 *  @param [in] event       kind of event
 *  @param [in] detail      detail
 *  @param [in] user_data   user data
 */
//--------------------------------------------------------------------------
void
CicoOnScreen::EventCallBack(const ico_syc_ev_e event,
                            const void *detail, void *user_data)
{
    ICO_TRA("CicoOnScreen::EventCallBack Enter(event %d)", event);

    switch (event) {
    case ICO_SYC_EV_RES_ACQUIRE:
        ICO_TRA("Received: ico syc event(ICO_SYC_EV_RES_ACQUIRE)");
        break;
    case ICO_SYC_EV_RES_DEPRIVE:
        ICO_TRA("Received: ico syc event(ICO_SYC_EV_RES_DEPRIVE)");
        break;
    case ICO_SYC_EV_RES_WAITING:
        ICO_TRA("Received: ico syc event(ICO_SYC_EV_RES_WAITING)");
        break;
    case ICO_SYC_EV_RES_REVERT:
        ICO_TRA("Received: ico syc event(ICO_SYC_EV_RES_REVERT)");
        break;
    case ICO_SYC_EV_RES_RELEASE:
        ICO_TRA("Received: ico syc event(ICO_SYC_EV_RES_RELEASE)");
        break;
    default:
        break;
    }

    ICO_TRA("CicoOnScreen::EventCallBack Leave");
}
// vim: set expandtab ts=4 sw=4:
