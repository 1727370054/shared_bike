#pragma once

#include <string>
#include <mutex>

#include "config_def.h"

class IniConfig
{
public:
    
    ~IniConfig();

    bool loadFile(const std::string &path);
    const ConfigDef &getConfig();

    static IniConfig* getInstance();

private:
    IniConfig();
    IniConfig(const IniConfig&) = delete;
    IniConfig& operator=(const IniConfig&) = delete;

private:
    ConfigDef config_;
    bool isLoaded_;
    static IniConfig* iniConfig_;
    static std::mutex mutex_;
};