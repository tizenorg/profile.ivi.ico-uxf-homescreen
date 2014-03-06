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
 *  @file   CicoComponentimplementation.cpp
 *
 *  @brief  This file is implimentation of CicoStatusBarClockComponent class
 *                                  and CicoNotificationPanelComponent class
 */
//==========================================================================

#include <ctime>
#include <cassert>
#include <memory>

#include <ico_log.h>
#include "CicoComponentImplementation.h"

#define IMG_DIR "/usr/apps/org.tizen.ico.statusbar/res/images/"

// set image size of clock
const int CicoStatusBarClockComponent::AMPM_WIDTH = 47;
const int CicoStatusBarClockComponent::AMPM_HEIGHT = 30;
const int CicoStatusBarClockComponent::HOUR10_WIDTH = 20;
const int CicoStatusBarClockComponent::HOUR10_HEIGHT = 30;
const int CicoStatusBarClockComponent::HOUR1_WIDTH = 20;
const int CicoStatusBarClockComponent::HOUR1_HEIGHT = 30;
const int CicoStatusBarClockComponent::COLON_WIDTH = 8;
const int CicoStatusBarClockComponent::COLON_HEIGHT = 20;
const int CicoStatusBarClockComponent::MIN10_WIDTH = 20;
const int CicoStatusBarClockComponent::MIN10_HEIGHT = 30;
const int CicoStatusBarClockComponent::MIN1_WIDTH = 20;
const int CicoStatusBarClockComponent::MIN1_HEIGHT = 30;

// set blank size
const int CicoStatusBarClockComponent::COLON_BLANK = 7;
const int CicoStatusBarClockComponent::AMPM_BLANK = 7;
const int CicoStatusBarClockComponent::LAST_BLANK = 20;

// set image position of clock
const int CicoStatusBarClockComponent::AMPM_POSX = 0;
const int CicoStatusBarClockComponent::AMPM_POSY = 8;
const int CicoStatusBarClockComponent::RELATIVE_HOUR10_POSX = AMPM_WIDTH + AMPM_BLANK;
const int CicoStatusBarClockComponent::RELATIVE_HOUR10_POSY = -8;
const int CicoStatusBarClockComponent::RELATIVE_HOUR1_POSX = RELATIVE_HOUR10_POSX + HOUR10_WIDTH;
const int CicoStatusBarClockComponent::RELATIVE_HOUR1_POSY = -8;
const int CicoStatusBarClockComponent::RELATIVE_COLON_POSX = RELATIVE_HOUR1_POSX + HOUR1_WIDTH + COLON_BLANK;
const int CicoStatusBarClockComponent::RELATIVE_COLON_POSY = -3;
const int CicoStatusBarClockComponent::RELATIVE_MIN10_POSX = RELATIVE_COLON_POSX + COLON_WIDTH + COLON_BLANK;
const int CicoStatusBarClockComponent::RELATIVE_MIN10_POSY = -8;
const int CicoStatusBarClockComponent::RELATIVE_MIN1_POSX = RELATIVE_MIN10_POSX + MIN10_WIDTH;
const int CicoStatusBarClockComponent::RELATIVE_MIN1_POSY = -8;

// set image file path of clock
const char *CicoStatusBarClockComponent::imgfilepath_am = IMG_DIR"time_am.png";
const char *CicoStatusBarClockComponent::imgfilepath_pm = IMG_DIR"time_pm.png";
const char *CicoStatusBarClockComponent::imgfilepath_0 = IMG_DIR"time_0.png";
const char *CicoStatusBarClockComponent::imgfilepath_1 = IMG_DIR"time_1.png";
const char *CicoStatusBarClockComponent::imgfilepath_2 = IMG_DIR"time_2.png";
const char *CicoStatusBarClockComponent::imgfilepath_3 = IMG_DIR"time_3.png";
const char *CicoStatusBarClockComponent::imgfilepath_4 = IMG_DIR"time_4.png";
const char *CicoStatusBarClockComponent::imgfilepath_5 = IMG_DIR"time_5.png";
const char *CicoStatusBarClockComponent::imgfilepath_6 = IMG_DIR"time_6.png";
const char *CicoStatusBarClockComponent::imgfilepath_7 = IMG_DIR"time_7.png";
const char *CicoStatusBarClockComponent::imgfilepath_8 = IMG_DIR"time_8.png";
const char *CicoStatusBarClockComponent::imgfilepath_9 = IMG_DIR"time_9.png";
const char *CicoStatusBarClockComponent::imgfilepath_colon = IMG_DIR"time_ten.png";

