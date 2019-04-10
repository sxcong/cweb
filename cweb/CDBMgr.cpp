
#include "CDBMgr.h"

#include <sys/types.h>
#include <iostream>
#include <iomanip>
#include <errno.h>
#include <stdlib.h>
#include <string.h>


using std::cerr;

#define DB_USE_DLL 1
#include "bdb18.1.32/db_cxx.h"

#ifdef _DEBUG
#pragma comment(lib, "debug/libdb181d.lib")
#else
#pragma comment(lib, "release/libdb181.lib")
#endif // DEBUG


const char *progname = "DTSSEnv";
const char *data_dir = "data";
const char *home = "d:\\dbhome";
const char * err1 = "DbEnv::open: No such file or directory";
const char * err2 = "Db::open: No such file or directory";

CDBMgr::CDBMgr(void)
{
	m_pDbEnv = NULL;
}

CDBMgr::~CDBMgr(void)
{
}

CDBMgr& CDBMgr::Instance()
{
	static CDBMgr agent;
	return agent;
}
/**
* open
*  �����ݿ⻷���������������ݿ��
* @param
* @return
*		�ɹ���return 0
*		ʧ�ܣ�return -1
*/
int CDBMgr::open()
{
	if (m_pDbEnv != NULL)
		return -1;

	m_pDbEnv = new DbEnv((u_int32_t)0);
	m_pDbEnv->set_error_stream(&cerr);
	m_pDbEnv->set_errpfx(progname);
	//
	// We want to specify the shared memory buffer pool cachesize,
	// but everything else is the default.
	//
	m_pDbEnv->set_cachesize(0, 64 * 1024, 0);

	// Databases are in a subdirectory.
	(void)m_pDbEnv->set_data_dir(data_dir);

	try {
		m_pDbEnv->open(home,
			DB_CREATE | DB_INIT_LOCK | DB_INIT_LOG | DB_INIT_MPOOL |
			DB_INIT_TXN, 0);
	}
	catch (DbException &dbe) {
		cerr << "EnvExample: " << dbe.what() << "\n";
		if (!strcmp(dbe.what(), err1)) {
			cout << "Please check whether "
				<< "home dir \"" << home << "\" exists.\n";
		}
		delete m_pDbEnv;
		m_pDbEnv = NULL;
		return -1;
	}

	m_pDBAlarm1 = openDB("alarm1.db");
	m_pDBAlarm2 = openDB("alarm2.db");
	m_pDBTemp1 = openDB("temp1.db");
	m_pDBTemp2 = openDB("temp2.db");
	m_pDBStrain1 = openDB("strain1.db");
	m_pDBStrain2 = openDB("strain2.db");

	return 0;
}
/**
* close
* �ر����ݿ⻷���������������ݿ��
* @param
* @return
*		�ɹ���return 0
*		ʧ�ܣ�return -1
*/
int CDBMgr::close()
{
	closeDB(m_pDBAlarm1);
	closeDB(m_pDBAlarm2);
	closeDB(m_pDBTemp1);
	closeDB(m_pDBTemp2);
	closeDB(m_pDBStrain1);
	closeDB(m_pDBStrain2);

	if (m_pDbEnv)
	{
		m_pDbEnv->close(0);
		delete m_pDbEnv;
		m_pDbEnv = NULL;
	}
	return 0;
}
/**
* openDB
* ��һ�������ļ�
* @param
*		pDb: 	���ݱ�ָ��
*		dbFile: �����ļ���
* @return
*		�ɹ���return 0
*		ʧ�ܣ�return -1
*/
Db* CDBMgr::openDB(const char* dbFile)
{
	// Open a database in the environment to verify the data_dir
	// has been set correctly.
	// Create a database handle, using the environment.	
	Db* pDb = new Db(m_pDbEnv, 0);
	//m_pDB->set_error_stream(&cerr);
	//m_pDB->set_errpfx("AccessExample");
	//m_pDB->set_pagesize(1024);		// Page size: 1K. 
	//m_pDB->set_cachesize(0, 32 * 1024, 0);
	//m_pDB->open(NULL, dbFile, NULL, DB_BTREE, DB_CREATE, 0664);

	// Open the database. 
	try {
		pDb->open(NULL, dbFile,
			NULL, DB_BTREE, DB_CREATE, 0644);
	}
	catch (DbException &dbe) {
		cerr << "EnvExample: " << dbe.what() << "\n";
		if (!strcmp(dbe.what(), err2)) {
			cout << "Please check whether data dir \"" << data_dir
				<< "\" exists under \"" << home << "\"\n";
		}
		delete pDb;
		pDb = NULL;
		return NULL;
	}

	return pDb;
}
/**
* closeDB
* �ر�һ�������ļ�
* @param
*		pDb: 	���ݱ�ָ��
* @return
*		�ɹ���return 0
*		ʧ�ܣ�return -1
*/
int CDBMgr::closeDB(Db* pDB)
{
	if (pDB)
	{
		pDB->close(0);
		delete pDB;
		pDB = NULL;
	}
	return 0;
}
/**
* addAlarmRecord
* ���ӱ�������
* @param
*		channel:ͨ����
*		time��	ʱ���
*		data��	д������
*       len��   ���ݳ���
* @return
*		�ɹ���return 0
*		ʧ�ܣ�return -1
*/
int CDBMgr::addAlarmRecord(int channel, int time, const unsigned char* data, int len)
{
	Db* pDB = NULL;
	switch (channel)
	{
	case 1:
		pDB = m_pDBAlarm1;
		break;
	case 2:
		pDB = m_pDBAlarm2;
		break;
	default:
		break;
	}
	if (pDB)
	{
		return writeBlob(pDB, time, data, len);
	}
	return 0;
}
/**
* addTempRecord
* �����¶�����
* @param
*		channel:ͨ����
*		time��	ʱ���
*		data��	д������
*       len��   ���ݳ���
* @return
*		�ɹ���return 0
*		ʧ�ܣ�return -1
*/
int CDBMgr::addTempRecord(int channel, int time, const unsigned char* data, int len)
{
	Db* pDB = NULL;
	switch (channel)
	{
	case 1:
		pDB = m_pDBTemp1;
		break;
	case 2:
		pDB = m_pDBTemp2;
		break;
	default:
		break;
	}
	if (pDB)
	{
		return writeBlob(pDB, time, data, len);
	}
	return 0;
}
/**
* addStrainRecord
* ����Ӧ������
* @param
*		channel:ͨ����
*		time��	ʱ���
*		data��	д������
*       len��   ���ݳ���
* @return
*		�ɹ���return 0
*		ʧ�ܣ�return -1
*/
int CDBMgr::addStrainRecord(int channel, int time, const unsigned char* data, int len)
{
	Db* pDB = NULL;
	switch (channel)
	{
	case 1:
		pDB = m_pDBStrain1;
		break;
	case 2:
		pDB = m_pDBStrain2;
		break;
	default:
		break;
	}
	if (pDB)
	{
		return writeBlob(pDB, time, data, len);
	}
	return -1;
}
/**
* putString
* �����ַ���
* @param
*		pDb: 	���ݱ�ָ��
*		key��	key
*		value��	д���ַ���
* @return
*		�ɹ���return 0
*		ʧ�ܣ�return -1
*/
int CDBMgr::putString(Db* pDb, const char* key, const char* value)
{
	Dbt dbKey((char*)key, strlen(key) + 1);
	Dbt dbValue((char*)value, strlen(value) + 1);
	int ret = pDb->put(0, &dbKey, &dbValue, DB_NOOVERWRITE);
	if (ret == DB_KEYEXIST)
	{
		return -1;
	}
	return 0;
}
/**
* getString
* �����ַ���
* @param
*		pDb: 	���ݱ�ָ��
*		key��	key
*		value��	�����ַ���
* @return
*		�ɹ���return 0
*		ʧ�ܣ�return -1
*/
int CDBMgr::getString(Db* pDb, const char* key, string& value)
{
	Dbt dbKey((char*)key, strlen(key) + 1);
	Dbt data;
	int ret = pDb->get(NULL, &dbKey, &data, 0);
	if (ret == 0)
	{
		char *key_string = (char *)dbKey.get_data();
		//char *data_string = (char *)data.get_data();
		//cout << key_string << " : " << data_string << "\n";
		value = (const char*)data.get_data();
		pDb->sync(0);
	}
	else
	{
		char* result = DbEnv::strerror(ret);
		printf(result);
		return -1;
	}

	return 0;
}
/**
* putBlob
* �������������
* @param
*		pDb: 	���ݱ�ָ��
*		key��	key
*		data��	д�������׵�ַ
*		len��	д�����ݳ���
* @return
*		�ɹ���return 0
*		ʧ�ܣ�return -1
*/
int CDBMgr::putBlob(Db* pDb, const char* key, const unsigned char* data, int len)
{

	Dbt dbKey((char*)key, strlen(key) + 1);
	Dbt dbValue((unsigned char*)data, len);
	int ret = -1;
	try {
		 ret = pDb->put(0, &dbKey, &dbValue, DB_NOOVERWRITE);
	}
	catch (DbException &dbe) {
		cerr << "Exception: " << dbe.what() << "\n";
		return (-1);
	}
	if (ret == DB_KEYEXIST)
	{
		return -1;
	}
	pDb->sync(0);

	return 0;
}
/**
* getBlob
* ��������������
* @param
*		pDb: 	���ݱ�ָ��
*		key��	key
*		len��	�������ݳ���
* @return
*		�ɹ������������׵�ַ
*		ʧ�ܣ�����NULL
*/
unsigned char* CDBMgr::getBlob(Db* pDb, const char* key, int& len)
{
	Dbt dbKey((char*)key, strlen(key) + 1);
	Dbt dbValue;
	int ret = pDb->get(NULL, &dbKey, &dbValue, 0);
	if (ret == 0)
	{
		char *key_string = (char *)dbKey.get_data();

		len = dbValue.get_size();
		return (unsigned char*)dbValue.get_data();
	}
	else
	{
		char* result = DbEnv::strerror(ret);
		printf(result);
	}
	return NULL;
}

int CDBMgr::writeBlob(Db* pDb, int key, const unsigned char* data, int len)
{
	Dbt dbKey(&key, sizeof(key));
	Dbt dbValue((unsigned char*)data, len);
	int ret = -1;
	try {
		ret = pDb->put(0, &dbKey, &dbValue, DB_NOOVERWRITE);
	}
	catch (DbException &dbe) {
		cerr << "Exception: " << dbe.what() << "\n";
		return (-1);
	}
	if (ret == DB_KEYEXIST)
	{
		return -1;
	}
	pDb->sync(0);

	return 0;
}


int CDBMgr::getRecordCount(Db* pDb, int begin, int end)
{
	Dbt dbKey(&begin, sizeof(begin));
	Dbt dbValue;
	Dbc *dbc;
	int count = 0;
	int ret = -1;
	if ((ret = m_pDBAlarm1->cursor(NULL, &dbc, 0)) != 0) {
		m_pDBAlarm1->err(ret, "can't open cursor");
		return NULL;
	}
	   
	memset(&dbValue, 0, sizeof(dbValue));
	dbValue.set_flags(DB_DBT_PARTIAL);
	dbValue.set_dlen(0);

	ret = dbc->get(&dbKey, &dbValue, DB_SET_RANGE);
	if (!ret) {
		do
		{
			int key_string = *(int*)dbKey.get_data();
			//char *value_string = (char *)dbValue.get_data();

			memset(&dbKey, 0, sizeof(dbKey));
			//memset(&dbValue, 0, sizeof(dbValue));
			if (key_string >= end)
			{
				if (key_string == end)
					count++;
				break;
			}
			else
				count++;
		} while (dbc->get(&dbKey, &dbValue, DB_NEXT) == 0);
	}

	dbc->close();
	return count;
}

int CDBMgr::getAlarmRecord(int channel, int begin, int end, map<int, string>& mapOut)
{

	Dbt dbKey(&begin, sizeof(begin));
	Dbt dbValue;
	Dbc *dbc;
	int ret = -1;
	if ((ret = m_pDBAlarm1->cursor(NULL, &dbc, 0)) != 0) {
		m_pDBAlarm1->err(ret, "can't open cursor");
		return -1;
	}

	ret = dbc->get(&dbKey, &dbValue, DB_SET_RANGE);
	if (!ret) {
		do 
		{
			int key_string = *(int*)dbKey.get_data();
			char *value_string = (char *)dbValue.get_data();

			char*value = new char[dbValue.get_size() + 1];
			memcpy(value, dbValue.get_data(), dbValue.get_size());
			value[dbValue.get_size()] = '\0';
			mapOut[key_string] = string(value);
			delete value;
			value = NULL;

			memset(&dbKey, 0, sizeof(dbKey));
			memset(&dbValue, 0, sizeof(dbValue));
			if (key_string >= end)
				break;


		} while (dbc->get(&dbKey, &dbValue, DB_NEXT) == 0);
	}

	dbc->close();
	return mapOut.size();
}