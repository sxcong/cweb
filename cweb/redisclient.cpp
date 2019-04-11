#include <windows.h>
#include <WinUser.h>
#include "redisclient.h"

#pragma comment(lib, "user32.lib")//error LNK2019: 无法解析的外部符号 __imp__wsprintfA

#ifdef _DEBUG
#pragma comment(lib, "debug/hiredis.lib")
#pragma comment(lib, "debug/Win32_Interop.lib")
#else
#pragma comment(lib, "release/hiredis.lib")
#pragma comment(lib, "release/Win32_Interop.lib")
#endif // DEBUG


RedisClient::RedisClient()
{
    m_redisContext = NULL;
    m_nEnable = 1;
	m_szIP = "127.0.0.1";
	m_nPort = 6379;
}

RedisClient& RedisClient::Instance()
{
    static RedisClient agent;
    return agent;
}

bool RedisClient::connect()
{
    if (m_redisContext != NULL)
    {
        redisFree(m_redisContext);
    }

    struct timeval timeout = { 1, 500000 }; // 1.5 seconds
    bool bRet = false;
    m_redisContext = redisConnectWithTimeout(m_szIP.c_str(), m_nPort, timeout);
    if (m_redisContext == NULL || m_redisContext->err) {
        bRet = false;
        if (m_redisContext) {
            redisFree(m_redisContext);
            m_redisContext = NULL;

        } else {
        }
    }
    else
    {
        bRet = true;
    }
    return bRet;
}

void RedisClient::disconnect()
{
    if (m_redisContext)
    {
         redisFree(m_redisContext);
         m_redisContext = NULL;
    }
}



bool RedisClient::ping()
{
    try{
        if (m_redisContext == NULL)
            return false;
        redisReply *reply = (redisReply *)redisCommand(m_redisContext,"PING");
        if (reply == NULL)
            return false;
        //printf("PING: %s\n", reply->str);
        freeReplyObject(reply);
        return true;
    }
    catch(...)
    {

    }
    return false;
}


char* RedisClient::listToBytes(vector<float>& dataList,int &len)
{
	
	int size = dataList.size();
	len = (size) * sizeof(float) + sizeof(int);
	int off = 0;
	
	char* bytes = new char[len];
	memcpy(bytes, &size, sizeof(int));
	
	for (auto &item : dataList)
	{
		memcpy(bytes+ off, &size, sizeof(float));
		off += sizeof(int);
	}

	return bytes;
}
vector<float> RedisClient::bytesToList(char* bytes, int len)
{
	vector<float> vecData;
	if (bytes == NULL)
		return vecData;

	int size = 0;
	int off = 0;
	memcpy(&size, bytes, sizeof(int));
	for (int i = 0; i < size; i++)
	{
		if ( off > len - sizeof(float))
			break;

		float f;
		memcpy(&f, bytes + off, sizeof(float));
		vecData.push_back(f);
	}

	return vecData;
}
//1. 对于二进制（如thrift序列化后的数据）构造格式输入时，需使用”%b”, val.data(), val.size() 的形式；
//2. 对于读取存储在redis内的二进制数据时， 对返回的string一定要通过string(reply->str, reply->len)构造返回，才能读取到完整的数据；
bool RedisClient::pushData(const string& listName, vector<float>& dataList)
{
    if (m_redisContext == NULL)
        return false;

    if (dataSize(listName) > 64)
    {
        clearData(listName);
        return false;
    }

	int len = 0;
    char* bytes = listToBytes(dataList, len);
	if (bytes != NULL && len > 0)
	{
		redisReply* reply = (redisReply*)redisCommand(m_redisContext, "RPUSH %s %b",
			listName.c_str(), bytes, (size_t)len);

		delete bytes;
		bytes = NULL;

		if (reply == NULL)
		{
			return false;
		}
		freeReplyObject(reply);
	}
    
    return true;
}

bool RedisClient::getData(const string& listName, vector<vector<float>>& dataList)
{
    if (m_redisContext == NULL)
        return false;
    redisReply* reply = (redisReply*)redisCommand(m_redisContext,"LRANGE %s 0 -1", listName);
    if (reply == NULL)
        return false;

    if (reply->type == REDIS_REPLY_ARRAY)
    {
        for (int j = 0; j < reply->elements; j++) {
            vector<float> list = bytesToList(reply->element[j]->str, reply->element[j]->len);
            dataList.push_back(list);
            list.clear();
        }
    }
    freeReplyObject(reply);
    return true;
}

void RedisClient::clearData(const string& listName)
{
    if (m_redisContext == NULL)
        return ;
    redisReply* reply = (redisReply*)redisCommand(m_redisContext,"LTRIM %s 2 1", listName.c_str());
    if (reply == NULL)
        return ;
    printf("LTRIM : %s\n", reply->str);
    freeReplyObject(reply);
}

bool RedisClient::pushString(const string& listName, vector<string>& dataList)
{
    if (m_redisContext == NULL)
        return false;

    if (dataSize(listName) > 256)
    {
        clearData(listName);
        return false;
    }

    for (int i = 0; i < dataList.size(); i++)
    {
        redisReply* reply = (redisReply*)redisCommand(m_redisContext,"RPUSH %s %s",
                                                      listName.c_str(), dataList[i].c_str());
        if (reply == NULL)
            return false;
        freeReplyObject(reply);
    }

    return true;
}

int RedisClient::dataSize(const string& listName)
{
    if (m_nEnable == 0)
        return 0;

    if (m_redisContext == NULL)
        return 0;

    redisReply* reply = (redisReply*)redisCommand(m_redisContext,"LLEN %s", listName.c_str());
    if (reply == NULL)
        return 0;

    int ret = reply->integer;
    freeReplyObject(reply);
    return ret;
}

int RedisClient::setString(const string& key, const string& value)
{
    if (m_nEnable == 0)
        return 0;

    if (m_redisContext == NULL)
        return 0;

    redisReply* reply = (redisReply*)redisCommand(m_redisContext,"SET %s %s", key.c_str(), value.c_str());
    if (reply == NULL)
        return 0;

    freeReplyObject(reply);
    return 0;
}

int RedisClient::getString(const string& key, string& value)
{
    if (m_nEnable == 0)
        return 0;

    if (m_redisContext == NULL)
        return 0;

    redisReply* reply = (redisReply*)redisCommand(m_redisContext,"GET %s", key.c_str());
    if (reply == NULL)
        return 0;
    if (reply->str != NULL)
        value = reply->str;

    freeReplyObject(reply);
    return value.size();
}
