/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Display screen clear tool
 *
 * @date    Sep-30-2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "ico_clear_screen.h"
#include <ico-uxf-weston-plugin/ico_window_mgr-client-protocol.h>

static struct display   *_display;
static int  signal_flag = 0;

static void
sigterm_catch(int signo)
{
    signal_flag = 1;

    if (_display->ico_window_mgr)   {
        ico_window_mgr_layout_surface(_display->ico_window_mgr, -1, -1,
                                      -1, -1, -1, -1, 0, "\0");
    }
}

void
wayland_dispatch_nonblock(struct wl_display *display)
{
    int nread;

    /* Check wayland input */
    do {
        /* Flush send data */
        wl_display_flush(display);

        nread = 0;
        if (ioctl(wl_display_get_fd(display), FIONREAD, &nread) < 0) {
            nread = 0;
        }
        if (nread >= 8) {
            /* Read event from wayland */
            wl_display_dispatch(display);
        }
    } while (nread > 0);
}

EGLDisplay
opengl_init(struct wl_display *display, EGLConfig *rconf, EGLContext *rctx)
{
    EGLDisplay dpy; /* EGL dsplay id */
    EGLint major, minor;
    EGLint num_configs;
    EGLConfig conf = 0;
    EGLContext ctx;

    static const EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 1,
        EGL_GREEN_SIZE, 1,
        EGL_BLUE_SIZE, 1,
        EGL_ALPHA_SIZE, 1,
        EGL_DEPTH_SIZE, 1,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };
    static const EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE
    };

    dpy = eglGetDisplay((EGLNativeDisplayType)display);
    if (! dpy) {
        fprintf(stderr, "eglGetDisplay Error\n");
        return NULL;
    }

    if (eglInitialize(dpy, &major, &minor) == EGL_FALSE) {
        fprintf(stderr, "eglInitialize Error\n");
        return NULL;
    }

    if (eglBindAPI(EGL_OPENGL_ES_API) == EGL_FALSE) {
        fprintf(stderr, "eglBindAPI Error\n");
        return NULL;
    }

    if (eglChooseConfig(dpy, config_attribs, &conf, 1, &num_configs) == EGL_FALSE) {
        fprintf(stderr, "eglChooseConfig Error\n");
        return NULL;
    }

    ctx = eglCreateContext(dpy, conf, EGL_NO_CONTEXT, context_attribs);
    if (! ctx) {
        fprintf(stderr, "eglCreateContext Error\n");
        return NULL;
    }
    *rconf = conf;
    *rctx = ctx;

    wayland_dispatch_nonblock(display);

    return(dpy);
}

EGLSurface
opengl_create_window(struct display *display, struct wl_surface *surface,
                     EGLDisplay dpy, EGLConfig conf, EGLContext ctx,
                     const int width, const int height, const unsigned int color,
                     const int displayno, const int posx, const int posy)
{
    struct wl_egl_window *egl_window;
    EGLSurface egl_surface;

    static const EGLint surface_attribs[] = {
        EGL_ALPHA_FORMAT, EGL_ALPHA_FORMAT_PRE, EGL_NONE
    };

    egl_window = wl_egl_window_create(surface, width, height);
    egl_surface = eglCreateWindowSurface(dpy, conf, (EGLNativeWindowType)egl_window,
                                         surface_attribs);
    eglMakeCurrent(dpy, egl_surface, egl_surface, ctx);
    glViewport(0, 0, width, height);

    wayland_dispatch_nonblock(display->display);

    opengl_clear_window(color);

    opengl_swap_buffer(display->display, dpy, egl_surface);

    ico_window_mgr_set_animation(display->ico_window_mgr, 0,
                                 ICO_WINDOW_MGR_ANIMATION_TYPE_HIDE|
                                   ICO_WINDOW_MGR_ANIMATION_TYPE_SHOW,
                                 display->animation, display->animatime);
    ico_window_mgr_layout_surface(display->ico_window_mgr, 0, display->init_layer,
                                  posx, posy, width, height, 1, "clear_screen");
    return(egl_surface);
}

void
opengl_clear_window(const unsigned int color)
{
    double r, g, b, a;

    r = (double)((color>>16) & 0x0ff);
    r = r / 256.0;
    g = (double)((color>>8) & 0x0ff);
    g = g / 256.0;
    b = (double)(color & 0x0ff);
    b = b / 256.0;
    a = (double)((color>>24) & 0x0ff);
    a = (a + 1.0) / 256.0;

    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT| GL_STENCIL_BUFFER_BIT);
}

void
opengl_swap_buffer(struct wl_display *display, EGLDisplay dpy, EGLSurface egl_surface)
{
    eglSwapBuffers(dpy, egl_surface);

    wayland_dispatch_nonblock(display);
}

void
shell_surface_ping(void *data, struct wl_shell_surface *wl_shell_surface, uint32_t serial)
{
}

void
shell_surface_configure(void *data, struct wl_shell_surface *wl_shell_surface,
uint32_t edges, int32_t width, int32_t height)
{
}

void
shell_surface_popup_done(void *data, struct wl_shell_surface *wl_shell_surface)
{
}

void
output_handle_geometry(void *data, struct wl_output *wl_output, int x, int y,
                       int physical_width, int physical_height, int subpixel,
                       const char *make, const char *model, int32_t transform)
{
    struct output *output = (struct output*)data;

    output->x = x;
    output->y = y;
}

void
output_handle_mode(void *data, struct wl_output *wl_output, uint32_t flags,
                   int width, int height, int refresh)
{
    struct output *output = (struct output*)data;

    if (flags & WL_OUTPUT_MODE_CURRENT) {
        output->width = width;
        output->height = height;
    }
}

void
handle_global(void *data, struct wl_registry *registry, uint32_t id,
              const char *interface, uint32_t version) {
    struct display *display = (struct display*)data;
    struct output *output;

    if (strcmp(interface, "wl_compositor") == 0) {
        display->compositor = (struct wl_compositor*)wl_registry_bind(display->registry, id,
                                                               &wl_compositor_interface, 1);
    }
    else if (strcmp(interface, "wl_output") == 0) {
        if (display->num_output < MAX_DISPLAY)  {
            output = (struct output*)malloc(sizeof *output);
            output->display = display;
            output->output = (struct wl_output*)wl_registry_bind(display->registry, id, &wl_output_interface, 1);
            wl_output_add_listener(output->output, &output_listener, output);
            display->output[display->num_output++] = output;
        }
    }
    else if (strcmp(interface, "wl_shell") == 0) {
        display->shell = (struct wl_shell*)wl_registry_bind(display->registry, id, &wl_shell_interface, 1);
    }
    else if (strcmp(interface, "ico_window_mgr") == 0) {
        display->ico_window_mgr = (struct ico_window_mgr *)wl_registry_bind(display->registry, id, &ico_window_mgr_interface, 1);
    }
}

void
surface_enter(void *data, struct wl_surface *wl_surface, struct wl_output *output)
{
    struct surface *surface = (struct surface *)data;

    surface->output = (struct output*)wl_output_get_user_data(output);
}

void
surface_leave(void *data, struct wl_surface *wl_surface, struct wl_output *output)
{
    struct surface *surface = (struct surface*)data;

    surface->output = NULL;

}

void
sleep_with_wayland(struct wl_display *display, int msec)
{
    int nread;
    int fd;

    fd = wl_display_get_fd(display);

    do {
        /* Flush send data */
        wl_display_flush(display);

        /* Check wayland input */
        nread = 0;
        if (ioctl(fd, FIONREAD, &nread) < 0) {
            nread = 0;
        }
        if (nread >= 8) {
            /* Read event from wayland */
            wl_display_dispatch(display);
        }
        msec -= 20;
        if (msec >= 0) usleep(20*1000);
    } while (msec > 0);
}

void
create_surface(struct display *display, const char *title) {
    struct surface *surface;

    if (display->num_surface >= MAX_SURFACE)    {
        exit(1);
    }
    surface = (struct surface *)malloc(sizeof(struct surface));
    assert(surface);
    memset(surface, 0, sizeof(struct surface));
    surface->display = display;
    display->surface[display->num_surface++] = surface;
    surface->surface = wl_compositor_create_surface(display->compositor);
    wl_surface_add_listener(surface->surface, &surface_listener, surface);

    if (display->shell) {
        surface->shell_surface = wl_shell_get_shell_surface(display->shell,
                                                            surface->surface);
        if (surface->shell_surface) {
            wl_shell_surface_add_listener(surface->shell_surface,
                                          &shell_surface_listener, display);
            wl_shell_surface_set_toplevel(surface->shell_surface);
            wl_shell_surface_set_title(surface->shell_surface, title);
        }
    }
    wl_display_flush(display->display);
    poll(NULL, 0, 100);

    wl_display_roundtrip(display->display);

    surface->dpy = opengl_init(display->display, &surface->conf, &surface->ctx);
    if (surface->dpy) {
        surface->egl_surface = opengl_create_window(display,
                                                    surface->surface,
                                                    surface->dpy,
                                                    surface->conf,
                                                    surface->ctx,
                                                    display->init_width,
                                                    display->init_height,
                                                    display->init_color,
                                                    display->displayno,
                                                    display->init_posx,
                                                    display->init_posy);
        clear_surface(surface);
    }
}

void
clear_surface(struct surface *surface)
{
    struct display *display = surface->display;

    opengl_clear_window(display->init_color);
    opengl_swap_buffer(display->display,
                       surface->dpy, surface->egl_surface);
}

int main(int argc, char *argv[])
{
    int         i;
    char        sname[64];

    _display = malloc(sizeof(struct display));
    memset(_display, 0, sizeof(struct display));

    _display->displayno = 0;
    _display->init_color = 0xff000000;
    _display->init_width = 1920;
    _display->init_height = 1920;
    _display->init_layer = 201;
    strcpy(_display->animation, "fade");
    _display->animatime = 600;

    for (i = 1; i < (argc-1); i++)  {
        if (strcasecmp(argv[i], "-display") == 0)   {
            i++;
            _display->displayno = strtol(argv[i], (char **)0, 0);
        }
        else if (strcasecmp(argv[i], "-width") == 0)    {
            i++;
            _display->init_width = strtol(argv[i], (char **)0, 0);
        }
        else if (strcasecmp(argv[i], "-height") == 0)   {
            i++;
            _display->init_height = strtol(argv[i], (char **)0, 0);
        }
        else if (strcasecmp(argv[i], "-color") == 0)   {
            i++;
            _display->init_color = strtoul(argv[i], (char **)0, 0);
        }
        else if (strcasecmp(argv[i], "-layer") == 0)   {
            i++;
            _display->init_layer = strtol(argv[i], (char **)0, 0);
        }
        else if (strcasecmp(argv[i], "-animation") == 0)   {
            i++;
            memset(_display->animation, 0, sizeof(_display->animation));
            strncpy(_display->animation, argv[i], sizeof(_display->animation)-1);
        }
        else if (strcasecmp(argv[i], "-animatime") == 0)   {
            i++;
            _display->animatime = strtol(argv[i], (char **)0, 0);
        }
        else    {
            fprintf(stderr,
                    "usage: %s [-display no][-layer layer][-color aarrggbb][-width width]"
                    "[-height height]\n", argv[0]);
            exit(1);
        }
    }

    _display->display = wl_display_connect(NULL);
    if (! _display->display) {
        fprintf(stderr, "can not connect to wayland\n");
        return 1;
    }
    _display->registry = wl_display_get_registry(_display->display);
    wl_registry_add_listener(_display->registry, &registry_listener, _display);

    wl_display_dispatch(_display->display);
    sleep_with_wayland(_display->display, 300);

    sprintf(sname, "Clear-Screen-%d-%d", getpid(), _display->displayno);
    create_surface(_display, sname);

    signal_flag = 0;
    signal(SIGTERM, sigterm_catch);

    while (signal_flag == 0)   {
        sleep_with_wayland(_display->display, 50);
    }
    for (i = 0; i < (450/50); i++)  {
        sleep_with_wayland(_display->display, 50);
    }
    return 0;
}

