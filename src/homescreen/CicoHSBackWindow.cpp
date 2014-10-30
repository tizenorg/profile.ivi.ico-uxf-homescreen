/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Back Screen
 *
 * @date    Aug-08-2013
 */
#include <tzplatform_config.h>
#include "CicoHSBackWindow.h"
#include "CicoResourceConfig.h"

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSBackWindow::CicoHSBackWindow
 *          Constractor
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoHSBackWindow::CicoHSBackWindow(void)
{
    evas = NULL;

    CicoResourceConfig::GetImagePath(img_dir_path,ICO_HS_MAX_PATH_BUFF_LEN);

}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSBackWindow::~CicoHSBackWindow
 *          Destractor
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoHSBackWindow::~CicoHSBackWindow(void)
{
    /* Do not somthing to do */
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSBackWindow::CreateBackWindow
 *          create window (back)
 *
 * @param[in]   pos_x    position x
 * @param[in]   pos_y    position y
 * @param[in]   width    width
 * @param[in]   height   height
 * @return      OK or ERRROR
 */
/*--------------------------------------------------------------------------*/
int
CicoHSBackWindow::CreateBackWindow(int pos_x,int pos_y,int width,int height)
{
    int ret;

    /*create window*/
    ret = CreateWindow(ICO_HS_BACK_WINDOW_TITLE, pos_x, pos_y, width, height, EINA_TRUE);
    if(ret != ICO_OK){
       return ret;
    }

    /* evas get */
    evas = ecore_evas_get(window);
    if (!evas) {
        ICO_ERR("CicoHSBackWindow::CreateBackWindow: could not get evas.");
        return ICO_ERROR;
    }

    /* set back */
    /* image file name*/
    const char *conf_path = tzplatform_mkpath3(TZ_USER_HOME, ICO_HS_CONFIG_TOP,
                                               ICO_HS_CONFIG_HOMESCREEN);
    CicoGKeyFileConfig config;
    config.Initialize(ICO_HOMESCREEN_CONFIG_FILE, conf_path);
    const char *back_image = config.ConfigGetString(
                                        "homescreen", "background",
                                        ICO_HS_DEFAULT_BACK_WINDOW_IMAGE_FILE_PATH);
    ICO_DBG("CicoHSBackWindow::CreateBackWindow: background(%s)", back_image);

    /* set object*/
    canvas = evas_object_image_filled_add(evas);
    evas_object_image_file_set(canvas, back_image, NULL);
    int err = evas_object_image_load_error_get(canvas);
    if (err != EVAS_LOAD_ERROR_NONE) {
        ICO_ERR("CicoHSBackWindow::CreateBackWindow: backgound image is not exist");
        evas_object_del(canvas);
        FreeWindow();
        return ICO_ERROR;
    }
    evas_object_image_fill_set(canvas, pos_x, pos_y, width, height);
    evas_object_resize(canvas, width,height);
    evas_object_show(canvas);

    return ICO_OK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSBackWindow::FreeBackWindow
 *          free window (back)
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSBackWindow::FreeBackWindow(void)
{
    evas_object_del(canvas);
    FreeWindow();
}
// vim: set expandtab ts=4 sw=4:
