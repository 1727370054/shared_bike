#include "mysql_connection.h"

MySQLConnection::MySQLConnection()
{
    mysql_ = (MYSQL*)malloc(sizeof(MYSQL));
}

MySQLConnection::~MySQLConnection()
{
    if (mysql_ != nullptr)
    {
        mysql_close(mysql_);
        mysql_ = nullptr;
    }
}

bool MySQLConnection::Init(const char* szHost, int16_t nPort, const char* szUser, const char* szPasswd, const char* szDb)
{
    LOG_INFO("MYSQL enter Init \n");
    if ((mysql_ = mysql_init(mysql_)) == NULL)
    {
        LOG_ERROR("init mysql failed %s, errno: %d \n", this->GetErrInfo(), errno);
        return false;
    }

    char cAuto = 1;
    if (mysql_options(mysql_, MYSQL_OPT_RECONNECT, &cAuto) != 0)
    {
        LOG_ERROR("mysql_options set MYSQL_OPT_RECONNECT failed\n");
    }
    
    if (mysql_real_connect(mysql_, szHost, szUser, szPasswd, szDb, nPort, NULL, 0) == NULL)
    {
        LOG_ERROR("connect mysql failed %s, errno: %d \n", this->GetErrInfo(), errno);
        return false;
    }

    mysql_set_character_set(mysql_, "utf8");

    return true;
}

bool MySQLConnection::Execute(const char* szSql)
{
    if (mysql_real_query(mysql_, szSql, strlen(szSql)) != 0)
    {
        if (mysql_errno(mysql_) == CR_SERVER_GONE_ERROR)
        {
            Reconnct();
        }
        return false;
    }
    return true;
}

bool MySQLConnection::Execute(const char* szSql, SqlRecordSet& recordSet)
{
    if (mysql_real_query(mysql_, szSql, strlen(szSql)) != 0)
    {
        if (mysql_errno(mysql_) == CR_SERVER_GONE_ERROR)
        {
            Reconnct();
        }
        return false;
    }

    MYSQL_RES* mysql_res = mysql_store_result(mysql_);
    if (!mysql_res)
    {
        return false;
    }

    recordSet.SetResult(mysql_res);

    return true;
}

int MySQLConnection::EscapeString(const char* pSrc, int nSrcLen, char* pDest)
{
    if (!mysql_)
    {
        return 0;
    }
    return mysql_real_escape_string(mysql_, pDest, pSrc, nSrcLen);
}

void MySQLConnection::Close()
{
    if (mysql_ != nullptr)
    {
        mysql_close(mysql_);
        mysql_ = nullptr;
    }
}

const char* MySQLConnection::GetErrInfo()
{
    return mysql_error(mysql_);
}

void MySQLConnection::Reconnct()
{
    mysql_ping(mysql_);
}
