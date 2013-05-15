/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of user experience liblary for HomeScreen(define symbols)
 *
 * @date    Feb-28-2013
 */

#ifndef _ICO_UXF_DEF_H_
#define _ICO_UXF_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

/* ECU type                                 */
#define ICO_UXF_HOSTTYPE_CENTER     0               /* Center display               */
#define ICO_UXF_HOSTTYPE_METER      1               /* Meter display                */
#define ICO_UXF_HOSTTYPE_REMOTE     2               /* Information device(mobile device)*/
#define ICO_UXF_HOSTTYPE_PASSENGER  3               /* Passenger display            */
#define ICO_UXF_HOSTTYPE_REAR       4               /* Rear-seat entertainment      */
#define ICO_UXF_HOSTTYPE_REARLEFT   5               /* Rear-seat entertainment(Left)*/
#define ICO_UXF_HOSTTYPE_REARRIGHT  6               /* Rear-seat entertainment(right)*/

/* Default DisplayId                        */
#define ICO_UXF_DISPLAY_DEFAULT     0               /* Default display              */

/* Display type                             */
#define ICO_UXF_DISPLAYTYPE_CENTER      0           /* Center display               */
#define ICO_UXF_DISPLAYTYPE_METER       1           /* Meter display                */
#define ICO_UXF_DISPLAYTYPE_REMOTE      2           /* Information device(mobile device)*/
#define ICO_UXF_DISPLAYTYPE_PASSENGER   3           /* Passenger display            */
#define ICO_UXF_DISPLAYTYPE_REAR        4           /* Rear-seat display            */
#define ICO_UXF_DISPLAYTYPE_REARLEFT    5           /* Rear-seat display(left)      */
#define ICO_UXF_DISPLAYTYPE_REARRIGHT   6           /* Rear-seat display(right)     */

/* Window active status                     */
#define ICO_UXF_WINDOW_INACTIVE         0           /* not active                   */
#define ICO_UXF_WINDOW_POINTER_ACTIVE   1           /* pointer active               */
#define ICO_UXF_WINDOW_KEYBOARD_ACTIVE  2           /* keyboard active              */
#define ICO_UXF_WINDOW_SELECT           4           /* window selected by operation */

/* Policy                                   */
#define ICO_UXF_POLICY_ALWAYS           0           /* No control                   */
#define ICO_UXF_POLICY_RUNNING          1           /* Runnning only                */
#define ICO_UXF_POLICY_PARKED           2           /* Parked only                  */
#define ICO_UXF_POLICY_SHIFT_PARKING    3           /* Shift-position is park       */
#define ICO_UXF_POLICY_SHIFT_REVERSES   4           /* Shift-position is reverses   */
#define ICO_UXF_POLICY_BLINKER_LEFT     5           /* Blinker is left              */
#define ICO_UXF_POLICY_BLINKER_RIGHT    6           /* Blinker is right             */

/* Privilege                                */
#define ICO_UXF_PRIVILEGE_ALMIGHTY      0           /* almighty privilege           */
#define ICO_UXF_PRIVILEGE_SYSTEM        1           /* system level                 */
#define ICO_UXF_PRIVILEGE_SYSTEM_AUDIO  2           /* system level(audio only)     */
#define ICO_UXF_PRIVILEGE_SYSTEM_VISIBLE 3          /* system level(visible only)   */
#define ICO_UXF_PRIVILEGE_MAKER         4           /* maker level                  */
#define ICO_UXF_PRIVILEGE_CERTIFICATE   5           /* certificated                 */
#define ICO_UXF_PRIVILEGE_NONE          6           /* no privilege                 */

