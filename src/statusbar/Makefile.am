AM_CFLAGS = $(GCC_CFLAGS)
AM_CPPFLAGS = $(GCC_CXXFLAGS)
AM_CXXFLAGS = $(GCC_CXXFLAGS)

COMMON_LIBS = -lm -lwebsockets -lico-uxf-weston-plugin -lsecurity-server-client

noinst_PROGRAMS   = \
	StatusBar

StatusBar_SOURCES = \
	status_bar.cpp						\
	CicoStatusBarControlWindow.cpp		\
	CicoStatusBarTime.cpp

StatusBar_CFLAGS = \
	$(EFL_CFLAGS)	\
	$(OPT_CFLAGS)	\
	$(WL_CFLAGS)	\
	$(AUL_CFLAGS)	\
	$(UWS_CFLAGS)	\
	-I/usr/include/ico-uxf-weston-plugin \
	-I/usr/include/ico-util	\
	-I../../include			\
	-I../../lib/common

StatusBar_CXXFLAGS = \
	$(EFL_CFLAGS)	\
	$(OPT_CFLAGS)	\
	$(WL_CFLAGS)	\
	$(AUL_CFLAGS)	\
	$(UWS_CFLAGS)	\
	-I/usr/include/ico-uxf-weston-plugin \
	-I/usr/include/ico-util \
	-I../../include			\
	-I../../lib/common

StatusBar_LDADD = \
	../../lib/common/.libs/libico-common.a \
	$(EFL_LIBS)		\
	$(OPT_LIBS)		\
	$(WL_LIBS)		\
	$(COMMON_LIBS)	\
	$(AUL_LIBS)		\
	$(UWS_LIBS)

CLEANFILES = $(BUILT_SOURCES)

install-exec-hook:
	mkdir -p $(INSTALL_ROOT)/usr/apps/org.tizen.ico.statusbar/bin
	mkdir -p $(INSTALL_ROOT)/usr/apps/org.tizen.ico.statusbar/res/config
	mkdir -p $(INSTALL_ROOT)/usr/apps/org.tizen.ico.statusbar/res/images
	mkdir -p $(INSTALL_ROOT)/usr/share/packages
	install -m 0755 StatusBar $(INSTALL_ROOT)/usr/apps/org.tizen.ico.statusbar/bin
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.statusbar/res/images/time*.png $(INSTALL_ROOT)/usr/apps/org.tizen.ico.statusbar/res/images
	install -m 0644 $(top_srcdir)/data/share/packages/org.tizen.ico.statusbar.xml $(INSTALL_ROOT)/usr/share/packages
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.statusbar/res/config/* $(INSTALL_ROOT)/usr/apps/org.tizen.ico.statusbar/res/config

uninstall:
	rm -rf $(INSTALL_ROOT)/usr/apps/org.tizen.ico.statusbar
	rm -f $(INSTALL_ROOT)/usr/share/packages/org.tizen.ico.statusbar.xml
