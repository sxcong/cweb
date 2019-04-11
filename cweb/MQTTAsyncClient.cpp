#include "MQTTAsyncClient.h"
#include <iostream>

#ifdef _DEBUG
#pragma comment(lib, "debug/paho-mqtt3a.lib")
#else
#pragma comment(lib, "release/paho-mqtt3a.lib")
#endif // DEBUG

#define QOS         1
#define TIMEOUT     10000L

MQTTAsyncClient::MQTTAsyncClient()
{
    m_pMQTTDataCallback = nullptr;

    m_conn_opts = MQTTAsync_connectOptions_initializer;
}

/**
 * @brief MQTTAsyncClient::connlost
 * 远程断开连接，被动
 * @param context
 * @param cause
 * @return
 */
void MQTTAsyncClient::connlost(void *context, char *cause)
{
    MQTTAsyncClient* client = (MQTTAsyncClient*)context;
    MQTTAsync* mqttAsync = (MQTTAsync*)client->m_mqttAsync;
    MQTTAsync_connectOptions conn_opts = client->m_conn_opts;

    printf("\nConnection lost\n");
    if (cause)
        printf("     cause: %s\n", cause);
    client->connected = 0;

  /*  printf("Reconnecting\n");
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    if ((rc = MQTTAsync_connect(mqttAsync, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start connect, return code %d\n", rc);
        client->finished = 1;
    }
    */
}

/**
 * @brief MQTTAsyncClient::msgarrvd
 * sub模式下有消息到来
 * @param context
 * @param topicName
 * @param topicLen
 * @param message
 * @return
 */
int MQTTAsyncClient::msgarrvd(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
    MQTTAsyncClient* client = (MQTTAsyncClient*)context;

	cout <<"Message arrived\n";
	cout <<" Message arrived, topic: "<<topicName;
	cout <<"   message: ";
#if 0
    //测试用
    {
        FILE* pf = fopen("d:\\round_new.png", "wb");
        fwrite(message->payload, message->payloadlen, 1, pf);
        fclose(pf);
    }
#endif

    if (client->m_pMQTTDataCallback)
    {
        client->m_pMQTTDataCallback->onRecv((const char*)message->payload, message->payloadlen);
    }


    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);
    return 1;
}

/**
 * @brief MQTTAsyncClient::onDisconnect
 * 主动结束
 * @param context
 * @param response
 * @return
 */
void MQTTAsyncClient::onDisconnect(void* context, MQTTAsync_successData* response)
{
    MQTTAsyncClient* client = (MQTTAsyncClient*)context;
    printf("Successful disconnection\n");
    client->connected = 0;
    client->finish = 1;//主动结束
}

/**
 * @brief MQTTAsyncClient::onSubscribe
 * 订阅成功
 * @param context
 * @param response
 * @return
 */
void MQTTAsyncClient::onSubscribe(void* context, MQTTAsync_successData* response)
{
    MQTTAsyncClient* client = (MQTTAsyncClient*)context;
    printf("Subscribe succeeded\n");
    client->subscribed = 1;
}

/**
 * @brief MQTTAsyncClient::onSubscribeFailure
 * 订阅失败
 * @param context
 * @param response
 * @return
 */
void MQTTAsyncClient::onSubscribeFailure(void* context, MQTTAsync_failureData* response)
{
    char buff[256];
    sprintf_s(buff, "Subscribe failed, rc %d\n", response ? response->code : 0);
	cout <<buff;
    MQTTAsyncClient* client = (MQTTAsyncClient*)context;
    client->subscribed = 0;
}

/**
 * @brief MQTTAsyncClient::onConnectFailure
 * 连接失败
 * @param context
 * @param response
 * @return
 */
void MQTTAsyncClient::onConnectFailure(void* context, MQTTAsync_failureData* response)
{
    MQTTAsyncClient* client = (MQTTAsyncClient*)context;
    char buff[256];
    sprintf_s(buff, "Connect failed, rc %d\n", response ? response->code : 0);
	cout <<buff;
    client->connected = 0;
}

/**
 * @brief MQTTAsyncClient::onConnect
 * 连接成功事件通知
 * @param context
 * @param response
 * @return
 */
void MQTTAsyncClient::onConnect(void* context, MQTTAsync_successData* response)
{
    MQTTAsyncClient* client = (MQTTAsyncClient*)context;
	cout <<"Successful connection\n";

    client->onConnect();
}

/**
 * @brief MQTTAsyncClient::onConnect
 * 连接成功回调函数
 * @param
 * @return
 */
void MQTTAsyncClient::onConnect()
{
    connected = 1;
    switch (m_enumType)
    {
    case ENUM_MQTT_CLIENT_SUB:
        {
            MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;

            opts.onSuccess = onSubscribe;
            opts.onFailure = onSubscribeFailure;
            opts.context = this;


            int rc = 0;

            if ((rc = MQTTAsync_subscribe(m_mqttAsync, m_topic.c_str(), QOS, &opts)) != MQTTASYNC_SUCCESS)
            {
				cout <<"Failed to start subscribe, return code" << rc;
            }
        }
        break;
    case ENUM_MQTT_CLIENT_PUB:
        {

        }
        break;
    }

}

