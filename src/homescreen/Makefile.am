AM_CFLAGS = $(GCC_CFLAGS)
AM_CPPFLAGS = $(GCC_CXXFLAGS)
AM_CXXFLAGS = $(GCC_CXXFLAGS)

COMMON_LIBS = -lm -lwebsockets -lico-uxf-weston-plugin -lsecurity-server-client

noinst_PROGRAMS   = \
	HomeScreen	\
	StatusBar

HomeScreen_SOURCES = \
	home_screen_main.cpp	\
	CicoSCAilItems.cpp	\
	CicoSCAulItems.cpp	\
	CicoSCSystemConfig.cpp	\
	CicoSCLifeCycleController.cpp	\
	CicoHSWindow.cpp	\
	CicoHSWindowController.cpp	\
	CicoHSMenuTouch.cpp	\
	CicoHomeScreen.cpp	\
	CicoHSMenuTile.cpp	\
	CicoHSBackWindow.cpp	\
	CicoHSControlBarWindow.cpp	\
	CicoHSControlBarTouch.cpp	\
	CicoHSMenuWindow.cpp	\
	CicoHSAppInfo.cpp	\
	CicoHomeScreenConfig.cpp	\
	CicoHomeScreenResourceConfig.cpp	\
	CicoHomeScreenSound.cpp \
	CicoHomeScreenSoundConfig.cpp	\
	CicoHSSystemState.cpp	\
	CicoHSSwipeTouch.cpp	\
	CicoHSSwipeInputWindow.cpp \
	CicoHSAppHistory.cpp	\
	CicoHSAppHistoryExt.cpp	\
	Cico_aul_listen_app.cpp \
	CicoSound.cpp

HomeScreen_CFLAGS= \
    $(EFL_CFLAGS) \
    $(OPT_CFLAGS) \
    $(WL_CFLAGS) \
    $(AUL_CFLAGS) \
    $(AIL_CFLAGS) \
    $(UWS_CFLAGS) \
    $(PKGMGR_CFLAGS) \
    -I/usr/include/ico-uxf-weston-plugin \
    -I/usr/include/ico-util
HomeScreen_CXXFLAGS= \
    $(EFL_CFLAGS) \
    $(OPT_CFLAGS) \
    $(WL_CFLAGS) \
    $(AUL_CFLAGS) \
    $(AIL_CFLAGS) \
    $(UWS_CFLAGS) \
    $(PKGMGR_CFLAGS) \
    -I/usr/include/ico-uxf-weston-plugin \
    -I/usr/include/ico-util \
    -I../../include
HomeScreen_LDADD = \
    $(EFL_LIBS) \
    $(OPT_LIBS) \
    $(WL_LIBS) \
    $(COMMON_LIBS) \
    $(UWS_LIBS) \
    $(AUL_LIBS) \
    $(AIL_LIBS) \
    -lico-util \
    -lico-uxf-weston-plugin \
    /usr/lib/libpkgmgr-client.so \
	../../lib/apps-framework/.libs/libico-appfw.so \
	-lwayland-egl -lEGL -lGLESv2

#OnScreen_SOURCES = \
#	on_screen.cpp        \
#    CicoOnScreenAction.cpp  \
#    CicoOnScreenAppList.cpp  \
#    CicoOnScreenControlWindow.cpp  \
#    CicoOnScreenContext.cpp  \
#	CicoHomeScreenConfig.cpp	\
#	CicoHomeScreenResourceConfig.cpp	\
#	CicoHomeScreenSound.cpp \
#        CicoHomeScreenSoundConfig.cpp
#OnScreen_CXXFLAGS = -I../include $(EFL_CFLAGS) $(OPT_CFLAGS) $(WL_CFLAGS) $(AUL_CFLAGS) $(UWS_CFLAGS) -I/usr/include/ico-uxf-weston-plugin -I/usr/include/ico-util
#OnScreen_LDADD = $(EFL_LIBS) $(OPT_LIBS) $(WL_LIBS) $(COMMON_LIBS) $(AUL_LIBS) $(UWS_LIBS) ../apps_controller/libico_syc-apc.a ../ico-app-framework/.libs/libico-app-fw-efl.so ../ico-app-framework/.libs/libico-app-fw.so

StatusBar_SOURCES = \
    	status_bar.cpp	\
        CicoStatusBarControlWindow.cpp \
        CicoStatusBarTime.cpp \
    	CicoHomeScreenConfig.cpp	\
    	CicoHomeScreenResourceConfig.cpp	\
    	CicoHomeScreenSound.cpp \
        CicoHomeScreenSoundConfig.cpp
StatusBar_CFLAGS = \
            $(EFL_CFLAGS) \
            $(OPT_CFLAGS) \
            $(WL_CFLAGS) \
            $(AUL_CFLAGS) \
            $(UWS_CFLAGS) \
    		-I/usr/include/ico-uxf-weston-plugin \
    		-I/usr/include/ico-util
StatusBar_CXXFLAGS = \
            $(EFL_CFLAGS) \
            $(OPT_CFLAGS) \
            $(WL_CFLAGS) \
            $(AUL_CFLAGS) \
            $(UWS_CFLAGS) \
    		-I/usr/include/ico-uxf-weston-plugin \
    		-I/usr/include/ico-util
StatusBar_LDADD = \
            $(EFL_LIBS) \
            $(OPT_LIBS) \
            $(WL_LIBS) \
            $(COMMON_LIBS) \
            $(AUL_LIBS) \
            $(UWS_LIBS)

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
	install -m 0755 HomeScreen $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/bin
	install -m 0644 home_screen_bg.edj $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/edj
	install -m 0644 home_screen_touch.edj $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/edj
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.homescreen/res/images/bg.png $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/images
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.homescreen/res/images/ctrl.png $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/images
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.homescreen/res/images/applist_off.png $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/images
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.homescreen/res/images/applist_off_2.png $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/images
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.homescreen/res/images/applist_on.png $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/images
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.homescreen/res/images/applist_on_2.png $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/images
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.homescreen/res/images/api_all_off.png $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/images
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.homescreen/res/images/api_all_on.png $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/images
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.homescreen/res/images/pagePointer_n.png $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/images
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.homescreen/res/images/pagePointer_p.png $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/images
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.homescreen/res/images/pageDownCursor.png $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/images
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.homescreen/res/images/pageUpCursor.png $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/images
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.homescreen/res/images/button_really.png $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/images
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.homescreen/res/images/button_no.png $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/images
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.homescreen/res/images/button_yes.png $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/images
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.homescreen/res/images/termIcon.png $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/images
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.homescreen/res/images/tizen_32.png $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/images
	install -m 0644 $(top_srcdir)/data/share/packages/org.tizen.ico.homescreen.xml $(INSTALL_ROOT)/usr/share/packages
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.homescreen/res/config/* $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/config
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.homescreen/res/sound/* $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen/res/sound
	cp -r $(top_srcdir)/data/apps/org.tizen.ico.homescreen $(INSTALL_ROOT)/usr/apps/

uninstall:
	rm -rf $(INSTALL_ROOT)/usr/apps/org.tizen.ico.homescreen
	rm -f $(INSTALL_ROOT)/usr/share/packages/org.tizen.ico.homescreen
