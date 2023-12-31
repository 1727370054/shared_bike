#include "logger.h"

#include <iostream>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/RemoteSyslogAppender.hh>
#include <log4cpp/PropertyConfigurator.hh>

Logger Logger::logger_;

bool Logger::initLogger(const std::string &log_conf_file)
{
    try
    {
        log4cpp::PropertyConfigurator::configure(log_conf_file);
    }
    catch (log4cpp::ConfigureFailure &f)
    {
        std::cerr << "load log config file" << log_conf_file.c_str() << " failed with result: " << f.what() << std::endl;
        return false;
    }

    category_ = &log4cpp::Category::getRoot();

    return true;
}
