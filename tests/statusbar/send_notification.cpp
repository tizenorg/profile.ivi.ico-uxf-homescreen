#include <iostream>
#include "notification.h"
using namespace std;

int main(){
    cout << "Notification TestProgram Start=========" <<endl;

    // notification variable declaration
    notification_h noti = NULL;
    notification_type_e type_ = NOTIFICATION_TYPE_NONE;
    notification_error_e noti_err = NOTIFICATION_ERROR_NONE;

    // Initialize
    notification_get_type(noti, &type_);
    noti = notification_new(NOTIFICATION_TYPE_NOTI,
                                NOTIFICATION_GROUP_ID_NONE,
                                NOTIFICATION_PRIV_ID_NONE);
    if (NULL == noti) {
        cout << "notification_new() failed" << endl;
        return -1;
    }

#if 0
    notification_list_h noti_list = NULL;
    notification_get_list(NOTIFICATION_TYPE_NOTI, -1, &noti_list);
    if (noti_err != NOTIFICATION_ERROR_NONE) {
            cout << "Failed to get notificationList <error code>:"<< noti_err<<endl;
            return -1;
         }
    if(!noti_list){ 
        cout <<"Nothing NotificationList" <<endl;
        return -1;  
    }
#endif

    //set Pkgname
    noti_err = notification_set_pkgname(noti,"org.tizen.dialer");
    if (noti_err != NOTIFICATION_ERROR_NONE) {
        cout << "Failed to set Pkgname <error code>:"<< noti_err<<endl;
        return -1;
    }

    //set Title 
    noti_err = notification_set_text(noti, NOTIFICATION_TEXT_TYPE_TITLE,
                                             "TESTaaaaaaaaaaaaaaa",
                                             NULL,
                                             NOTIFICATION_VARIABLE_TYPE_NONE);
    if (noti_err != NOTIFICATION_ERROR_NONE) {
            cout << "Failed to set notification title <error code>:"<< noti_err<<endl;
            return -1;
    }

    //set Content
    noti_err = notification_set_text(noti, NOTIFICATION_TEXT_TYPE_CONTENT,
                                             "ContentAAAAAAAAAAAAAAA",
                                             NULL,
                                             NOTIFICATION_VARIABLE_TYPE_NONE);
    if (noti_err != NOTIFICATION_ERROR_NONE) {
        cout << "Failed to set notification content <error code>:"<< noti_err<<endl;
        return -1;
    }


    //set Icon_path 
    noti_err = notification_set_icon(noti, "/opt/usr/apps/lYjFlj49Q4/res/wgt/saythis.png");
    if (noti_err != NOTIFICATION_ERROR_NONE) {
        cout <<"Failed to set icon_path <error code>:"<< noti_err <<endl;
        return -1;
    }

    //insert to DB and appear on the notification_area
    //noti_err = notification_update(noti);
    noti_err = notification_insert(noti,NULL);
    if (noti_err != NOTIFICATION_ERROR_NONE) {
        if(noti_err == NOTIFICATION_ERROR_INVALID_DATA){
            cout << "Invalid parameter"<<endl;
        }

        cout <<"Failed to insert notification <error code>:"<< noti_err <<endl;
        cout <<"please start notification.service" <<endl;
        return -1;
    }

    //free to notification
    noti_err = notification_free(noti);
    if (noti_err != NOTIFICATION_ERROR_NONE) {
        cout <<"Failed to free notification <error code>:"<< noti_err <<endl;
            return -1;
    }

    cout << "Notification_TestProgram End!=========" <<endl;
    return 0;

}

