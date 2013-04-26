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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "ico_uxf.h"
#include "ico_uxf_conf_def.h"
#include "home_screen.h"
#include "home_screen_res.h"
#include "home_screen_conf.h"

/*============================================================================*/
/* definition                                                                 */
/*============================================================================*/
typedef struct _hs_snd_conf hs_snd_conf_t;
struct _hs_snd_conf {
    hs_snd_conf_t *next;
    int type;                               /* operation type                 */
    char *file;                             /* sound file name                */
};

/*============================================================================*/
/* static(internal) functions prototype                                       */
/*============================================================================*/
static hs_snd_conf_t *hs_snd_read_configuration(int type, char *conf_name);

/*============================================================================*/
/* variabe                                                                    */
/*============================================================================*/
static int hs_snd_is_initialize = false;
static hs_snd_conf_t *hs_sound_files = NULL;

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_snd_play
 *          play the given sound file(.wav).
 *
 * @param[in]   wav_file            wav formated file.
 * @return      result
 * @retval      ICO_HS_OK           success
 * @retval      ICO_HS_ERR          err
 */
/*--------------------------------------------------------------------------*/
int
hs_snd_play(char *wav_file)
{
    int pid;
    int ret;
    char command[ICO_HS_TEMP_BUF_SIZE];
    char path[ICO_HS_TEMP_BUF_SIZE];

    if (hs_snd_is_initialize != true) {
        ret = hs_snd_init();
        if (ret != ICO_HS_OK) {
            return ICO_HS_ERR;
        }
    }

    if (!wav_file) {
        uifw_warn("hs_snd_play: Leave(error file name is NULL");
        return ICO_HS_ERR;
    }

    if (wav_file[0] == '/') {
        /* given by full file path */
        snprintf(command, sizeof(command), 
                "paplay --stream-name=HOMESCREEN_BEEP %s", wav_file);
    }
    else {
        hs_get_sound_path(path, sizeof(path));
        snprintf(command, sizeof(command), 
                "paplay --stream-name=HOMESCREEN_BEEP %s/%s", path, wav_file);
    }

    uifw_trace("hs_snd_play: system(%s)", command);
    pid = fork();
    if (!pid) {
        system(command);
        exit(0);
    }

    return ICO_HS_OK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_snd_init
 *          initialize to sound a operation
 *
 * @param       none
 * @return      result
 * @retval      ICO_HS_OK           success
 * @retval      ICO_HS_ERR          err
 */
/*--------------------------------------------------------------------------*/
int
hs_snd_init(void)
{
    if (hs_snd_is_initialize != true) {
        /* read sound file name from configuration */
        hs_snd_read_configuration(ICO_HS_SND_TYPE_DEFAULT,
                                  ICO_HS_SND_CNAME_DEFAULT);
        hs_snd_is_initialize = true;
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
hs_snd_get_filename(int type)
{
    hs_snd_conf_t *conf;

    if (hs_snd_is_initialize != true) {
        hs_snd_init();
    }

    conf = hs_sound_files;
    while (conf) {
        if (conf->type == type) {
            return conf->file;
        }
        conf = conf->next;
    }

    return NULL;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_snd_read_configuration
 *          read configuration and set
 *
 * @param[in]   type                operation type
 * @param[in]   conf_name           configuration name
 * @return      result 
 * @retval      >=0                 success
 * @retval      NULL                err
 */
/*--------------------------------------------------------------------------*/
static hs_snd_conf_t *
hs_snd_read_configuration(int type, char *conf_name)
{
    char *file;
    hs_snd_conf_t *conf;

    conf = calloc(sizeof(hs_snd_conf_t), 1);
    if (!conf) {
        uifw_warn("hs_snd_read_configuration: Leave(fail to alloc memory");
        return NULL;
    }

    if (strcmp(conf_name, ICO_HS_SND_CNAME_DEFAULT) == 0) {
        file = (char *)hs_conf_get_string(ICO_HS_CONFIG_SOUND,
                                          conf_name,
                                          ICO_HS_SND_FNAME_DEFAULT);
    }
    else {
        file = (char *)hs_conf_get_string(ICO_HS_CONFIG_HOMESCREEN,
                                          conf_name,
                                          NULL);
    }

    if (!file) {
        uifw_trace("hs_snd_read_configuration: Leave(given config is not exist");
        free(conf);
        return NULL;
    }

    conf->file = file;
    conf->type = type;

    conf->next = hs_sound_files;
    hs_sound_files = conf;

    return conf;
}
