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
 *  @file   CicoModuleImplementation.cpp
 *
 *  @brief  This file is implimention of CicoTextModule class
 */
//==========================================================================

#include <Evas.h>
#include <Elementary.h>
#include <cstring>
#include <cassert>

#include <ico_log.h>
#include "CicoModuleImplementation.h"

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 *  @param[in]  none
 *  @return     none
 */
//--------------------------------------------------------------------------
CicoImageModule::CicoImageModule() 
    : CicoCommonModule()
{
    memset(filepath_, 0, sizeof(filepath_));
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 *  @param[in]  none
 *  @return     none
 */
//--------------------------------------------------------------------------
CicoImageModule::~CicoImageModule()
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  initialize of image module
 *  @param[in]  windowobj
 *  @return     true: success   false: failed
 */
//--------------------------------------------------------------------------
bool
CicoImageModule::Initialize(Evas_Object *windowobj)
{
    ICO_TRA("CicoImageModule::Initialize Enter");
    if (windowobj == NULL) {
        return false;
    }
    evasobj_ = elm_image_add(windowobj);
    if (evasobj_ == NULL) {
        ICO_ERR("elm_image_add() failed.");
        ICO_TRA("CicoImageModule::Initialize Leave(false)");
        return false;
    }
    if (strcmp("elm_image", evas_object_type_get(evasobj_)) != 0) {
        ICO_TRA("CicoImageModule::Initialize Leave(false)");
        return false;
    }
    ICO_DBG("object name=%s", evas_object_name_get(evasobj_));
    ICO_TRA("CicoImageModule::Initialize Leave(true)");
    return true;
}

//--------------------------------------------------------------------------
/**
 *  @brief  set filepaht of image module
 *  @param[in]  path    image file path
 *  @return     none
 */
//--------------------------------------------------------------------------
bool
CicoImageModule::SetFilePath(const char *path)
{
    //ICO_TRA("CicoImageModule::SetFilePath Enter");
    assert(evasobj_ != NULL);
    memset(filepath_, 0, sizeof(filepath_));
    strcpy(filepath_, path);
    //ICO_TRA("CicoImageModule::SetFilePath Leave");
    return (elm_image_file_set(evasobj_, filepath_, NULL) == EINA_TRUE) ? true : false;
}

const double CicoTextModule::FONT_SIZE_RATE = 0.7;

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 *  @param[in]  none
 *  @return     none
 */
//--------------------------------------------------------------------------
CicoTextModule::CicoTextModule()
    : CicoCommonModule(), fontsize_(50)
{
    ICO_TRA("CicoTextModule::CicoTextModule Enter");
    memset(text_, 0, sizeof(text_));
    memset(fontstyle_, 0, sizeof(fontstyle_));
    strcpy(fontstyle_, "Sans");
    ICO_TRA("CicoTextModule::CicoTextModule Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 *  @param[in]  none
 *  @return     none
 */
//--------------------------------------------------------------------------
CicoTextModule::~CicoTextModule()
{
    ICO_TRA("CicoTextModule::~CicoTextModule Enter");
    ICO_TRA("CicoTextModule::~CicoTextModule Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief initialize of text module
 *  @param[in]  windowobj 
 *  @return     none
 */
//--------------------------------------------------------------------------
bool
CicoTextModule::Initialize(Evas_Object *windowobj)
{
    ICO_TRA("CicoTextModule::Initialize Enter");
    evasobj_ = elm_label_add(windowobj);
    elm_label_line_wrap_set(evasobj_, ELM_WRAP_NONE);
    elm_object_style_set(evasobj_, "default");

    strcpy(fontstyle_, "Sans");
    SetFont(fontstyle_, fontsize_);

    ICO_DBG("object name=%s", evas_object_name_get(evasobj_));
    ICO_TRA("CicoTextModule::Initialize Leave");
    return (evasobj_ != NULL);
}

//--------------------------------------------------------------------------
/**
 *  @brief  set text of text module
 *  @param[in]  text
 *  @return     true: success
 */
//--------------------------------------------------------------------------
bool
CicoTextModule::SetText(const char *text)
{
    ICO_TRA("CicoTextModule::SetText Enter(text=%s)", text);
    memset(text_, 0, sizeof(text_));
    strncpy(text_, text, (sizeof(text_)-1));
    ICO_DBG("text_=%s", text_);

    elm_object_part_text_set(evasobj_, NULL, text_);
    SetFont(fontstyle_, fontsize_);
    SetSize(FONT_SIZE_RATE * fontsize_ * strlen(text), height_);

    ICO_TRA("CicoTextModule::SetText Leave");
    return true;
}

//--------------------------------------------------------------------------
/**
 *  @brief  set font of text module
 *  @param[in]  style   font style
 *  @param[in]  size    font size
 *  @return     none
 */
//--------------------------------------------------------------------------
void
CicoTextModule::SetFont(const char *style, const int size)
{
    ICO_TRA("CicoTextModule::SetFont Enter"
            "(style=%s size=%d)", style, size);

    if (style != NULL) {
        strcpy(fontstyle_, style);
    }
    if (size > 0) {
        fontsize_ = size;
    }

    const char *textclassname = evas_object_type_get(evasobj_);
    char textclassname2[strlen(textclassname) + 1];
    if (strncmp(textclassname, "elm_", 4) == 0) {
        memset(textclassname2, 0, sizeof(textclassname2));
        for (int i = 0; textclassname[i + 4] != '\0'; i++) {
            textclassname2[i] = textclassname[i + 4];
        }
        textclassname = textclassname2;
    }
    elm_config_font_overlay_set(textclassname, fontstyle_, fontsize_);
    elm_config_font_overlay_apply();
    ICO_TRA("CicoTextModule::SetFont Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  set color of text module
 *  @param[in]  r   Red color
 *  @param[in]  g   Green color
 *  @param[in]  b   Blue color
 *  @param[in]  a   alpha color
 *  @return     none
 */
//--------------------------------------------------------------------------
void CicoTextModule::SetColor(const int r, const int g, 
                              const int b, const int a)
{
    ICO_TRA("CicoTextModule::SetColor Enter"
            "(r/g/b/a=%d/%d/%d/%d)", r, g, b, a);
    elm_object_style_set(evasobj_, "marker");
    evas_object_color_set(evasobj_, r, g, b, a);
    ICO_TRA("CicoTextModule::SetColor Leave");
}
// vim: set expandtab ts=4 sw=4:
