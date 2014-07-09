AM_CFLAGS = $(GCC_CFLAGS)
AM_CPPFLAGS = $(GCC_CXXFLAGS)
AM_CXXFLAGS = $(GCC_CXXFLAGS)

noinst_PROGRAMS   = \
	StatusBar

StatusBar_SOURCES = \
	CicoCommonModule.cpp 			\
	CicoCommonWindow.cpp			\
	CicoComponentImplementation.cpp	\
	CicoStatusBar.cpp				\
	CicoSBEFLApp.cpp				\
	main.cpp

StatusBar_CFLAGS = \
	@CAPIAPPFWAPPLICATION_CFLAGS@ 	\
	$(EFL_CFLAGS)			\
	$(OPT_CFLAGS)			\
	$(AUL_CFLAGS)			\
	$(SYSCOND_CFLAGS)		\
	$(ICOUWS_CFLAGS)		\
	$(NOTIFICATION_CFLAGS)	\
	-I../../lib/common      \
	-I../../lib/notification

StatusBar_CXXFLAGS = \
	@CAPIAPPFWAPPLICATION_CFLAGS@ 	\
	$(EFL_CFLAGS)			\
	$(OPT_CFLAGS)			\
	$(AUL_CFLAGS)			\
	$(SYSCOND_CFLAGS)		\
	$(ICOUWS_CFLAGS)		\
	$(NOTIFICATION_CFLAGS)	\
	-I../../lib/common	\
	-I../../lib/notification \
	-std=c++11

StatusBar_LDADD = \
	../../lib/common/.libs/libico-common.a \
	../../lib/notification/.libs/libico-notification.a \
	@CAPIAPPFWAPPLICATION_LIBS@ 	\
	$(ECOREEVAS_LIBS)		\
	$(EFL_LIBS)				\
	$(OPT_LIBS)				\
	$(AUL_LIBS)				\
	$(SYSCOND_LIBS)			\
	$(ICOUWS_LIBS)			\
	$(NOTIFICATION_LIBS)

SUFFIXES = .edc .edj

BUILT_SOURCES = \
	statusbar.edj

.edc.edj:
	edje_cc $< -id ../../res/org.tizen.ico.statusbar/res/images \
	               -fd ../../res/org.tizen.ico.statusbar/res/images \
	           -sd ../../res/org.tizen.ico.statusbar/res/sounds -o $@

CLEANFILES = $(BUILT_SOURCES)

install-exec-hook:
	mkdir -p $(INSTALL_ROOT)/usr/apps/org.tizen.ico.statusbar/bin
	mkdir -p $(INSTALL_ROOT)/usr/apps/org.tizen.ico.statusbar/res/config
	mkdir -p $(INSTALL_ROOT)/usr/apps/org.tizen.ico.statusbar/res/images
	mkdir -p $(INSTALL_ROOT)/usr/apps/org.tizen.ico.statusbar/res/themes
	mkdir -p $(INSTALL_ROOT)/usr/share/packages
	install -m 0755 StatusBar $(INSTALL_ROOT)/usr/apps/org.tizen.ico.statusbar/bin
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.statusbar/res/images/time*.png $(INSTALL_ROOT)/usr/apps/org.tizen.ico.statusbar/res/images
	install -m 0644 $(top_srcdir)/data/share/packages/org.tizen.ico.statusbar.xml $(INSTALL_ROOT)/usr/share/packages
	install -m 0644 $(top_srcdir)/res/org.tizen.ico.statusbar/res/config/* $(INSTALL_ROOT)/usr/apps/org.tizen.ico.statusbar/res/config
	install -m 0644 *.edj $(INSTALL_ROOT)/usr/apps/org.tizen.ico.statusbar/res/themes

uninstall:
	rm -rf $(INSTALL_ROOT)/usr/apps/org.tizen.ico.statusbar
	rm -f $(INSTALL_ROOT)/usr/share/packages/org.tizen.ico.statusbar.xml
