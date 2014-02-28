/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

//==========================================================================
/**
 *  @file   CicoSCPolicyDef.h
 *
 *  @brief  This file is definition of StateMachine state and event for policy
 */
//==========================================================================
#ifndef __CICO_SC_POLICY_DEF_H__
#define __CICO_SC_POLICY_DEF_H__

// running state id
#define STID_DRIVING                    1000
#define STID_DRIVING_STOPPING           1001
#define STID_DRIVING_RUNNING            1002

// shift position state
#define STID_SHIFTPOS_PARKING           2001
#define STID_SHIFTPOS_REVERSE           2002
#define STID_SHIFTPOS_OTHER             2003

// winker state id
#define STID_WINKER_OFF                 3001
#define STID_WINKER_RIGHT               3002
#define STID_WINKER_LEFT                3003

// camera state id
#define STID_CAMERA                     4000
#define STID_CAMERA_OFF                 4001
#define STID_CAMERA_BACK                4002
#define STID_CAMERA_RIGHT               4003
#define STID_CAMERA_LEFT                4004

// winker state id
#define STID_NIGHTMODE                  5000
#define STID_NIGHTMODE_OFF              5001
#define STID_NIGHTMODE_ON               5002

// driving regulation state id
#define STID_DRVREGULATION              9000
#define STID_DRVREGULATION_OFF          9001
#define STID_DRVREGULATION_ON           9002

// display0 zone state id
#define STID_DISPLAY0_ZONE              10000
#define STID_DISPLAY0_ZONE1             10001
#define STID_DISPLAY0_ZONE2             10002
#define STID_DISPLAY0_ZONE3             10003
#define STID_DISPLAY0_ZONE4             10004
#define STID_DISPLAY0_ZONE5             10005
#define STID_DISPLAY0_ZONE6             10006
#define STID_DISPLAY0_ZONE7             10007
#define STID_DISPLAY0_ZONE8             10008
#define STID_DISPLAY0_ZONE9             10009
#define STID_DISPLAY0_ZONE10            10010
#define STID_DISPLAY0_ZONE11            10011
#define STID_DISPLAY0_ZONE12            10012
#define STID_DISPLAY0_ZONE13            10013
#define STID_DISPLAY0_ZONE14            10014
#define STID_DISPLAY0_ZONE15            10015
#define STID_DISPLAY0_ZONE16            10016
#define STID_DISPLAY0_ZONE17            10017
#define STID_DISPLAY0_ZONE18            10018
#define STID_DISPLAY0_ZONE19            10019
#define STID_DISPLAY0_ZONE20            10020
#define STID_DISPLAY0_NOOWER            10999

// display1 zone state id
#define STID_DISPLAY1_ZONE              11000
#define STID_DISPLAY1_ZONE1             11001
#define STID_DISPLAY1_ZONE2             11002
#define STID_DISPLAY1_ZONE3             11003
#define STID_DISPLAY1_ZONE4             11004
#define STID_DISPLAY1_ZONE5             11005
#define STID_DISPLAY1_ZONE6             11006
#define STID_DISPLAY1_ZONE7             11007
#define STID_DISPLAY1_ZONE8             11008
#define STID_DISPLAY1_ZONE9             11009
#define STID_DISPLAY1_ZONE10            11010
#define STID_DISPLAY1_ZONE11            11011
#define STID_DISPLAY1_ZONE12            11012
#define STID_DISPLAY1_ZONE13            11013
#define STID_DISPLAY1_ZONE14            11014
#define STID_DISPLAY1_ZONE15            11015
#define STID_DISPLAY1_ZONE16            11016
#define STID_DISPLAY1_ZONE17            11017
#define STID_DISPLAY1_ZONE18            11018
#define STID_DISPLAY1_ZONE19            11019
#define STID_DISPLAY1_ZONE20            11020
#define STID_DISPLAY1_NOOWER            11999

