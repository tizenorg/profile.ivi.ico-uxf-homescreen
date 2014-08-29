#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
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
            err = notification_delete_all_by_type("", NOTIFICATION_TYPE_ONGOING);
            if (NOTIFICATION_ERROR_NONE != err) {
                printf("notification_delete_group_by_priv_id failed(%d).\n", err);
                return -1;
            }
            return 0;
        }
    }

    if (argc < 3) {
        printf("%s [--ongoing] pkgname priv_id\n", argv[0]);
        return -1;
    }
    int n_geta = 1;
    notification_type_e nType = NOTIFICATION_TYPE_NOTI;
    string sargv1(argv[1]);
    if (0 == sargv1.compare("--ongoing")) {
        nType = NOTIFICATION_TYPE_ONGOING;
        n_geta = 2;
        if (argc < 4) {
            printf("%s --ongoing pkgname priv_id\n", argv[0]);
            return -1;
        }
    }

    char *pkgname = argv[n_geta];
    int  priv_id  = atoi(argv[n_geta+1]);
    err = notification_delete_by_priv_id(pkgname, nType, priv_id);
    if (err != NOTIFICATION_ERROR_NONE) {
        printf("notification_delete_by_priv_id failed(%d).\n", err);
        return -1;
    }

    return 0;
}
