/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   onscreen application
 *
 * @date    Feb-15-2013
 */
#include "CicoOnScreenAppList.h"

/*============================================================================*/
/* Declare static values                                                      */
/*============================================================================*/
CicoOnScreenControlWindow *CicoOnScreenAppList::control_window;
int CicoOnScreenAppList::applist_idx; 
int CicoOnScreenAppList::app_cnt; 

/*============================================================================*/
/* Functions Declaration (CicoOnScreenAppList)                          */
/*============================================================================*/

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenAppList::Initialize
 *          initialize applist
 *
 * @param[in]   controlWindow    instance of CicoOnScreenControlWindow
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void 
CicoOnScreenAppList::Initialize(CicoOnScreenControlWindow *control_window_tmp)
{
    control_window = NULL;
    applist_idx = 0;
    app_cnt = 0;

    control_window = control_window_tmp;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenAppList::SetConfigCallback
 *          set callback for config
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void 
CicoOnScreenAppList::SetConfigCallback(void)
{
    ico_uxf_econf_setAppUpdateCb(ConfigEvent);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenAppList::GetAppIndex
 *          return a application index that to be set indicated index.
 *
 * @param[in]   filepath            file path
 * @return      index or ERROR
 */
/*--------------------------------------------------------------------------*/
int 
CicoOnScreenAppList::GetAppIndex(int idx)
{
    int ii;
    int appidx = idx + applist_idx * ICO_ONS_APPLI_NUM + 1;
    int cnt = 0;
    Ico_Uxf_App_Config *appconf = const_cast<Ico_Uxf_App_Config *>(ico_uxf_getAppConfig());

    ICO_DBG("CicoOnScreenAppList::GetAppIndex: idx=%d appidx=%d appcnt=%d",
               idx, appidx, app_cnt);

    for (ii = 0; ii < appconf->applicationNum; ii++) {
        if ((! appconf->application[ii].noicon) &&
            (strcmp(appconf->application[ii].type, ICO_HS_GROUP_SPECIAL) != 0)) {
            ++cnt;
        }
        if (cnt == appidx) {
            return ii;
        }
    }

    return -1;
}
                                                                              
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenAppList::LoadConfig
 *          load/reload configuration of Application.
 *          and except special group applications and no icon applications from showing.
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void 
CicoOnScreenAppList::LoadConfig(void)  
{
    Ico_Uxf_App_Config *appconf;
    int appcnt;
    int appidx, idx, cnt;

    ICO_DBG("CicoOnScreenAppList::LoadConfig: Enter");

    appconf = const_cast<Ico_Uxf_App_Config *>(ico_uxf_getAppConfig());
    appcnt = appconf->applicationNum;
    for (appidx = 0; appidx < appconf->applicationNum; appidx++) {
        if ((appconf->application[appidx].noicon) ||
            (strcmp(appconf->application[appidx].type, ICO_HS_GROUP_SPECIAL) == 0)) {
            appcnt--;
            ICO_DBG("CicoOnScreenAppList::LoadConfig: No Need appid=%s noicon=%d type=%s",
                       appconf->application[appidx].appid,
                       appconf->application[appidx].noicon,
                       appconf->application[appidx].type);
        }
    }
    cnt = 0;
    for (idx = 0; idx < appcnt; idx++) {
        appidx = GetAppIndex(idx);
        if (appidx > 0) {
            ICO_DBG("CicoOnScreenAppList::LoadConfig: appid=%s seat=%d idx=%d seatcnt=%d",
                       appconf->application[appidx].appid, cnt
                               / ICO_ONS_APPLI_NUM, idx - ICO_ONS_APPLI_NUM
                               * (cnt / ICO_ONS_APPLI_NUM), ((appcnt - 1)
                               / ICO_ONS_APPLI_NUM) + 1);
            ++cnt;
        }
    }

    app_cnt = appcnt;
    applist_idx = 0;
    ICO_DBG("CicoOnScreenAppList::LoadConfig: Leave(appcnt=%d)", appcnt);

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenAppList::ConfigEvent
 *          This is a callback function called when configurations
 *          were updated.
 *
 * @param[in]   appid               application id
 * @param[in]   type                event type(install/uninstall)
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void 
CicoOnScreenAppList::ConfigEvent(const char *appid, int type)
{
    ICO_DBG("CicoOnScreenAppList::LoadConfig: Enter(appid=%s, type=%d)", 
               appid, type);

    LoadConfig();

    ICO_DBG("CicoOnScreenAppList::LoadConfig: Leave");

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenAppList::SetAppIcons
 *          set App Icon on rect
 *
 * @param[in]   part               part
 * @param[in]   partname           part name
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void 
CicoOnScreenAppList::SetAppIcons(Evas_Object* part,const char *partname)
{
    int idx; /* rect index */
    int appidx; /* appli index */
    char imgfile[ICO_ONS_BUF_SIZE];
    Ico_Uxf_App_Config *appconf;
    Evas_Object *img = NULL;

    memset(imgfile, 0, sizeof(imgfile));
    /* check name, if part is rect/next_bt/cancel_bt */
    if (strncmp(partname, ICO_HS_ONS_PART_RECT_NAME,
                sizeof(ICO_HS_ONS_PART_RECT_NAME) - 1) == 0) {
        img = control_window->EvasObjectImageFilledAdd();
        /* get rect index from partname(rect_01, rect_02, ...) */
        sscanf(partname, ICO_HS_ONS_PART_RECT_NAME"%d", &idx);
        idx -= 1;

        appconf = const_cast<Ico_Uxf_App_Config *>(ico_uxf_getAppConfig());

        appidx = GetAppIndex(idx);
        ICO_DBG("CicoOnScreenAppList::SetAppIcons: idx=%d appidx=%d",
                   idx, appidx);
        if ((appidx < 0) || (appidx > appconf->applicationNum)) {
	    control_window->EvasObjectDel(img);
            return;
        }

        /* set icon file name */
        snprintf(imgfile, ICO_ONS_BUF_SIZE, "%s",
                 appconf->application[appidx].icon_key_name);
        ICO_DBG("CicoOnScreenAppList::SetAppIcons: set image = %s(%d/%d), app=%s, group=%s",
                   imgfile, appidx, app_cnt, appconf->application[appidx].appid,
                   appconf->application[appidx].group);
        /* set mouse call back function */
        control_window->EvasObjectEventCallBack(img, EVAS_CALLBACK_MOUSE_UP,
                                       CicoOnScreenAction::TouchUpEdje,
                                       appconf->application[appidx].appid);
    }
    else if (strcmp(partname, ICO_HS_ONS_PART_NEXT_NAME) == 0) {
        control_window->EvasObjectEventCallBack(part, EVAS_CALLBACK_MOUSE_UP,
                                       CicoOnScreenAction::TouchUpNext, 
                                       static_cast<void*>(const_cast<char*>(partname)));
    }
    else if (strcmp(partname, ICO_HS_ONS_PART_CANCEL_NAME) == 0) {
        control_window->EvasObjectEventCallBack(part, EVAS_CALLBACK_MOUSE_UP,
                                       CicoOnScreenAction::TouchUpEdje, 
                                       static_cast<void*>(const_cast<char*>(partname)));
    }
    else {
        return;
    }

    control_window->UpdateEvasObject(img,imgfile,partname);
    
    
    control_window->EinaListAppend(img);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenAppList::GetFname(
 *          get filename from the full path
 *
 * @param[in]   filepath            file path
 * @return      filename
 */
/*--------------------------------------------------------------------------*/
const char *
CicoOnScreenAppList::GetFname(const char *filepath)
{
    int ii;
    const char *name = filepath;

    for (ii = 0; ii < ICO_ONS_BUF_SIZE - 1; ii++) {
        if (filepath[ii] == 0)
            break;
        if (filepath[ii] == '/')
            name = &filepath[ii + 1];
    }

    return name;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenAppList::LoadIconsEdjeFile
 *          loadIcons
 *
 * @param[in]   evas               evas
 * @param[in]   edje               edje
 * @param[in]   part               part
 * @param[in]   partname           part name
 * @return      none
 */
/*--------------------------------------------------------------------------*/
int
CicoOnScreenAppList::LoadIconsEdjeFile(const char *edje_file)
{
    int ret;
    Evas_Object *part; /* part handle */
    Eina_List *group; /* edje group list */
    Eina_List *list; /* part list in edje */
    int group_count = 0; /* group counter */
    int name_count = 0; /* name counter */
  
    /* init evas (if it doesn't yet) */
    ret = control_window->InitEvas();
    if(ret < 0){
        return -1;
    }

    /* delete pre image */
    control_window->DeletePreImage();
 
    /* create and show edje*/
    control_window->CreateAndShowEdje();

    /* get group list */
    group = control_window->GetEdjeFileCollectionList(edje_file);
    while (group != NULL) {
        ret = control_window->SetEdjeFile(group,edje_file);
 
        ICO_DBG("CicoOnScreenAppList::LoadIconsEdjeFile: group[%d] data : %s",
                   group_count,static_cast<const char *>(group->data));
        /* get list */
        list = control_window->GetEdjeObjectAccessPartList();
        while (list != NULL) {
            ICO_DBG("CicoOnScreenAppList::LoadIconsEdjeFile: list[%d] data : %s",
                       name_count,static_cast<const char *>(list->data));

            /* set callback for part name */
            part = control_window->GetEdjeObjectPartObject((const char *)list->data);
            if (part != NULL) {
                ICO_DBG("CicoOnScreenAppList::LoadIconsEdjeFile: list[%d] name : %s",
                           name_count,static_cast<const char *>(list->data));

                /* if not applist */
                if (strncmp(GetFname(edje_file), ICO_HS_ONS_APPLI_LIST_NAME,
                            sizeof(ICO_HS_ONS_APPLI_LIST_NAME) - 1) != 0) {
                    control_window->EvasObjectEventCallBack(part,
                                                   EVAS_CALLBACK_MOUSE_UP,
                                                   CicoOnScreenAction::TouchUpEdje,
                                                   list->data);
                }
                /* if applist */
                else {
                    SetAppIcons(part, static_cast<const char *>(list->data));
                }
            }
            else {
                ICO_DBG("CicoOnScreenAppList::LoadIconsEdjeFile: list[%d] is NULL",
                           name_count);
            }
            /* to next list */
            list = list->next;
            ++name_count;
        }
        /* to next group */
        group = group->next;
        ++group_count;
    }
    ICO_DBG("CicoOnScreenAppList::LoadIconsEdjeFile: group num is %d", group_count);
    ICO_DBG("CicoOnScreenAppList::LoadIconsEdjeFile: name num is %d", name_count);

    return 0;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoOnScreenAppList::NextList
 *          change to NextList
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void CicoOnScreenAppList::NextList(void){
    int listcnt;
 
    if (app_cnt > 0) {
        listcnt = ((app_cnt - 1) / ICO_ONS_APPLI_NUM) + 1;
    }
    else {
        listcnt = 1;
    }
    applist_idx += 1;
    if (applist_idx >= listcnt) {
        applist_idx = 0;
    }
}


