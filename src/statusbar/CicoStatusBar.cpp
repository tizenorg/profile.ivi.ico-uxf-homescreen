/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

#include <cassert>
#include <Ecore.h>
#include <Ecore_Wayland.h>
#include <Evas.h>
#include <Elementary.h>
#include <ico_log.h>
#include "CicoStatusBar.h"

const int CicoStatusBar::STATUSBAR_POSX = 0; 
const int CicoStatusBar::STATUSBAR_POSY = 0; 
const int CicoStatusBar::STATUSBAR_HEIGHT = 64;

const int CicoStatusBar::CLOCK_WIDTH = 
                        CicoStatusBarClockComponent::CLOCK_COMPONENT_WIDTH;
const int CicoStatusBar::CLOCK_HEIGHT = 
                        CicoStatusBarClockComponent::CLOCK_COMPONENT_HEIGHT;

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 *  @param[in]  none
 *  @return     none
 */
//--------------------------------------------------------------------------
CicoStatusBar::CicoStatusBar()
    : CicoCommonWindow()
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  initialize status bar
 *  @param[in]  void
 *  @return     true: success   false: failed
 */
//--------------------------------------------------------------------------
bool
CicoStatusBar::Initialize(void)
{
    ICO_TRA("CicoStatusBar::Initialize() Enter");
    ICO_DBG("Initialize start window_.");

    window_ = ecore_evas_new(NULL, 0, 0, 1, 1, "frame=0");
    if (window_ == NULL) {
        return false;
    }
    ecore_main_loop_iterate();
    ecore_wl_screen_size_get(&width_, &height_);

    ecore_evas_size_min_set(window_, width_, STATUSBAR_HEIGHT);
    ecore_evas_size_max_set(window_, width_, STATUSBAR_HEIGHT);

    ecore_evas_move(window_, STATUSBAR_POSX, STATUSBAR_POSY);
    ecore_evas_resize(window_, width_, STATUSBAR_HEIGHT);

    // if use alpha need to ecore_evas_alpha_set(window_, 1);
    ICO_DBG("Initialize end window_.");

    ICO_DBG("Initialize start windowobj_.");
    windowobj_ = evas_object_rectangle_add(ecore_evas_get(window_));

    //statusbar RGBcolor setting
    evas_object_color_set(windowobj_, 0, 0, 0, 255);
    ICO_DBG("Initialize setpos windowobj_.");

    SetPos(STATUSBAR_POSX, STATUSBAR_POSY);
    ICO_DBG("Initialize setsize windowobj_.");
    SetSize(width_, STATUSBAR_HEIGHT);

    ecore_evas_object_associate(window_, windowobj_,
                                ECORE_EVAS_OBJECT_ASSOCIATE_BASE);
    ICO_DBG("Initialize end windowobj_.");

    ICO_DBG("Initialize start Clock_Component.");
    clockcomp_ = std::make_shared<CicoStatusBarClockComponent>(CicoStatusBarClockComponent());
    if (!clockcomp_->Initialize(windowobj_,
                                (width_ - CLOCK_WIDTH - 100),
                                ((STATUSBAR_HEIGHT - CLOCK_HEIGHT) / 2))){
        ICO_ERR("Failed to initialize Clock_Componet.");
        return false;
    }
    ICO_DBG("Initialize end Clock_Component.");

    ICO_DBG("Initialize start Notification_Component.");
    noticomp_ = std::make_shared<CicoNotificationPanelComponent>(CicoNotificationPanelComponent());
    if (!noticomp_->Initialize(windowobj_)) {
        ICO_ERR("Failed to initialize Notification_Component.");
        return false;
    }

    noticomp_->SetPos(STATUSBAR_POSX, STATUSBAR_POSY);
    notiservice_.SetCallback(NotificationCallback, this);
    ICO_DBG("Initialize end Notification_Component.");

    Show();

    // add timer callback function of time update
    ecore_timer_add(3.0, ecoreUpdateTimeCB, this);

    ICO_TRA("CicoStatusBar::Initialize() Leave");
    return true;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   update clock
 *
 * @param[in]   none
 * @return      clockcomp_->Update()
                false :update failed
 */
/*--------------------------------------------------------------------------*/
bool
CicoStatusBar::UpdateTime()
{
    //ICO_TRA("CicoStatusBar::UpdateTime() Enter");
    if (window_ != NULL && windowobj_ != NULL && clockcomp_ != NULL) {
        //ICO_TRA("CicoStatusBar::UpdateTime() Leave");
        return clockcomp_->Update();
    }

    ICO_ERR("Faied to update Clock_Componet.");
    return false;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   update notification panel 
 *  @param[in]  msg     message
 *  @param[in]  iconpaht    icon image path
 *  @param[in]  soundpath   sound data path
 *  @return     true: success   false: failed
 */
/*--------------------------------------------------------------------------*/
bool
CicoStatusBar::UpdateNotificationPanel(const char *msg,
                                       const char *iconpath,
                                       const char *soundpath)
{
    ICO_TRA("CicoStatusBar::UpdateNotificationPanel Enter"
            "(msg=%s icon=%s sound=%s)", msg, iconpath, soundpath);
    if (msg == NULL && iconpath == NULL && soundpath == NULL) {
        ICO_ERR("notification info is NULL.");
        return false;
    }
    noticomp_->SetNotification(msg, iconpath, soundpath);
    noticomp_->Show();
    if (notitimer_ != NULL) {
        ecore_timer_del(notitimer_);
        notitimer_ = NULL;
    }
    notitimer_ = ecore_timer_add(3.0, HideNotification, this);
    ICO_TRA("CicoStatusBar::UpdateNotificationPanel Leave");
    return true;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   notification callback
 *
 * @param[in]   data    StatusBar object
 * @param[in]   type    notification type
 * @param[in]   op_list notification open list
 * @param[in]   num_op  notification open number
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoStatusBar::NotificationCallback(void *data,
                                    notification_type_e type,
                                    notification_op *op_list,
                                    int num_op)
{
    ICO_TRA("CicoStatusBar::NotificationCallback Enter(type=%d num_op=%d)",
            type, num_op);
    CicoStatusBar *sb = reinterpret_cast<CicoStatusBar*>(data);
    notification_op_type_e op_type;
    notification_list_h notificationlist = NULL;
    notification_list_h getlist = NULL;
    for (int i = 0; i < num_op; i++) {
        op_type = op_list[i].type;
        switch (op_type) {
        case NOTIFICATION_OP_INSERT :
        case NOTIFICATION_OP_UPDATE :
        {
            ICO_DBG("NOTIFICATION_OP_INSERT/NOTIFICATION_OP_UPDATE");
            notification_error_e err = NOTIFICATION_ERROR_NONE;
            ICO_DBG("called: notification_get_list(NOTIFICATION_TYPE_NOTI)");
            err = notification_get_list(NOTIFICATION_TYPE_NOTI,
                                        -1, &notificationlist);
            if (NOTIFICATION_ERROR_NONE != err) {
                ICO_ERR("notification_get_list() failed(%d).", err);
                break;
            }

            if (notificationlist) {
                ICO_DBG("called: notification_list_get_head(0x%08x)",
                        notificationlist);
                getlist = notification_list_get_head(notificationlist);
                do {
                    CicoNotification noti(notification_list_get_data(getlist));
                    if (noti.Empty()) {
                        break;
                    }
                    sb->UpdateNotificationPanel(noti.GetTitle(),
                                                noti.GetIconPath(), 
                                                NULL);
#if 1
                    int grpid = 0;
                    int privid = 0;
                    notification_get_id(notification_list_get_data(getlist),
                                        &grpid, &privid);
                    notification_delete_group_by_priv_id(noti.GetPkgname(),
                                                         noti.GetType(),
                                                         privid);
#endif

                    ICO_DBG("called: notification_list_get_next(0x%08x)",
                            getlist);
                    getlist = notification_list_get_next(getlist);
                } while (getlist != NULL);
            }

            if (notificationlist != NULL) {
                ICO_DBG("called: notification_free_list(0x%08x)",
                        notificationlist);
                notification_error_e err = NOTIFICATION_ERROR_NONE;
                err = notification_free_list(notificationlist);
                if (NOTIFICATION_ERROR_NONE != err) {
                    ICO_ERR("notification_free_list() failed.");
                }
                notificationlist = NULL;
            }
            break;
        }
        case NOTIFICATION_OP_DELETE:
            ICO_DBG("NOTIFICATION_OP_DELETE");
            break;
        case NOTIFICATION_OP_DELETE_ALL:
            ICO_DBG("NOTIFICATION_OP_DELETE_ALL");
            break;
        case NOTIFICATION_OP_REFRESH:
            ICO_DBG("NOTIFICATION_OP_REFRESH");
            break;
        case NOTIFICATION_OP_SERVICE_READY:
            ICO_DBG("NOTIFICATION_OP_SERVICE_READY");
            break;
        default :
            break;
        }
    }
    ICO_TRA("CicoStatusBar::NotificationCallback Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hide notification panel 
 *
 * @param[in]   data    StatusBar object
 * @return      ECORE_CALLBACK_CANCEL
 */
/*--------------------------------------------------------------------------*/
Eina_Bool
CicoStatusBar::HideNotification(void *data)
{
    ICO_TRA("CicoStatusBar::HideNotification() Enter");
    CicoStatusBar *sb = reinterpret_cast<CicoStatusBar*>(data);
    sb->noticomp_->Hide();
    ecore_timer_del(sb->notitimer_);
    sb->notitimer_ = NULL;
    ICO_TRA("CicoStatusBar::HideNotification Leave");
    return ECORE_CALLBACK_CANCEL;
}

//==========================================================================
//  public functions
//==========================================================================
//--------------------------------------------------------------------------
/**
 *  @brief  callback function of time update 
 *
 *  @param [in] data    user date
 */
//--------------------------------------------------------------------------
Eina_Bool
CicoStatusBar::ecoreUpdateTimeCB(void *data)
{
    assert(data != NULL);
    CicoStatusBar* statusbar = reinterpret_cast<CicoStatusBar*>(data);
    assert(statusbar->UpdateTime());
    return ECORE_CALLBACK_RENEW;
}
// vim: set expandtab ts=4 sw=4:
