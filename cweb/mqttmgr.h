#ifndef MQTTMGR_H
#define MQTTMGR_H

#include "MQTTAsyncClient.h"
//#include "MQTTDataProcessThread.h"
#include <string>
using namespace  std;

/**
 * class MQTTMgr
 * MQTT�ͻ��˹����࣬���������ͻ��ˣ�MQTT�������MQTT�������ݡ�
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

    //MQTTDataProcessThread* m_pMQTTDataProcessThread;//������Ϣ�����̣߳��ڲ�ά����Ϣ����
   // MQTTDataProcessThread* m_pMQTTSendThread;//�����߳�

    MQTTConfig m_mqttPubConfig; //MQTT������������
    MQTTConfig m_mqttSubConfig; //MQTT������������
    int m_nEnableMQTT;          //�Ƿ�����MQTT
};

#endif // MQTTMGR_H
