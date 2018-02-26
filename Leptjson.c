#include <assert.h>
#include "Leptjson.h"
#include<stdlib.h>

#define EXPECT(c ,h)      do{assert(*c->json == (h)) c->json++ } while (0);


typedef struct 
{
	const char* json;
} lept_context;

//是用来删除空白符的然后把删除空白符的数据
static void lept_parse_whitespace(lept_context * c) {
	const char* p = c->json;
	while(*p == ' ' || *p == '\n' || *p =='\t' || *p == '\r')
		p++;
	c->json = p;
}

static int lept_parse_true(lept_context* context,lept_value* v)
{
	EXPECT(context,'t');
	if (context->json[0] != 'r' || context->json[1] != 'u'
		|| context->json[2] != 'e')
		return LEPT_PARSE_INVALID_VALUE;
	context->json += 3;
	v->type = LEPT_TRUE;
	return LEPT_PARSE_OK;
}

static int lept_parse_false(lept_context* context, lept_value* v)
{
	EXPECT(context, 'f');
	if(context->json[0] !='a' || context->json[1] != 'l'
	|| context->json[2] !='s' || context->json[3] != 'e')
		return LEPT_PARSE_INVALID_VALUE;
	context->json += 4;
	v->type = LEPT_FALSE;
	return LEPT_PARSE_OK;
}

static int lept_parse_null(lept_context* context, lept_value* v)
{	EXPECT(context,'n');
	if(context->json[0] != 'u' || context->json[1] !='l' 
		|| context->json[2] != 'l')
		return LEPT_PARSE_INVALID_VALUE;
	context->json += 3;
	v->type = LEPT_NULL;
	return LEPT_PARSE_OK;
}

static int lept_parse_value(lept_context* context, lept_value* v)
{
	switch(*context -> json)
	{
		case 'n':       return lept_parse_null(context, v);
		case 'f':		return lept_parse_false(context,v);
		case 't':		return lept_parse_true(context,v);
		case '\0':	    return LEPT_PARSE_EXPECT_VALUE;
		default:        return LEPT_PARSE_INVALID_VALUE;
	}
}


int lept_parse(lept_value* v, const char* json)
{
	lept_context c;
	int ret;
	c.json = json;
	v->type = LEPT_NULL;
	lept_parse_whitespace(&c);
	if((ret = lept_parse(&c,v)) == LEPT_PARSE_OK)
	{
		lept_parse_whitespace(&c);
		if(*c.json !='\0')
			return LEPT_PARSE_ROOT_NOT_SINGULAR;
	}
	return ret;
}


lept_type lept_get_type(const lept_value* v)
{
	assert(v != NULL);
	return v->type;
}