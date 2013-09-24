#include <map>
#include "CIcoNotificationService.h"

CIcoNotificationService::CIcoNotificationService() : privateid_(0), 
																										 user_data_(NULL) {
}

CIcoNotificationService::~CIcoNotificationService() {
}

bool CIcoNotificationService::Add(CIcoNotification noti) {
	if (noti.Empty()) {
		return false;
	}
	int retid;
	notification_error_e notification_error_;
	notification_error_ = notification_insert(noti.Unpack(), &retid);
	privateid_ = retid;
	return (notification_error_ == NOTIFICATION_ERROR_NONE) ? true : false;
}

bool CIcoNotificationService::Update(CIcoNotification noti) {
	if (noti.Empty() || privateid_ == 0) {
		return false;
	}
	notification_error_e notification_error_;
	notification_error_ = notification_update(noti.Unpack());
	return (notification_error_ == NOTIFICATION_ERROR_NONE) ? true : false;
}

bool CIcoNotificationService::Delete(CIcoNotification noti) {
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

bool CIcoNotificationService::SetCallback(void (*detailed_changed_cb)
																											(void *data, 
																										  notification_type_e type, 
																											notification_op *op_list, 
																											int num_op), 
										                      void *user_data) {
	if (user_data_ != NULL) {
		return false;
	}
	notification_error_e notification_error_;
	notification_error_ = notification_register_detailed_changed_cb(
																												detailed_changed_cb, 
																												user_data);
	if (notification_error_ == NOTIFICATION_ERROR_NONE) {
		user_data_ = user_data;
		return true;
	}
	return false;
}

bool CIcoNotificationService::UnsetCallback(void (*detailed_changed_cb)
																											(void *data, 
																											notification_type_e type, 
																											notification_op *op_list, 
																											int num_op)) {
	if (user_data_) {
		return false;
	}
	notification_error_e notification_error_;
	notification_error_ = notification_unregister_detailed_changed_cb(
																												detailed_changed_cb, 
																												user_data_);
	return (notification_error_ == NOTIFICATION_ERROR_NONE) ? true : false;
}
