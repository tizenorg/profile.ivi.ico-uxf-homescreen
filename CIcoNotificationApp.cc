#include <cassert>
#include "CIcoNotificationApp.h"

CIcoNotificationApp::CIcoNotificationApp() : privateid_(0) {
}

CIcoNotificationApp::~CIcoNotificationApp() {
}

bool CIcoNotificationApp::Add(CIcoNotification noti) {
	if (noti.Empty()) {
		return false;
	}
	int retid;
	notification_error_e notification_error_;
	notification_error_ = notification_insert(noti.Unpack(), &retid);
	privateid_ = retid;
	return (notification_error_ == NOTIFICATION_ERROR_NONE) ? true : false;
}

bool CIcoNotificationApp::Update(CIcoNotification noti) {
	if (noti.Empty() || privateid_ == 0) {
		return false;
	}
	notification_error_e notification_error_;
	notification_error_ = notification_update(noti.Unpack());
	return (notification_error_ == NOTIFICATION_ERROR_NONE) ? true : false;
}

bool CIcoNotificationApp::Delete(CIcoNotification noti) {
	if (noti.Empty() || privateid_ == 0) {
		return false;
	}
	const char *pkgname;
	pkgname = noti.GetPkgname();
	if (pkgname == NULL) {
		return false;
	}
	notification_error_e notification_error_;
	notification_error_ = notification_delete_by_priv_id(pkgname, noti.GetType(), 
																											 privateid_);
	return (notification_error_ == NOTIFICATION_ERROR_NONE) ? true : false;
}
