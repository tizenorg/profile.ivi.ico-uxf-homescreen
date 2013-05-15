/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of user experience library for HomeScreen
 *
 * @date    Feb-28-2013
 */

#ifndef _ICO_UXF_PROTO_H_
#define _ICO_UXF_PROTO_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize/terminate/MainLoop/Others           */
struct wl_display;
int ico_uxf_init(const char *name);
                                        /* Initialize(for general application)  */
int ico_uxf_shutdown(void);
                                        /* Terminate                            */
void ico_uxf_main_loop_iterate(void);
                                        /* Iterate mainLoop                     */
int ico_uxf_callback_set(const unsigned int mask,
                         Ico_Uxf_Event_Cb func, const int arg);
                                        /* Add event callback function          */
int ico_uxf_callback_remove(const unsigned int mask,
                            Ico_Uxf_Event_Cb func, const int arg);
                                        /* Remove event callback function       */

/* Control window                               */
int ico_uxf_window_event_mask(const int window, const unsigned int mask);
                                        /* Set event to receive through the window*/
int ico_uxf_window_attribute_get(const int window, Ico_Uxf_WindowAttr *attr);
                                        /* Get window attribute                 */
int ico_uxf_window_query_display(const int display, Ico_Uxf_DisplayAttr *attr);
                                        /* Get display attribute                */
int ico_uxf_window_query_displays(Ico_Uxf_DisplayAttr attr[], const int num);
                                        /* Get all displays attribute           */
int ico_uxf_window_query_layer(const int display, const int layer,
                               Ico_Uxf_LayerAttr *attr);
                                        /* Get layer attribute                  */
int ico_uxf_window_query_layers(const int display,
                                Ico_Uxf_LayerAttr attr[], const int num);
                                        /* Get all layers attribute             */
int ico_uxf_window_query_windows(const int display,
                                 Ico_Uxf_WindowAttr attr[], const int num);
                                        /* Get all windows attribute            */
void ico_uxf_window_screen_size_get(int *width, int *height);
                                        /* Get display size                     */
int ico_uxf_window_layer(const int window, const int layer);
                                        /* Set window layer                     */
int ico_uxf_layer_visible(const int display, const int layer, const int visible);
                                        /* Show/Hide layer                      */
int ico_uxf_window_show(const int window);
                                        /* Show window                          */
int ico_uxf_window_hide(const int window);
                                        /* Hide window                          */
int ico_uxf_window_visible_raise(const int window,
                                 const int visible, const int raise);
                                        /* Set window show/hide and raise/lower */
int ico_uxf_window_resize(const int window, const int w, const int h);
                                        /* Resize window size                   */
int ico_uxf_window_move(const int window, const int x, const int y);
                                        /* Move window                          */
int ico_uxf_window_raise(const int window);
                                        /* Raise window                         */
int ico_uxf_window_lower(const int window);
                                        /* Lower window                         */
int ico_uxf_window_active(const int window, const int target);
                                        /* Activate window                      */
int ico_uxf_window_control(const char *appid, const int winidx,
                           const int control, const int onoff);
                                        /* Window visible control from AppsController*/
void ico_uxf_window_hook(Ico_Uxf_Hook func);
                                        /* Set window control hook              */
/* Input switch                                 */
Ico_Uxf_InputDev *ico_uxf_inputdev_attribute_get(const int devidx);
                                        /* get input device attribute           */
Ico_Uxf_InputSw  *ico_uxf_inputsw_attribute_get(Ico_Uxf_InputDev *inputdev, const int swidx);
                                        /* get input switch attribute           */
int ico_uxf_input_control(const int add, const char *appid,
                          const char *device, const int input);
                                        /* control input switch                 */
/* Launcher API                                 */
int ico_uxf_process_execute(const char *name);
                                        /* Start application                    */
int ico_uxf_process_terminate(const char *process);
                                        /* Stop application                     */
int ico_uxf_process_attribute_get(const char *process, Ico_Uxf_ProcessAttr *attr);
                                        /* Get application status               */
int ico_uxf_process_is_active(const char *process);
                                        /* Get application activity             */
void *ico_uxf_process_exttable_get(const char *process);
                                        /* Get extended table address           */
int ico_uxf_process_exttable_set(const char *process, void *table);
                                        /* Set extended table address           */
int ico_uxf_process_window_get(const char *process,
                               Ico_Uxf_ProcessWin *attr, const int num);
                                        /* Get all windows of one application   */
int ico_uxf_process_window_get_one(const char *process,
                                   Ico_Uxf_ProcessWin *attr, const int winidx);
                                        /* Get one windows of one application   */
int ico_uxf_process_query_processes(Ico_Uxf_ProcessAttr attr[], const int num);
                                        /* Get all aplications attribute        */

/* Wayland depend function                      */
int ico_uxf_wl_display_fd(void);
                                        /* Get wayland connect file descriptor  */
struct wl_display *ico_uxf_wl_display(void);
                                        /* Get wayland display                  */
struct wl_compositor *ico_uxf_wl_compositor(void);
                                        /* Get wayland compositor               */
struct wl_egl_window *ico_uxf_wl_egl_window(const int window);
                                        /* Get EGL window(surface)              */
struct wl_surface *ico_uxf_wayland_surface(const int window);
                                        /* Get wayland surface                  */
int ico_uxf_wl_display_fd(void);
                                        /* Get wayland connect file descriptor  */
#ifdef __cplusplus
}
#endif
#endif  /* _ICO_UXF_PROTO_H_ */