// display0 category state id
#define STID_DISPLAY0_CATEGORY          12000
#define STID_DISPLAY0_CATEGORY1         12001
#define STID_DISPLAY0_CATEGORY2         12002
#define STID_DISPLAY0_CATEGORY3         12003
#define STID_DISPLAY0_CATEGORY4         12004
#define STID_DISPLAY0_CATEGORY5         12005
#define STID_DISPLAY0_CATEGORY6         12006
#define STID_DISPLAY0_CATEGORY7         12007
#define STID_DISPLAY0_CATEGORY8         12008
#define STID_DISPLAY0_CATEGORY9         12009
#define STID_DISPLAY0_CATEGORY10        12010
#define STID_DISPLAY0_CATEGORY11        12011
#if 1
#define STID_DISP0_ZONE2_CATEGORY       12100
#define STID_DISP0_ZONE2_CATEGORY1      12101
#define STID_DISP0_ZONE2_CATEGORY2      12102
#define STID_DISP0_ZONE2_CATEGORY3      12103
#define STID_DISP0_ZONE2_CATEGORY4      12104
#define STID_DISP0_ZONE2_CATEGORY5      12105
#define STID_DISP0_ZONE2_CATEGORY6      12106
#define STID_DISP0_ZONE2_CATEGORY7      12107
#define STID_DISP0_ZONE2_CATEGORY8      12108
#define STID_DISP0_ZONE2_CATEGORY9      12109
#define STID_DISP0_ZONE2_CATEGORY10     12110
#define STID_DISP0_ZONE2_CATEGORY11     12111
#define STID_DISP0_ZONE3_CATEGORY       12200
#define STID_DISP0_ZONE3_CATEGORY1      12201
#define STID_DISP0_ZONE3_CATEGORY2      12202
#define STID_DISP0_ZONE3_CATEGORY3      12203
#define STID_DISP0_ZONE3_CATEGORY4      12204
#define STID_DISP0_ZONE3_CATEGORY5      12205
#define STID_DISP0_ZONE3_CATEGORY6      12206
#define STID_DISP0_ZONE3_CATEGORY7      12207
#define STID_DISP0_ZONE3_CATEGORY8      12208
#define STID_DISP0_ZONE3_CATEGORY9      12209
#define STID_DISP0_ZONE3_CATEGORY10     12210
#define STID_DISP0_ZONE3_CATEGORY11     12211
#endif

// display1 category state id
#define STID_DISPLAY1_CATEGORY          13000
#define STID_DISPLAY1_CATEGORY1         13001
#define STID_DISPLAY1_CATEGORY2         13002
#define STID_DISPLAY1_CATEGORY3         13003
#define STID_DISPLAY1_CATEGORY4         13004
#define STID_DISPLAY1_CATEGORY5         13005
#define STID_DISPLAY1_CATEGORY6         13006
#define STID_DISPLAY1_CATEGORY7         13007
#define STID_DISPLAY1_CATEGORY8         13008
#define STID_DISPLAY1_CATEGORY9         13009
#define STID_DISPLAY1_CATEGORY10        13010
#define STID_DISPLAY1_CATEGORY11        13011

// onscreen state id
#define STID_ONSCREEN                   14000
#define STID_ONSCREEN_WARNING           14001
#define STID_ONSCREEN_ATTENTION         14002
#define STID_ONSCREEN_MESSAGE           14003

// interrput display0 zone1 state id
#define STID_INTTERPUT_D0_Z1            15000
#define STID_INTTERPUT_D0_Z1_PRI1       15001
#define STID_INTTERPUT_D0_Z1_PRI2       15002
#define STID_INTTERPUT_D0_Z1_PRI3       15003
#define STID_INTTERPUT_D0_Z1_INVISIBLE  15999

// interrput display0 zone2 state id
#define STID_INTTERPUT_D0_Z2            16000
#define STID_INTTERPUT_D0_Z2_PRI1       16001
#define STID_INTTERPUT_D0_Z2_PRI2       16002
#define STID_INTTERPUT_D0_Z2_PRI3       16003
#define STID_INTTERPUT_D0_Z2_INVISIBLE  16999

// interrput display0 zone3 state id
#define STID_INTTERPUT_D0_Z3            17000
#define STID_INTTERPUT_D0_Z3_PRI1       17001
#define STID_INTTERPUT_D0_Z3_PRI2       17002
#define STID_INTTERPUT_D0_Z3_PRI3       17003
#define STID_INTTERPUT_D0_Z3_INVISIBLE  17999

// sound zone state id
#define STID_SOUND_ZONE                 20000
#define STID_SOUND_ZONE1                20001
#define STID_SOUND_ZONE2                20002
#define STID_SOUND_ZONE3                20003

// display0 category state id
#define STID_SOUND_CATEGORY             22000
#define STID_SOUND_CATEGORY1            22001
#define STID_SOUND_CATEGORY2            22002
#define STID_SOUND_CATEGORY3            22003
#define STID_SOUND_CATEGORY4            22004
#define STID_SOUND_CATEGORY5            22005
#define STID_SOUND_CATEGORY6            22006
#define STID_SOUND_CATEGORY7            22007
#define STID_SOUND_CATEGORY8            22008
#define STID_SOUND_CATEGORY9            22009
#define STID_SOUND_CATEGORY10           22010
#define STID_SOUND_CATEGORY11           22011

// interrupt display0 zone1 state id
#define STID_INTTERPUT_S_Z1             25000
#define STID_INTTERPUT_S_Z1_PRI1        25001
#define STID_INTTERPUT_S_Z1_PRI2        25002
#define STID_INTTERPUT_S_Z1_PRI3        25003
#define STID_INTTERPUT_S_Z1_INVISIBLE   25999

// interrupt display0 zone2 state id
#define STID_INTTERPUT_S_Z2            26000
#define STID_INTTERPUT_S_Z2_PRI1       26001
#define STID_INTTERPUT_S_Z2_PRI2       26002
#define STID_INTTERPUT_S_Z2_PRI3       26003
#define STID_INTTERPUT_S_Z2_INVISIBLE  26999

// interrupt display0 zone3 state id
#define STID_INTTERPUT_S_Z3            27000
#define STID_INTTERPUT_S_Z3_PRI1       27001
#define STID_INTTERPUT_S_Z3_PRI2       27002
#define STID_INTTERPUT_S_Z3_PRI3       27003
#define STID_INTTERPUT_S_Z3_INVISIBLE  27999

// steering switch state id
#define STID_INPUT1_NOTUSING           30001
#define STID_INPUT1_USING              30002
#define STID_INPUT2_NOTUSING           30101
#define STID_INPUT2_USING              30102

// event id
#define EVID_VELOCTY                   1000
#define EVID_SHIFTPOS                  2000
#define EVID_TURN_OFF                  3000
#define EVID_TURN_LEFT                 3001
#define EVID_TURN_RIGHT                3002
#define EVID_CAMERA                    4000
#define EVID_NIGHTMODE                 5000
#define EVID_DRVREGULATION             9000

#define EVID_DISPLAY_ZONE_ACQUIRE      10000
#define EVID_DISPLAY_ZONE_RELEASE      10999

#if 1
#define ZONEID_CENTER_FULL             1
#define ZONEID_CENTER_UPPER            2
#define ZONEID_CENTER_LOWER            3
#endif


#define EVID_DISPLAY0_CATEGORY         12000
#if 1
#define EVID_DISP0_ZONE2_CATEGORY      12100
#define EVID_DISP0_ZONE3_CATEGORY      12200
#endif
#define EVID_DISPLAY1_CATEGORY         13000

#define EVID_ONSCREEN                  14000

#define EVID_INTTERPUT_D0_Z1           15000
#define EVID_INTTERPUT_D0_Z2           16000
#define EVID_INTTERPUT_D0_Z3           17000

#define EVID_SOUND_ZONE                20000
#define EVID_SOUND_ZONE_NOUSE          20999

#define EVID_SOUND_CATEGORY            22000
#define EVID_SOUND_CATEGORY_UNKNOWN    22999

#define EVID_INTTERPUT_S_Z1            25000
#define EVID_INTTERPUT_S_Z1_NOOUTPUT   25999
#define EVID_INTTERPUT_S_Z2            26000
#define EVID_INTTERPUT_S_Z2_NOOUTPUT   26999
#define EVID_INTTERPUT_S_Z3            27000
#define EVID_INTTERPUT_S_Z3_NOOUTPUT   27999

#define EVID_INPUT1_ACQUIRE            30000
#define EVID_INPUT1_RELEASE            30001

#define EVID_INPUT2_ACQUIRE            30100
#define EVID_INPUT2_RELEASE            30101

#endif  // __CICO_SC_POLICY_DEF_H__
// vim:set expandtab ts=4 sw=4:
