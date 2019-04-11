#ifndef MQTTMGR_H
#define MQTTMGR_H

#include "MQTTAsyncClient.h"
//#include "MQTTDataProcessThread.h"
#include <string>
using namespace  std;

/**
 * class MQTTMgr
 * MQTT客户端管理类，管理两个客户端：MQTT接收命令，MQTT发送数据。
 */

class MQTTMgr
{

    MQTTMgr();
    static MQTTMgr& Instance();

    void startMQTTClient();
    void stopMQTTClient();
    //void sendData(const MQTTData& data);
    int  enableMQTT();
    void loadCinfig();

private:
    int m_timerId;
    bool m_bIsRunning;

    //MQTTDataProcessThread* m_pMQTTDataProcessThread;//接收消息处理线程，内部维护消息队列
   // MQTTDataProcessThread* m_pMQTTSendThread;//发送线程

    MQTTConfig m_mqttPubConfig; //MQTT发送配置内容
    MQTTConfig m_mqttSubConfig; //MQTT接收配置内容
    int m_nEnableMQTT;          //是否启用MQTT
};

#endif // MQTTMGR_H
