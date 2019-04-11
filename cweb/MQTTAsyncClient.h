#ifndef MQTTASYNCCLIENT_H
#define MQTTASYNCCLIENT_H
#include <string>
using namespace  std;
#include "paho.mqtt.c/MQTTAsync.h"

typedef enum _ENUM_MQTT_CLIENT_TYPE
{
    ENUM_MQTT_CLIENT_SUB,
    ENUM_MQTT_CLIENT_PUB
}ENUM_MQTT_CLIENT_TYPE;

class MQTTConfig
{
public:
    string m_szUrl;
    string m_szClientId;
    string m_szTopic;
};


class IMQTTDataCallback
{
public:
    IMQTTDataCallback(){}
    virtual ~IMQTTDataCallback(){}
    virtual void onRecv(const char*buff, int len) = 0;
};


class MQTTAsyncClient
{
public:
    MQTTAsyncClient();

    int init(ENUM_MQTT_CLIENT_TYPE enumType, const MQTTConfig& config);
    void unInit();
    void connect();
    void disconnect();
    void checkConnect();
    int pub(const char*buff, int len);
    void setCallback(IMQTTDataCallback* pMQTTDataCallback);

    static void connlost(void *context, char *cause);
    static int msgarrvd(void *context, char *topicName, int topicLen, MQTTAsync_message *message);
    static void onDisconnect(void* context, MQTTAsync_successData* response);
    static void onSubscribe(void* context, MQTTAsync_successData* response);
    static void onSubscribeFailure(void* context, MQTTAsync_failureData* response);
    static void onConnectFailure(void* context, MQTTAsync_failureData* response);
    static void onConnect(void* context, MQTTAsync_successData* response);
    static void onSend(void* context, MQTTAsync_successData* response);
    static void onSendFailure(void* context, MQTTAsync_failureData* response);

private:
    void onConnect();

public:
    ENUM_MQTT_CLIENT_TYPE m_enumType = ENUM_MQTT_CLIENT_SUB;
    string m_url;
    string m_clientId;
    int subscribed = 0;
    int connected = 0;
    int finish = 0;
    string m_topic;

    MQTTAsync m_mqttAsync;
    MQTTAsync_connectOptions m_conn_opts;// = MQTTAsync_connectOptions_initializer;
   // MQTTAsync_disconnectOptions m_disc_opts = MQTTAsync_disconnectOptions_initializer;
    IMQTTDataCallback* m_pMQTTDataCallback;
};

#endif // MQTTASYNCCLIENT_H
