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
 *  @file   CicoModuleInterface.h
 *
 *  @brief  This file is definition of CicoModuleInterface class
 */
//==========================================================================
#ifndef __CICO_MODULE_IMPLEMENTATION_H__
#define __CICO_MODULE_IMPLEMENTATION_H__

#include <Evas.h>
#include "CicoModuleInterface.h"

//==========================================================================
/**
 *  @brief  This class provide the common module component interfaces
 */
//==========================================================================
class CicoCommonModule : public CicoModuleInterface
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
 *  @brief  This class provide the image module
 */
//==========================================================================
class CicoImageModule : public CicoCommonModule
{
public:
    // default constructor
    CicoImageModule();

    // destructor
    virtual ~CicoImageModule();

    // initialize of image module
    bool Initialize(Evas_Object *windowobj);

    // set filepath of image module
    bool SetFilePath(const char *path);
protected:
    char filepath_[256];    ///image file path
};

//==========================================================================
/**
 *  @brief  This class provide the text module
 */
//==========================================================================
class CicoTextModule : public CicoCommonModule
{
public:
    // default contructor
    CicoTextModule();

    // destructor
    ~CicoTextModule();

    // initialize of text module
    bool Initialize(Evas_Object *windowobj);

    // set text of text module
    bool SetText(const char *text);

    // set font of text module
    void SetFont(const char *style, const int size);

    // set color of text module
    void SetColor(const int r, const int g, const int b, const int a);
protected:
    static const double FONT_SIZE_RATE; /// rate of font size
    char text_[256];                    /// text of textmodule
    char fontstyle_[256];               /// font style of textmodule
    int fontsize_;                      /// font size of textmodule
};
#endif  //  __CICO_MODULE_IMPLEMENTATION_H__
// vim: set expandtab ts=4 sw=4:
