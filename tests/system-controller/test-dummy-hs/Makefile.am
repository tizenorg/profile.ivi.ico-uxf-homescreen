AM_CFLAGS = $(GCC_CFLAGS)
AM_CPPFLAGS = $(GCC_CXXFLAGS)

COMMON_LIBS = -L/usr/local/lib -lm -lwebsockets -lico-uxf-weston-plugin

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
	-I/usr/include/ico-uxf-weston-plugin	\
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
    -lico-uxf-weston-plugin \
	-lico-util-com			\
	../../../lib/apps-framework/libico-appfw.la

CLEANFILES = $(BUILT_SOURCES)
