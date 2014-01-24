AM_CFLAGS = $(GCC_CFLAGS)
AM_CPPFLAGS = $(GCC_CXXFLAGS)
AM_CXXFLAGS = $(GCC_CXXFLAGS)

noinst_PROGRAMS   = \
	OnScreen

OnScreen_SOURCES = \
	on_screen_main.cpp \
	CicoOSEFLApp.cpp   \
	CicoOnScreen.cpp

OnScreen_CFLAGS= 					\
	@CAPIAPPFWAPPLICATION_CFLAGS@ 	\
    $(EFL_CFLAGS) 					\
    $(OPT_CFLAGS) 					\
    $(AUL_CFLAGS) 					\
    $(UWS_CFLAGS) 					\
	$(NOTIFICATION_CFLAGS)          \
    -I../../include 				\
    -I../../lib/common              \
    -I../../lib/notification

OnScreen_CXXFLAGS= 				\
	@CAPIAPPFWAPPLICATION_CFLAGS@ 	\
    $(EFL_CFLAGS) 					\
    $(OPT_CFLAGS) 					\
    $(AUL_CFLAGS) 					\
    $(UWS_CFLAGS) 					\
	$(NOTIFICATION_CFLAGS)          \
    -I../../include 				\
    -I../../lib/common              \
    -I../../lib/notification

OnScreen_LDADD = 					\
	../../lib/common/.libs/libico-common.a 			\
	../../lib/notification/.libs/libico-notification.a \
	@CAPIAPPFWAPPLICATION_LIBS@ 	\
    $(EFL_LIBS) 					\
    $(OPT_LIBS) 					\
    $(AUL_LIBS) 					\
    $(UWS_LIBS) 					\
	$(NOTIFICATION_LIBS)

CLEANFILES = $(BUILT_SOURCES)

install-exec-hook:
	mkdir -p $(INSTALL_ROOT)/usr/apps/org.tizen.ico.onscreen/bin
	mkdir -p $(INSTALL_ROOT)/usr/apps/org.tizen.ico.onscreen/res/config
	mkdir -p $(INSTALL_ROOT)/usr/share/packages
	install -m 0755 OnScreen $(INSTALL_ROOT)/usr/apps/org.tizen.ico.onscreen/bin
	install -m 0644 $(top_srcdir)/data/share/packages/org.tizen.ico.onscreen.xml $(INSTALL_ROOT)/usr/share/packages
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.onscreen/res/config/* $(INSTALL_ROOT)/usr/apps/org.tizen.ico.onscreen/res/config

uninstall:
	rm -rf $(INSTALL_ROOT)/usr/apps/org.tizen.ico.onscreen
	rm -f $(INSTALL_ROOT)/usr/share/packages/org.tizen.ico.onscreen.xml
