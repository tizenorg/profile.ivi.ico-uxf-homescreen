lib_LTLIBRARIES = libico-app-fw.la	\
				libico-app-fw-efl.la

#include_HEADERS =	\
#	ico_apf.h					\
#	ico_apf_error.h				\
#	ico_apf_resource_control.h	\
#	ico_apf_ecore.h
	
libico_app_fw_la_CFLAGS = -I../include -I/usr/include/ico-uxf-weston-plugin $(AUL_CFLAGS) $(SLPPKGMGR_CFLAGS) @GLIB_CFLAGS@
libico_app_fw_la_LIBADD = $(WL_LIBS) $(AUL_LIBS) $(SLPPKGMGR_LIBS) @GLIB_LIBS@
libico_app_fw_la_LDFLAGS = -version-info 0:5:0 
libico_app_fw_la_SOURCES =          \
	ico_uxf_conf_common.c		\
	ico_uxf_conf_sys.c			\
	ico_uxf_conf_app.c			\
	ico_uxf_init.c				\
	ico_uxf_launcher.c			\
	ico_uxf_window.c			\
	ico_uxf_inputdev.c			\
	ico_apf_log.c				\
	ico_apf_communication.c		\
	ico_apf_resource_control.c	\
	ico_apf_appmgr.c

libico_app_fw_efl_la_CFLAGS = -I../include -I/usr/include/ico-uxf-weston-plugin $(EFL_CFLAGS) @GLIB_CFLAGS@
libico_app_fw_efl_la_LIBADD = $(WL_LIBS) $(EFL_LIBS) @GLIB_LIBS@
libico_app_fw_efl_la_LDFLAGS = -version-info 0:5:0
libico_app_fw_efl_la_SOURCES =          \
	ico_apf_ecore.c             \
	ico_uxf_conf_ecore.c

AM_CFLAGS = $(GCC_CFLAGS)
AM_CPPFLAGS = $(GCC_CFLAGS)	\
	-DDATADIR='"$(datadir)"'		\
	-DMODULEDIR='"$(moduledir)"'		\
	-DLIBEXECDIR='"$(libexecdir)"'
AM_LDFLAGS = -module -avoid-version -rpath $(libdir) -lwayland-egl -lEGL -lGLESv2 

.FORCE :

