#include "CIcoNotificationService.h"
#include <cassert>
#include <cstring>
#include <iostream>
#include <Ecore.h>
#include <unistd.h>

void changed_cb(void *data, notification_type_e type, 
							  notification_op *op_list, int num_op) {
	notification_list_h notificationlist = NULL;
	notification_list_h getlist = NULL;
	int groupid = 0;
	int privateid = 0;
	char *pkgname = NULL;
	char *title = NULL;
	char *content = NULL;
	notification_op_type_e op_type;
	for (int i = 0; i < num_op; i++) {
		op_type = op_list[i].type;
		switch (op_type) {
		case NOTIFICATION_OP_INSERT :
		case NOTIFICATION_OP_UPDATE :
			notification_get_list(NOTIFICATION_TYPE_NOTI, -1, &notificationlist);
			if (notificationlist) {
				getlist = notification_list_get_head(notificationlist);
				do {
					CIcoNotification noti(notification_list_get_data(getlist));
					if (noti.Empty()) {
						break;
					}
					std::cout << "Receive data[" << op_type << "]:\n";
					std::cout << "Package name: " << noti.GetPkgname() << "\n";
					std::cout << "Title       : " << noti.GetTitle() << "\n";
					std::cout << "Content     : " << noti.GetContent() << std::endl;
					getlist = notification_list_get_next(getlist);
				} while (getlist != NULL);
			}
			if (notificationlist != NULL) {
				notification_free_list(notificationlist);
				notificationlist = NULL;
			}
			break;
		case NOTIFICATION_OP_DELETE :
			std::cout << "Receive data[" << op_type << "]\n";
			break;
		case NOTIFICATION_OP_SERVICE_READY :
			std::cerr << "Start Notification service." << std::endl;
			break;
		default:
			break;
		}
	}
}

int main(int argc, char *argv[]) {
	if (!ecore_init()) {
		std::cerr << "Can't init ECore." << std::endl;
		exit(-1);
	}
	const char *appname = "org.notification.testservice";
	const char *title = "Notification Service";
	const char *content = "Notification Content";
	CIcoNotificationService service;
	assert(service.SetCallback(changed_cb, NULL));
	usleep(50 * 1000);
	CIcoNotification noti;
	assert(noti.Initialize(NOTIFICATION_TYPE_NOTI));
	assert (noti.SetPkgname(appname));
	std::cerr << "Package name is " << noti.GetPkgname() << std::endl;
	assert (noti.SetTitle(title));
	std::cerr << "Title is " << noti.GetTitle() << std::endl;
	assert (noti.SetContent(content));
	std::cerr << "Content is " << noti.GetContent() << std::endl;
	assert(service.Add(noti));
	usleep(100 * 1000);
	const char *content2 = "Update Content";
	assert(noti.SetContent(content));
	assert(service.Update(noti));
	usleep(100 * 1000);
	assert(service.Delete(noti));
	ecore_main_loop_begin();
	//ecore_shutdown();
	assert(service.UnsetCallback(changed_cb));
	return 0;
}
