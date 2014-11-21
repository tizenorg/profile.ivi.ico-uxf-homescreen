AM_CPPFLAGS = $(GCC_CXXFLAGS)

noinst_LTLIBRARIES =\
	libico-common.la

libico_common_la_LDFLAGS = -version-info 0:9:0 
libico_common_la_SOURCES =	\
	CicoEFLApp.cpp			\
	CicoSystemConfig.cpp	\
	CicoAilItems.cpp		\
	CicoAulItems.cpp		\
	Cico_aul_listen_app.cpp	\
	CicoGKeyFileConfig.cpp	\
	CicoResourceConfig.cpp	\
	CicoSound.cpp			\
	CicoLibString.cpp

libico_common_la_CPPFLAGS =			\
	$(GCC_CXXFLAGS)					\
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

libico_common_la_LIBADD =			\
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
