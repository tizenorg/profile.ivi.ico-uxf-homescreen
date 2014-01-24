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
 *  @file   CicoComponentImplementation.h
 *
 *  @brief  This file is definition of component classes
 */
//==========================================================================
#ifndef __CICO_COMPONENT_IMPLEMENTATION_H__
#define __CICO_COMPONENT_IMPLEMENTATION_H__

#include <Elementary.h>
#include <ctime>
#include <list>
#include <vector>
#include "CicoComponentInterface.h"
#include "CicoCommonComponent.h"
#include "CicoModuleImplementation.h"

//==========================================================================
/**
 *  @brief  This class provide the clock component
 */
//==========================================================================
class CicoStatusBarClockComponent : public CicoCommonComponent
{
public:
    // default constructor
    CicoStatusBarClockComponent();

    // destructor
    ~CicoStatusBarClockComponent();

    // initialize of clock component
    bool Initialize(Evas_Object *window, int posx, int posy);

    // update of clock component
    bool Update(void);

    // set rate of clock component 
    bool SetRate(double rate);

    static const int CLOCK_COMPONENT_WIDTH; /// clock component width
    static const int CLOCK_COMPONENT_HEIGHT;/// clock component height

private:
    static const int AMPM_WIDTH;            /// ampm image width
    static const int AMPM_HEIGHT;           /// ampm image height
    static const int HOUR10_WIDTH;          /// hour10 image width
    static const int HOUR10_HEIGHT;         /// hour10 image height
    static const int HOUR1_WIDTH;           /// hour1 image width
    static const int HOUR1_HEIGHT;          /// hour1 image height
    static const int COLON_WIDTH;           /// colon image width
    static const int COLON_HEIGHT;          /// colon image height
    static const int MIN10_WIDTH;           /// min10 image width
    static const int MIN10_HEIGHT;          /// min10 image height
    static const int MIN1_WIDTH;            /// min1 image width
    static const int MIN1_HEIGHT;           /// min1 image height

    static const int COLON_BLANK;           /// colon blank size
    static const int AMPM_BLANK;            /// ampm blank size
    static const int LAST_BLANK;            /// last blank size

    static const int AMPM_POSX;             /// ampm image position x
    static const int AMPM_POSY;             /// ampm image position y
    static const int RELATIVE_HOUR10_POSX;  /// hour10 image position x
    static const int RELATIVE_HOUR10_POSY;  /// hour10 image position y
    static const int RELATIVE_HOUR1_POSX;   /// hour1 image position x
    static const int RELATIVE_HOUR1_POSY;   /// hour1 image position y
    static const int RELATIVE_COLON_POSX;   /// colon image position x
    static const int RELATIVE_COLON_POSY;   /// colon image position y
    static const int RELATIVE_MIN10_POSX;   /// min10 image position x
    static const int RELATIVE_MIN10_POSY;   /// min10 image position y
    static const int RELATIVE_MIN1_POSX;    /// min1 image position x
    static const int RELATIVE_MIN1_POSY;    /// min1 image position y

    static const char *imgfilepath_am;      /// am image filepath
    static const char *imgfilepath_pm;      /// pm image filepath
    static const char *imgfilepath_0;       /// 0 image filepath
    static const char *imgfilepath_1;       /// 1 image filepath
    static const char *imgfilepath_2;       /// 2 image filepath
    static const char *imgfilepath_3;       /// 3 image filepath
    static const char *imgfilepath_4;       /// 4 image filepath
    static const char *imgfilepath_5;       /// 5 image filepath
    static const char *imgfilepath_6;       /// 6 image filepath
    static const char *imgfilepath_7;       /// 7 image filepath
    static const char *imgfilepath_8;       /// 8 image filepath
    static const char *imgfilepath_9;       /// 9 image filepath
    static const char *imgfilepath_colon;   /// colon image filepath

    std::vector<const char*> imgfilelist_number;    /// image filepath list
    double rate_;
};

//==========================================================================
/**
 *  @brief  This class provide the notification panel component
 */
//==========================================================================
class CicoNotificationPanelComponent : public CicoCommonComponent
{
public:
    // default constructor
    CicoNotificationPanelComponent();

    // destructor
    ~CicoNotificationPanelComponent();

    // initialize of notification component
    bool Initialize(Evas_Object *window);

    // set notification panel
    void SetNotification(const char *text,
                         const char *iconpath,
                         const char *soundpath);

    // set rate of notification component
    bool SetRate(double rate);

protected:
    static const int ICON_POSX;             /// notification icon position x
    static const int ICON_POSY;             /// notification icon position y
    static const int ICON_WIDTH;            /// notification icon width
    static const int ICON_HEIGHT;           /// notification icon height
    static const int RELATIVE_TEXT_POSX;    /// notification text position x
    static const int RELATIVE_TEXT_POSY;    /// notification text position y
    static const int TEXT_WIDTH;            /// notification text width
    static const int TEXT_HEIGHT;           /// notification text height
    static const char *FONT_STYLE;          /// notification text font-style
    static const int FONT_SIZE;             /// notification text font-size

    std::shared_ptr<CicoTextModule> textmod_;   /// notification text module
    std::shared_ptr<CicoImageModule> imgmod_;   /// notification icon module
    double rate_;                               /// view rate of notification component 
};
#endif  // __CICO_COMPONENT_IMPLEMENTATION_H__
// vim: set expandtab ts=4 sw=4:
