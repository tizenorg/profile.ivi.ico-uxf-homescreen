AM_CFLAGS = $(GCC_CFLAGS)
AM_CPPFLAGS = $(GCC_CXXFLAGS)

noinst_PROGRAMS = \
	ico_send_notification \
	ico_send_notification2 \
	ico_dump_notification \
	ico_del_notification

check_PROGRAMS =	\
	ico_send_notification \
	ico_send_notification2 \
	ico_dump_notification \
	ico_dump_notification

#
#
#
ico_send_notification_SOURCES = \
	send_notification.cpp

ico_send_notification_CXXFLAGS = \
	$(NOTIFICATION_CFLAGS)

ico_send_notification_LDADD = \
	$(NOTIFICATION_LIBS)
	
ico_send_notification2_SOURCES = \
	send_notification2.cpp

ico_send_notification2_CXXFLAGS = \
	$(NOTIFICATION_CFLAGS)

ico_send_notification2_LDADD = \
	$(NOTIFICATION_LIBS) -lappsvc
	
#
#
#
ico_dump_notification_SOURCES = \
	dump_notification.cpp

ico_dump_notification_CXXFLAGS = \
	$(NOTIFICATION_CFLAGS)

ico_dump_notification_LDADD = \
	$(NOTIFICATION_LIBS)
	
#
#
#
ico_del_notification_SOURCES = \
	del_notification.cpp

ico_del_notification_CXXFLAGS = \
	$(NOTIFICATION_CFLAGS)

ico_del_notification_LDADD = \
	$(NOTIFICATION_LIBS)
