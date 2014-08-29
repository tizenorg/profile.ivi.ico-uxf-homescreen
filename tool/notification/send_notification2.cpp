#include <cstdio>
#include <iostream>
#include <string>
#include <functional>
#include <algorithm>
#include <appsvc/appsvc.h>
#include "notification.h"
using namespace std;

const char *
errorToString(notification_error_e error)
{
    const char *strerr = NULL;

    switch (error) {
    case NOTIFICATION_ERROR_NONE:
        strerr = "NOTIFICATION_ERROR_NONE";
        break;
    case NOTIFICATION_ERROR_INVALID_DATA:
        strerr = "NOTIFICATION_ERROR_INVALID_DATA";
        break;
    case NOTIFICATION_ERROR_NO_MEMORY:
        strerr = "NOTIFICATION_ERROR_NO_MEMORY";
        break;
    case NOTIFICATION_ERROR_FROM_DB:
        strerr = "NOTIFICATION_ERROR_FROM_DB";
        break;
    case NOTIFICATION_ERROR_ALREADY_EXIST_ID:
        strerr = "NOTIFICATION_ERROR_ALREADY_EXIST_ID";
        break;
    case NOTIFICATION_ERROR_FROM_DBUS:
        strerr = "NOTIFICATION_ERROR_FROM_DBUS";
        break;
    case NOTIFICATION_ERROR_NOT_EXIST_ID:
        strerr = "NOTIFICATION_ERROR_NOT_EXIST_ID";
        break;
    case NOTIFICATION_ERROR_IO:
        strerr = "NOTIFICATION_ERROR_IO";
        break;
    case NOTIFICATION_ERROR_SERVICE_NOT_READY:
        strerr = "NOTIFICATION_ERROR_SERVICE_NOT_READY";
        break;
    case NOTIFICATION_ERROR_PERMISSION_DENIED:
        strerr = "NOTIFICATION_ERROR_PERMISSION_DENIED";
        break;
    default:
        strerr = "UNKOWN";
        break;
    }

    return strerr;
}

int
insert_notification(notification_type_e type,
                    int group_id,
                    int priv_id,
                    const char *pkgname,
                    const char *title,
                    const char *content,
                    const char *icon)
{
    notification_h noti = notification_new(type,
                                           group_id,
                                           priv_id);
    if (NULL == noti) {
        cout << "notification_new() failed." << endl;
        return -1;
    }

    //set Pkgname
    if (NULL == pkgname) {
        return -1;
    }

    notification_error_e noti_err = NOTIFICATION_ERROR_NONE;

    noti_err = notification_set_pkgname(noti, pkgname);
    if (noti_err != NOTIFICATION_ERROR_NONE) {
        printf("notification_set_pkgname failed(%s).\n",
               errorToString(noti_err));
        return -1;
    }
     
    //set Title 
    if (NULL == title) {
        return -1;
    }
    noti_err = notification_set_text(noti,
                                     NOTIFICATION_TEXT_TYPE_TITLE,
                                     title,
                                     NULL,
                                     NOTIFICATION_VARIABLE_TYPE_NONE);
    if (noti_err != NOTIFICATION_ERROR_NONE) {
        cout << "Failed to set notification title <error code>:"<< noti_err<<endl;
        return -1;
    }
    
    //set Content
    if (NULL == content) {
        return -1;
    }
    noti_err = notification_set_text(noti,
                                     NOTIFICATION_TEXT_TYPE_CONTENT,
                                     content,
                                     NULL,
                                     NOTIFICATION_VARIABLE_TYPE_NONE);
    if (noti_err != NOTIFICATION_ERROR_NONE) {
        cout << "Failed to set notification content <error code>:"<< noti_err<<endl;
        return -1;
    }
    
    //set Icon_path 
    if (NULL == icon) {
        return -1;
    }
    noti_err = notification_set_icon(noti, icon);
    if (noti_err != NOTIFICATION_ERROR_NONE) {
        cout <<"Failed to set icon_path <error code>:"<< noti_err <<endl;
            return -1;
    }

    bundle *b = NULL;
    b = bundle_create();
    appsvc_set_pkgname(b, "org.tizen.dialer");
    noti_err = notification_set_execute_option(noti, NOTIFICATION_EXECUTE_TYPE_SINGLE_LAUNCH, "Launch", NULL, b);
    if (noti_err != NOTIFICATION_ERROR_NONE) {
        printf("Failed to set notification execute option: %d", noti_err);
        return -1;
    }


    //insert to DB and appear on the notification_area
    //noti_err = notification_update(noti);
    int private_id = 0;
    noti_err = notification_insert(noti, &private_id);
    if (noti_err != NOTIFICATION_ERROR_NONE) {
        printf("notification_insert failed(%s).\n",
               errorToString(noti_err));
        return -1;
    } 

    //free to notification
    noti_err = notification_free(noti);
    if (noti_err != NOTIFICATION_ERROR_NONE) {
        cout <<"Failed to free notification <error code>:"<< noti_err <<endl;
        return -1;
    } 
 
    return 0;
} 

int
main(int argc, char **argv)
{
    const char* msgTyp = "TYPE_NOTI";
    notification_type_e nType = NOTIFICATION_TYPE_NOTI;
    
    if (argc == 2) {
        string sargv1(argv[1]);
        if (0 == sargv1.compare("--ongoing")) {
            msgTyp = "TYPE_ONGOING";
            nType = NOTIFICATION_TYPE_ONGOING;
        }
    }

    cout << "Notification TestProgram Start("<<msgTyp<<")=========" <<endl;
    
    insert_notification(nType,
                        NOTIFICATION_GROUP_ID_DEFAULT,
                        NOTIFICATION_PRIV_ID_NONE,
                        "org.tizen.dialer",
                        "Incoming call",
                        "888-8888",
                        "/usr/share/icons/default/small/org.tizen.dialer.png");

    cout << "Notification_TestProgram End!=========" <<endl;
    return 0;
}
