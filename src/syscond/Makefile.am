AM_CPPFLAGS = $(GCC_CXXFLAGS)

noinst_PROGRAMS   = \
	IcoSysconDaemon

check_PROGRAMS =	\
	IcoSysconDaemon

IcoSysconDaemon_SOURCES =	\
	CicoEFLApp.cpp			\
	CicoSysConDaemon.cpp	\
	main.cpp

IcoSysconDaemon_CPPFLAGS = 	\
	@ECORE_CFLAGS@			\
	@EINA_CFLAGS@			\
	@GLIB_CFLAGS@			\
	@AIL_CFLAGS@			\
	@BUNDLE_CFLAGS@			\
	@AUL_CFLAGS@			\
	@PKGMGR_CFLAGS@			\
	@WAYLANDCLIENT_CFLAGS@	\
	@CAPIAPPFWAPPLICATION_CFLAGS@ \
	@JSONGLIB_CFLAGS@					\
	@MURPHYCOMMON_CFLAGS@				\
	@MURPHYDOMAINCONTROLLER_CFLAGS@		\
	@MURPHYECORE_CFLAGS@				\
	@MURPHYRESOURCE_CFLAGS@				\
	$(UWS_CFLAGS)						\
	$(WESTONPLUGIN_CFLAGS)				\
	-I../../include						\
    -I../../lib/system-controller


IcoSysconDaemon_LDADD = \
	../../lib/system-controller/.libs/libico-system-controller.a	\
	../../lib/misc/state-machine/.libs/libico-state-machine.a		\
	@ECORE_LIBS@					\
	@EINA_LIBS@						\
	@GLIB_LIBS@						\
	@AIL_LIBS@						\
	@BUNDLE_LIBS@					\
	@AUL_LIBS@						\
	@PKGMGR_LIBS@					\
	@WAYLANDCLIENT_LIBS@			\
	@CAPIAPPFWAPPLICATION_LIBS@ 	\
	@JSONGLIB_LIBS@ 				\
	@MURPHYCOMMON_LIBS@				\
	@MURPHYDOMAINCONTROLLER_LIBS@	\
	@MURPHYECORE_LIBS@				\
	@MURPHYRESOURCE_LIBS@			\
	$(UWS_LIBS)						\
	$(WESTONPLUGIN_LIBS)			\
	../../lib/misc/state-machine/.libs/libico-state-machine.so

#	-lmurphy-resolver

CLEANFILES = $(BUILT_SOURCES)

install-exec-hook:
	mkdir -p $(INSTALL_ROOT)/usr/apps/org.tizen.ico.system-controller/bin
	mkdir -p $(INSTALL_ROOT)/usr/apps/org.tizen.ico.system-controller/res/config
	mkdir -p $(INSTALL_ROOT)/usr/share/packages
	mkdir -p $(INSTALL_ROOT)/usr/lib/systemd/user
	mkdir -p $(INSTALL_ROOT)/usr/lib/systemd/user/weston.target.wants
	mkdir -p $(INSTALL_ROOT)/home/app/ico
	install -m 0755 IcoSysconDaemon $(INSTALL_ROOT)/usr/apps/org.tizen.ico.system-controller/bin
	install -m 0644 ../../data/share/packages/org.tizen.ico.system-controller.xml $(INSTALL_ROOT)/usr/share/packages
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.system-controller/res/config/* $(INSTALL_ROOT)/usr/apps/org.tizen.ico.system-controller/res/config
	install -d -m 0744 app $(INSTALL_ROOT)/home/app/ico
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.system-controller/res/ico/* $(INSTALL_ROOT)/home/app/ico
	chown -fHR app $(INSTALL_ROOT)/home/app; set $?=0
	chgrp -fHR app $(INSTALL_ROOT)/home/app; set $?=0
	install -m 0644 $(top_srcdir)/data/lib/systemd/user/ico-uxf-wait-launchpad-ready.path $(INSTALL_ROOT)/usr/lib/systemd/user
	install -m 0644 $(top_srcdir)/data/lib/systemd/user/ico-system-controller.service $(INSTALL_ROOT)/usr/lib/systemd/user
	ln -sf ../ico-uxf-wait-launchpad-ready.path $(INSTALL_ROOT)/usr/lib/systemd/user/weston.target.wants

uninstall:
	rm -rf $(INSTALL_ROOT)/usr/apps/org.tizen.ico.system-controller
	rm -f $(INSTALL_ROOT)/usr/share/packages/org.tizen.ico.system-controller.xml
	rm -rf $(INSTALL_ROOT)/home/app/ico
