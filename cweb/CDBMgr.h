/**
* Copyright (c) 2019 Huiangu.  All rights reserved.
* FileName: CDBMgr.h
* 数据库管理类
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

	int open(); //打开数据库环境
	int close();//关闭数据库环境

	int addAlarmRecord(int channel, int time, const unsigned char* data, int len);//增加报警记录
	int addTempRecord(int channel, int time, const unsigned char* data, int len); //增加温度记录
	int addStrainRecord(int channel, int time, const unsigned char* data, int len);//增加应变记录 

	int getAlarmRecord(int channel, int begin, int end, map<int, string>& mapOut);//读取报警记录
	
	
private:
	Db* openDB(const char* dbFile); //打开一个数据文件
	int closeDB(Db* pDB);           //关闭一个数据文件
	 
	int putString(Db* pDb, const char* key, const char* value); //写入一个字符串
	int getString(Db* pDb, const char* key, string& value);     //读出一个字符串

	int putBlob(Db* pDb, const char* key, const unsigned char* data, int len);//写入二进制
	unsigned char* getBlob(Db* pDb, const char* key, int& len);               //读出二进制数据

	int writeBlob(Db* pDb, int key, const unsigned char* data, int len);//写入二进制

	int getRecordCount(Db* pDb, int begin, int end);//读取记录数量

private:
	DbEnv* m_pDbEnv; 
	Db* m_pDBAlarm1;//报警表1
	Db* m_pDBAlarm2;//报警表2
	Db* m_pDBTemp1; //温度表1 
	Db* m_pDBTemp2; //温度表2
	Db* m_pDBStrain1; //应变表1
	Db* m_pDBStrain2; //应变表2
};
