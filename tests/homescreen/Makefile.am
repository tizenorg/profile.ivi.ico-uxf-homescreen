AM_CFLAGS = $(GCC_CFLAGS)
AM_CPPFLAGS = $(GCC_CFLAGS)

bin_PROGRAMS =		\
	ico_set_vehicleinfo	\
	ico_send_hscommand

check_PROGRAMS =	\
	ico_set_vehicleinfo

ico_set_vehicleinfo_SOURCES = \
	ico_set_vehicleinfo.c
ico_set_vehicleinfo_LDADD = -lwebsockets 

ico_send_hscommand_SOURCES = \
	ico_send_hscommand.c
ico_send_hscommand_CFLAGS = -I../include $(UWS_CFLAGS)
#ico_send_hscommand_LDADD = $(UWS_LIBS) -lwebsockets -ldlog
ico_send_hscommand_LDADD = $(UWS_LIBS) 

