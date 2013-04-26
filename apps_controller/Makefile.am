noinst_LIBRARIES = libico_syc-apc.a

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

AM_LDFLAGS = -module -avoid-version -rpath $(libdir) -lwayland-egl -lEGL -lGLESv2

.FORCE :

