AM_CPPFLAGS = $(GCC_CXXFLAGS)

noinst_LTLIBRARIES =\
	libico-notification.la

#lib_LTLIBRARIES =\
#	libico-system-controller.la

libico_notification_la_LDFLAGS = -version-info 0:9:0 
libico_notification_la_SOURCES =	\
	CicoNotification.cpp			\
	CicoNotificationList.cpp		\
	CicoNotificationApp.cpp			\
	CicoNotificationService.cpp

libico_notification_la_CPPFLAGS =	\
	$(GCC_CXXFLAGS)					\
	$(NOTIFICATION_CFLAGS) 			\
	@ECORE_CFLAGS@					\
	@EINA_CFLAGS@					\
	@GLIB_CFLAGS@					\
	@AIL_CFLAGS@					\
	@BUNDLE_CFLAGS@					\
	@AUL_CFLAGS@					\
	@PKGMGR_CFLAGS@					\
	@CAPIAPPFWAPPLICATION_CFLAGS@	\
	$(UWS_CFLAGS)					\
	-I../../include

libico_notification_la_LIBADD =		\
	$(NOTIFICATION_LIBS)			\
	@ECORE_LIBS@					\
	@EINA_LIBS@						\
	@GLIB_LIBS@						\
	@AIL_LIBS@						\
	@BUNDLE_LIBS@					\
	@AUL_LIBS@						\
	@PKGMGR_LIBS@					\
	@CAPIAPPFWAPPLICATION_LIBS@		\
	$(UWS_LIBS)

AM_LDFLAGS = -module -avoid-version -rpath $(libdir)

.FORCE :

# vim:set ts=4 sw=4:
