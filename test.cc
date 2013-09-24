#include "CIcoNotification.h"
#include <cassert>
#include <cstring>
#include <iostream>

int main(int argc, char *argv[]) {
	CIcoNotification noti;
	const char *title = "Test Notification Title.";
	const char *content = "Test Notification Content.";
	assert (noti.Initialize(NOTIFICATION_TYPE_NOTI));
	assert (noti.SetTitle(title));
	assert (strcmp(noti.GetTitle(), title) == 0);
	assert (noti.SetContent(content));
	assert (strcmp(noti.GetContent(), content) == 0);
	assert (noti.Terminate());
	assert (!noti.SetTitle(title));
	assert (!noti.GetTitle());
	assert (!noti.SetContent(content));
	assert (!noti.GetContent());
	return 0;
}
