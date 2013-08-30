AM_CFLAGS = $(GCC_CFLAGS)
AM_CPPFLAGS = $(GCC_CXXFLAGS)
AM_CXXFLAGS = $(GCC_CXXFLAGS)

COMMON_LIBS = -lm -lwebsockets -lico-uxf-weston-plugin

noinst_PROGRAMS   = \
	HomeScreen	\
	StatusBar               

HomeScreen_SOURCES = \
	home_screen_main.cpp        \
	ico_log.c        \
	CicoSCAilItems.cpp        \
	CicoSCAulItems.cpp        \
	CicoSCSystemConfig.cpp        \
	CicoSCLifeCycleController.cpp        \
	CicoHSWindow.cpp        \
	CicoHSWindowController.cpp        \
	CicoHSMenuTouch.cpp        \
	CicoHomeScreen.cpp        \
    CicoHSMenuTile.cpp  \
	CicoHSBackWindow.cpp \
	CicoHSControlBarWindow.cpp \
	CicoHSControlBarTouch.cpp \
	CicoHSMenuWindow.cpp \
	CicoHSAppInfo.cpp \
	CicoHomeScreenConfig.cpp      \
	CicoHomeScreenResourceConfig.cpp	\
	CicoHomeScreenSound.cpp \
    CicoHomeScreenSoundConfig.cpp
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
    -lico-util-com \
    -lico-uxf-weston-plugin \
    /usr/lib/libpkgmgr-client.so \
	../../lib/apps-framework/.libs/libico-appfw.so

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
#OnScreen_CXXFLAGS = -I../include $(EFL_CFLAGS) $(OPT_CFLAGS) $(WL_CFLAGS) $(AUL_CFLAGS) $(UWS_CFLAGS) -I/usr/include/ico-util
#OnScreen_LDADD = $(EFL_LIBS) $(OPT_LIBS) $(WL_LIBS) $(COMMON_LIBS) $(AUL_LIBS) $(UWS_LIBS) ../apps_controller/libico_syc-apc.a ../ico-app-framework/.libs/libico-app-fw-efl.so ../ico-app-framework/.libs/libico-app-fw.so

StatusBar_SOURCES = \
    	status_bar.cpp	\
	    ico_log.c        \
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
            $(UWS_CFLAGS) 
StatusBar_CXXFLAGS = \
            $(EFL_CFLAGS) \
            $(OPT_CFLAGS) \
            $(WL_CFLAGS) \
            $(AUL_CFLAGS) \
            $(UWS_CFLAGS) 
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
