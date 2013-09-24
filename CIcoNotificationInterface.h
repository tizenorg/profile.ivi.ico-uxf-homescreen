#ifndef CICO_NOTIFICATION_INTERFACE_H_
#define CICO_NOTIFICATION_INTERFACE_H_
#include "CIcoNotification.h"

class CIcoNotificationAppInterface {
	public:
		virtual bool Add(CIcoNotification noti) = 0;
		virtual bool Update(CIcoNotification noti) = 0;
		virtual bool Delete(CIcoNotification noti) = 0;
};

class CIcoNotificationServiceInterface {
	public:
		virtual bool SetCallback(void (*detailed_changed_cb)
																							(void *data, 
																							 notification_type_e type, 
																							 notification_op *op_list, 
																							 int num_op), 
														 void *user_data) = 0;
		virtual bool UnsetCallback(void (*detailed_changed_cb)
																								(void *data, 
																								 notification_type_e type, 
																								 notification_op *op_list, 
																								 int num_op)
															) = 0;
};
#endif //  CICO_NOTIFICATION_INTERFACE_H_
