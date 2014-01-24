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

using namespace std;

/*============================================================================*/
/* static members                                                             */
/*============================================================================*/
CicoOnScreen * CicoOnScreen::os_instance=NULL;
struct popup_data CicoOnScreen::ico_appdata;
struct lemolo_noti_data CicoOnScreen::ico_notidata;

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreen::CicoOnScreen
 *          Constractor
 *
 * @param[in]   none
 * @return      none

 */
/*--------------------------------------------------------------------------*/
CicoOnScreen::CicoOnScreen(void)
{
    ICO_TRA("CicoOnScreen::CicoOnScreen Enter");

    InitializePopupData();
    InitializeNotificationData();

    ICO_TRA("CicoOnScreen::CicoOnScreen Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreen::~CicoOnScreen
 *          Destractor
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoOnScreen::~CicoOnScreen(void)
{
//    ICO_TRA("CicoOnScreen::~CicoOnScreen Enter");
//    ICO_TRA("CicoOnScreen::~CicoOnScreen Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreen::InitializePopupData
 *          Initialize popup struct data
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoOnScreen::InitializePopupData(void)
{
    ICO_TRA("CicoOnScreen::InitializePopupData Enter");

    ico_appdata.window = NULL;
    ico_appdata.evas = NULL;
    ico_appdata.background = NULL;
    ico_appdata.icon = NULL;
    ico_appdata.title = NULL;
    ico_appdata.content = NULL;
    ico_appdata.icon_bg = NULL;
    ico_appdata.title_bg = NULL;
    ico_appdata.content_bg = NULL;
    ico_appdata.show_flag = FALSE;

    ICO_TRA("CicoOnScreen::InitializePopupData Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreen::InitializePopupData
 *          Initialize notification struct data
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoOnScreen::InitializeNotificationData(void)
{
    ICO_TRA("CicoOnScreen::InitializeNotificationData Enter");

    ico_notidata.title = NULL;
    ico_notidata.content = NULL;
    ico_notidata.icon = NULL;
    ico_notidata.text = NULL;
    ico_notidata.service_handle = NULL;

    ICO_TRA("CicoOnScreen::InitializeNotificationData Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreen::StartOnScreen
 *          Start on screen
 *
 * @param[in]   none
 * @return      none
 * @return true on success, false on error
 */
/*--------------------------------------------------------------------------*/
bool
CicoOnScreen::StartOnScreen(int orientation)
{
    ICO_TRA("CicoOnScreen::StartOnScreen Enter");

    /* save instance pointer */
    os_instance = this;

#if 0
    /* config copy*/
    this->config = new CicoGKeyFileConfig();
    this->config->Initialize(ICO_ONSCREEN_CONFIG_FILE);

    /* Get screen size */
    CicoOSWindowController::GetFullScreenSize(orientation,
                                              &full_width,&full_height);

    ICO_DBG("full screen size x=%d y=%d",full_width,full_height);
#endif

    /* Initialize */
    ecore_evas_init();
    InitializePopup();

    /* set notification callback function */
    notiservice_.SetCallback(NotificationCallback, this);

    ICO_TRA("CicoOnScreen::StartOnScreen Leave(true)");
    return true;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreen::Finalize
 *          Finalize
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoOnScreen::Finalize(void)
{
    ICO_TRA("CicoOnScreen::Finalize Enter");

    /* Free window handle */
    if (ico_appdata.window != NULL) {
        ecore_evas_free(ico_appdata.window);
    }

    ICO_TRA("CicoOnScreen::Finalize Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreen::ShowPopup
 *          Show popup window
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoOnScreen::ShowPopup(void)
{
    ICO_TRA("CicoOnScreen::ShowPopup Enter");

    evas_object_show(ico_appdata.background);

    /* Icon show */
    evas_object_show(ico_appdata.icon_bg);
    if (ico_notidata.icon != NULL) {
        evas_object_image_file_set(ico_appdata.icon, ico_notidata.icon, NULL);
        evas_object_image_fill_set(ico_appdata.icon, 0, 0, 50, 50);
        evas_object_show(ico_appdata.icon);
    }

    /* Title show */
    evas_object_show(ico_appdata.title_bg);
    evas_object_textblock_text_markup_set(ico_appdata.title, ico_notidata.title);
    evas_object_show(ico_appdata.title);

    /* Content show */
    evas_object_show(ico_appdata.content_bg);
    evas_object_textblock_text_markup_set(ico_appdata.content, ico_notidata.content);
    evas_object_show(ico_appdata.content);

    ICO_TRA("CicoOnScreen::ShowPopup Leave");
}

/*--------------------------------------------------------------------------*/
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
/*--------------------------------------------------------------------------*/
void
CicoOnScreen::HidePopup(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
    ICO_TRA("CicoOnScreen::HidePopup Enter");

    /* Background(popup frame) hide */
    evas_object_hide(ico_appdata.background);

    /* Icon hide */
    evas_object_hide(ico_appdata.icon_bg);
    evas_object_hide(ico_appdata.icon);

    /* Title hide */
    evas_object_hide(ico_appdata.title_bg);
    evas_object_hide(ico_appdata.title);

    /* Content hide */
    evas_object_hide(ico_appdata.content_bg);
    evas_object_hide(ico_appdata.content);

    ico_appdata.show_flag = FALSE;

    ICO_TRA("CicoOnScreen::HidePopup Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreen::InitializePopup
 *          Initialize popup window
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoOnScreen::InitializePopup(void)
{
    Evas_Textblock_Style* ts;
    static const char *style_buf =
        "DEFAULT='font=Sans font_size=30 color=#000 align=center text_class=entry'"
        "newline='br'"
        "b='+ font=Sans:style=bold'";

    ICO_TRA("CicoOnScreen::InitializePopup Enter");

    /* Window setup */
    ico_appdata.window = ecore_evas_new(NULL, 0, 0, WIDTH, HEIGHT, "frame=0");
    if (ico_appdata.window == NULL) {
        ICO_ERR("ecore_evas_new() error");
        Finalize();
        ICO_TRA("CicoOnScreen::InitializePopup Leave(ERR)");
        return;
    }
    ecore_evas_show(ico_appdata.window);

    /* Background setup */
    ico_appdata.evas = ecore_evas_get(ico_appdata.window);
    ico_appdata.background = evas_object_rectangle_add(ico_appdata.evas);
    evas_object_color_set(ico_appdata.background, 255, 255, 255, 255);
    evas_object_move(ico_appdata.background, POPUP_FRAME_ST_X, POPUP_FRAME_ST_Y);
    evas_object_resize(ico_appdata.background, POPUP_FRAME_WIDTH, POPUP_FRAME_HEIGHT);

    /* Icon Background setup */
    ico_appdata.icon_bg = evas_object_rectangle_add(ico_appdata.evas);
    evas_object_color_set(ico_appdata.icon_bg, 192, 192, 192, 255);
    evas_object_move(ico_appdata.icon_bg, POPUP_ICON_ST_X, POPUP_ICON_ST_Y);
    evas_object_resize(ico_appdata.icon_bg, POPUP_ICON_WIDTH, POPUP_ICON_HEIGHT);

    /* Icon setup */
    ico_appdata.icon = evas_object_image_add(ico_appdata.evas);
    evas_object_move(ico_appdata.icon, POPUP_ICON_ST_X, POPUP_ICON_ST_Y);
    evas_object_resize(ico_appdata.icon, POPUP_ICON_WIDTH, POPUP_ICON_HEIGHT);

    /* Title Background setup */
    ico_appdata.title_bg = evas_object_rectangle_add(ico_appdata.evas);
    evas_object_color_set(ico_appdata.title_bg, 192, 192, 192, 255);
    evas_object_move(ico_appdata.title_bg, POPUP_TITLE_ST_X, POPUP_TITLE_ST_Y);
    evas_object_resize(ico_appdata.title_bg, POPUP_TITLE_WIDTH, POPUP_TITLE_HEIGHT);

    /* text style set */
    ts = evas_textblock_style_new();
    evas_textblock_style_set(ts, style_buf);

    /* Title setup */
    ico_appdata.title = evas_object_textblock_add(ico_appdata.evas);
    evas_object_textblock_style_set(ico_appdata.title, ts);
    evas_object_textblock_valign_set(ico_appdata.title, 0.5);
    evas_object_textblock_text_markup_set(ico_appdata.title, "default");
    evas_object_move(ico_appdata.title, POPUP_TITLE_ST_X, POPUP_TITLE_ST_Y);
    evas_object_resize(ico_appdata.title, POPUP_TITLE_WIDTH, POPUP_TITLE_HEIGHT);

    /* Content Background setup */
    ico_appdata.content_bg = evas_object_rectangle_add(ico_appdata.evas);
    evas_object_color_set(ico_appdata.content_bg, 224, 224, 224, 255);
    evas_object_move(ico_appdata.content_bg, POPUP_CONTENT_BG_ST_X, POPUP_CONTENT_BG_ST_Y);
    evas_object_resize(ico_appdata.content_bg, POPUP_CONTENT_BG_WIDTH, POPUP_CONTENT_BG_HEIGHT);

    /* Content setup */
    ico_appdata.content = evas_object_textblock_add(ico_appdata.evas);
    evas_object_textblock_style_set(ico_appdata.content, ts);
    evas_object_textblock_valign_set(ico_appdata.content, 0.5);
    evas_object_textblock_text_markup_set(ico_appdata.content, "default");
    evas_object_move(ico_appdata.content, POPUP_CONTENT_BG_ST_X, POPUP_CONTENT_BG_ST_Y);
    evas_object_resize(ico_appdata.content, POPUP_CONTENT_BG_WIDTH, POPUP_CONTENT_BG_HEIGHT);

    /* set callback event */
    evas_object_event_callback_add(ico_appdata.icon_bg, EVAS_CALLBACK_MOUSE_UP,
        HidePopup, NULL);
    evas_object_event_callback_add(ico_appdata.icon, EVAS_CALLBACK_MOUSE_UP,
        HidePopup, NULL);
    evas_object_event_callback_add(ico_appdata.title_bg, EVAS_CALLBACK_MOUSE_UP,
        HidePopup, NULL);
    evas_object_event_callback_add(ico_appdata.title, EVAS_CALLBACK_MOUSE_UP,
        HidePopup, NULL);
    evas_object_event_callback_add(ico_appdata.content_bg, EVAS_CALLBACK_MOUSE_UP,
        HidePopup, NULL);
    evas_object_event_callback_add(ico_appdata.content, EVAS_CALLBACK_MOUSE_UP,
        HidePopup, NULL);

    ICO_TRA("CicoOnScreen::InitializePopup Leave(OK)");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreen::NotificationCallback
 *          Notification callback function
 *
 * @param[in]  data        The user data passed from the callback
 *                         registration function
 * @param[in]  type        The type of notification
 * @return     none
 */
/*--------------------------------------------------------------------------*/
void
CicoOnScreen::NotificationCallback(void *data, notification_type_e type,
        notification_op *op_list, int num_op)
{
    notification_h noti_h = NULL;
    notification_list_h notification_list = NULL;
    notification_list_h get_list = NULL;
//    notification_op_type_e op_type;
    int op_type;
    int group_id = 0;
    int length1 = 0;
    int length2 = 0;
    int ret_err = NOTIFICATION_ERROR_NONE;
    int priv_id = NOTIFICATION_PRIV_ID_NONE;
    char *pkgname = NULL;

    ICO_TRA("CicoOnScreen::NotificationCallback Enter");

    os_instance->InitializeNotificationData();

    for (int i = 0; i < num_op; i++) {
        op_type = (int)op_list[i].type;

        switch (op_type) {
        case NOTIFICATION_OP_INSERT :
        case NOTIFICATION_OP_UPDATE :
            notification_get_list(NOTIFICATION_TYPE_NOTI, -1, &notification_list);
            if (notification_list) {
                get_list = notification_list_get_head(notification_list);

                while(get_list != NULL) {
                    noti_h = notification_list_get_data(get_list);
                    CicoNotification noti(noti_h);
                    if (noti.Empty()) {
                        break;
                    }

                    pkgname = (char *)noti.GetPkgname();
                    if(pkgname == NULL){
                        notification_get_application(noti_h, &pkgname);
                    }

                    length1 = strlen(pkgname);
                    length2 = strlen(LEMOLO_PKGNAME);
                    length1 = (length1<=length2) ? length1 : length2;

                    if (pkgname != NULL && 
                        strncmp(pkgname, LEMOLO_PKGNAME, length1) == 0) {

                        /* Get priv_id */
                        notification_get_id(noti_h, &group_id, &priv_id);
                        /* Get title */
                        ico_notidata.title = (char *)noti.GetTitle();
                        /* Get content */
                        ico_notidata.content = (char *)noti.GetContent();
                        /* Get execute option */
                        notification_get_execute_option(noti_h,
                            NOTIFICATION_EXECUTE_TYPE_SINGLE_LAUNCH,
                            (const char **)(&ico_notidata.text),
                            &ico_notidata.service_handle);
                        /* Get icon path */
                        ico_notidata.icon = (char *)noti.GetIconPath();
#if 0
                        ico_notidata.icon = ICON_PATH;  // test
#endif
                        ICO_DBG("Received: %s : %i : %s : %s : %s : %x",
                            pkgname, priv_id, ico_notidata.title,
                            ico_notidata.content,
                            ico_notidata.text,
                            (int)ico_notidata.service_handle);

                        /* Show popup window */
                        if (ico_appdata.show_flag == FALSE) {
                            ico_appdata.show_flag = TRUE;
                            os_instance->ShowPopup();
                        }

                        /* Delete received notification from DB */
                        ret_err = notification_delete_by_priv_id(pkgname,
                            NOTIFICATION_TYPE_NOTI, priv_id);
                        if (ret_err != NOTIFICATION_ERROR_NONE) {
                            ICO_ERR("notification_delete_by_priv_id:error(%d)", ret_err);
                            break;
                        }
                    }

                    get_list = notification_list_get_next(get_list);
                }
            }
        }
    }

    if (notification_list != NULL) {
        notification_free_list(notification_list);
        notification_list = NULL;
    }

    ICO_TRA("CicoOnScreen::NotificationCallback Leave");
}
// vim: set expandtab ts=4 sw=4:
