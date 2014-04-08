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
 *  @file   CicoSCCommand.h
 *
 *  @brief  This file is definition of CicoSCCommand class
 */
//==========================================================================
#ifndef __CICO_SC_COMMAND_H__
#define __CICO_SC_COMMAND_H__

#include <string>
#include <boost/property_tree/ptree.hpp>

#include "CicoLog.h"

//==========================================================================
/**
 *  @brief  command option variables
 *          This class is abstract class
 */
//==========================================================================
class CicoSCCommandOption
{
public:
    /// default constructor
    CicoSCCommandOption() {}

    /// destructor
    virtual ~CicoSCCommandOption() {}

    /// dump log this class member variables
    virtual void dump(void) {}
};

//==========================================================================
/**
 *  @brief  This class holds the option of window control command
 */
//==========================================================================
class CicoSCCmdWinCtrlOpt : public CicoSCCommandOption
{
public:
    /// default constructor
    CicoSCCmdWinCtrlOpt()
        : nodeid(0), displayid(0), layerid(0),
          zone(""), surfaceid(-1), animationType(0), animation(""), animationTime(0),
          x(-1), y(-1), width(-1), height(-1), raise(-1),
          visible(-1), active(-1), framerate(0), stride(-1), format(-1) {}

    /// destructor
    virtual ~CicoSCCmdWinCtrlOpt() {}

    /// dump log this class member variables
    void dump(void)
    {
        ICO_DBG("WinCtrlOpt: "
                "surfaceid=%08x layerid=%d nodeid=%d zone=%s "
                "animation=%s animaType=%x animaTime=%d "
                "x=%d y=%d w=%d h=%d raise=%d "
                "visible=%d active=%d framerate=%d stride=%d format=%d",
                surfaceid, layerid, nodeid, zone.c_str(),
                animation.c_str(), animationType, animationTime,
                x, y, width, height, raise,
                visible, active, framerate, stride, format);
    }

    int         nodeid;         //!< id of node
    int         displayid;      //!< id of display
    int         layerid;        //!< id of layer
    std::string zone;           //!< name of display zone
    int         surfaceid;      //!< id of surface
    int         animationType;  //!< type of animation
    std::string animation;      //!< name of animation
    int         animationTime;  //!< time of animation
    int         x;              //!< x position of window
    int         y;              //!< y position of window
    int         width;          //!< width of window
    int         height;         //!< height of window
    int         raise;          //!< status of raise(raise/lower)
    int         visible;        //!< status of visible(visible/invisible)
    int         active;         //!< status of active(active/nonactive):
    int         framerate;      //!< frame rate for thumbnail
    int         stride;         //!< stride for thumbnail
    int         format;         //!< format of color for thumbnail
};

//==========================================================================
/**
 *  @brief  This class holds the option of input device control command
 */
//==========================================================================
class CicoSCCmdInputDevCtrlOpt : public CicoSCCommandOption
{
public:
    /// default constructor
    CicoSCCmdInputDevCtrlOpt()
        : device(""), input(-1), surfaceid(-1), deviceno(-1),
          fix(-1), keycode(-1), evtype(-1), evtime(0), evcode(-1),
          evvalue(-1) {}

    /// destructor
    virtual ~CicoSCCmdInputDevCtrlOpt() {}

    /// dump log this class member variables
    void dump(void)
    {
        ICO_DBG("InputCtrlOpt: "
                "device=%s input=%d surfaceid=%08x deviceno=%d "
                "fix=%d keycode=%d evtype=%d evtime=%d evcode=%d evvalue=%d",
                device.c_str(), input, surfaceid, deviceno,
                fix, keycode, evtype, evtime, evcode, evvalue);
    }

    std::string device;     //!< name of input device
    int         input;      //!< number of input
    int         surfaceid;  //!< id of surface
    int         deviceno;   //!< number of device
    int         fix;        //!< status of fix
    int         keycode;    //!< code of key
    int         evtype;     //!< type of event
    int         evtime;     //!< time of event
    int         evcode;     //!< code of event
    int         evvalue;    //!< value of event
};

//==========================================================================
/**
 *  @brief  This class holds the option of user manager command
 */
//==========================================================================
class CicoSCCmdUserMgrOpt : public CicoSCCommandOption
{
public:
    /// default constructor
    CicoSCCmdUserMgrOpt()
        : user(""), pass(""), lastinfo("") {}

    /// destructor
    virtual ~CicoSCCmdUserMgrOpt() {}

    /// dump log this class member variables
    void dump(void)
    {
        ICO_DBG("UserMgrOpt: user=%s pass=%s lastinfo=%s",
                user.c_str(), pass.c_str(), lastinfo.c_str());
    }

    std::string user;
    std::string pass;
    std::string lastinfo;
};

//==========================================================================
/**
 *  @brief  This class holds the option of resource control command
 */
//==========================================================================
class CicoSCCmdResCtrlOpt : public CicoSCCommandOption
{
public:
    /// default constructor
    CicoSCCmdResCtrlOpt()
        : dispres(false), dispzone(""), layerid(-1), winname(""), surfaceid(-1),
          animation(""), animationTime(0),
          ECU(""), display(""), layer(""), layout(""), area(""), dispatchApp(""),
          role(""), resourceID(-1),
          soundres(false), soundzone(""), soundname(""), adjust(0),
          inputres(false), device(""), input(0), type(0) {}

    /// destructor
    virtual ~CicoSCCmdResCtrlOpt() {}

    /// dump log this class member variables
    void dump(void)
    {
        ICO_DBG("ResCtrlOpt: "
                "diapres=%s dispzone=%s layerid=%d winname=%s srufaceid=%08x "
                "animation=%s animatime=%d soundres=%s soundzone=%s"
                "soundname=%s soundid=%d adjust=%d inputres=%s "
                "device=%s input=%d type=%d",
                dispres ? "true" : "false",  dispzone.c_str(), layerid,
                winname.c_str(), surfaceid, animation.c_str(),
                animationTime, soundres ? "true" : "false",
                soundzone.c_str(), soundname.c_str(),
                soundid, adjust, inputres ? "true" : "false",
                device.c_str(), input, type);
        ICO_DBG("ResCtrlOptEx: "
                "dispresEx=%s ECU=%s display=%s layer=%s layout=%s area=%s "
                "dispatchApp=%s role=%s resourceID=%d",
                dispresEx? "true": "false", ECU.c_str(), display.c_str(),
                layer.c_str(), layout.c_str(), area.c_str(), dispatchApp.c_str(),
                role.c_str(), resourceID);
    }

    bool        dispres;         //!< flag of display resource
    std::string dispzone;        //!< name of display zone
    int         layerid;         //!< id of window layer
    std::string winname;         //!< name of window
    int         surfaceid;       //!< id of surface
    std::string animation;       //!< name of animation
    int         animationTime;   //!< time of animation[ms]
    bool        dispresEx;       //!< Expansion flag
    std::string ECU;             //!< name to identify ECU
    std::string display;         //!< name to identify Display in ECU
    std::string layer;           //!< name to identify Layer in Display
    std::string layout;          //!< name to identify Layout in Layer
    std::string area;            //!< name to Output position in Layout
    std::string dispatchApp;     //!< dispatch of application
    std::string role;            //!< role of notice
    int         resourceID;      //!< ID number of resource

    bool        soundres;        //!< flag of sound resource
    std::string soundzone;       //!< name of sound zone
    std::string soundname;       //!< name of sound(stream)
    int         soundid;         //!< id of sound(stream)
    int         adjust;          //!< parameter of adjust

    bool        inputres;        //!< flag of input resource
    std::string device;          //!< name of input device
    int         input;           //!< input

    int         type;            //!<< type of request for display and sound
};

//==========================================================================
/**
 *  @brief  This class holds the option of input device setting command
 */
//==========================================================================
class CicoSCCmdInputDevSettingOpt: public CicoSCCommandOption
{
public:
    /// default constructor
    CicoSCCmdInputDevSettingOpt()
        : winname(""), x(0), y(0), width(0), height(0), hotspot_x(0), hotspot_y(0),
          cursor_x(0), cursor_y(0), cursor_width(0), cursor_height(0), attr(0) {}

    /// destructor
    virtual ~CicoSCCmdInputDevSettingOpt() {}

    /// dump log this class member variables
    void dump(void)
    {
        ICO_DBG("InputDevSettingOpt: winname=%s x=%d y=%d w=%d h=%d attr=%d",
                winname.c_str(), x, y, width, height, attr);
    }

    std::string winname;//!< window name of surface
    short x;            //!< x position of input region
    short y;            //!< y position of input region
    short width;        //!< width of input region
    short height;       //!< height of input region
    short hotspot_x;    //!< hotspot of X relative coordinate
    short hotspot_y;    //!< hotspot of Y relative coordinate
    short cursor_x;     //!< cursor region X coordinate
    short cursor_y;     //!< cursor region Y coordinate
    short cursor_width; //!< cursor region width
    short cursor_height;//!< cursor region height
    int attr;           //!< attribute of input region
};

//==========================================================================
/**
 *  @brief  This class holds command information
 */
//==========================================================================
class CicoSCCommand
{
public:
    // default constructor
    CicoSCCommand();

    // destructor
    ~CicoSCCommand();

    // parse from message of process communication
    int parseMessage(const char *cmdMessage);

    // dump log this class member variables
    void dump(void);

private:
    // get integer value
    int getIntValue(const boost::property_tree::ptree & root,
                    const char* key);

    // get string value
    std::string getStrValue(const boost::property_tree::ptree & root,
                            const char* key);

    // check exist object
    bool isExistObject(const boost::property_tree::ptree & root,
                       const char *key);

    // parse window control options
    void parseWinCtrlOpt(const boost::property_tree::ptree & root);

    // parse input device control options
    void parseInputDevCtrlOpt(const boost::property_tree::ptree & root);

    // parse user control options
    void parseUserMgrOpt(const boost::property_tree::ptree & root);

    // parse resource control options
    void parseResCtrlOpt(const boost::property_tree::ptree & root);

    // parse input device setting options
    void parseInputDevSettingOpt(const boost::property_tree::ptree & root);

public:
    unsigned long       cmdid;  //!< id of command
    std::string         appid;  //!< id of application
    int                 pid;    //!< id of process
    CicoSCCommandOption *opt;   //!< command option instance
};
#endif  // __CICO_SC_COMMAND_H__
// vim:set expandtab ts=4 sw=4:
