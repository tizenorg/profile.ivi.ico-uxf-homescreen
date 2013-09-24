#ifndef CICO_NOTIFICATION_SERVICE_H_
#define CICO_NOTIFICATION_SERVICE_H_
#include "CIcoNotificationInterface.h"

class CIcoNotificationService 
: public CIcoNotificationAppInterface, CIcoNotificationServiceInterface
{
	public:
		CIcoNotificationService();
		~CIcoNotificationService();
		bool Add(CIcoNotification noti);
		bool Update(CIcoNotification noti);
		bool Delete(CIcoNotification noti);
		bool SetCallback(void (*detailed_changed_cb)(void *data, 
																							   notification_type_e type, 
																							   notification_op *op_list, 
																							   int num_op), 
										 void *user_data);
		bool UnsetCallback(void (*detailed_changed_cb)(void *data, 
																								   notification_type_e type, 
  																								 notification_op *op_list, 
  																								 int num_op));
	private:
		int privateid_;
		void *user_data_;
};
#endif // CICO_NOTIFICATION_SERVICE_H_
