lib_LTLIBRARIES = libico-app-fw.la	\
				libico-app-fw-efl.la

#include_HEADERS =	\
#	ico_apf.h					\
#	ico_apf_error.h				\
#	ico_apf_resource_control.h	\
#	ico_apf_ecore.h
	
noinst_PROGRAMS =	\
	test_server		\
	test_client

libico_app_fw_la_CFLAGS = -I../include -I/usr/include/ico-uxf-weston-plugin $(AUL_CFLAGS) @GLIB_CFLAGS@
libico_app_fw_la_LDFLAGS = -version-info 0:5:0
libico_app_fw_la_SOURCES =          \
	ico_syc_log.c				\
	ico_uxfw_conf_common.c		\
	ico_uxfw_conf_sys.c			\
	ico_uxfw_conf_app.c			\
	ico_uxfw_init.c				\
	ico_uxfw_launcher.c			\
	ico_uxfw_window.c			\
	ico_apf_communication.c		\
	ico_apf_resource_control.c	\
	ico_apf_appmgr.c

libico_app_fw_efl_la_CFLAGS = -I../include $(EFL_CFLAGS) @GLIB_CFLAGS@
libico_app_fw_efl_la_LDFLAGS = -version-info 0:5:0
libico_app_fw_efl_la_SOURCES =          \
	ico_apf_ecore.c

AM_CFLAGS = $(GCC_CFLAGS)
AM_CPPFLAGS = $(GCC_CFLAGS)	\
	-DDATADIR='"$(datadir)"'		\
	-DMODULEDIR='"$(moduledir)"'		\
	-DLIBEXECDIR='"$(libexecdir)"'
AM_LDFLAGS = -module -avoid-version -rpath $(libdir) -lwayland-egl -lEGL -lGLESv2

check_PROGRAMS =	test_client

test_client_lib = ./.libs/libico-app-fw-efl.so ./.libs/libico-app-fw.so  -lico-uxf-weston-plugin -laul
test_server_lib = ./.libs/libico-app-fw-efl.so ./.libs/libico-app-fw.so  -lico-uxf-weston-plugin -laul
test_efl_libs = -lecore_evas -lecore -levas -lecore_wayland -lelementary

test_client_SOURCE = test_client.c
test_client_CFLAGS = -I../include $(EFL_CFLAGS) @GLIB_CFLAGS@
test_client_LDADD = $(test_client_lib) $(test_efl_libs) -lwebsockets -lrt -lm

test_server_SOURCE = test_client.c
test_server_CFLAGS = -I../include $(EFL_CFLAGS) @GLIB_CFLAGS@
test_server_LDADD = $(test_server_lib) $(test_efl_libs) -lwebsockets -lrt -lm

.FORCE :

