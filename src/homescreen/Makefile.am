AM_CFLAGS = $(GCC_CFLAGS)
AM_CPPFLAGS = $(GCC_CXXFLAGS)
AM_CXXFLAGS = $(GCC_CXXFLAGS)

COMMON_LIBS = -lm -lwebsockets -lico-uxf-weston-plugin -lsecurity-server-client

noinst_PROGRAMS   = \
	HomeScreen

HomeScreen_SOURCES =					\
	CicoHSEFLApp.cpp					\
	CicoHSCmdOpts.cpp					\
	home_screen_main.cpp				\
	CicoHSLifeCycleController.cpp		\
	CicoHSWindow.cpp					\
	CicoHSWindowController.cpp			\
	CicoHSMenuTouch.cpp					\
	CicoHomeScreen.cpp					\
	CicoHSMenuTile.cpp					\
	CicoHSBackWindow.cpp				\
	CicoHSControlBarWindow.cpp			\
	CicoHSControlBarTouch.cpp			\
	CicoHSMenuWindow.cpp				\
	CicoHSAppInfo.cpp					\
	CicoHSSystemState.cpp				\
	CicoHSSwipeTouch.cpp				\
	CicoHSSwipeInputWindow.cpp			\
	CicoHSAppHistory.cpp				\
	CicoHSAppHistoryExt.cpp             \
    CicoHSCommand.cpp                   \
    CicoHSServer.cpp                    \
    CicoHSMessage.cpp                   \
    CicoHSAppControl.cpp                \
    CicoHSCommandArg.cpp                 

HomeScreen_CFLAGS= 					\
	@CAPIAPPFWAPPLICATION_CFLAGS@ 	\
    $(EFL_CFLAGS) 					\
    $(OPT_CFLAGS) 					\
    $(WL_CFLAGS) 					\
    $(AUL_CFLAGS) 					\
    $(AIL_CFLAGS) 					\
    $(UWS_CFLAGS) 					\
    $(PKGMGR_CFLAGS) 				\
	$(UWS_CFLAGS)					\
    -I../../include 				\
    -I../../lib/common

HomeScreen_CXXFLAGS= 				\
	@CAPIAPPFWAPPLICATION_CFLAGS@ 	\
    $(EFL_CFLAGS) 					\
    $(OPT_CFLAGS) 					\
    $(WL_CFLAGS) 					\
    $(AUL_CFLAGS) 					\
    $(AIL_CFLAGS) 					\
    $(UWS_CFLAGS) 					\
    $(PKGMGR_CFLAGS) 				\
	$(UWS_CFLAGS)					\
    -I../../include 				\
    -I../../lib/common

HomeScreen_LDADD = 					\
	../../lib/common/.libs/libico-common.a 			\
	@CAPIAPPFWAPPLICATION_LIBS@ 	\
	@PKGMGR_LIBS@					\
    $(ECOREEVAS_LIBS)					\
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

SUFFIXES = .edc .edj

BUILT_SOURCES = \
	home_screen_bg.edj	\
	home_screen_touch.edj	\
	appli_list.edj	\
	appli_kill.edj

.edc.edj:
	edje_cc $< -id ../../res/org.tizen.ico.homescreen/res/images -fd ../../res/org.tizen.ico.homescreen/res/images -sd ../../res/org.tizen.ico.homescreen/res/sounds -o $@

CLEANFILES = $(BUILT_SOURCES)

install-exec-hook:
	mkdir -p $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/bin
	mkdir -p $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/var
	mkdir -p $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/config
	mkdir -p $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/images
	mkdir -p $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/sound
	mkdir -p $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/edj
	mkdir -p $(INSTALL_ROOT)/usr/share/packages
	mkdir -p $(INSTALL_ROOT)/usr/share/icons/default/small/
	install -m 0755 HomeScreen $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/bin
	install -m 0644 home_screen_bg.edj $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/edj
	install -m 0644 home_screen_touch.edj $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/edj
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.homescreen/res/images/* $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/images
	install -m 0644 $(top_srcdir)/data/share/packages/org.tizen.ico.homescreen.xml $(INSTALL_ROOT)/usr/share/packages
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.homescreen/res/config/* $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/config
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.homescreen/res/sound/* $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/sound
	install -m 0644 $(top_srcdir)/data/apps/org.tizen.ico.homescreen/res/icons/default/small/* $(INSTALL_ROOT)/usr/share/icons/default/small/

uninstall:
	rm -rf $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen
	rm -f $(INSTALL_ROOT)/usr/share/packages/org.tizen.ico.homescreen.xml
