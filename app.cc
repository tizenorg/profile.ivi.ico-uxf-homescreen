#include "CIcoNotificationApp.h"
#include <cassert>
#include <cstring>
#include <iostream>
#include <unistd.h>

int main(int argc, char *argv[]) {
	const char *appname = "org.notification.testapp";
	const char *title = "Notification App";
	const char *content = "Notification Content";
	CIcoNotificationApp appnoti;
	CIcoNotification noti;
	assert (noti.Initialize(NOTIFICATION_TYPE_NOTI));
	assert (noti.SetPkgname(appname));
	std::cerr << "Package name is " << noti.GetPkgname() << std::endl;
	assert (noti.SetTitle(title));
	std::cerr << "Title is " << noti.GetTitle() << std::endl;
	assert (noti.SetContent(content));
	std::cerr << "Content is " << noti.GetContent() << std::endl;
	assert (appnoti.Add(noti));
	usleep(200 * 1000);
	const char *content2 = "Update Content.";
	assert (noti.SetContent(content2));
	assert (appnoti.Update(noti));
	sleep(1);
	assert (appnoti.Delete(noti));
	assert (!noti.Empty());
	//assert (noti.Terminate());
	return 0;
}
