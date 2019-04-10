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

	//���ߺ���

	static string JsonGetString(json_t* value);// ����json,ת��Ϊ�ַ������ݲ�����

	static double JsonGetDouble(json_t* value);	// ����json��ת��Ϊdouble���ݲ�����
	
	static int JsonGetInt(json_t* value);// ����json��ת��Ϊint���ݲ�����
	
	static __int64 JsonGetInt64(json_t* value);// ����json��ת��ΪINT64���ݲ�����	
};

