#pragma once
#include <string>
using namespace std;
#include "jansson2.12/jansson.h"
class JsonUtils
{
public:
	JsonUtils();
	~JsonUtils();

	static json_t *load_json(const char *text);
	static void print_json(json_t *root);
	static void print_json_aux(json_t *element, int indent);
	static void print_json_indent(int indent);
	static const char *json_plural(int count);
	static void print_json_object(json_t *element, int indent);
	static void print_json_array(json_t *element, int indent);
	static void print_json_string(json_t *element, int indent);
	static void print_json_integer(json_t *element, int indent);
	static void print_json_real(json_t *element, int indent);
	static void print_json_true(json_t *element, int indent);
	static void print_json_false(json_t *element, int indent);
	static void print_json_null(json_t *element, int indent);

	//工具函数

	static string JsonGetString(json_t* value);// 解析json,转换为字符串数据并返回

	static double JsonGetDouble(json_t* value);	// 解析json，转换为double数据并返回
	
	static int JsonGetInt(json_t* value);// 解析json，转换为int数据并返回
	
	static __int64 JsonGetInt64(json_t* value);// 解析json，转换为INT64数据并返回	
};

