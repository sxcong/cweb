#include "mqttmgr.h"


MQTTMgr::MQTTMgr()
{
   // m_pMQTTDataProcessThread = nullptr;
   // m_pMQTTSendThread = nullptr;
    m_bIsRunning = false;
    m_nEnableMQTT = 0;
}

MQTTMgr& MQTTMgr::Instance()
{
    static MQTTMgr agent;
    return agent;
}


/**
 * @brief MQTTMgr::loadCinfig
 * ����mqtt��������
 * @param
 * @return
 */
void MQTTMgr::loadCinfig()
{
    /*****test*******/
    m_mqttPubConfig.m_szUrl = "tcp://iot.eclipse.org:1883";
    m_mqttPubConfig.m_szClientId = "ExamplePlatPub";
    m_mqttPubConfig.m_szTopic = "MQTT Examples/test";

    m_mqttSubConfig.m_szUrl = "tcp://iot.eclipse.org:1883";
    m_mqttSubConfig.m_szClientId = "ExamplePlatSub";
    m_mqttSubConfig.m_szTopic = "MQTT Examples/test";

    m_nEnableMQTT = 1;
    /*****test*******/
	/*
    QString path = QCoreApplication::applicationDirPath();
    QString cfgpath = path + "/config/mqtt.ini";
    if (QFile::exists(cfgpath))
    {
        QSettings setting(cfgpath, QSettings::IniFormat);
        {
            if (setting.contains("enable"))
                m_nEnableMQTT = setting.value("enable").toInt();

            //pub ����
            if (setting.contains("pub/m_szUrl"))
                m_mqttPubConfig.m_szUrl = setting.value("pub/m_szUrl").toString().toStdString();
            if (setting.contains("pub/m_szClientId"))
                m_mqttPubConfig.m_szClientId = setting.value("pub/m_szClientId").toString().toStdString();
            if (setting.contains("pub/m_szTopic"))
                m_mqttPubConfig.m_szTopic = setting.value("pub/m_szTopic").toString().toStdString();

            // sub ����
            if (setting.contains("sub/m_szUrl"))
                m_mqttSubConfig.m_szUrl = setting.value("sub/m_szUrl").toString().toStdString();
            if (setting.contains("sub/m_szClientId"))
                m_mqttSubConfig.m_szClientId = setting.value("sub/m_szClientId").toString().toStdString();
            if (setting.contains("sub/m_szTopic"))
                m_mqttSubConfig.m_szTopic = setting.value("sub/m_szTopic").toString().toStdString();
        }
    }*/
}

/**
 * @brief MQTTMgr::startMQTTClient
 * ����MQTT�ͻ��ˣ������������ļ��ж�ȡ
 * @param
 * @return
 */
void MQTTMgr::startMQTTClient()
{
    if (m_bIsRunning)
        return;

    //�����߳�
    // m_pMQTTDataProcessThread = new MQTTDataProcessThread;
    //  m_pMQTTDataProcessThread->init(false, m_mqttSubConfig);
    // connect(m_pMQTTDataProcessThread, &QThread::finished, this, &QObject::deleteLater);
    //  m_pMQTTDataProcessThread->start();

    //�����߳�
//    m_pMQTTSendThread = new MQTTDataProcessThread;
//    m_pMQTTSendThread->init(true, m_mqttPubConfig);
//    connect(m_pMQTTSendThread, &QThread::finished, this, &QObject::deleteLater);
//    m_pMQTTSendThread->start();

    //������ض�ʱ��
     // m_timerId = startTimer(5*1000);
     // m_bIsRunning = true;
}

/**
 * @brief MQTTMgr::enableMQTT
 * �Ƿ�����MQTT
 * @return
 */
int MQTTMgr::enableMQTT()
{
    return m_nEnableMQTT;
}

/**
 * @brief MQTTMgr::sendData
 * ����ʵʱ�򱨾�����
 * @param data
 */
/*
void MQTTMgr::sendData(const MQTTData& data)
{
    if (m_pMQTTSendThread)
    {
        m_pMQTTSendThread->setData(data);
    }
}
*/
/**
 * @brief MQTTMgr::stopMQTTClient
 * ֹͣMQTT�ͻ���
 * @param
 * @return
 */
void MQTTMgr::stopMQTTClient()
{
	/*
    killTimer(m_timerId);
    if (m_pMQTTDataProcessThread)
    {
        m_pMQTTDataProcessThread->exitThread();
        m_pMQTTDataProcessThread->quit();
        m_pMQTTDataProcessThread->wait();
    }

    if (m_pMQTTSendThread)
    {
        m_pMQTTSendThread->exitThread();
        m_pMQTTSendThread->quit();
        m_pMQTTSendThread->wait();
    }
	*/
    m_bIsRunning = false;
}



/**
 * @brief MQTTMgr::timerEvent
 * ��ʱ�����MQTT�ͻ���״̬������Ͽ����ӣ���Ҫ��������
 * @param event
 * @param
 * @return
 */
/*
void MQTTMgr::timerEvent(QTimerEvent *event)
{
    if(event->timerId() == m_timerId)
    {
    }
}
*/
