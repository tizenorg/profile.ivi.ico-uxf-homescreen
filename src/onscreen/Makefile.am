AM_CFLAGS = $(GCC_CFLAGS)
AM_CPPFLAGS = $(GCC_CXXFLAGS)
AM_CXXFLAGS = $(GCC_CXXFLAGS)

COMMON_LIBS = -lm -lwebsockets -lico-uxf-weston-plugin -lsecurity-server-client

noinst_PROGRAMS   = \
	OnScreen

OnScreen_SOURCES = \
	on_screen.cpp					\
	CicoOnScreenAction.cpp			\
	CicoOnScreenAppList.cpp			\
	CicoOnScreenControlWindow.cpp	\
	CicoOnScreenContext.cpp

OnScreen_CFLAGS= 					\
	@CAPIAPPFWAPPLICATION_CFLAGS@ 	\
    $(EFL_CFLAGS) 					\
    $(OPT_CFLAGS) 					\
    $(WL_CFLAGS) 					\
    $(AUL_CFLAGS) 					\
    $(AIL_CFLAGS) 					\
    $(UWS_CFLAGS) 					\
    $(PKGMGR_CFLAGS) 				\
	$(UWS_CFLAGS)					\
	$(WESTONPLUGIN_CFLAGS)			\
    -I../../include 				\
    -I../../lib/common

OnScreen_CXXFLAGS= 				\
	@CAPIAPPFWAPPLICATION_CFLAGS@ 	\
    $(EFL_CFLAGS) 					\
    $(OPT_CFLAGS) 					\
    $(WL_CFLAGS) 					\
    $(AUL_CFLAGS) 					\
    $(AIL_CFLAGS) 					\
    $(UWS_CFLAGS) 					\
    $(PKGMGR_CFLAGS) 				\
	$(UWS_CFLAGS)					\
	$(WESTONPLUGIN_CFLAGS)			\
    -I../../include 				\
    -I../../lib/common

OnScreen_LDADD = 					\
	../../lib/common/.libs/libico-common.a 			\
	@CAPIAPPFWAPPLICATION_LIBS@ 	\
	@PKGMGR_LIBS@					\
    $(EFL_LIBS) 					\
    $(OPT_LIBS) 					\
    $(WL_LIBS) 						\
    $(COMMON_LIBS) 					\
    $(UWS_LIBS) 					\
    $(AUL_LIBS) 					\
    $(AIL_LIBS) 					\
	$(UWS_LIBS)						\
	../../lib/apps-framework/.libs/libico-appfw.so	\
	-lwayland-egl -lEGL -lGLESv2

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
