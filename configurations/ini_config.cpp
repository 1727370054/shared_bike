#include "ini_config.h"
#include "logger.h"

#include <iostream>
#include <string>
#include <iniparser/iniparser.h>

std::mutex IniConfig::mutex_;
IniConfig* IniConfig::iniConfig_ = nullptr;

IniConfig::IniConfig() : isLoaded_(false)
{
}

IniConfig::~IniConfig()
{
}

bool IniConfig::loadFile(const std::string &path)
{
    dictionary *ini = nullptr;
    ini = iniparser_load(path.c_str());
    if (ini == nullptr)
    {
        fprintf(stderr, "cannot parse file: %s\n", path.c_str());
        return false;
    }

    const char *ip = iniparser_getstring(ini, "database:ip", "127.0.0.1");
    int port = iniparser_getint(ini, "database:port", 3306);
    const char *user = iniparser_getstring(ini, "database:user", "root");
    const char *pwd = iniparser_getstring(ini, "database:pwd", "123456");
    const char *db = iniparser_getstring(ini, "database:db", "shared_bike");
    int sport = iniparser_getint(ini, "server:port", 9090);
    int read_timeout = iniparser_getint(ini, "timeout:read_timeout", 30);
    int write_timeout = iniparser_getint(ini, "timeout:write_timeout", 30);

    config_ = ConfigDef(static_cast<std::string>(ip), static_cast<uint16_t>(port), static_cast<std::string>(user),
                        static_cast<std::string>(pwd), static_cast<std::string>(db), static_cast<uint16_t>(sport)
                        ,read_timeout, write_timeout);

    iniparser_freedict(ini);
    isLoaded_ = true;

    return isLoaded_;
}

const ConfigDef &IniConfig::getConfig()
{
    return config_;
}

IniConfig* IniConfig::getInstance()
{
    if (iniConfig_ == nullptr)
    {
        mutex_.lock();
        if (iniConfig_ == nullptr)
        {
            iniConfig_ = new IniConfig();
        }
        mutex_.unlock();
    }
    return iniConfig_;
}
