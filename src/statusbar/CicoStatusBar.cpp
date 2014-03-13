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

using namespace std;

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
 *  @brief  default destructor
 *  @param[in]  none
 *  @return     none
 */
//--------------------------------------------------------------------------
CicoStatusBar::~CicoStatusBar()
{
    while( !noti_list.empty() ) {
        DeleteNotification( this );
    }
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
CicoStatusBar::UpdateNotificationPanel( CicoStatusBar *sb )
{

    /*  if timer is active then wait timeout  */
    if (notitimer_ != NULL) {
        ICO_TRA("CicoStatusBar::UpdateNotificationPanel Timer not Terminate" );
        return false;
    }

    /*  get Notification , if it is empty then return  */
    CicoNotification* noti = sb->GetNotification(sb);
    if ( noti == NULL ) {
        ICO_TRA("CicoStatusBar::UpdateNotificationPanel empty" );
        return false;
    }
    //const char *msg = noti->GetTitle();
    const char *msg = noti->GetContent();
    const char *iconpath = noti->GetIconPath();
    const char *soundpath = NULL;

    /*  show Notification */
    ICO_TRA("CicoStatusBar::UpdateNotificationPanel Enter"
            "(msg=%s icon=%s sound=%s)", msg, iconpath, soundpath);
    if (msg == NULL && iconpath == NULL && soundpath == NULL) {
        ICO_ERR("notification info is NULL.");
        sb->DeleteNotification(sb);
        return false;
    }
    noticomp_->SetNotification(msg, iconpath, soundpath);
    noticomp_->Show();

    /* delete Notification and set disp timer */
    sb->DeleteNotification(sb);

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

    for (int i = 0; i < num_op; i++) {
        op_type = op_list[i].type;
        switch (op_type) {
        case NOTIFICATION_OP_INSERT :
        case NOTIFICATION_OP_UPDATE :
            ICO_DBG("NOTIFICATION_OP_INSERT/NOTIFICATION_OP_UPDATE");

            /*  add notification and update SB  */
            sb->AddNotification( sb, op_list[i].noti );
            sb->UpdateNotificationPanel( sb );

            break;
        case NOTIFICATION_OP_DELETE:
            ICO_DBG("NOTIFICATION_OP_DELETE");
            sb->DeleteNotification( sb, op_list[i].priv_id );
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

    /*  update notification ,if Notification exist in the queue   */
    sb->UpdateNotificationPanel( sb );

    ICO_TRA("CicoStatusBar::HideNotification Leave");
    return ECORE_CALLBACK_CANCEL;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief    add last notification
 *
 * @param[in]   sb    StatusBar object
 * @param[in]   noti_h  notification handle
 * @return      non
 */
/*--------------------------------------------------------------------------*/
void 
CicoStatusBar::AddNotification(CicoStatusBar* sb, notification_h noti_h)
{
    ICO_TRA("CicoStatusBar::AddNotification() Enter");

    CicoNotification *noti = new CicoNotification( noti_h );
    if (noti->GetType() != NOTIFICATION_TYPE_ONGOING ) {
        delete noti;
        return;
    }
    sb->noti_list.push_back(noti);

    ICO_TRA("CicoStatusBar::AddNotification Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief    get first notification
 *
 * @param[in]   sb    StatusBar object
 * @return      CicoNotification class pointer
 */
/*--------------------------------------------------------------------------*/
CicoNotification* 
CicoStatusBar::GetNotification(CicoStatusBar* sb)
{
    ICO_TRA("CicoStatusBar::GetNotification() Enter");

    if ( ! sb->noti_list.empty() ) {
        CicoNotification *noti = sb->noti_list.front();
        if ( noti ) {
            return noti;
        }
        else {
            DeleteNotification(sb);
        }
    }
    ICO_TRA("CicoStatusBar::GetNotification Leave");
    return NULL;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief    delete first notification
 *
 * @param[in]   sb    StatusBar object
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoStatusBar::DeleteNotification(CicoStatusBar *sb)
{
    ICO_TRA("CicoStatusBar::DeleteNotification() Enter");

    if ( ! sb->noti_list.empty() ) {
        CicoNotification *noti = sb->noti_list.front();
        if ( noti ) {
            delete noti;
        }
        sb->noti_list.pop_front();
    }

    ICO_TRA("CicoStatusBar::DeleteNotification Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief    delete  notification
 *
 * @param[in]   sb    StatusBar object
 * @param[in]   noti_h  notification handle
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoStatusBar::DeleteNotification(CicoStatusBar *sb, int priv_id)
{
    ICO_TRA("CicoStatusBar::DeleteNotification() Enter");

    List_CicoNoti_pt pt = noti_list.begin();
    for (; pt != noti_list.end(); ++pt) {

        CicoNotification *noti_src = *pt;
        if ( priv_id == noti_src->GetPrivId() ) {

            delete noti_src;
            noti_list.erase(pt);
            break;
        }
    }

    ICO_TRA("CicoStatusBar::DeleteNotification Leave");
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
