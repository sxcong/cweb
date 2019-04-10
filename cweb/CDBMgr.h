/**
* Copyright (c) 2019 Huiangu.  All rights reserved.
* FileName: CDBMgr.h
* ���ݿ������
*/
#pragma once
#include <map>
#include <string>
using namespace std;

class Db;
class DbEnv;
class CDBMgr
{
public:
	static CDBMgr& Instance();
	CDBMgr(void);
	~CDBMgr(void);

	int open(); //�����ݿ⻷��
	int close();//�ر����ݿ⻷��

	int addAlarmRecord(int channel, int time, const unsigned char* data, int len);//���ӱ�����¼
	int addTempRecord(int channel, int time, const unsigned char* data, int len); //�����¶ȼ�¼
	int addStrainRecord(int channel, int time, const unsigned char* data, int len);//����Ӧ���¼ 

	int getAlarmRecord(int channel, int begin, int end, map<int, string>& mapOut);//��ȡ������¼
	
	
private:
	Db* openDB(const char* dbFile); //��һ�������ļ�
	int closeDB(Db* pDB);           //�ر�һ�������ļ�
	 
	int putString(Db* pDb, const char* key, const char* value); //д��һ���ַ���
	int getString(Db* pDb, const char* key, string& value);     //����һ���ַ���

	int putBlob(Db* pDb, const char* key, const unsigned char* data, int len);//д�������
	unsigned char* getBlob(Db* pDb, const char* key, int& len);               //��������������

	int writeBlob(Db* pDb, int key, const unsigned char* data, int len);//д�������

	int getRecordCount(Db* pDb, int begin, int end);//��ȡ��¼����

private:
	DbEnv* m_pDbEnv; 
	Db* m_pDBAlarm1;//������1
	Db* m_pDBAlarm2;//������2
	Db* m_pDBTemp1; //�¶ȱ�1 
	Db* m_pDBTemp2; //�¶ȱ�2
	Db* m_pDBStrain1; //Ӧ���1
	Db* m_pDBStrain2; //Ӧ���2
};