/* Event values                             */
#define ICO_UXF_EVENT_ALL               0xffffffff  /* Mask value of all events     */
#define ICO_UXF_EVENT_VISIBLE           0x00000001  /* Change window visibility     */
#define ICO_UXF_EVENT_RESIZE            0x00000002  /* Change window position/size  */
#define ICO_UXF_EVENT_VISIBLE_REQ       0x00000004  /* Request for window visibility*/
#define ICO_UXF_EVENT_RESIZE_REQ        0x00000008  /* request for window position/size*/
#define ICO_UXF_EVENT_NEWWINDOW         0x00000010  /* Created new window           */
#define ICO_UXF_EVENT_DESTORYWINDOW     0x00000020  /* Destroyed a window           */
#define ICO_UXF_EVENT_ACTIVEWINDOW      0x00000040  /* Change window active status  */
#define ICO_UXF_EVENT_EXECPROCESS       0x00000100  /* Application started          */
#define ICO_UXF_EVENT_TERMPROCESS       0x00000200  /* Application stopped          */
#define ICO_UXF_EVENT_EXINPUT           0x00200000  /* extra input event            */

#define ICO_UXF_EVENT_VALIDALL          0x8020037f  /* Set of the event with a meaning*/

/* Application status                       */
#define ICO_UXF_PROCSTATUS_STOP     0x00000000      /* Stopped                      */
#define ICO_UXF_PROCSTATUS_INIT     0x00000001      /* Initializing                 */
#define ICO_UXF_PROCSTATUS_RUN      0x00000002      /* Running                      */
#define ICO_UXF_PROCSTATUS_TERM     0x00000004      /* Terminating                  */

/* Window controll hook code                */
#define ICO_UXF_HOOK_WINDOW_CREATE_MAIN     0x00    /* created main window          */
#define ICO_UXF_HOOK_WINDOW_CREATE_SUB      0x01    /* created sub window           */
#define ICO_UXF_HOOK_WINDOW_DESTORY_MAIN    0x10    /* destoryed main window        */
#define ICO_UXF_HOOK_WINDOW_DESTORY_SUB     0x11    /* destoryed sub window         */

/* API return code                          */
#define ICO_UXF_EOK             0                   /* OK                           */
#define ICO_UXF_EPERM           -1                  /* Operation not permitted      */
#define ICO_UXF_ENOENT          -2                  /* No such object               */
#define ICO_UXF_ESRCH           -3                  /* Not initialized              */
#define ICO_UXF_EIO             -5                  /* I/O(send/receive) error      */
#define ICO_UXF_ENXIO           -6                  /* Not exist                    */
#define ICO_UXF_E2BIG           -7                  /* Buffer size too smale        */
#define ICO_UXF_EBADF           -9                  /* Illegal data type            */
#define ICO_UXF_EAGAIN          -11                 /* Try again                    */
#define ICO_UXF_ENOMEM          -12                 /* Out of memory                */
#define ICO_UXF_EFAULT          -14                 /* Bad address                  */
#define ICO_UXF_EBUSY           -16                 /* Not available now            */
#define ICO_UXF_EEXIST          -17                 /* Multiple define              */
#define ICO_UXF_EINVAL          -22                 /* Invalid argument             */
#define ICO_UXF_ENOSYS          -38                 /* System error                 */

/* AppsControol control flag                */
#define ICO_UXF_APPSCTL_INVISIBLE   0x01            /* invisibled by AppsController */
#define ICO_UXF_APPSCTL_REGULATION  0x02            /* invisibled by Regulation     */
#define ICO_UXF_APPSCTL_TEMPVISIBLE 0x10            /* tempolary visible for menu   */

/* Log output level                         */
#define ICO_UXF_LOGLEVEL_ERROR    4                 /* Error                        */
#define ICO_UXF_LOGLEVEL_CRITICAL 8                 /* Critical                     */
#define ICO_UXF_LOGLEVEL_WARNING 16                 /* Warning                      */
#define ICO_UXF_LOGLEVEL_INFO    64                 /* Information                  */
#define ICO_UXF_LOGLEVEL_DEBUG  128                 /* Debug write                  */

/* Macros for generate hash value               */
extern int ico_uxf_misc_hashByName(const char *name);
#define ICO_UXF_MISC_HASHSIZE       64      /* Hash table size(must be 2's factorial*/
#define ICO_UXF_MISC_HASHBYID(v)        (v & (ICO_UXF_MISC_HASHSIZE-1))
#define ICO_UXF_MISC_HASHBYNAME(name)   ico_uxf_misc_hashByName(name)

#ifdef __cplusplus
}
#endif
#endif  /* _ICO_UXF_DEF_H_ */

