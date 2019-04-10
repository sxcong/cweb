
#include "JsonUtils.h"


#ifdef _DEBUG
#pragma comment(lib, "debug/jansson_d.lib")
#else
#pragma comment(lib, "release/jansson.lib")
#endif // DEBUG

JsonUtils::JsonUtils()
{
}


JsonUtils::~JsonUtils()
{
}


/**
* JsonGetString
* 解析json,转换为字符串数据并返回
* @param 参数
*	     value: json 对象
* @return
*		CString 类型数据
*/
string JsonUtils::JsonGetString(json_t* value)
{
	string str = "";
	if (json_is_string(value))
	{
		if (json_string_length(value) > 0)
		{
			//str = UTF8ToUnicode(json_string_value(value));
			str = json_string_value(value);
		}
	}
	else if (json_is_integer(value))
	{
		char buff[32];
		sprintf_s(buff, "%I64d", json_integer_value(value));
		//str.Format(_T("%I64d"), json_integer_value(value));
		str = buff;
	}
	else if (json_is_real(value))
	{
		char buff[32];
		sprintf_s(buff, "%f", json_real_value(value));
		str = buff;
		//str.Format(_T("%f"), json_real_value(value));
	}

	//  std::ostringstream stringStream;
	//stringStream << "Hello";
	//std::string copyOfStr = stringStream.str();
	return str;
}

/**
* JsonGetDouble
* 解析json，转换为double数据并返回
* @param 参数
*	     value:json对象
* @return
*		返回 double 类型数据
*/
double JsonUtils::JsonGetDouble(json_t* value)
{
	double dRet = DBL_MAX;
	if (json_is_string(value))
	{
		if (json_string_length(value) > 0)
			dRet = atof(json_string_value(value));
	}
	else if (json_is_integer(value))
		dRet = (double)json_integer_value(value);
	else if (json_is_real(value))
		dRet = json_real_value(value);
	return dRet;
}

/**
* JsonGetInt
* 解析json，转换为int数据并返回
* @param 参数
*	     value:json对象
* @return
*		int 类型数据
*/
int JsonUtils::JsonGetInt(json_t* value)
{
	int nRet = INT_MIN;
	if (json_is_string(value))
	{
		if (json_string_length(value) > 0)
			nRet = atoi(json_string_value(value));
	}
	else if (json_is_integer(value))
		nRet = (int)json_integer_value(value);
	else if (json_is_real(value))
		nRet = (int)json_real_value(value);
	return nRet;
}

/**
* JsonGetInt64
* 解析json，转换为INT64数据并返回
* @param 参数
*	     value:json对象
* @return
*		INT64 类型数据
*/
__int64 JsonUtils::JsonGetInt64(json_t* value)
{
	__int64 llRet = _I64_MIN;
	if (json_is_string(value))
	{
		if (json_string_length(value) > 0)
			llRet = _atoi64(json_string_value(value));
	}
	else if (json_is_integer(value))
		llRet = (__int64)json_integer_value(value);
	else if (json_is_real(value))
		llRet = (__int64)json_real_value(value);
	return llRet;
}

void JsonUtils::print_json_array(json_t *element, int indent) {
	size_t i;
	size_t size = json_array_size(element);
	print_json_indent(indent);

	printf("JSON Array of %ld element%s:\n", size, json_plural(size));
	for (i = 0; i < size; i++) {
		print_json_aux(json_array_get(element, i), indent + 2);
	}
}

void JsonUtils::print_json_string(json_t *element, int indent) {
	print_json_indent(indent);
	printf("JSON String: \"%s\"\n", json_string_value(element));
}

void JsonUtils::print_json_integer(json_t *element, int indent) {
	print_json_indent(indent);
	printf("JSON Integer: \"%" JSON_INTEGER_FORMAT "\"\n", json_integer_value(element));
}

void JsonUtils::print_json_real(json_t *element, int indent) {
	print_json_indent(indent);
	printf("JSON Real: %f\n", json_real_value(element));
}

void JsonUtils::print_json_true(json_t *element, int indent) {
	(void)element;
	print_json_indent(indent);
	printf("JSON True\n");
}

void JsonUtils::print_json_false(json_t *element, int indent) {
	(void)element;
	print_json_indent(indent);
	printf("JSON False\n");
}

void JsonUtils::print_json_null(json_t *element, int indent) {
	(void)element;
	print_json_indent(indent);
	printf("JSON Null\n");
}

void JsonUtils::print_json_indent(int indent) {
	int i;
	for (i = 0; i < indent; i++) { putchar(' '); }
}
const char *JsonUtils::json_plural(int count) {
	return count == 1 ? "" : "s";
}

void JsonUtils::print_json_aux(json_t *element, int indent) {
	switch (json_typeof(element)) {
	case JSON_OBJECT:
		print_json_object(element, indent);
		break;
	case JSON_ARRAY:
		print_json_array(element, indent);
		break;
	case JSON_STRING:
		print_json_string(element, indent);
		break;
	case JSON_INTEGER:
		print_json_integer(element, indent);
		break;
	case JSON_REAL:
		print_json_real(element, indent);
		break;
	case JSON_TRUE:
		print_json_true(element, indent);
		break;
	case JSON_FALSE:
		print_json_false(element, indent);
		break;
	case JSON_NULL:
		print_json_null(element, indent);
		break;
	default:
		fprintf(stderr, "unrecognized JSON type %d\n", json_typeof(element));
	}
}
void JsonUtils::print_json_object(json_t *element, int indent)
{
	size_t size;
	const char *key;
	json_t *value;

	print_json_indent(indent);
	size = json_object_size(element);

	printf("JSON Object of %ld pair%s:\n", size, json_plural(size));
	json_object_foreach(element, key, value) {
		print_json_indent(indent + 2);
		printf("JSON Key: \"%s\"\n", key);
		print_json_aux(value, indent + 2);
	}

}

void JsonUtils::print_json(json_t *root) {
	print_json_aux(root, 0);
}

/*
 * Parse text into a JSON object. If text is valid JSON, returns a
 * json_t structure, otherwise prints and error and returns null.
 */
json_t *JsonUtils::load_json(const char *text) {
	json_t *root;
	json_error_t error;

	root = json_loads(text, 0, &error);

	if (root) {
		return root;
	}
	else {
		fprintf(stderr, "json error on line %d: %s\n", error.line, error.text);
		return (json_t *)0;
	}
}
