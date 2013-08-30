/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   window control for onscreen application
 *
 * @date    Feb-15-2013
 */
#include "CicoOnScreenControlWindow.h"

/*============================================================================*/
/* Functions Declaration (CicoOnscreenControlWindow)                          */
/*============================================================================*/

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenControlWindow::CicoOnScreenControlWindow
 *          constractor 
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoOnScreenControlWindow::CicoOnScreenControlWindow()
{
    window = NULL;
    evas = NULL;
    edje = NULL;
    img_list = NULL;

    /* Reset a ecore_evas */
    ecore_evas_init();

    /* Initialize a edje */
    edje_init();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenControlWindow::~CicoOnScreenControlWindow
 *          destructor
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoOnScreenControlWindow::~CicoOnScreenControlWindow()
{
    /* edje shutdown*/
    edje_shutdown();

    if(window != NULL){
        ecore_evas_free(window);
    }
    /* end the ecore_evas */
    ecore_evas_shutdown();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenControlWindow::CreateEcoreEvas
 *          make new ecore_evas 
 *
 * @param[in]   none
 * @return      0:success -1:failed
 */
/*--------------------------------------------------------------------------*/
int
CicoOnScreenControlWindow::CreateEcoreEvas(void)
{
    /* Make a new ecore_evas */
    window = ecore_evas_new(NULL, 0, 0, 1, 1, "frame=0");

    /* if not for a window, return NULL */
    if (!window) {
        EINA_LOG_CRIT("OnScreen: could not create ons_window.");
        return -1;
    }
    ecore_evas_callback_delete_request_set(window, QuitEcoreLoop);

    return 0; 
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenControlWindow::ResizeWindow
 *          resize window 
 *
 * @param[in]   configExist     if config is existing this parameter is ICO_HS_OK
 * @param[in]   orientation     setting ICO_ONS_HORIZONTAL or ICO_ONS_VERTICAL
 *                              (if config is existing,this value is ignored)
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoOnScreenControlWindow::ResizeWindow(CicoHomeScreenConfig *homescreen_config,int orientation)
{
    int width_tmp, height_tmp;
  
    /* load configuration */
    if (homescreen_config != NULL) {
       orientation = homescreen_config->ConfigGetInteger(ICO_HS_CONFIG_ONSCREEN,
                                          ICO_HS_CONFIG_ORIENTAION,
                                          orientation);
    }

    /* resize window */
    ecore_main_loop_iterate();
    ecore_wl_screen_size_get(&width_tmp, &height_tmp); /*getting size of screen*/
    if (orientation == ICO_ONS_VERTICAL) {
        width = width_tmp > height_tmp ? height_tmp : width_tmp;
        height = width_tmp > height_tmp ? width_tmp : height_tmp;
    }
    else {
        width = width_tmp < height_tmp ? height_tmp : width_tmp;
        height = width_tmp < height_tmp ? width_tmp : height_tmp;
    }
    ecore_evas_resize(window, width, height); /*resize*/
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenControlWindow::ShowWindow
 *          show Window
 *
 * @param[in]   void
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoOnScreenControlWindow::ShowWindow(void)
{
    ecore_evas_alpha_set(window, EINA_TRUE); 
    ecore_evas_show(window);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenControlWindow::StartEcoreLoop
 *          start ecore loop
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoOnScreenControlWindow::StartEcoreLoop(void)
{
    /* Start main loop */
    ecore_main_loop_begin();
}

/*--------------------------------------------------------------------------*/
/*
 * @brief   CicoOnScreenControlWindow::QuitEcoreLoop
 *          callback function called by EFL when ecore destroyed.
 *          exit ecore main loop.
 *
 * @param[in]   ee                  ecore evas object
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoOnScreenControlWindow::QuitEcoreLoop(Ecore_Evas *ee)
{
    uifw_trace("CicoOnScreenControlWindow::QuitEcoreLoop: Enter");
    ecore_main_loop_quit();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenControlWindow::GetWidth
 *          width of window
 *
 * @param[in]   none
 * @return      width
 */
/*--------------------------------------------------------------------------*/
int
CicoOnScreenControlWindow::GetWidth(void)
{
    return width;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenControlWindow::GetHeight
 *          height of window
 *
 * @param[in]   none
 * @return      height
 */
/*--------------------------------------------------------------------------*/
int
CicoOnScreenControlWindow::GetHeight(void)
{
    return height;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenControlWindow::InitEvas
 *          initialize evas 
 *
 * @param[in]   none
 * @return      0:OK -1:NG
 */
/*--------------------------------------------------------------------------*/
int 
CicoOnScreenControlWindow::InitEvas(void)
{
    Evas_Object *canvas;

    if (evas) {
        return 0;
    }

    evas = ecore_evas_get(window);
    if (!evas) {
        uifw_trace("CicoOnScreenControlWindow::InitEvas: could not create evas.");
        return -1;
    }

    /* set color */
    canvas = evas_object_rectangle_add(evas);
    evas_object_color_set(canvas, 0, 0, 0, 191);
    evas_object_move(canvas, 0, 0);
    evas_object_resize(canvas, width, height);
    evas_object_show(canvas);
    
    return 0;

}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenControlWindow::DeletePreImage
 *          delete PreImage 
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoOnScreenControlWindow::DeletePreImage(void)
{
   Eina_List *l, *l_next;
   Evas_Object *data;
   void* vpdata;

   uifw_trace("CicoOnScreenControlWindow::DeletePreImage: img list is %08x",
              img_list);   

   EINA_LIST_FOREACH_SAFE(img_list, l, l_next,vpdata) {
        data=static_cast<Evas_Object *>(vpdata);
        uifw_trace("CicoOnScreenControlWindow::DeletePreImage: del data is %08x",
                   data);
        evas_object_del(data);
        img_list = eina_list_remove_list(img_list, l);
    }

    /* delete edje */
    if (edje) {
        evas_object_del(edje);
    }

}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenControlWindow::CreateAndShowEdje
 *          delete PreImage 
 *
 * @param[in]   none
 * @return      0:OK -1:NG
 */
/*--------------------------------------------------------------------------*/
int
CicoOnScreenControlWindow::CreateAndShowEdje(void)
{
    int moveX, moveY; /* move list to center */
 
    /* create and add object in canvas from edje */
    edje = edje_object_add(evas);
    if (!edje) {
        uifw_trace("CicoOnScreenControlWindow::CreateAndShowEdje: could not create edje object!");
        return -1;
    }

    uifw_trace("CicoOnScreenControlWindow::CreateAndShowEdje: w=%d h=%d",
               width, height);

    /* calc move */
    moveX = (width - ICO_HS_SIZE_LAYOUT_WIDTH) / 2;
    moveY = (height - ICO_HS_SIZE_LAYOUT_HEIGHT) / 2;

    /* Put in the image */
    evas_object_move(edje, moveX, moveY);
    /* Resize the image */
    evas_object_resize(edje, width, height);
    /* Show the image */
    evas_object_show(edje);

    return 0;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenControlWindow::GetEdjeFileCollectionList
 *          get edje file collection list
 *
 * @param[in]   edje_file   edje file
 * @return      EinaList
 */
/*--------------------------------------------------------------------------*/
Eina_List *
CicoOnScreenControlWindow::GetEdjeFileCollectionList(const char *edje_file)
{
    return edje_file_collection_list(edje_file);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenControlWindow::SetEdjeFile
 *          set edje file
 *
 * @param[in]   group     eina listons_wait_reply
 * @param[in]   edje_file     edje file 
 * @return      EinaList
 */
/*--------------------------------------------------------------------------*/
int 
CicoOnScreenControlWindow::SetEdjeFile(Eina_List* group,const char *edje_file)
{
    /* Set the edj file */
    if (!edje_object_file_set(edje, edje_file, static_cast<const char *>(group->data))) {
        Edje_Load_Error err = edje_object_load_error_get(edje);
        const char *errmsg = edje_load_error_str(err);
        uifw_trace("CicoOnScreenControlWindow::SetEdjeFile: could not load %s: %s",
                   edje_file, errmsg);

        edje_file_collection_list_free(group);
        evas_object_del(edje);
        return -1;
    }
  
    return 0;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenControlWindow::GetEdjeObjectAccessPartList
 *          get edje object access part list
 *
 * @param[in]   none
 * @return      EinaList
 */
/*--------------------------------------------------------------------------*/
Eina_List *
CicoOnScreenControlWindow::GetEdjeObjectAccessPartList(void)
{
    return edje_object_access_part_list_get(edje);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenControlWindow::GetEdjeObjectPartObject
 *          get edje object part object
 *
 * @param[in]   data     data
 * @return      Evas_Object
 */
/*--------------------------------------------------------------------------*/
Evas_Object *
CicoOnScreenControlWindow::GetEdjeObjectPartObject(const char * data)
{
    return  const_cast<Evas_Object *>(edje_object_part_object_get(edje,data));
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenControlWindow::EvasObjectEventCallBack
 *          set evas object event callback
 *
 * @param[in]   obj         Evas_Ojbect
 * @param[in]   type        Evas_CallBack_Type
 * @param[in]   func        callback function 
 * @param[in]   data        data
 * @return      Evas_Object
 */
/*--------------------------------------------------------------------------*/
void 
CicoOnScreenControlWindow::EvasObjectEventCallBack(Evas_Object *obj,
                                                   Evas_Callback_Type type,
                                                   void (*func)(void*,
                                                       Evas*,
                                                       Evas_Object*,
                                                       void*),
                                                   void *data)
{
    evas_object_event_callback_add(obj,type,func,data);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenControlWindow::EvasObjectImageFilledAdd
 *          evas object image filled add
 *
 * @param[in]   none
 * @return      Evas_Object
 */
/*--------------------------------------------------------------------------*/
Evas_Object * 
CicoOnScreenControlWindow::EvasObjectImageFilledAdd(void)
{
    return evas_object_image_filled_add(evas);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenControlWindow::EvasObjectDel
 *          delete evas object 
 *
 * @param[in]   object    Evas_Object
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoOnScreenControlWindow::EvasObjectDel(Evas_Object * object)
{
    evas_object_del(object);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenControlWindow::UpdateEvasObject
 *          update evas object
 *
 * @param[in]   img        Evas_Object
 * @param[in]   imgfile    img file
 * @param[in]   partname   part name
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void 
CicoOnScreenControlWindow::UpdateEvasObject(Evas_Object *img,
                                            const char *imgfile,
                                            const char *partname)
{
    int x, y, w, h;
    if (strlen(imgfile) > 0) {
        /* calculation icon pos */
        edje_object_part_geometry_get(edje, partname, &x, &y, &w, &h);
        x += (width - ICO_HS_SIZE_LAYOUT_WIDTH) / 2;
        y += (height - ICO_HS_SIZE_LAYOUT_HEIGHT) / 2;

        evas_object_image_file_set(img, imgfile, NULL);
        evas_object_move(img, x, y);
        evas_object_resize(img, w, h);
        evas_object_show(img);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenControlWindow::EinaListAppend
 *          eina list append
 *
 * @param[in]   img          Evas_Object
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoOnScreenControlWindow::EinaListAppend(Evas_Object *img)
{
    img_list = eina_list_append(img_list, img);
}


