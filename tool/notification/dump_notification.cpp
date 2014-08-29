#include <stdio.h>
#include <iostream>
#include "notification.h"
using namespace std;

int
main()
{
    cout << "Notification TestProgram Start=========" << endl;
    
    //notification variable declaration
    notification_error_e err = NOTIFICATION_ERROR_NONE;
    notification_list_h noti_list = NULL;

    err = notification_get_list(NOTIFICATION_TYPE_NONE, -1, &noti_list);
    if (err != NOTIFICATION_ERROR_NONE) {
        cout << "Failed to get notificationList <error code>:"<< err << endl;
        return -1;
    }

    if (NULL == noti_list){ 
        cout <<"Nothing NotificationList" <<endl;
        return -1;  
    }

    printf("gid | typ | pid | %-30s | %-30s | content\n", "pkgname", "title");
    printf("----+-----+-----+--------------------------------+--------------------------------+--------------------------------\n");
    //notification_list_h getlist = notification_list_get_head(noti_list);
    notification_list_h getlist = notification_list_get_tail(noti_list);
    do {
        notification_h noti = notification_list_get_data(getlist);


        notification_type_e type = NOTIFICATION_TYPE_NONE;
        err = notification_get_type(noti, &type);
        if (NOTIFICATION_ERROR_NONE != err) {
            printf("notification_set_text() failed(%d).\n", err);
            return -1;
        }

        int grpid = 0;
        int privid = 0;
        err = notification_get_id(noti, &grpid, &privid);
        if (NOTIFICATION_ERROR_NONE != err) {
            printf("notification_set_text() failed(%d).\n", err);
            return -1;
        }

        char *caller_pkgname = NULL;
        err = notification_get_pkgname(noti, &caller_pkgname);
        if (NOTIFICATION_ERROR_NONE != err) {
            printf("notification_get_pkgname() failed(%d).\n", err);
            return -1;
        }

        char *title = NULL;
        err = notification_get_text(noti,
                                    NOTIFICATION_TEXT_TYPE_TITLE,
                                    &title);
        if (NOTIFICATION_ERROR_NONE != err) {
            printf("notification_set_text() failed(%d).\n", err);
            return -1;
        }

        char *content = NULL;
        err = notification_get_text(noti,
                                    NOTIFICATION_TEXT_TYPE_CONTENT,
                                    &content);
        if (NOTIFICATION_ERROR_NONE != err) {
            printf("notification_get_text() failed(%d).\n", err);
            return -1;
        }

        printf("%3d | %3d | %3d | %-30s | %-30s | %-30s\n",
               grpid, type, privid, caller_pkgname, title, content);

        //getlist = notification_list_get_next(getlist);
        getlist = notification_list_get_prev(getlist);
    } while (getlist != NULL);


    cout << "Notification_TestProgram End!=========" << endl;
    return 0;
}
