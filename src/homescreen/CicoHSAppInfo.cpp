/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Application info
 *
 * @date    Aug-08-2013
 */
#include "CicoHSAppInfo.h"


/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppInfo::CicoHSAppInfo
 *          Constractor
 *
 * @param[in]   appid    application id
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoHSAppInfo::CicoHSAppInfo(const char *appid)
{
    for(int ii = 0;ii < ICO_HS_APP_MAX_WINDOW_NUM;ii++){
        window_info_i[ii].valid = false;
    }

    window_num = 0;
    last_surface = 0;
    if(appid == NULL){
        return;
    } 
    strncpy(this->appid,appid,ICO_HS_MAX_PROCESS_NAME);

    life_cycle_controller = CicoSCLifeCycleController::getInstance();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppInfo::~CicoHSAppInfo
 *          Destractor
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoHSAppInfo::~CicoHSAppInfo(void)
{

}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppInfo::GetAppId
 *          get application id
 *
 * @param[in]   none
 * @return      application id
 */
/*--------------------------------------------------------------------------*/
char*
CicoHSAppInfo::GetAppId(void)
{
    return appid;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppInfo::SetWindowInfo
 *          set window information
 *
 * @param[in]   idx              index
 * @param[in]   window info      window information
 * @return      void
 */
/*--------------------------------------------------------------------------*/
void 
CicoHSAppInfo::SetWindowInfo(ico_hs_window_info *hs_window_info,ico_syc_win_info_t *window_info)
{
    hs_window_info->showed = false;
    if(window_info->appid != NULL){
        strncpy(hs_window_info->appid,window_info->appid,
                ICO_HS_MAX_PROCESS_NAME);
    }
    if(window_info->name != NULL){
        strncpy(hs_window_info->name,window_info->name,
                ICO_HS_MAX_WINDOW_NAME);
    }
    hs_window_info->surface = window_info->surface;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppInfo::GetFreeWindowInfoBuffer
 *          get internal window info buffer
 *
 * @param[in]   none
 * @return      buffer
 */
/*--------------------------------------------------------------------------*/
ico_hs_window_info *
CicoHSAppInfo::GetFreeWindowInfoBuffer(void)
{
    ico_hs_window_info *tmp_win_info;
    for(int ii = 0;ii < ICO_HS_APP_MAX_WINDOW_NUM;ii++){
        if(window_info_i[ii].valid == false){
           tmp_win_info = &window_info_i[ii];
           tmp_win_info->valid = true;
           return tmp_win_info;
        }
    }
    return NULL;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppInfo::FreeWindowInfoBuffer
 *          get internal window info buffer
 *
 * @param[in]   name          window name
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSAppInfo::FreeWindowInfo(const char *name)
{
    for(std::vector<ico_hs_window_info*>::iterator it_window_info = 
                 window_info.begin();
        it_window_info != window_info.end();
        it_window_info++){
        if((strncmp(name,(*it_window_info)->name,
            ICO_HS_MAX_WINDOW_NAME) == 0)){
            (*it_window_info)->valid = false;
            window_info.erase(it_window_info);
            --window_num;
            break;
        }
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppInfo::SetWindowAttr
 *          set window attribute
 *
 * @param[in]   idx              index
 * @param[in]   window_attr      window attribute
 * @return      void
 */
/*--------------------------------------------------------------------------*/
void 
CicoHSAppInfo::SetWindowAttr(ico_hs_window_info *hs_window_info,ico_syc_win_attr_t *window_attr)
{
    if(window_attr->appid != NULL){
        strncpy(hs_window_info->appid,window_attr->appid,
                ICO_HS_MAX_PROCESS_NAME);
    }
    if(window_attr->name != NULL){ 
        strncpy(hs_window_info->name,window_attr->name,
                ICO_HS_MAX_WINDOW_NAME);
    }
    hs_window_info->surface = window_attr->surface;
    hs_window_info->nodeid = window_attr->nodeid;
    hs_window_info->layer = window_attr->layer;
    hs_window_info->pos_x = window_attr->pos_x;
    hs_window_info->pos_y = window_attr->pos_y;
    hs_window_info->width = window_attr->width;
    hs_window_info->height = window_attr->height;
    hs_window_info->raise = window_attr->raise;
    hs_window_info->visible = window_attr->visible;
    hs_window_info->active = window_attr->active;

}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppInfo::SetWindowAttr
 *          set window attribute
 *
 * @param[in]   window_attr      window information
 * @return      OK or ERROR
 */
/*--------------------------------------------------------------------------*/
int
CicoHSAppInfo::AddWindowAttr(ico_syc_win_attr_t *window_attr)
{
    for(std::vector<ico_hs_window_info*>::iterator it_window_info = 
                 window_info.begin();
        it_window_info != window_info.end();
        it_window_info++){
        if((strncmp(window_attr->name,(*it_window_info)->name,
            ICO_HS_MAX_WINDOW_NAME) == 0)){
            /*update*/
            SetWindowAttr(*it_window_info,window_attr);
            return ICO_OK;
        }
    }

    ico_hs_window_info *tmp_win_info = GetFreeWindowInfoBuffer();
    if(tmp_win_info == NULL){
        return ICO_ERROR;
    }   
 
    SetWindowAttr(tmp_win_info,window_attr);
    window_info.push_back(tmp_win_info);
    ++window_num;
  
    return ICO_OK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppInfo::SetWindowInfo
 *          set window information
 *
 * @param[in]   window info      window information
 * @return      OK or ERROR
 */
/*--------------------------------------------------------------------------*/
int
CicoHSAppInfo::AddWindowInfo(ico_syc_win_info_t *window_info)
{
    for(std::vector<ico_hs_window_info*>::iterator it_window_info 
                 = this->window_info.begin();
        it_window_info != this->window_info.end();
        it_window_info++){
        if((strncmp(window_info->name,(*it_window_info)->name,
            ICO_HS_MAX_WINDOW_NAME) == 0)){
            /*update*/
            SetWindowInfo(*it_window_info,window_info);
            return ICO_OK;
        }
    }
    
    ico_hs_window_info *tmp_win_info = GetFreeWindowInfoBuffer();
    if(tmp_win_info == NULL){
        return ICO_ERROR;
    }   
    SetWindowInfo(tmp_win_info,window_info);
    this->window_info.push_back(tmp_win_info);
    ++window_num;

    return ICO_OK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppInfo::GetWindowNum
 *          get number of window 
 *
 * @param[in]   none
 * @return      window num
 */
/*--------------------------------------------------------------------------*/
int 
CicoHSAppInfo::GetWindowNum(void)
{
    return window_num;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppInfo::GetWindowInfo
 *          get window information 
 *
 * @param[in]   idx index
 * @return      window information
 */
/*--------------------------------------------------------------------------*/
ico_hs_window_info*
CicoHSAppInfo::GetWindowInfo(const char* name)
{
    for(std::vector<ico_hs_window_info*>::iterator it_window_info 
                = window_info.begin();
        it_window_info != window_info.end();
        it_window_info++){
        if((strncmp(name,(*it_window_info)->name,ICO_HS_MAX_WINDOW_NAME) == 0)){
            return *it_window_info;
        }
    }
    return NULL;  
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppInfo::SetShowed
 *          set showing status;          
 *
 * @param[in]   showed     showing status 
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHSAppInfo::SetShowed(ico_hs_window_info *info,bool showed)
{
    info->showed = showed;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppInfo::GetShowed
 *          get showing status;          
 *
 * @param[in]    none
 * @return      showing status 
 */
/*--------------------------------------------------------------------------*/
bool
CicoHSAppInfo::GetShowed(ico_hs_window_info *info)
{
    return info->showed;
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppInfo::Execute
 *          execute application 
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
int
CicoHSAppInfo::Execute(void)
{
    /*call execute api*/
    int ret = life_cycle_controller->launch(appid);    
    if(ret < 0){
        return ret;
    }
  
    return ret;
}

/*--------------------------------------------------------------------------*/
/*
 * @brief   CicoHSAppInfo::Terminate
 *          terminate application 
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
int
CicoHSAppInfo::Terminate(void)
{
    /*call Terminate api*/
    int ret = life_cycle_controller->terminate(appid);    
    if(ret < 0){
        return ret;
    }

   return ret;
}


/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHSAppInfo::GetStatus
 *          get status 
 *
 * @param[in]   none
 * @return      status
 */
/*--------------------------------------------------------------------------*/
bool 
CicoHSAppInfo::GetStatus(void)
{
    return life_cycle_controller->isRunning(appid);
}


void 
CicoHSAppInfo::SetLastSurface(int last_surface)
{
    this->last_surface = last_surface;
}

int 
CicoHSAppInfo::GetLastSurface(void)
{
    return last_surface;
}
