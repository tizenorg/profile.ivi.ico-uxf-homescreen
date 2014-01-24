AM_CFLAGS = $(GCC_CFLAGS)
AM_CPPFLAGS = $(GCC_CXXFLAGS)

noinst_PROGRAMS = \
	ico_send_notification

check_PROGRAMS =	\
	ico_send_notification

ico_send_notification_SOURCES = \
	send_notification.cpp

ico_send_notification_CXXFLAGS = \
	$(NOTIFICATION_CFLAGS)

ico_send_notification_LDADD = \
	$(NOTIFICATION_LIBS)
	
