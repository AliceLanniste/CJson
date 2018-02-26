#ifndef LPETJSON_H__H
#define LPETJSON_H__H 

typedef enum JSON
{
	LEPT_NULL,
	LEPT_TRUE,
	LEPT_FALSE,
	LEPT_MEMBER,
	LEPT_NUMBER,
	LEPT_STRING,
	LEPT_ARRAY,
	LEPT_OBJECT
} lept_type;


typedef struct {

	lept_type type;

} lept_value;

//返回结果
enum 
{
	LEPT_PARSE_OK = 0,
	LEPT_PARSE_EXPECT_VALUE,
	LEPT_PARSE_INVALID_VALUE,
	LEPT_PARSE_ROOT_NOT_SINGULAR
};

int lept_parse(lept_value* v, const char* json);

//获取访问结果函数
lept_type lept_get_type(const lept_value* v);
#endif /* LEPTJSON_H__ */