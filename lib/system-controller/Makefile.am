AM_CPPFLAGS = $(GCC_CXXFLAGS)

noinst_LTLIBRARIES =\
	libico-system-controller.la

#lib_LTLIBRARIES =\
#	libico-system-controller.la

libico_system_controller_la_SOURCES =	\
	CicoSCAilItems.cpp					\
	CicoSCAulItems.cpp					\
	CicoSCLifeCycleController.cpp		\
	ico_log.c							\
	CicoSCSystemConfig.cpp				\
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
	CicoSCCommandParser.cpp 			\
	CicoSCMessage.cpp       			\
	CicoSCUser.cpp						\
	CicoSCUserManager.cpp				\
	CicoSCSysResourceController.cpp		\
	CicoSCSysResourceMonitor.cpp		\
	CicoSCResourceManager.cpp			\
	ico_syc_mrp_resource.c				\
	CicoSCPolicyManager.cpp

libico_system_controller_la_CPPFLAGS = 	\
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
	-I../../include						\
	-I/usr/include/ico-util				\
	-I/usr/include/ico-uxf-weston-plugin \
	-I../misc/state-machine

libico_system_controller_la_LIBADD =	\
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
	-L/usr/lib							\
	-lico-uxf-weston-plugin 			\
	-lico-util-com

AM_LDFLAGS = -module -avoid-version -rpath $(libdir)

.FORCE :
