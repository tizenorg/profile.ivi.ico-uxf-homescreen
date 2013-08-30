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
 *  @file   CicoSCCommandParser.h
 *
 *  @brief  
 */
//==========================================================================
#ifndef __CICO_SC_COMMAND_PARSER_H__
#define __CICO_SC_COMMAND_PARSER_H__

#include <string>
using namespace std;

#include "CicoLog.h"

class CicoSCCommandOption
{
public:
    CicoSCCommandOption() {};
    virtual ~CicoSCCommandOption() {};
};

class CicoSCCmdWinCtrlOpt : public CicoSCCommandOption
{
public:
    CicoSCCmdWinCtrlOpt()
        : nodeid(0), displayid(0), layerid(0), layerAttr(0),
          zone(""), surfaceid(-1), animation(""), animationTime(-1),
          x(-1), y(-1), width(-1), height(-1), raise(-1),
          visible(-1), active(-1), framerate(0), stride(-1), format(-1) {}
    void dumpOpt(void)
    {
        ICO_DBG("WinCtrlOpt : "
                "surfaceid(%d) layerid(%d) nodeid(%d) zone(%s) "
                "animation(%s) animationTime(%d) layerAttr(%d) "
                "x(%d) y(%d) width(%d) height(%d) raise(%d) "
                "visible(%d) active(%d) framerate(%d) stride(%d) format(%d)",
                surfaceid, layerid, nodeid, zone.c_str(),
                animation.c_str(), animationTime, layerAttr,
                x, y, width, height, raise,
                visible, active, framerate, stride, format);

    }

    int    nodeid;
    int    displayid;
    int    layerid;
    int    layerAttr;
    string zone;
    int    surfaceid;
    string animation;
    int    animationTime;
    int    x;
    int    y;
    int    width;
    int    height;
    int    raise;
    int    visible;
    int    active;
    int    framerate;
    int    stride;
    int    format;
};

class CicoSCCmdInputDevCtrlOpt : public CicoSCCommandOption
{
public:
    CicoSCCmdInputDevCtrlOpt()
        : device(""), inputNum(-1), surfaceid(-1), deviceno(-1),
          fix(-1), keyCode(-1), eventType(-1), eventCode(-1),
          eventValue(-1) {}

    string device;
    int    inputNum;
    int    surfaceid;
    int    deviceno;
    int    fix;
    int    keyCode;
    int    eventType;
    int    eventCode;
    int    eventValue;
};

class CicoSCCmdUserMgrOpt : public CicoSCCommandOption
{
public:
    CicoSCCmdUserMgrOpt()
        : user(""), pass("") {}

    string user;
    string pass;
};

class CicoSCCmdResCtrlOpt : public CicoSCCommandOption
{
public:
    CicoSCCmdResCtrlOpt()
        : displayres(false), displayZone(""), windowName(""), surfaceid(-1),
          soundres(false), soundZone(""), soundName(""), adjust(0),
          inputres(false), device(""), input(0), type(0) {}

    bool displayres;
    string displayZone;
    string windowName;
    int surfaceid;

    bool soundres;
    string soundZone;
    string soundName;
    int soundid;
    int adjust;

    bool inputres;
    string device;
    int input;

    int type;

};

class CicoSCCmdInputDevSettingOpt: public CicoSCCommandOption
{
public:
    CicoSCCmdInputDevSettingOpt()
        : surfaceid(-1), x(-1), y(-1), width(-1), height(-1), attr(-1) {}

    int surfaceid;
    int x;
    int y;
    int width;
    int height;
    int attr;
};

class CicoSCCommand
{
public:
    CicoSCCommand()
        : appid(""), pid(-1), cmdid(0), opt(NULL) {}
    ~CicoSCCommand() { delete opt; }

public:
    string             appid;
    int                pid;
    unsigned long      cmdid;
    CicoSCCommandOption* opt;
};

#include <string>
#include <boost/property_tree/ptree.hpp>
using namespace boost::property_tree;

class CicoSCCommandParser
{
public:
    CicoSCCommandParser();

    ~CicoSCCommandParser();

    int parse(const char *cmdMessage, CicoSCCommand & cmd);

private:
    int getIntValue(const ptree & root, const char* key);
    string getStrValue(const ptree & root, const char* key);
    bool isExistObject(const ptree & root, const char *key);

    void parseWinCtrlOpt(const ptree & root, CicoSCCommand & cmd);
    void parseInputDevCtrlOpt(const ptree & root, CicoSCCommand & cmd);
    void parseUserMgrOpt(const ptree & root, CicoSCCommand & cmd);
    void parseResCtrlOpt(const ptree & root, CicoSCCommand & cmd);
    void parseInputDevSettingOpt(const ptree & root, CicoSCCommand & cmd);

};
#endif  // __CICO_SC_COMMAND_PARSER_H__
// vim:set expandtab ts=4 sw=4:
