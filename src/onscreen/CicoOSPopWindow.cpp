/*
 * Copyright (c) 2014, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   On Screen Popup Window
 *
 * @date    Feb-17-2014
 */
#include <Ecore.h>
#include <Ecore_Wayland.h>
#include <appsvc/appsvc.h>
#include "CicoOnScreen.h"
#include "CicoNotification.h"
#include "CicoOSPopWindow.h"
#include "ico_syc_type.h"
#include "ico_syc_appresctl.h"
#include "CicoOSClient.h"

using namespace std;

//==========================================================================
// static members
//==========================================================================

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
CicoOSPopWindow::CicoOSPopWindow(notification_type_e type) : CicoNotification(type)
{
    ICO_TRA("Enter type(%d)", (int)type);
    m_window      = NULL;
    m_icon        = NULL;
    m_theme       = NULL;
    m_resourceId  = 0;
    m_context     = NULL;
    m_buttonTouch = false;
    ICO_TRA("Leave");
}

CicoOSPopWindow::CicoOSPopWindow(notification_h noti) : CicoNotification(noti)
{
    ICO_TRA("Enter");
    m_window      = NULL;
    m_icon        = NULL;
    m_theme       = NULL;
    m_resourceId  = 0;
    m_context     = NULL;
    m_buttonTouch = false;
    ICO_TRA("Leave");
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
CicoOSPopWindow::~CicoOSPopWindow(void)
{
    ICO_TRA("Enter sur:%d, priv_id:%d", m_resourceId, GetPrivId());
    if (NULL != m_window) {
        ecore_evas_free(m_window);
        m_window = NULL;
    }
    // if (NULL != m_icon) /* import m_window */
    m_icon = NULL;
    // if (NULL != m_theme) /* import m_window */
    m_theme = NULL;
    if (NULL != m_context) {
        int r = ico_syc_release_res(m_context);
        ICO_DBG("_____ %d = ico_syc_release_res", r);
    }

    ICO_DBG("_____ %d, %d", (int)m_buttonTouch, (int)m_appsvc_pkgname.empty());

    if ((true == m_buttonTouch) && (false == m_appsvc_pkgname.empty())) {
        CicoOSClient* cosc = CicoOSClient::getInstance();
        if (NULL != cosc) {
            int r = cosc->sendLaunchMessage(m_appsvc_pkgname);
            ICO_DBG("_____ SendMsg:%d, %s", r, m_appsvc_pkgname.c_str());
        }
    }

    ICO_TRA("Leave");
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
bool
CicoOSPopWindow::showPopup()
{
    ICO_TRA("Enter");
    if (NULL == m_window) {
        if (false == InitializeWindow()) {
            ICO_TRA("Leave false");
            return false;
        }
    }
    // get caller pachage name
    const char *pkgname = GetPkgname();

    // get priv_id
    int priv_id = GetPrivId();

    // Get title
    const char *title = GetTitle();

    // Get content
    const char *content = GetContent();

    // Get execute option
    const char *text = NULL;
    bundle *service_handle = NULL;
    (void)GetExecuteOption(NOTIFICATION_EXECUTE_TYPE_SINGLE_LAUNCH, &text,
                           &service_handle);

    // Get icon path
    const char *icon = GetIconPath();
    ICO_DBG("Received: %s : %i : %s : %s : %s : %p",
            pkgname, priv_id, title, content,
            text, (void *)service_handle);

    if (NULL != service_handle) {
        const char* pn = appsvc_get_pkgname(service_handle);
        if (NULL != pn) {
            ICO_DBG("Received: appsvc_get_pkgname:%s", pn);
            m_appsvc_pkgname = pn;
            m_buttonTouch = false;
        }
    }

    if (icon) {
        if (NULL != m_icon) {
            evas_object_image_file_set(m_icon, icon, NULL);
            evas_object_show(m_icon);
        }
    }

    if (title) {
        if (!edje_object_part_text_set(m_theme, "title_text", title)) {
            ICO_WRN("could not set the text. Maybe part 'text' does not exist?");
        }
    }

    if (content) {
        if (strlen(content) <= 25) {
            ICO_DBG("content text center");
            if (!edje_object_part_text_set(m_theme, "content_text_center", content)) {
                ICO_WRN("could not set the text. Maybe part 'text' does not exist?");
            }
            if (!edje_object_part_text_set(m_theme, "content_text_left", "")) {
                ICO_WRN("could not set the text. Maybe part 'text' does not exist?");
            }
        }
        else {
            ICO_DBG("content text left");
            if (!edje_object_part_text_set(m_theme, "content_text_left", content)) {
                ICO_WRN("could not set the text. Maybe part 'text' does not exist?");
            }
            if (!edje_object_part_text_set(m_theme, "content_text_center", "")) {
                ICO_WRN("could not set the text. Maybe part 'text' does not exist?");
            }
        }
    }

    if (text) {
        if (!edje_object_part_text_set(m_theme, "button_text", text)) {
            ICO_WRN("could not set the text. Maybe part 'text' does not exist?");
        }
    }
    else {
        if (!edje_object_part_text_set(m_theme, "button_text", "OK")) {
            ICO_WRN("could not set the text. Maybe part 'text' does not exist?");
        }
    }

    evas_object_show(m_theme);
    ICO_TRA("Leave true");
    return true;
}

bool 
CicoOSPopWindow::acquireRes()
{
    ICO_TRA("Enter");
    if (NULL != m_context) {
        ICO_TRA("Leave false");
        return false;
    }
    ico_syc_res_window_t w;
    makeResWindowT(w);
    m_context = ico_syc_acquire_res( &w, NULL, NULL, ICO_SYC_RES_ONSCREEN);
    if (NULL == m_context) {
        ICO_TRA("Leave false");
        return false;
    }
    ICO_TRA("Leave true");
    return true;
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
CicoOSPopWindow::hidePopup(bool buttonTouch)
{
    ICO_TRA("Enter %s", buttonTouch? "true": "false");
    m_buttonTouch = buttonTouch;
    releaseRes();
    ICO_TRA("Leave");
}

bool    
CicoOSPopWindow::releaseRes()
{
    ICO_TRA("Enter");
    if (NULL != m_context) {
        int r = ico_syc_release_res(m_context);
        ICO_DBG("_____ %d = ico_syc_release_res", r);
        if (ICO_SYC_ERR_NONE == r) {
            m_context = NULL;
            ICO_TRA("Leave true");
            return true;
        }
    }
    ICO_TRA("Leave false");
    return false;
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
bool
CicoOSPopWindow::InitializeWindow(void)
{
    ICO_TRA("Enter");
    if (false == createMainWindow()) {
        ICO_TRA("Leave(ERR)");
        return false;
    }
    m_theme = edje_object_add(ecore_evas_get(m_window));
    if (NULL == m_theme) {
        ICO_ERR("could not create edje object!");
        ICO_TRA("Leave(ERR)");
        return false;
    }
    if (!edje_object_file_set(m_theme, ICO_OS_THEMES_EDJ_FILEPATH, "main")) {
        Edje_Load_Error err = edje_object_load_error_get(m_theme);
        const char *errmsg = edje_load_error_str(err);
        ICO_ERR("could not load 'main' from onscreen.edj: %s", errmsg);
        evas_object_del(m_theme);
        m_theme = NULL;
        return false;
    }
    // icon setup
    m_icon = evas_object_image_filled_add(ecore_evas_get(m_window));
    edje_object_part_swallow(m_theme, "icon", m_icon);
    /* getting size of screen */
    /* home screen size is full of display*/
    int display_width  = 0;
    int display_height = 0;
    ecore_wl_screen_size_get(&display_width, &display_height);

    ICO_DBG("display size w/h=%d/%d", display_width, display_height);
    int popup_x = (display_width / 2) - (POPUP_FRAME_WIDTH / 2);
    int popup_y = (display_height / 2) - (POPUP_FRAME_HEIGHT/ 2);
    ICO_DBG("popup postion x/y=%d/%d", popup_x, popup_y);
    evas_object_move(m_theme, popup_x, popup_y);
    evas_object_resize(m_theme, POPUP_FRAME_WIDTH, POPUP_FRAME_HEIGHT);

    Evas_Object* obj = NULL;
    obj = (Evas_Object*)edje_object_part_object_get(m_theme, "button_text");
    if (NULL != obj) {
        evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_UP,
                                       evasMouseUpCB, this);
    }

    obj = (Evas_Object*)edje_object_part_object_get(m_theme, "button");
    if (NULL != obj) {
        evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_UP,
                                       evasMouseUpCB, this);
    }
    evas_object_show(m_theme);
    ICO_TRA("Leave(OK)");
    return true;
}

bool
CicoOSPopWindow::createMainWindow()
{
    ICO_TRA("Enter");
    // Window setup
    m_window = ecore_evas_new(NULL, 0, 0, WIDTH, HEIGHT, "frame=0");
    if (NULL == m_window) {
        ICO_ERR("ecore_evas_new() error");
        ICO_TRA("Leave(ERR)");
        return false;
    }
    ecore_evas_alpha_set(m_window, EINA_TRUE);
    ecore_evas_show(m_window);
    ICO_TRA("Leave");
    return true;
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
CicoOSPopWindow::evasMouseUpCB(void *data, Evas *e, Evas_Object *obj,
                               void *event_info)
{
    ICO_TRA("Enter %x, %x, %x, %x", data, e, obj, event_info);
    if (NULL != obj) {
        ICO_DBG("_____ obj name=%s", evas_object_name_get(obj));
    }
    static_cast<CicoOSPopWindow*>(data)->hidePopup(true);
    ICO_TRA("Leave");
}


static char id0_ECU[]         = "Center";

static char id00_display[]   = "Center";
// static char id01_display[]   = "Mid";

// static char id001_layer[]    = "Application";
// static char id002_layer[]    = "SoftKeyboard";
// static char id003_layer[]    = "HomeScreen";
// static char id004_layer[]    = "ControlBar";
// static char id005_layer[]    = "InterruptApp";
static char id006_layer[]    = "OnScreen";
// static char id007_layer[]    = "Touch";
// static char id008_layer[]    = "Cursor";
// static char id011_layer[]    = "MainApp";
// static char id012_layer[]    = "SubDispMainApp";
// static char id013_layer[]    = "SubDispSubApp";

static char id00611_layout[] = "Whole";
// static char id00612_layout[] = "Half";

static char id006111_area[]  = "Full";
// static char id006121_area[]  = "Upper";
// static char id006122_area[]  = "Lower";

static char role_incoming[] = "incoming";
static char role_message[]  = "message";
void
CicoOSPopWindow::makeResWindowT(ico_syc_res_window_t& w)
{
    w.ECU         = id0_ECU;
    w.display     = id00_display;
    w.layer       = id006_layer;
    w.layout      = id00611_layout;
    w.area        = id006111_area;
    w.dispatchApp = (char*)GetPkgname();
    if (0 == strcmp(w.dispatchApp,"org.tizen.dialer")) {
        w.role    = role_incoming;
    }
    else {
        w.role    = role_message;
    }
    w.resourceId  = m_resourceId;
}

// vim: set expandtab ts=4 sw=4:
