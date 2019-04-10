
#include "IRESTful.h"
#include <iostream>
#include <sstream>
#include <codecvt>
#include "JsonUtils.h"


IRESTful::IRESTful()
{
}


IRESTful::~IRESTful()
{
}

IRESTful& IRESTful::Instance()
{
	static IRESTful agent;
	return agent;
}

/**
* SetData
* 传入接收到的HTTP数据
* 链接地址：
* @param
*		path: 	    HTTP请求路径
*		vecParam：	参数列表
*		postBody:   post data
*		out     :   return value
* @return
*		成功：      返回数据首地址
*		失败：      返回NULL
*/
void IRESTful::setData(const string& path, const vector<HttpParam>& vecParam, const string& postBody, string& out)
{
	if (path == "/saveData")//LaserStatus ? channel = 1 / 2
	{
		if (vecParam.size() == 1)
		{
			
		}
	}
	else if (path == "")
	{

	}
}

void IRESTful::getData(const string& path, const vector<HttpParam>& vecParam, const string& postBody, string& out)
{
	//http_GetmParams();
	int m_laserStatus = 0;
	double m_laserPower = 0;
	double m_laserTemp = 0;
	int m_laserTime = 0; 
	double m_caseTemp = 0;
	double m_heatsinkTemp = 0;


	json_t *json = makeJsonHeader(path, 0);
	
	json_t *value = json_object();
	json_object_set_new(value, "m_laserStatus", json_integer(m_laserStatus)); 
	json_object_set_new(value, "m_laserPower", json_real(m_laserPower));      
	json_object_set_new(value, "m_laserTemp", json_real(m_laserTemp));        
	json_object_set_new(value, "m_caseTemp", json_real(m_caseTemp));          
	json_object_set_new(value, "m_heatsinkTemp", json_real(m_heatsinkTemp));  

	json_object_set_new(json, "returmValue", value);
	out = makeJsonString(json);

//0x00c95bf8 "{\"request\": \"/LaserStatus1\", \"returnCode\": \"0\", \"timeStamp\": 1554692657, \"returmValue\": {\"m_laserStatus\": 0, \"m_laserPower\": 0.0, \"m_laserTemp\": 0.0, \"m_caseTemp\": 0.0, \"m_heatsinkTemp\": 0.0}}"
	

	//for test
	{
		json_t* root;
		json_error_t error;
		root = json_loads(out.c_str(), 0, &error);
		JsonUtils::print_json(root);
		json_decref(root);
		/*if (root)
		{
			json_t* requestUrl = json_object_get(root, "requestUrl");
			json_t* returnCode = json_object_get(root, "returnCode");
			json_t* returnMsg = json_object_get(root, "returnMsg");
			json_t* returnTime = json_object_get(root, "timeStamp");

			string url = JsonUtils::JsonGetString(requestUrl);
			int n1 = JsonUtils::JsonGetInt(returnCode);
			int t = JsonUtils::JsonGetInt(returnTime);
			INT64 n2 = JsonUtils::JsonGetInt64(returnTime);

			json_t* obj = json_object_get(root, "returmValue");

			json_t* obj1 = json_object_get(obj, "m_laserStatus");
			json_t* obj2 = json_object_get(obj, "m_laserPower");
			json_t* obj3 = json_object_get(obj, "m_laserTemp");
			json_t* obj4 = json_object_get(obj, "m_caseTemp");
			json_t* obj5 = json_object_get(obj, "m_heatsinkTemp");

			int nn1 = JsonUtils::JsonGetInt(obj1);
			double nn2 = JsonUtils::JsonGetDouble(obj2);
			double n3 = JsonUtils::JsonGetDouble(obj3);
			double n4 = JsonUtils::JsonGetDouble(obj4);
			double n5 = JsonUtils::JsonGetDouble(obj5);

			json_decref(root);
		}
		*/
	}
}


json_t* IRESTful::makeJsonHeader(const string& path, int code)
{
	json_t *json = json_object();
	//1 拼json
	json_object_set_new(json, "requestUrl", json_string(path.c_str()));  //请求路径
	json_object_set_new(json, "returnCode", json_integer(0));            //结果代码
	json_object_set_new(json, "returnMsg", json_string(""));             //结果内容
	json_object_set_new(json, "timeStamp", json_integer(time(NULL)));    //返回值
	return json;
}

string IRESTful::makeJsonString(json_t* json)
{
	string out = "";
	char* pJsonStr = json_dumps(json, 0);
	if (pJsonStr != NULL)
	{
		out = pJsonStr;
		free(pJsonStr);
	}
	json_decref(json);
	return out;
}
/**
* parsePostParam
* 解析POST的包体
* 链接地址：
* @param
*		input: 	    POST包体字符串
*		vecParam：	解析出来的参数列表
* @return
*		成功：      0
*		失败：      -1
*/
int IRESTful::parsePostParam(const string& input, vector<HttpParam>& vecParam)
{
	json_t *root = JsonUtils::load_json(input.c_str());
	if (root == NULL)
		return -1;

	JsonUtils::print_json(root);

	json_decref(root);
	return 0;
}
