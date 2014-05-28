AM_CPPFLAGS = $(GCC_CXXFLAGS)

noinst_LTLIBRARIES =\
	libico-system-controller.la

#lib_LTLIBRARIES =\
#	libico-system-controller.la

libico_system_controller_la_LDFLAGS = -version-info 0:9:0
libico_system_controller_la_SOURCES =	\
	CicoSCLifeCycleController.cpp		\
	CicoSCWaylandIF.cpp					\
	CicoSCWayland.cpp					\
	CicoSCWlWinMgrIF.cpp				\
	CicoSCWindow.cpp					\
	CicoSCDisplay.cpp					\
	CicoSCLayer.cpp						\
	CicoSCDisplayZone.cpp				\
	CicoSCWindowController.cpp			\
	CicoSCWlInputMgrIF.cpp				\
	CicoSCInputDev.cpp					\
	CicoSCSwitch.cpp					\
	CicoSCInputController.cpp			\
	CicoSCServer.cpp					\
	CicoSCCommand.cpp 					\
	CicoSCMessage.cpp       			\
	CicoSCMessageRes.cpp       			\
	CicoSCUser.cpp						\
	CicoSCUserManager.cpp				\
	CicoSCSysResourceController.cpp		\
	CicoSCSysResourceMonitor.cpp		\
	CicoSCAppResourceController.cpp		\
	CicoSCResourceManager.cpp			\
	ico_syc_mrp_resource.c				\
	CicoSCPolicyManager.cpp				\
	CicoSCVInfoManager.cpp

libico_system_controller_la_CPPFLAGS =	\
	$(GCC_CXXFLAGS)						\
	@ECORE_CFLAGS@						\
	@EINA_CFLAGS@						\
	@GLIB_CFLAGS@						\
	@AIL_CFLAGS@						\
	@BUNDLE_CFLAGS@						\
	@AUL_CFLAGS@						\
	@PKGMGR_CFLAGS@						\
	@WAYLANDCLIENT_CFLAGS@				\
	@CAPIAPPFWAPPLICATION_CFLAGS@ 		\
	@JSONGLIB_CFLAGS@					\
	@MURPHYCOMMON_CFLAGS@				\
	@MURPHYDOMAINCONTROLLER_CFLAGS@		\
	@MURPHYECORE_CFLAGS@				\
	@MURPHYRESOURCE_CFLAGS@				\
	$(UWS_CFLAGS)						\
	-I../../include						\
	-I../common							\
	-I../misc/state-machine

libico_system_controller_la_LIBADD =	\
	../common/.libs/libico-common.a		\
	@ECORE_LIBS@						\
	@EINA_LIBS@							\
	@GLIB_LIBS@							\
	@AIL_LIBS@							\
	@BUNDLE_LIBS@						\
	@AUL_LIBS@							\
	@PKGMGR_LIBS@						\
	@WAYLANDCLIENT_LIBS@				\
	@CAPIAPPFWAPPLICATION_LIBS@ 		\
	@JSONGLIB_LIBS@ 					\
	@MURPHYCOMMON_LIBS@					\
	@MURPHYDOMAINCONTROLLER_LIBS@		\
	@MURPHYECORE_LIBS@					\
	@MURPHYRESOURCE_LIBS@				\
	$(UWS_LIBS)							\
	$(GENIVI_LIBS)						\
	$(WESTONPLUGIN_LIBS)

AM_LDFLAGS = -module -avoid-version -rpath $(libdir)

.FORCE :

# vim:set ts=4 sw=4:
