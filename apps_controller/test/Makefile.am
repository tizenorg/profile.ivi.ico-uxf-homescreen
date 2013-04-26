noinst_LIBRARIES = libico_syc-apc.a

noinst_PROGRAMS =       \
    apc_test

includedir = /usr/include/ico-uxf-weston-plugin

libico_syc_apc_a_CFLAGS = -I/usr/include/ico-uxf-weston-plugin -I../include $(EFL_CFLAGS) @DBUS_CFLAGS@ @GLIB_CFLAGS@
libico_syc_apc_a_SOURCES =          \
	ico_syc_apc_control.c		\
	ico_syc_apc_regulation.c

AM_CFLAGS = $(GCC_CFLAGS)
AM_CPPFLAGS = $(GCC_CFLAGS)	\
	-DDATADIR='"$(datadir)"'		\
	-DMODULEDIR='"$(moduledir)"'		\
	-DLIBEXECDIR='"$(libexecdir)"'

check_PROGRAMS = apc_test

AM_LDFLAGS = -module -avoid-version -rpath $(libdir) -lwayland-egl -lEGL -lGLESv2

apc_test_lib = libico_syc-apc.a ../ico-app-framework/.libs/libico-app-fw-efl.so ../ico-app-framework/.libs/libico-app-fw.so  -lico-uxf-weston-plugin -lwebsockets $(AUL_LIBS)
apc_test_dbus = -ldbus-1

apc_test_SOURCE = apc_test.c
apc_test_CFLAGS = -I/usr/include/ico-uxf-weston-plugin -I../include $(EFL_CFLAGS) @DBUS_CFLAGS@ @GLIB_CFLAGS@
apc_test_LDADD = $(apc_test_lib) $(apc_test_dbus) -lrt -lm

.FORCE :

