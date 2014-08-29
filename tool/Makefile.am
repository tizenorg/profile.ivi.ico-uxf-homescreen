export abs_builddir

wayland_client_lib = -lwayland-client
wayland_ivi_client_lib = $(GENIVI_LIBS)
appfw_client_lib = ../lib/apps-framework/.libs/libico-appfw.so

AM_CFLAGS = $(GCC_CFLAGS) -I../include -I/usr/include/ico-util
AM_LDFLAGS = -module -avoid-version -rpath $(libdir) $(GLIB_LIBS)

bin_PROGRAMS =		\
	ico_change_loginuser

check_LTLIBRARIES = $(TESTS)
check_PROGRAMS = ico_login_user

ico_change_loginuser = \
	ico_change_loginuser.c
ico_change_loginuser_LDADD = $(SIMPLE_CLIENT_LIBS) $(appfw_client_lib) -lico-util

