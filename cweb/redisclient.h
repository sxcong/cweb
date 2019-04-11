#ifndef REDISCLIENT_H
#define REDISCLIENT_H


#include <vector>
#include <string>
using namespace std;
#include "hiredis/hiredis.h"
class RedisClient
{
public:
    RedisClient();
    static RedisClient& Instance();
    bool connect();
    void disconnect();

    bool ping();
    bool pushData(const string& listName, vector<float>& dataList);
	bool pushString(const string& listName, vector<string>& dataList);
    bool getData(const string& listName, vector<vector<float>>& dataList);


    void clearData(const string& listName);
    int dataSize(const string& listName);

    int setString(const string& key, const string& value);
    int getString(const string& key, string& value);

	char* listToBytes(vector<float>& dataList, int &len);
	vector<float> bytesToList(char* bytes, int len);


private:
    redisContext *m_redisContext;
    const char *hostname = "127.0.0.1";
    int port = 6379;
    string m_szIP;
    int m_nPort;
    int m_nEnable;
    string m_userName;
    string m_passwordk;

};

#endif // REDISCLIENT_H
