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

#include <Evas.h>
#include <ctime>
#include <list>
#include <vector>

#define   CLOCK_OBJECT_COUNT   (6)

//==========================================================================
/**
 *  @brief  This class provide the common module component interfaces
 */
//==========================================================================
class CicoCommonModule
{
public:
    // default constructor
    CicoCommonModule();

    // destructor
    virtual ~CicoCommonModule();

    // terminate module
    virtual bool Terminate(void);

    // show module
    virtual void Show(void);

    // hide module
    virtual void Hide(void);

    // set position of module
    virtual void SetPos(int x, int y);

    // set size of module
    virtual void SetSize(int w, int h);

    // move module
    virtual void Move(int diffx, int diffy);

    // resize module
    virtual void Resize(double ratew, double rateh);

    // reset position and size of module
    virtual void Reallocate(int standardx, int standardy,
                            double ratew, double rateh);

    // get raw data
    Evas_Object *GetRawData(void) const;
    bool operator==(const CicoCommonModule& moduleb) {
        return (this->evasobj_ == moduleb.evasobj_);
    }

protected:
    Evas_Object *evasobj_;  /// module object
    int posx_;              /// module position x
    int posy_;              /// module position y
    int width_;             /// module width
    int height_;            /// module height
};

//==========================================================================
/**
 *  @brief  This class provide the clock component
 */
//==========================================================================
class CicoStatusBarClockComponent : public CicoCommonModule
{
public:
    // default constructor
    CicoStatusBarClockComponent();

    // destructor
    ~CicoStatusBarClockComponent();

    // initialize of clock component
    bool Initialize(Evas_Object *window, Evas *evas);

    // update of clock component
    bool Update(void);

    // set rate of clock component
    bool SetRate(double rate);

    // get clock start position
    bool GetClockStart( Evas_Coord *x, Evas_Coord *y);

private:
    int last_clock_image[6];

};

//==========================================================================
/**
 *  @brief  This class provide the notification panel component
 */
//==========================================================================
class CicoNotificationPanelComponent : public CicoCommonModule
{
public:
    // default constructor
    CicoNotificationPanelComponent();

    // destructor
    ~CicoNotificationPanelComponent();

    // initialize of notification component
    bool Initialize(Evas_Object *window,Evas *evas);

    // set notification panel
    void SetNotification(const char *text,
                         const char *iconpath,
                         const char *soundpath);

    // set text end position
    void SetTextEndPosition( Evas_Coord x, Evas_Coord y);

private:
    // set text to content_text
    bool SetText(const char *text);

};
#endif  // __CICO_COMPONENT_IMPLEMENTATION_H__
// vim: set expandtab ts=4 sw=4:
