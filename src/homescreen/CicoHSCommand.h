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
 *  @file   CicoHSCommand.h
 *
 *  @brief  This file is definition of CicoHSCommand class
 */
//==========================================================================
#ifndef __CICO_HS_COMMAND_H__
#define __CICO_HS_COMMAND_H__

#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>

#include "CicoHSCommandArg.h"
#include <ico_log.h>

//==========================================================================
/**
 *  @brief  command option variables
 *          This class is abstract class
 */
//==========================================================================
class CicoHSCommandOption
{
public:
    /// default constructor
    CicoHSCommandOption() {}

    /// destructor
    virtual ~CicoHSCommandOption() {}

    /// dump log this class member variables
    virtual void dump(void) {}
};

class CicoHSCmdAppCtlOpt : public CicoHSCommandOption
{
public:
    /// default constructor
    CicoHSCmdAppCtlOpt() {}

    /// destructor
    virtual ~CicoHSCmdAppCtlOpt() {}

#if 0           /* for debug    */
    /// dump log this class member variables
    void dump(void)
    {
        ICO_DBG("AppCtlOpt: "
                "tgt_appid=%s tgt_pid=%d zone=%s visible=%d",
                tgt_appid.c_str(), tgt_pid, zone.c_str(), visible);
    }
#endif

    vector<CicoHSCommandArg> arg; //!< appid,pid,zone,visible
};

//==========================================================================
/**
 *  @brief  This class holds command information
 */
//==========================================================================
class CicoHSCommand
{
public:
    // default constructor
    CicoHSCommand();

    // destructor
    ~CicoHSCommand();

    // parse from message of process communication
    int parseMessage(const char *cmdMessage);

    // dump log this class member variables
    void dump(void);

    // get arg
    bool getArg(const boost::property_tree:: ptree& pt, std::vector<CicoHSCommandArg>& vArg);

    // get arg array
    bool getArgArray(const boost::property_tree:: ptree& pt, std::vector<CicoHSCommandArg>& vArg);

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

    // parse application control  options
    void parseAppCtlOpt(const boost::property_tree::ptree & root);

public:
    unsigned long       cmdid;  //!< id of command
    std::string         appid;  //!< id of application
    int                 pid;    //!< id of pid
    int                 seq_no; //!< id of sequence number
    CicoHSCommandOption *opt;   //!< command option instance
};
#endif  // __CICO_HS_COMMAND_H__
// vim:set expandtab ts=4 sw=4:
