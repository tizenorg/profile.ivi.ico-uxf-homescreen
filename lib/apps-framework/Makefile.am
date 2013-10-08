lib_LTLIBRARIES = libico-appfw.la

COMMON_LIBS = -lm -lwebsockets -lico-uxf-weston-plugin

libico_appfw_la_CFLAGS = -I../../include $(OPT_CFLAGS) $(UWS_CFLAGS) \
                          $(AUL_CFLAGS) @ECORE_CFLAGS@ \
                          -I/usr/include/ico-uxf-weston-plugin \
                          -I/usr/include/ico-util

libico_appfw_la_LIBADD =  $(OPT_LIBS) $(WL_LIBS) $(AUL_LIBS) $(UWS_LIBS) $(COMMON_LIBS) @ECORE_LIBS@
libico_appfw_la_LDFLAGS = -version-info 0:9:0 
libico_appfw_la_SOURCES =          \
    ico_syc_appresctl.c \
	ico_syc_common.c    \
	ico_syc_inputctl.c  \
	ico_syc_msg.c       \
	ico_syc_userctl.c   \
	ico_syc_winctl.c

AM_CFLAGS = $(GCC_CFLAGS)
AM_CPPFLAGS = $(GCC_CFLAGS)	\
	-DDATADIR='"$(datadir)"'		\
	-DMODULEDIR='"$(moduledir)"'		\
	-DLIBEXECDIR='"$(libexecdir)"'
AM_LDFLAGS = -module -avoid-version -rpath $(libdir) -lwebsockets

install-exec-hook:
	mkdir -p $(INSTALL_ROOT)/$(includedir)/ico-appfw
	install -m 644 ../../include/ico_syc_application.h $(INSTALL_ROOT)/$(includedir)/ico-appfw
	install -m 644 ../../include/ico_syc_appresctl.h $(INSTALL_ROOT)/$(includedir)/ico-appfw
	install -m 644 ../../include/ico_syc_common.h $(INSTALL_ROOT)/$(includedir)/ico-appfw
	install -m 644 ../../include/ico_syc_error.h $(INSTALL_ROOT)/$(includedir)/ico-appfw
	install -m 644 ../../include/ico_syc_inputctl.h $(INSTALL_ROOT)/$(includedir)/ico-appfw
	install -m 644 ../../include/ico_syc_private.h $(INSTALL_ROOT)/$(includedir)/ico-appfw
	install -m 644 ../../include/ico_syc_privilege.h $(INSTALL_ROOT)/$(includedir)/ico-appfw
	install -m 644 ../../include/ico_syc_type.h $(INSTALL_ROOT)/$(includedir)/ico-appfw
	install -m 644 ../../include/ico_syc_userctl.h $(INSTALL_ROOT)/$(includedir)/ico-appfw
	install -m 644 ../../include/ico_syc_winctl.h $(INSTALL_ROOT)/$(includedir)/ico-appfw

uninstall:
	rm -rf $(INSTALL_ROOT)/$(includedir)/ico-appfw
	rm -rf $(INSTALL_ROOT)/$(libdir)/libico-appfw*

.FORCE :

