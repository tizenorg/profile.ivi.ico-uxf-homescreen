#ifndef CICO_NOTIFICATION_APP_H_
#define CICO_NOTIFICATION_APP_H_
#include "CIcoNotificationInterface.h"

class CIcoNotificationApp : public CIcoNotificationAppInterface
{
	public:
		CIcoNotificationApp();
		~CIcoNotificationApp();
		bool Add(CIcoNotification noti);
		bool Update(CIcoNotification noti);
		bool Delete(CIcoNotification noti);
	private:
		int privateid_;
};
#endif // CICO_NOTIFICATION_APP_H_

