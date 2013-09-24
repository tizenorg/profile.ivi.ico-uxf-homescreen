#ifndef CICO_NOTIFICATION_H_
#define CICO_NOTIFICATION_H_
#include <notification.h>

class CIcoNotification {
	public:
		CIcoNotification();
		CIcoNotification(notification_h noti);
		~CIcoNotification();

		bool Initialize(notification_type_e type);
		bool Terminate();
		notification_h Unpack() const;
		notification_h Clone();
		bool Empty() const;
		notification_type_e GetType() const;
		bool SetPkgname(const char *pkgname);
		const char *GetPkgname();
		bool SetTitle(const char *title);
		const char *GetTitle();
		bool SetContent(const char *content);
		const char *GetContent();
	private:
		notification_h notification_;		
		notification_type_e type_;
};

#endif //  CICO_NOTIFICATION_H_

