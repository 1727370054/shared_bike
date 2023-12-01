#pragma once

#include <string>

class ConfigDef
{
public:
    ConfigDef() = default;
    ConfigDef(const std::string &db_ip, uint16_t db_port, const std::string &db_user,
              const std::string &db_pwd, const std::string &db_name, uint16_t svr_port, int read_timeout, int write_timeout)
        : db_ip_(db_ip), db_port_(db_port), db_user_(db_user), db_pwd_(db_pwd), db_name_(db_name), svr_port_(svr_port)
        ,read_timeout_(read_timeout), write_timeout_(write_timeout)
    {
    }

    ConfigDef &operator=(const ConfigDef &config)
    {
        if (this != &config)
        {
            this->db_ip_ = config.db_ip_;
            this->db_port_ = config.db_port_;
            this->db_user_ = config.db_user_;
            this->db_pwd_ = config.db_pwd_;
            this->db_name_ = config.db_name_;
            this->svr_port_ = config.svr_port_;
            this->read_timeout_ = config.read_timeout_;
            this->write_timeout_ = config.write_timeout_;
        }
        return *this;
    }

    const std::string& getDbIp() const { return db_ip_; }
    const uint16_t getDbPort() const { return db_port_; }
    const std::string& getDbUser() const { return db_user_; }
    const std::string &getDbPwd() const { return db_pwd_; }
    const std::string &getDbName() const { return db_name_; }
    const uint16_t getSvrPort() const { return svr_port_; }
    const int getReadTimeout() const { return read_timeout_; }
    const int getWriteTimeout() const { return write_timeout_; }

    ~ConfigDef() {}

private:
    // 数据库配置
    std::string db_ip_;
    uint16_t db_port_;
    std::string db_user_;
    std::string db_pwd_;
    std::string db_name_;

    // 服务器配置
    uint16_t svr_port_;
    int read_timeout_;
    int write_timeout_;
};
