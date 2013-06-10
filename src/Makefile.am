AM_CFLAGS = $(GCC_CFLAGS)
AM_CPPFLAGS = $(GCC_CFLAGS)

COMMON_LIBS = -lm -lwebsockets -lico-uxf-weston-plugin

noinst_PROGRAMS   = \
	HomeScreen		\
	OnScreen		\
	StatusBar


HomeScreen_SOURCES = \
	home_screen_main.c		\
	home_screen_conf.c	\
	home_screen_parser.c			\
	home_screen_lib.c		\
	home_screen_sound.c		\
	resource_conf.c
HomeScreen_CFLAGS = -I../include $(EFL_CFLAGS) $(OPT_CFLAGS) $(WL_CFLAGS) $(AUL_CFLAGS)
HomeScreen_LDADD = $(EFL_LIBS) $(OPT_LIBS) $(WL_LIBS) $(COMMON_LIBS) ../apps_controller/libico_syc-apc.a ../ico-app-framework/.libs/libico-app-fw-efl.so ../ico-app-framework/.libs/libico-app-fw.so 

OnScreen_SOURCES = \
	on_screen.c		\
	home_screen_conf.c	\
	home_screen_sound.c		\
	resource_conf.c
OnScreen_CFLAGS = -I../include $(EFL_CFLAGS) $(OPT_CFLAGS) $(WL_CFLAGS) $(AUL_CFLAGS)
OnScreen_LDADD = $(EFL_LIBS) $(OPT_LIBS) $(WL_LIBS) $(COMMON_LIBS) $(AUL_LIBS) ../apps_controller/libico_syc-apc.a ../ico-app-framework/.libs/libico-app-fw-efl.so ../ico-app-framework/.libs/libico-app-fw.so

StatusBar_SOURCES = \
	status_bar.c	\
	home_screen_conf.c	\
	home_screen_sound.c		\
	resource_conf.c
StatusBar_CFLAGS = -I../include $(EFL_CFLAGS) $(OPT_CFLAGS) $(WL_CFLAGS) $(AUL_CFLAGS) 
StatusBar_LDADD = $(EFL_LIBS) $(OPT_LIBS) $(WL_LIBS) $(COMMON_LIBS) $(AUL_LIBS) ../apps_controller/libico_syc-apc.a ../ico-app-framework/.libs/libico-app-fw-efl.so ../ico-app-framework/.libs/libico-app-fw.so

SUFFIXES = .edc .edj

BUILT_SOURCES = \
	home_screen_bg.edj	\
	home_screen_touch.edj	\
	appli_list.edj	\
	appli_kill.edj

.edc.edj:  
	edje_cc $< -id ../res/images -fd ../res/images -sd ../res/sounds -o $@

CLEANFILES = $(BUILT_SOURCES)
