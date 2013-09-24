#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "CIcoNotification.h"

CIcoNotification::CIcoNotification() : notification_(NULL), 
																			 type_(NOTIFICATION_TYPE_NONE) {
}

CIcoNotification::CIcoNotification(notification_h noti) 
: notification_(noti), type_(NOTIFICATION_TYPE_NONE) {
	notification_type_e type;
	notification_get_type(notification_, &type);
}

CIcoNotification::~CIcoNotification() {
/*
	if (notification_ != NULL) {
		notification_free(notification_);
		notification_ = NULL;
	}
*/
}

bool CIcoNotification::Initialize(notification_type_e type) {
	if (notification_) {
		return true;
	}
	notification_ = notification_create(type);	
	if (notification_) {
		type_ = type;
		return true;
	}
	return false;

}

bool CIcoNotification::Terminate() {
	if (notification_ != NULL) {
		notification_error_e notification_error_;
		notification_error_ = notification_free(notification_);
		if (notification_error_ != NOTIFICATION_ERROR_NONE) {
			return false;
		}
		notification_ = NULL;
	}
	return true;
}

notification_h CIcoNotification::Unpack() const {
	return notification_;
}

notification_h CIcoNotification::Clone() {
	notification_h ret_noti;
	notification_error_e notification_error_;
	notification_error_ = notification_clone(notification_, &ret_noti);
	return (notification_error_ == NOTIFICATION_ERROR_NONE && ret_noti != NULL) ?
				 ret_noti : NULL;
}

bool CIcoNotification::Empty() const {
	return (notification_ == NULL) ? true : false;
}

notification_type_e CIcoNotification::GetType() const {
	return type_;
}

bool CIcoNotification::SetPkgname(const char *pkgname) {
	if (!notification_) {
		return false;
	}
	notification_error_e notification_error_;
	notification_error_ = notification_set_pkgname(notification_, pkgname);
	return (notification_error_ == NOTIFICATION_ERROR_NONE) ? true : false;
}

const char *CIcoNotification::GetPkgname() {
	if (!notification_) {
		return NULL;
	}
	char *caller_pkgname;
	notification_error_e notification_error_;
	notification_error_ = notification_get_pkgname(notification_, 
																								 &caller_pkgname);
	return (notification_error_ == NOTIFICATION_ERROR_NONE &&
					caller_pkgname != NULL) ? caller_pkgname : NULL;
}

bool CIcoNotification::SetTitle(const char *title) {
	if (!notification_) {
		return false;
	}
	notification_error_e notification_error_;
	notification_error_ = notification_set_text(notification_, 
																							NOTIFICATION_TEXT_TYPE_TITLE, 
																							title, NULL, 
																							NOTIFICATION_VARIABLE_TYPE_NONE);
	return (notification_error_ == NOTIFICATION_ERROR_NONE) ? true : false;
}

const char *CIcoNotification::GetTitle() {
	if (!notification_) {
		return NULL;
	}
	notification_error_e notification_error_;
	char *title;
	notification_error_ = notification_get_text(notification_, 
																						  NOTIFICATION_TEXT_TYPE_TITLE,
																							&title);
	return (notification_error_ == NOTIFICATION_ERROR_NONE && title != NULL) ?
				 title : NULL;
}

bool CIcoNotification::SetContent(const char *content) {
	if (!notification_) {
		return false;
	}
	notification_error_e notification_error_;
	notification_error_ = notification_set_text(notification_, 
																				      NOTIFICATION_TEXT_TYPE_CONTENT, 
																							content, NULL, 
																							NOTIFICATION_VARIABLE_TYPE_NONE);
	return (notification_error_ == NOTIFICATION_ERROR_NONE) ? true : false;
}

const char *CIcoNotification::GetContent() {
	if (!notification_) {
		return NULL;
	}
	notification_error_e notification_error_;
	char *content;
	notification_error_ = notification_get_text(notification_, 
																							NOTIFICATION_TEXT_TYPE_CONTENT,
																							&content);
	return (notification_error_ == NOTIFICATION_ERROR_NONE && content != NULL) ?
				 content : NULL;
}
