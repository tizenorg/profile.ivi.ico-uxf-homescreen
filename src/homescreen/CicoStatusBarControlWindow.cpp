/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   windows control for statusbar application
 *
 * @date    Feb-15-2013
 */
#include "CicoStatusBarControlWindow.h"

/*============================================================================*/
/* Declare static values                                                      */
/*============================================================================*/
CicoStatusBarControlWindow* CicoStatusBarControlWindow::instance;

/*============================================================================*/
/* Functions Declaration (CicoStatusBarControlWindow)                         */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoStatusBarControlWindow::CicoStatusBarControlWindow
 *          constractor
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoStatusBarControlWindow::CicoStatusBarControlWindow()
{
    width = 0;
    moveH = 0;
    window = NULL;
    shtct_list = NULL;

    /* Initialize a new system of Ecore_Evas */
    ecore_evas_init();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoStatusBarControlWindow::~CicoStatusBarControlWindow
 *          destractor
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoStatusBarControlWindow::~CicoStatusBarControlWindow()
{
    /* Free an Ecore_Evas */
    if(window != NULL){
        ecore_evas_free(window);
    }
    /* Shut down the Ecore_Evas system */
    ecore_evas_shutdown();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoStatusBarControlWindow::Initialize
 *          Initialize
 *
 * @param[in]   homescreen_config_tmp    pointer to instance of CicoHomeScreenConfig
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoStatusBarControlWindow::Initialize(CicoHomeScreenConfig *homescreen_config_tmp)
{
    homescreen_config = homescreen_config_tmp;

    /* get image path*/
    CicoHomeScreenResourceConfig::GetImagePath(respath, sizeof(respath));

    /*create instances*/
    time_data[0] = new CicoStatusBarTime(fname_num0);
    time_data[1] = new CicoStatusBarTime(fname_num1);
    time_data[2] = new CicoStatusBarTime(fname_num2);
    time_data[3] = new CicoStatusBarTime(fname_num3);
    time_data[4] = new CicoStatusBarTime(fname_num4);
    time_data[5] = new CicoStatusBarTime(fname_num5);
    time_data[6] = new CicoStatusBarTime(fname_num6);
    time_data[7] = new CicoStatusBarTime(fname_num7);
    time_data[8] = new CicoStatusBarTime(fname_num8);
    time_data[9] = new CicoStatusBarTime(fname_num9);
    time_data[10] = new CicoStatusBarTime(fname_am);
    time_data[11] = new CicoStatusBarTime(fname_pm);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoStatusBarControlWindow::Finalize
 *          Finalize
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoStatusBarControlWindow::Finalize(void)
{
    /*delete instances*/
    for(int ii = 0;ii < ICO_SB_TIME_IMG_PARTS;ii++){
        delete time_data[ii];
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoStatusBarControlWindow::SetInstanceForClock
 *          seting instance for static method
 *
 * @param[in]   instance_tmp    instance of this class
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoStatusBarControlWindow::SetInstanceForClock(CicoStatusBarControlWindow *instance_tmp)
{
    instance = instance_tmp;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoStatusBarControlWindow::CreateEcoreEvas
 *          make new ecore_evas
 *
 * @param[in]   none
 * @return      0:success -1:failed
 */
/*--------------------------------------------------------------------------*/
int
CicoStatusBarControlWindow::CreateEcoreEvas(void)
{
    /* Make a new ecore_evas */
    window = ecore_evas_new(NULL, 0, 0, 1, 1, "frame=0");

    /* if not for a window, return NULL */
    if (!window) {
        EINA_LOG_CRIT("StatusBar: could not create ons_window.");
        return -1;
    }
    ecore_evas_callback_delete_request_set(window,QuitEcoreLoop);

    return 0;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoStatusBarControlWindow::StartEcoreLoop
 *          start ecore loop
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoStatusBarControlWindow::StartEcoreLoop(void)
{
    /* Start main loop */
    ecore_main_loop_begin();
}

/*--------------------------------------------------------------------------*/
/*
 * @brief   CicoStatusBarControlWindow::QuitEcoreLoop
 *          callback function called by EFL when ecore destroyed.
 *          exit ecore main loop.
 *
 * @param[in]   ee                  ecore evas object
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoStatusBarControlWindow::QuitEcoreLoop(Ecore_Evas *ee)
{
    ICO_DBG("CicoStatusBarControlWindow::QuitEcoreLoop: Enter");
    ecore_main_loop_quit();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoStatusBarControlWindow::ResizeWindow
 *          resize window
 *
 * @param[in]   orientation     setting ICO_ONS_HORIZONTAL or ICO_ONS_VERTICAL
 *                              (if config is existing,this value is ignored)
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoStatusBarControlWindow::ResizeWindow(int orientation)
{
    int width_tmp, height_tmp;

    /* load configuration */
    if (homescreen_config != NULL) {
       orientation = homescreen_config->ConfigGetInteger(ICO_HS_CONFIG_STATUBAR,
                                          ICO_HS_CONFIG_ORIENTAION,
                                          orientation);
    }

    /* resize window */
    ecore_main_loop_iterate();
    ecore_wl_screen_size_get(&width_tmp, &height_tmp);
    if (orientation == ICO_SB_VERTICAL) {
        width = width_tmp > height_tmp ? height_tmp : width_tmp;
        moveH = width - ICO_HS_SIZE_LAYOUT_WIDTH;
    }
    else {
        width = width_tmp < height_tmp ? height_tmp : width_tmp;
        moveH = width - ICO_HS_SIZE_LAYOUT_WIDTH;
    }

    /* Set the window size of the maximum and minimum */
    ecore_evas_size_min_set(window, width, ICO_HS_SIZE_SB_HEIGHT);
    ecore_evas_size_max_set(window, width, ICO_HS_SIZE_SB_HEIGHT);

    /* Set the title of an Ecore_Evas window */
    /* ecore_evas_title_set(ee, "Ecore_Evas buffer (image) example"); */
    ecore_evas_move(window, 0, ICO_SB_POS_Y);
    ecore_evas_resize(window, width, ICO_HS_SIZE_SB_HEIGHT);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoStatusBarControlWindow::ShowWindow
 *          show Window
 *
 * @param[in]   void
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoStatusBarControlWindow::ShowWindow(void)
{
    ecore_evas_show(window);
}


/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoStatusBarControlWindow::InitEvas
 *          initialize evas
 *
 * @param[in]   none
 * @return      0:OK -1:NG
 */
/*--------------------------------------------------------------------------*/
int
CicoStatusBarControlWindow::InitEvas(void)
{
    Evas_Object *canvas;

    if (evas) {
        return 0;
    }

    evas = ecore_evas_get(window);
    if (!evas) {
        ICO_DBG("CicoStatusBarControlWindow::InitEvas: could not create evas.");
        return -1;
    }

    /* BG color set */
    canvas = evas_object_rectangle_add(evas);
    evas_object_color_set(canvas, 0, 0, 0, 255);
    evas_object_move(canvas, 0, ICO_SB_POS_Y);
    evas_object_resize(canvas, width, ICO_HS_SIZE_SB_HEIGHT);
    evas_object_show(canvas);

    return 0;

}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoStatusBarControlWindow::ShowClockHour
 *          show the clock's hour image on statubar
 *
 * @param[in]   tm                  time data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoStatusBarControlWindow::ShowClockHour(struct tm *t_st)
{
    char file[ICO_SB_BUF_SIZE];
    int ampm_index;

    //AM or PM
    if (t_st->tm_hour < ICO_SB_TIME_AMPM_THRESHOLD) {
        ampm_index = ICO_SB_TIME_AM;
    }
    else {
        ampm_index = ICO_SB_TIME_PM;
    }
    sprintf(file, "%s%s", respath,
                time_data[ampm_index]->GetFileName());
    evas_object_image_file_set(ampm, file, NULL);

    //Hour
    sprintf(file, "%s%s", respath,
            time_data[(t_st->tm_hour % ICO_SB_TIME_AMPM_THRESHOLD) / 10]->GetFileName());
    evas_object_image_file_set(hour1, file, NULL);

    sprintf(file, "%s%s", respath,
            time_data[(t_st->tm_hour % ICO_SB_TIME_AMPM_THRESHOLD) % 10]->GetFileName());
    evas_object_image_file_set(hour2, file, NULL);

    /*Makes the given Evas object visible*/
    evas_object_show(ampm);
    evas_object_show(hour1);
    evas_object_show(hour2);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoStatusBarControlWindow::ShowClockMinutes
 *          show the clock's minutes image on statubar
 *
 * @param[in]   tm                  time data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoStatusBarControlWindow::ShowClockMinutes(struct tm *t_st)
{
    char file[ICO_SB_BUF_SIZE];
    //Minute
    sprintf(file, "%s%s", respath, time_data[t_st->tm_min / 10]->GetFileName());
    evas_object_image_file_set(min1, file, NULL);
    sprintf(file, "%s%s", respath, time_data[t_st->tm_min % 10]->GetFileName());
    evas_object_image_file_set(min2, file, NULL);

    /*Makes the given Evas object visible*/
    evas_object_show(min1);
    evas_object_show(min2);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoStatusBarControlWindow::UpdateTime
 *          callback function called by Ecore.
 *          get time data, and show the clock's image on statubar.
 *
 * @param[in]   thread_data         user data
 * @return      call back setting
 * @retval      ECORE_CALLBACK_RENEW    set callback
 */
/*--------------------------------------------------------------------------*/
Eina_Bool
CicoStatusBarControlWindow::UpdateTime(void *thread_data)
{
    time_t timer;
    struct tm *t_st, *old_t_st;

    old_t_st = &instance->latest_time;

    /* get time */
    time(&timer);
    t_st = localtime(&timer);

    if (old_t_st->tm_hour != t_st->tm_hour) {
        /* hour file set */
        instance->ShowClockHour(t_st);
    }
    if (old_t_st->tm_min != t_st->tm_min) {
        /* min file set */
        instance->ShowClockMinutes(t_st);
    }
    memcpy(old_t_st, t_st, sizeof(struct tm));

    return ECORE_CALLBACK_RENEW;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoStatusBarControlWindow::ShowClock
 *          show the clock's
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoStatusBarControlWindow::ShowClock(void)
{
    Evas_Object *colon;
    time_t timer;
    struct tm *t_st;
    char file[ICO_SB_BUF_SIZE];
    /* ... */
    ampm = evas_object_image_filled_add(evas);
    hour1 = evas_object_image_filled_add(evas);
    hour2 = evas_object_image_filled_add(evas);
    min1 = evas_object_image_filled_add(evas);
    min2 = evas_object_image_filled_add(evas);
    colon = evas_object_image_filled_add(evas);

    /* This function will make layout change */
    evas_object_move(ampm, moveH + ICO_SB_POS_AMPM_X, ICO_SB_POS_AMPM_Y);
    evas_object_move(hour1, moveH + ICO_SB_POS_HOUR1_X, ICO_SB_POS_HOUR1_Y);
    evas_object_move(hour2, moveH + ICO_SB_POS_HOUR2_X, ICO_SB_POS_HOUR2_Y);
    evas_object_move(min1, moveH + ICO_SB_POS_MIN1_X, ICO_SB_POS_MIN1_Y);
    evas_object_move(min2, moveH + ICO_SB_POS_MIN2_X, ICO_SB_POS_MIN2_Y);
    evas_object_move(colon, moveH + ICO_SB_POS_COLON_X, ICO_SB_POS_COLON_Y);

    /* This function will make size change of picture */
    evas_object_resize(ampm, ICO_SB_SIZE_AMPM_W, ICO_SB_SIZE_AMPM_H);
    evas_object_resize(hour1, ICO_SB_SIZE_NUM_W, ICO_SB_SIZE_NUM_H);
    evas_object_resize(hour2, ICO_SB_SIZE_NUM_W, ICO_SB_SIZE_NUM_H);
    evas_object_resize(min1, ICO_SB_SIZE_NUM_W, ICO_SB_SIZE_NUM_H);
    evas_object_resize(min2, ICO_SB_SIZE_NUM_W, ICO_SB_SIZE_NUM_H);
    evas_object_resize(colon, ICO_SB_SIZE_COLON_W, ICO_SB_SIZE_COLON_H);

    /* Makes the given Evas object visible */
    evas_object_show(ampm);
    evas_object_show(hour1);
    evas_object_show(hour2);
    evas_object_show(min1);
    evas_object_show(min2);
    evas_object_show(colon);

    /*first set*/
    time(&timer);
    t_st = localtime(&timer);
    ShowClockHour(t_st);
    ShowClockMinutes(t_st);

    /* Set the image file */
    for (int ii = 0; ii < ICO_SB_TIME_IMG_PARTS; ii++) {
        time_data[ii]->SetTimeImg(evas_object_image_filled_add(evas));
        sprintf(file, "%s%s", respath, time_data[ii]->GetFileName());
        evas_object_image_file_set(time_data[ii]->GetTimeImg(), file, NULL);
    }
    sprintf(file, "%s%s", respath, fname_colon);
    evas_object_image_file_set(colon, file, NULL);

    ecore_timer_add(ICO_SB_TIMEOUT, UpdateTime, NULL);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoStatusBarControlWindow::EvasObjectImageFileSetAndShow
 *          set the evas object image file and show it
 *
 * @param[in]   obj    evas object
 * @param[in]   img    image
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoStatusBarControlWindow::EvasObjectImageFileSetAndShow(Evas_Object *obj,char *icon)
{
    char img[ICO_HS_TEMP_BUF_SIZE];

    sprintf(img, icon, respath);
    evas_object_image_file_set(obj, img, NULL);
    evas_object_show(obj);
}
