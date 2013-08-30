/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   sound an operation of homescreen
 *
 * @date    Apr-04-2013
 */

#include "CicoHomeScreenSound.h"

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreenSound::ReadSoundConfiguration
 *          read configuration and set
 *
 * @param[in]   homescreen_config   configuration instance
 * @param[in]   type                operation type
 * @param[in]   conf_name           configuration name
 * @return      result 
 * @retval      >=0                 success
 * @retval      NULL                err
 */
/*--------------------------------------------------------------------------*/
CicoHomeScreenSoundConfig *
CicoHomeScreenSound::ReadSoundConfiguration(CicoHomeScreenConfig *homescreen_config,
                                            int type, char *conf_name)
{
    char *file;
    CicoHomeScreenSoundConfig *sound_config;

    sound_config = new CicoHomeScreenSoundConfig();
    if (sound_config == NULL) {
        ICO_WRN("CicoHomeScreenSound::ReadSoundConfiguration: Leave(fail to alloc memory");
        return NULL;
    }

    if (strcmp(conf_name, ICO_HS_SND_CNAME_DEFAULT) == 0) {
        file = (char *)homescreen_config->ConfigGetString(ICO_HS_CONFIG_SOUND,
                                          conf_name,ICO_HS_SND_FNAME_DEFAULT);
    }
    else {
        file = (char *)homescreen_config->ConfigGetString(ICO_HS_CONFIG_HOMESCREEN,
                                          conf_name,NULL);
    }

    if (!file) {
        ICO_ERR("CicoHomeScreenSound::ReadSoundConfiguration: Leave(given config is not exist");
        delete sound_config;
        return NULL;
    }

    sound_config->SetFile(file);
    sound_config->SetType(type);
    sound_config->SetNext(sound_files);

    sound_files = sound_config;

    return sound_config;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreenSound::CicoHomeScreenSound
 *          constractor
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
CicoHomeScreenSound::CicoHomeScreenSound(void){
    snd_is_initialize = false;
    sound_files = NULL;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   Initialize
 *          initialize to sound a operation
 *
 * @param[in]   homescreen_config   configuration instance
 * @return      result
 * @retval      ICO_HS_OK           success
 * @retval      ICO_HS_ERR          err
 */
/*--------------------------------------------------------------------------*/
int
CicoHomeScreenSound::Initialize(CicoHomeScreenConfig *homescreen_config)
{
    if (snd_is_initialize != true) {
        /* read sound file name from configuration */
        ReadSoundConfiguration(homescreen_config,ICO_HS_SND_TYPE_DEFAULT,
                               const_cast<char*>(ICO_HS_SND_CNAME_DEFAULT));

        CicoHomeScreenResourceConfig::GetSoundPath(soundpath, sizeof(soundpath));
        snd_is_initialize = true;
    }

    return ICO_HS_OK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreenSound::PlaySound
 *          play the given sound file(.wav).
 *
 * @param[in]   wav_file            wav formated file.
 * @return      result
 * @retval      ICO_HS_OK           success
 * @retval      ICO_HS_ERR          err
 */
/*--------------------------------------------------------------------------*/
int
CicoHomeScreenSound::PlaySound(char *wav_file)
{
    int pid;
    char command[ICO_HS_TEMP_BUF_SIZE];
 
    if (snd_is_initialize != true) {
        return ICO_HS_ERR;
    }

    if (!wav_file) {
        ICO_WRN("CicoHomeScreenSound::PlaySound: Leave(error file name is NULL");
        return ICO_HS_ERR;
    }

    if (wav_file[0] == '/') {
        /* given by full file path */
        snprintf(command, sizeof(command), 
                "paplay --stream-name=HOMESCREEN_BEEP %s", wav_file);
    }
    else {
        snprintf(command, sizeof(command), 
                "paplay --stream-name=HOMESCREEN_BEEP %s/%s", soundpath, wav_file);
    }

    ICO_DBG("CicoHomeScreenSound::PlaySound: system(%s)", command);
    pid = fork();
    if (!pid) {
        system(command);
        exit(0);
    }

    return ICO_HS_OK;
}


/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_snd_get_filename
 *          get the file name by given operation type
 *
 * @param[in]   type                operation type
 * @return      pointer to the file name 
 * @retval      >=0                 success
 * @retval      NULL                err
 */
/*--------------------------------------------------------------------------*/
char *
CicoHomeScreenSound::GetSoundFileName(int type)
{
    CicoHomeScreenSoundConfig *sound_config;

    if (snd_is_initialize != true) {
        return NULL;
    }

    sound_config = sound_files;
    while (sound_config != NULL) {
        if (sound_config->GetType() == type) {
            return sound_config->GetFile();
        }
        sound_config = sound_config->GetNext();
    }

    return NULL;
}