void MQTTAsyncClient::onSendFailure(void* context, MQTTAsync_failureData* response)
{
   // QString str = QString("Message with token value %1 delivery confirmed\n").arg(response->token);
    
    //LOG(INFO)<<str.toStdString();
}

/**
 * @brief MQTTAsyncClient::onSend
 * 发送成功后的回调
 * @param context
 * @param response
 */
void MQTTAsyncClient::onSend(void* context, MQTTAsync_successData* response)
{
   // QString str = QString("Message with token value %1 delivery confirmed\n").arg(response->token);
   // qDebug()<<str;
  //  LOG(INFO)<<str.toStdString();

}

/**
 * @brief MQTTAsyncClient::init
 * 初始化函数，第一参数表示是连接SUB还是PUB通道
 * @param enumType
 * @param url
 * @param clientId
 * @return
 */
int MQTTAsyncClient::init(ENUM_MQTT_CLIENT_TYPE enumType, const MQTTConfig& config)
{
    m_enumType = enumType;

    m_url = config.m_szUrl;
    m_clientId = config.m_szClientId;
    m_topic = config.m_szTopic;

    //MQTTAsync_nameValue* valueInfo =  MQTTAsync_getVersionInfo();
    m_conn_opts = MQTTAsync_connectOptions_initializer;
    int rc = MQTTAsync_create(&m_mqttAsync, m_url.c_str(), m_clientId.c_str(), MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (rc == 0)
    {
        switch (enumType)
        {
        case ENUM_MQTT_CLIENT_SUB:
            MQTTAsync_setCallbacks(m_mqttAsync, this, connlost, msgarrvd, NULL);
            break;
        case ENUM_MQTT_CLIENT_PUB:
            MQTTAsync_setCallbacks(m_mqttAsync, NULL, connlost, NULL, NULL);
            break;
        }

    }
    return rc;
}

/**
 * @brief MQTTAsyncClient::setCallback
 * 设置回调函数
 * @param pMQTTDataCallback
 * @return
 */
void MQTTAsyncClient::setCallback(IMQTTDataCallback* pMQTTDataCallback)
{
    m_pMQTTDataCallback = pMQTTDataCallback;
}

/**
 * @brief MQTTAsyncClient::unInit
 * 结束客户端工作
 * @param
 * @return
 */
void MQTTAsyncClient::unInit()
{
    MQTTAsync_destroy(&m_mqttAsync);
}

/**
 * @brief MQTTAsyncClient::connect
 * 连接服务器
 * @param
 * @return
 */
void MQTTAsyncClient::connect()
{
    int rc = 0;
    m_conn_opts.keepAliveInterval = 20;
    m_conn_opts.cleansession = 1;
    m_conn_opts.onSuccess = onConnect;
    m_conn_opts.onFailure = onConnectFailure;
    m_conn_opts.context = this;
    if ((rc = MQTTAsync_connect(m_mqttAsync, &m_conn_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start connect, return code %d\n", rc);
    }
}

/**
 * @brief MQTTAsyncClient::disconnect
 * 主动断开服务器
 * @param
 * @return
 */
void MQTTAsyncClient::disconnect()
{
    int rc = 0;
    MQTTAsync_disconnectOptions disc_opts = MQTTAsync_disconnectOptions_initializer;
    //disc_opts.onSuccess = onDisconnect;
    if ((rc = MQTTAsync_disconnect(m_mqttAsync, &disc_opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start disconnect, return code %d\n", rc);
        finish = 1;
    }
}

/**
 * @brief MQTTAsyncClient::checkConnect
 * 检查是否连接，没连接需要重连
 * @param
 * @return
 */
void MQTTAsyncClient::checkConnect()
{
    if (!MQTTAsync_isConnected(m_mqttAsync))
    {
        connect();
    }
}

/**
 * @brief MQTTAsyncClient::pub
 * 发送消息
 * @param buff
 * @param len
 * @return 0:发送成功，其他为错误值
 */
int MQTTAsyncClient::pub(const char*buff, int len)
{
    if (!MQTTAsync_isConnected(m_mqttAsync))
        return -1;

    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
    int rc;

    opts.onSuccess = onSend;
    opts.onFailure = onSendFailure;
    opts.context = this;

    pubmsg.payload = (void*)buff;
    pubmsg.payloadlen = len;
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    if ((rc = MQTTAsync_sendMessage(m_mqttAsync, m_topic.c_str(), &pubmsg, &opts)) != MQTTASYNC_SUCCESS)
    {
        printf("Failed to start sendMessage, return code %d\n", rc);
        return rc;
    }
    return 0;
}
