AM_CFLAGS = $(GCC_CFLAGS)
AM_CPPFLAGS = $(GCC_CXXFLAGS)

COMMON_LIBS = -L/usr/local/lib -lm -lwebsockets

noinst_PROGRAMS   = \
	DummyHomeScreen

DummyHomeScreen_SOURCES =	\
	CicoLog.cpp				\
	CicoBackground.cpp		\
	CicoBaseWin.cpp			\
	CicoGeometry.cpp		\
	CicoColor.cpp			\
	CicoEFLApp.cpp			\
	CicoEvasObject.cpp		\
	CicoUISample.cpp		\
	main.cpp

DummyHomeScreen_CXXFLAGS = 	\
    @ECORE_CFLAGS@			\
    @EINA_CFLAGS@			\
	@ECOREEVAS_CFLAGS@		\
	@EVAS_CFLAGS@			\
	@GLIB_CFLAGS@			\
	@AIL_CFLAGS@			\
	@BUNDLE_CFLAGS@			\
	@AUL_CFLAGS@			\
	@PKGMGR_CFLAGS@			\
	@WAYLANDCLIENT_CFLAGS@	\
	@CAPIAPPFWAPPLICATION_CFLAGS@ \
	-I/usr/include/ico-util	\
	-I../../../include

DummyHomeScreen_LDADD =		\
    @ECORE_LIBS@			\
    @EINA_LIBS@				\
	@ECOREEVAS_LIBS@		\
	@EVAS_LIBS@				\
	@GLIB_LIBS@				\
	@AIL_LIBS@				\
	@BUNDLE_LIBS@			\
	@AUL_LIBS@				\
	@PKGMGR_LIBS@			\
	@WAYLANDCLIENT_LIBS@	\
	@CAPIAPPFWAPPLICATION_LIBS@ \
	-L/usr/lib				\
	-lico-util				\
	../../../lib/apps-framework/.libs/libico-appfw.so \
	-lreadline


CLEANFILES = $(BUILT_SOURCES)

#install-exec-hook:
#	mkdir -p $(INSTALL_ROOT)/usr/apps/org.tizen.ico.dummy-homescreen/bin
#	mkdir -p $(INSTALL_ROOT)/usr/share/packages
#	install -m 0755 DummyHomeScreen $(INSTALL_ROOT)/usr/apps/org.tizen.ico.dummy-homescreen/bin
#	install -m 0644 org.tizen.ico.dummy-homescreen.xml $(INSTALL_ROOT)/usr/share/packages
#
#uninstall:
#	rm -rf $(INSTALL_ROOT)/usr/apps/org.tizen.ico.dummy-homescreen
#	rm -f $(INSTALL_ROOT)/usr/share/packages/org.tizen.ico.dummy-homescreen.xml
#	rm -f $(INSTALL_ROOT)/usr/share/applications/org.tizen.ico.dummy-homescreen.desktop

# vim:set ts=4 sw=4:
