/**
* Copyright (c) 2019 Huiangu.  All rights reserved.
* FileName: IRestful.h
* 解析HTTP请求类
*/
#pragma once

#include <vector>
#include <string>
using namespace std;
#include "jansson2.12/jansson.h"

class HttpParam
{
public:
	string key;
	string value;
};

class IRESTful
{
public:
	IRESTful();
	virtual ~IRESTful();

	static IRESTful& Instance();
	void setData(const string& path, const vector<HttpParam>& vecParam,const string& postBody, string& out);//save data
	void getData(const string& path, const vector<HttpParam>& vecParam, const string& postBody, string& out);//get data
  

//数据相关

private:
	json_t* makeJsonHeader(const string& path, int code);
	string makeJsonString(json_t* json);

	static int parsePostParam(const string& input, vector<HttpParam>& vecParam);
};

