AM_CFLAGS = $(GCC_CFLAGS)
AM_CPPFLAGS = $(GCC_CFLAGS)

bin_PROGRAMS =		\
	ico_set_vehicleinfo	\
	ico_send_inputevent \
	ico_send_hscommand

check_PROGRAMS =	\
	ico_set_vehicleinfo	\
	ico_send_inputevent

ico_set_vehicleinfo_SOURCES = \
	ico_set_vehicleinfo.c
ico_set_vehicleinfo_LDADD = -lwebsockets 

ico_send_inputevent_SOURCE = \
	ico_send_inputevent.c

ico_send_hscommand_SOURCES = \
	ico_send_hscommand.c
ico_send_hscommand_CFLAGS = -I../include
ico_send_hscommand_LDADD = -lwebsockets 