// set clock component size
const int CicoStatusBarClockComponent::CLOCK_COMPONENT_WIDTH = AMPM_WIDTH +
                                                               AMPM_BLANK +
                                                               HOUR10_WIDTH +
                                                               HOUR1_WIDTH +
                                                               COLON_BLANK +
                                                               COLON_WIDTH +
                                                               COLON_BLANK +
                                                               MIN10_WIDTH +
                                                               MIN1_WIDTH;

const int CicoStatusBarClockComponent::CLOCK_COMPONENT_HEIGHT = HOUR10_HEIGHT;

/*--------------------------------------------------------------------------*/
/**
 *  @brief   default constructor
 *
 *  @param[in]   none
 *  @return      none
 */
/*--------------------------------------------------------------------------*/
CicoStatusBarClockComponent::CicoStatusBarClockComponent()
    : rate_(1.0)
{
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief   destructor
 *
 *  @param[in]   none
 *  @return      none
 */
/*--------------------------------------------------------------------------*/
CicoStatusBarClockComponent::~CicoStatusBarClockComponent()
{
    imgfilelist_number.clear();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   initialize of clock component
 *
 *  @param[in]  windowobj
 *  @param[in]  posx
 *  @param[in]  posy
 *  @return     true: success   false: failed
 */
/*--------------------------------------------------------------------------*/
bool
CicoStatusBarClockComponent::Initialize(Evas_Object *windowobj, int posx, int posy)
{
    ICO_TRA("CicoStatusBarClockComponent::Initialize Enter");
    if (imgfilelist_number.empty()) {
        imgfilelist_number.push_back(imgfilepath_0);
        imgfilelist_number.push_back(imgfilepath_1);
        imgfilelist_number.push_back(imgfilepath_2);
        imgfilelist_number.push_back(imgfilepath_3);
        imgfilelist_number.push_back(imgfilepath_4);
        imgfilelist_number.push_back(imgfilepath_5);
        imgfilelist_number.push_back(imgfilepath_6);
        imgfilelist_number.push_back(imgfilepath_7);
        imgfilelist_number.push_back(imgfilepath_8);
        imgfilelist_number.push_back(imgfilepath_9);
    }

    std::shared_ptr<CicoImageModule> ampm_module;
    ampm_module = std::make_shared<CicoImageModule>(CicoImageModule());
    if (!ampm_module->Initialize(windowobj)) {
        ICO_TRA("CicoStatusBarClockComponent::Initialize Leave(false)");
        return false;
    }

    std::shared_ptr<CicoImageModule> hour10_module;
    hour10_module = std::make_shared<CicoImageModule>(CicoImageModule());
    if (!hour10_module->Initialize(windowobj)) {
        ICO_TRA("CicoStatusBarClockComponent::Initialize Leave(false)");
        return false;
    }

    std::shared_ptr<CicoImageModule> hour1_module;
    hour1_module = std::make_shared<CicoImageModule>(CicoImageModule());
    if (!hour1_module->Initialize(windowobj)) {
        ICO_TRA("CicoStatusBarClockComponent::Initialize Leave(false)");
        return false;
    }

    std::shared_ptr<CicoImageModule> colon_module;
    colon_module = std::make_shared<CicoImageModule>(CicoImageModule());
    if (!colon_module->Initialize(windowobj)) {
        ICO_TRA("CicoStatusBarClockComponent::Initialize Leave(false)");
        return false;
    }

    std::shared_ptr<CicoImageModule> min10_module;
    min10_module = std::make_shared<CicoImageModule>(CicoImageModule());
    if (!min10_module->Initialize(windowobj)) {
        ICO_TRA("CicoStatusBarClockComponent::Initialize Leave(false)");
        return false;
    }

    std::shared_ptr<CicoImageModule> min1_module;
    min1_module = std::make_shared<CicoImageModule>(CicoImageModule());
    if (!min1_module->Initialize(windowobj)) {
        ICO_TRA("CicoStatusBarClockComponent::Initialize Leave(false)");
        return false;
    }

    PackModule(ampm_module);
    PackModule(hour10_module);
    PackModule(hour1_module);
    PackModule(colon_module);
    PackModule(min10_module);
    PackModule(min1_module);

    SetPos(posx, posy);
    SetSize(AMPM_WIDTH + HOUR10_WIDTH + HOUR1_WIDTH + COLON_WIDTH +
            MIN10_WIDTH + MIN1_WIDTH,
            AMPM_HEIGHT);
    SetRate(1.5);

    ICO_TRA("CicoStatusBarClockComponent::Initialize Leave");
    return Update();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   update of clock component
 *
 *  @param[in]  none
 *  @return     true: success   false: failed
 */
/*--------------------------------------------------------------------------*/
bool
CicoStatusBarClockComponent::Update()
{
    //ICO_TRA("CicoStatusBarClockComponent::Update Enter");
    if (modulelist_.empty()) {
        return false;
    }

    time_t nowtime;
    nowtime = std::time(NULL);
    std::tm *tm = std::localtime(&nowtime);
    int min = tm->tm_min;
    int hour = tm->tm_hour;

    std::shared_ptr<CicoImageModule> imagemodule;
    assert(!modulelist_.empty());
    std::list<std::shared_ptr<CicoCommonModule>>::iterator itr;
    itr = modulelist_.begin();
    imagemodule = std::static_pointer_cast<CicoImageModule>((*itr));
    assert(imagemodule != NULL);

    if (hour > 11) {
        if (!imagemodule->SetFilePath(imgfilepath_pm)) {
            ICO_TRA("CicoStatusBarClockComponent::Update Leave(false)");
            return false;
        }
    }
    else {
        if (!imagemodule->SetFilePath(imgfilepath_am)) {
            ICO_TRA("CicoStatusBarClockComponent::Update Leave(false)");
            return false;
        }
    }
    imagemodule->SetSize(rate_ * AMPM_WIDTH, rate_ * AMPM_HEIGHT);
    imagemodule->SetPos(posx_, posy_);
    imagemodule->Show();

    time_t hour_m = hour % 12;
    itr++;
    imagemodule = std::static_pointer_cast<CicoImageModule>((*itr));
    assert(imagemodule != NULL);
    if (!imagemodule->SetFilePath(imgfilelist_number[hour_m / 10])) {
        ICO_TRA("CicoStatusBarClockComponent::Update Leave(false)");
        return false;
    }
    imagemodule->SetSize(rate_ * HOUR10_WIDTH, rate_ * HOUR10_HEIGHT);
    imagemodule->SetPos(posx_ + rate_ * RELATIVE_HOUR10_POSX,
                        posy_ + rate_ * RELATIVE_HOUR10_POSY);
    imagemodule->Show();

    itr++;
    imagemodule = std::static_pointer_cast<CicoImageModule>((*itr));
    assert(imagemodule != NULL);
    if (!imagemodule->SetFilePath(imgfilelist_number[hour_m % 10])) {
        ICO_TRA("CicoStatusBarClockComponent::Update Leave(false)");
        return false;
    }
    imagemodule->SetSize(rate_ * HOUR1_WIDTH, rate_ * HOUR1_HEIGHT);
    imagemodule->SetPos(posx_ + rate_ * RELATIVE_HOUR1_POSX,
                        posy_ + rate_ * RELATIVE_HOUR1_POSY);
    imagemodule->Show();

    itr++;
    imagemodule = std::static_pointer_cast<CicoImageModule>((*itr));
    assert(imagemodule != NULL);
    if (!imagemodule->SetFilePath(imgfilepath_colon)) {
        ICO_TRA("CicoStatusBarClockComponent::Update Leave(false)");
        return false;
    }
    imagemodule->SetSize(rate_ * COLON_WIDTH, rate_ * COLON_HEIGHT);
    imagemodule->SetPos(posx_ + rate_ * RELATIVE_COLON_POSX,
                        posy_ + rate_ * RELATIVE_COLON_POSY);
    imagemodule->Show();

    itr++;
    imagemodule = std::static_pointer_cast<CicoImageModule>((*itr));
    assert(imagemodule != NULL);
    if (!imagemodule->SetFilePath(imgfilelist_number[min / 10])) {
        ICO_TRA("CicoStatusBarClockComponent::Update Leave(false)");
        return false;
    }
    imagemodule->SetSize(rate_ * MIN10_WIDTH, rate_ * MIN10_HEIGHT);
    imagemodule->SetPos(posx_ + rate_ * RELATIVE_MIN10_POSX,
                        posy_ + rate_ * RELATIVE_MIN10_POSY);
    imagemodule->Show();

    itr++;
    imagemodule = std::static_pointer_cast<CicoImageModule>((*itr));
    assert(imagemodule != NULL);
    if (!imagemodule->SetFilePath(imgfilelist_number[min % 10])) {
        ICO_TRA("CicoStatusBarClockComponent::Update Leave(false)");
        return false;
    }
    imagemodule->SetSize(rate_ * MIN1_WIDTH, rate_ * MIN1_HEIGHT);
    imagemodule->SetPos(posx_ + rate_ * RELATIVE_MIN1_POSX,
                        posy_ + rate_ * RELATIVE_MIN1_POSY);
    imagemodule->Show();

    //ICO_TRA("CicoStatusBarClockComponent::Update Leave(true)");
    return true;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   set rate of clock component
 *
 *  @param[in]  rate
 *  @return     true: success   false: failed
 */
/*--------------------------------------------------------------------------*/
bool
CicoStatusBarClockComponent::SetRate(double rate)
{
    if (modulelist_.empty()) {
        return false;
    }

    rate_ = rate;
    width_ *= rate_;
    height_ *= rate_;
    SetPos(posx_, posy_);
    SetSize(width_, height_);
    return true;
}

// set notification(icon-image, text) position and font
const int CicoNotificationPanelComponent::ICON_POSX = 0;
const int CicoNotificationPanelComponent::ICON_POSY = 0;
const int CicoNotificationPanelComponent::ICON_WIDTH = 48;
const int CicoNotificationPanelComponent::ICON_HEIGHT = 48;
const int CicoNotificationPanelComponent::RELATIVE_TEXT_POSX = 70;
const int CicoNotificationPanelComponent::RELATIVE_TEXT_POSY = 10;
const int CicoNotificationPanelComponent::TEXT_WIDTH = 450;
const int CicoNotificationPanelComponent::TEXT_HEIGHT = 50;
const char *CicoNotificationPanelComponent::FONT_STYLE = "Sans";
const int CicoNotificationPanelComponent::FONT_SIZE = 50;

/*--------------------------------------------------------------------------*/
/**
 * @brief   default constructor
 *
 *  @param[in]  none
 *  @return     none
 */
/*--------------------------------------------------------------------------*/
CicoNotificationPanelComponent::CicoNotificationPanelComponent()
    : CicoCommonComponent(), rate_(1.0)
{
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   destructor
 *
 *  @param[in]  none
 *  @return     none
 */
/*--------------------------------------------------------------------------*/
CicoNotificationPanelComponent::~CicoNotificationPanelComponent()
{
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   initialize of notification component
 *
 *  @param[in]  window
 *  @return     true: success   false: failed
 */
/*--------------------------------------------------------------------------*/
bool
CicoNotificationPanelComponent::Initialize(Evas_Object *window)
{
    ICO_TRA("CicoNotificationPanelComponent::Initialize Enter");

    if (!modulelist_.empty()) {
        ICO_TRA("CicoNotificationPanelComponent::Initialize Levae(false)");
        return false;
    }

    if (window == NULL) {
        ICO_TRA("CicoNotificationPanelComponent::Initialize Levae(false)");
        return false;
    }

    posx_= ICON_POSX;
    posy_ = ICON_POSY;
    width_ = RELATIVE_TEXT_POSX + TEXT_WIDTH;
    height_ = ICON_HEIGHT - ICON_POSY;

    imgmod_ = std::make_shared<CicoImageModule>(CicoImageModule());
    if (imgmod_ == NULL) {
        ICO_TRA("CicoNotificationPanelComponent::Initialize Levae(false)");
        return false;
    }

    if (!imgmod_->Initialize(window)) {
        ICO_TRA("CicoNotificationPanelComponent::Initialize Levae(false)");
        return false;
    }

    imgmod_->SetPos(posx_,7);

    imgmod_->SetSize(ICON_WIDTH, ICON_HEIGHT);
    imgmod_->Show();

    ICO_DBG("Create Text Module start");
    textmod_ = std::make_shared<CicoTextModule>(CicoTextModule());
    if (textmod_ == NULL) {
        ICO_TRA("CicoNotificationPanelComponent::Initialize Levae(false)");
        return false;
    }
    ICO_DBG("Create Text Module end");
    if (!textmod_->Initialize(window)) {
        ICO_TRA("CicoNotificationPanelComponent::Initialize Levae(false)");
        return false;
    }

    textmod_->SetPos(posx_ + RELATIVE_TEXT_POSX, posy_ + RELATIVE_TEXT_POSY);
    textmod_->SetSize(TEXT_WIDTH, TEXT_HEIGHT);
    textmod_->SetFont(FONT_STYLE, FONT_SIZE);
    textmod_->Show();

    PackModule(imgmod_);
    PackModule(textmod_);

    ICO_TRA("CicoNotificationPanelComponent::Initialize Levae(true)");
    return true;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   set notification panel
 *
 *  @param[in]  text
 *  @param[in]  iconpaht
 *  @param[in]  soundpath
 *  @return     true: success   false: failed
 */
/*--------------------------------------------------------------------------*/
void
CicoNotificationPanelComponent::SetNotification(const char *text,
                                                const char *iconpath,
                                                const char *soundpath)
{
    ICO_TRA("CicoNotificationPanelComponent::SetNotification Enter"
            "(text=%s icon=%s sound=%s)", text, iconpath, soundpath);

    if (iconpath != NULL) {
        imgmod_->SetFilePath(iconpath);
    }

    if (text != NULL) {
        textmod_->SetText(text);
    }

    textmod_->SetFont(NULL,50);
    textmod_->SetColor(0, 255, 255, 255);
    Show();

    ICO_TRA("CicoNotificationPanelComponent::SetNotification Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   set rate of notification component
 *
 *  @param[in]  rate
 *  @return     true: success   false: failed
 */
/*--------------------------------------------------------------------------*/
bool
CicoNotificationPanelComponent::SetRate(double rate)
{
    if (modulelist_.empty()) {
        return false;
    }
    rate_ = rate;
    width_ *= rate;
    height_ *= rate;
    SetPos(posx_, posy_);
    SetSize(width_, height_);
    return true;
}
// vim: set expandtab ts=4 sw=4:
