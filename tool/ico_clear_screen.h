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

#ifndef _CLEAR_SCREEN_H_
#define _CLEAR_SCREEN_H_

#include <sys/ioctl.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include <wayland-egl.h>
#include <wayland-util.h>
#include <poll.h>
#include <assert.h>

#define MAX_CON_NAME 127
#define MAX_DISPLAY 4
#define MAX_SURFACE 4

struct display {
    struct wl_display *display;
    struct wl_registry *registry;
    struct wl_compositor *compositor;
    struct wl_shell *shell;
    struct ico_window_mgr *ico_window_mgr;
    struct ico_exinput *ico_exinput;
    struct input *input;
    int num_output;
    struct output *output[MAX_DISPLAY];
    int num_surface;
    struct surface *surface[MAX_SURFACE];
    unsigned int init_color;
    int init_width;
    int init_height;
    int init_layer;
    int displayno;
    int init_posx;
    int init_posy;
    char animation[64];
    int animatime;
    char connect[MAX_CON_NAME + 1];
};

struct input {
    struct display *display;
    struct wl_seat *seat;
    struct wl_pointer *pointer;
    struct wl_keyboard *keyboard;
    float x, y;
    uint32_t button_mask;
    struct surface *pointer_focus;
    struct surface *keyboard_focus;
    uint32_t last_key, last_key_state;
};

struct output {
    struct display *display;
    struct wl_output *output;
    int x, y;
    int width, height;
    int trans;
};

struct surface {
    struct display *display;
    struct wl_surface *surface;
    struct wl_shell_surface *shell_surface;
    struct output *output;
    EGLDisplay dpy;
    EGLConfig conf;
    EGLContext ctx;
    EGLSurface egl_surface;
};

void wayland_dispatch_nonblock(struct wl_display *display);
void sleep_with_wayland(struct wl_display *display, int msec);
void wait_with_wayland(struct wl_display *display, int msec, int *endflag);
int sec_str_2_value(const char *ssec);
EGLDisplay opengl_init(struct wl_display *display, EGLConfig *rconf, EGLContext *rctx);
EGLSurface opengl_create_window(struct display *display, struct wl_surface *surface,
                                EGLDisplay dpy, EGLConfig conf, EGLContext ctx,
                                const int width, const int height, const unsigned int color,
                                const int displayno, const int posx, const int posy);
void opengl_clear_window(const unsigned int color);
void opengl_swap_buffer(struct wl_display *display, EGLDisplay dpy, EGLSurface egl_surface);
void create_surface(struct display *display, const char *title);
void clear_surface(struct surface *surface);
void shell_surface_ping(void *data, struct wl_shell_surface *wl_shell_surface, uint32_t serial);
void shell_surface_configure(void *data, struct wl_shell_surface *wl_shell_surface,
                             uint32_t edges, int32_t width, int32_t height);
void shell_surface_popup_done(void *data, struct wl_shell_surface *wl_shell_surface);
void output_handle_geometry(void *data, struct wl_output *wl_output, int x, int y,
                            int physical_width, int physical_height, int subpixel,
                            const char *make, const char *model, int32_t transform);
void output_handle_mode(void *data, struct wl_output *wl_output, uint32_t flags,
                        int width, int height, int refresh);
void handle_global(void *data, struct wl_registry *registry, uint32_t id,
                   const char *interface, uint32_t version);
void surface_enter(void *data, struct wl_surface *wl_surface, struct wl_output *output);
void surface_leave(void *data, struct wl_surface *wl_surface, struct wl_output *output);

const struct wl_shell_surface_listener shell_surface_listener = {
    shell_surface_ping,
    shell_surface_configure,
    shell_surface_popup_done
};

const struct wl_output_listener output_listener = {
    output_handle_geometry,
    output_handle_mode
};

const struct wl_registry_listener registry_listener = {
    handle_global
};

const struct wl_surface_listener surface_listener = {
    surface_enter,
    surface_leave
};

#endif  /*_CLEAR_SCREEN_H_*/
