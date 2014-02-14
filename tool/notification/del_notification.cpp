#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "notification.h"
using namespace std;

int
main(int argc, char **argv)
{
    notification_error_e err = NOTIFICATION_ERROR_NONE;

    if (argc == 2) {
        if (0 == strcmp("--all", argv[1])) {
            printf("delete all notifications\n");
            err = notification_delete_all_by_type("", NOTIFICATION_TYPE_NOTI);
            if (NOTIFICATION_ERROR_NONE != err) {
                printf("notification_delete_group_by_priv_id failed(%d).\n", err);
                return -1;
            }
            return 0;
        }
    }

    if (argc < 3) {
        printf("%s pkgname priv_id\n", argv[0]);
        return -1;
    }

    char *pkgname = argv[1];
    int  priv_id  = atoi(argv[2]);
    err = notification_delete_by_priv_id(pkgname, 
                                         NOTIFICATION_TYPE_NONE,
                                         priv_id);
    if (err != NOTIFICATION_ERROR_NONE) {
        printf("notification_delete_by_priv_id failed(%d).\n", err);
        return -1;
    }

    return 0;
}
