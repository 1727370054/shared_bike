#pragma once

#include <string>
#include <log4cpp/Category.hh>

class Logger
{
public:
    bool initLogger(const std::string &log_conf_file);

    static Logger *getInstance() { return &logger_; }

    log4cpp::Category *getHandle() { return category_; }

    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

private:
    Logger() {}
    static Logger logger_;
    log4cpp::Category *category_;
};

#define LOG_INFO Logger::getInstance()->getHandle()->info
#define LOG_DEBUG Logger::getInstance()->getHandle()->debug
#define LOG_ERROR Logger::getInstance()->getHandle()->error
#define LOG_WARN Logger::getInstance()->getHandle()->warn