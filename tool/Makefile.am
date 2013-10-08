export abs_builddir

wayland_client_lib = -lwayland-client
wayland_ivi_client_lib = -lico-uxf-weston-plugin
wayland_ivi_client_inc = -I/usr/include/ico-uxf-weston-plugin
appfw_client_lib = ../lib/apps-framework/.libs/libico-appfw.so

AM_CFLAGS = $(GCC_CFLAGS) -I../include -I/usr/include/ico-util $(wayland_ivi_client_inc)
AM_LDFLAGS = -module -avoid-version -rpath $(libdir) $(GLIB_LIBS)

bin_PROGRAMS =		\
	ico_clear_screen	\
	ico_change_loginuser

check_LTLIBRARIES = $(TESTS)
check_PROGRAMS = ico_clear_screen ico_login_user

ico_clear_screen_SOURCES = \
	ico_clear_screen.c
ico_clear_screen_LDADD = $(SIMPLE_CLIENT_LIBS) -lico-util $(wayland_ivi_client_lib) $(wayland_client_lib) -lwayland-egl -lEGL -lGLESv2 

ico_change_loginuser = \
	ico_change_loginuser.c
ico_change_loginuser_LDADD = $(SIMPLE_CLIENT_LIBS) $(appfw_client_lib) -lico-util

