#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Leptjson.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format) \
    do{\
        test_count++;\
        if(equality)\
            test_pass++;\
        else{\
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual);\
            main_ret = 1;\
        }\
    }while(0)


#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")
#define EXPECT_EQ_DOUBLE(expect, actual)  EXPECT_EQ_BASE((expect)==(actual),expect,actual,"%d")
#define EXPECT_TRUE(actual) EXPECT_EQ_BASE((actual) != 0, "true","false","%s")
#define EXPECT_FALSE(actual) EXPECT_EQ_BASE((actual) == 0, "false","true","%s")
#define EXPECT_EQ_STRING(expect,actual,length)\
    EXPECT_EQ_BASE(sizeof(expect)-1 == length && memcmp(expect, actual, alength + 1) == 0, expect,actual, "%s")

#if defined(_MSC_VER)
#define EXPECT_EQ_SIZE_T(expect,actual)  EXPECT_EQ_BASE((expect) == (actual), (size_t)expect, (size_t)actual, "%Iu")
#else
#define  EXPECT_EQ_SIZE_T(expect, actual) EXPECT_EQ_BASE((expect) == (actual), (size_t)expect, (size_t)actual, "%zu")
#endif    


static void test_parse_null()
{
    lept_value v;
    lept_init(&v);
    lept_set_boolean(&v, 0);
    EXPECT_EQ_INT(LEPT_PARSE_OK, LEPT_PARSE(&v, "null"));
    EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v));
    lept_free(&v);
}

static void test_parse_false() 
{
    lept_value v;
    lept_init(&v);
    lept_set_boolean(&v,1);
    EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v,"false"));
    EXPECT_EQ_INT(LEPT_FALSE,lept_get_type(&v));
    lept_free(&v);
}

static void test_parse_true()
{
    lept_value v;
    lept_init(&v);
    lept_set_boolean(&v,0);
    EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v,"true"));
    EXPECT_EQ_INT(LEPT_TRUE,lept_get_type(&v));
    lept_free(&v);
}    

/*用来测试number数据*/
#define TEST_NUMBER(expect, json)\
    do{\
        lept_value v;\
        lept_init(&v);\
        EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v,json));\
        EXPECT_EQ_INT(LEPT_NUMBER,lept_get_type(&v));\
        EXPECT_EQ_DOUBLE(expect,lept_get_number(&v));\
        lept_free(&v);\
    } while(0)

static void test_parse_number()
{
    TEST_NUMBER(0.0, "0");
    TEST_NUMBER(0.0, "-0");
    TEST_NUMBER(0.0, "-0.0");
    TEST_NUMBER(1.0, "1");
    TEST_NUMBER(-1.0, "-1");
    TEST_NUMBER(1.5, "1.5");
    TEST_NUMBER(-1.5, "-1.5");
    TEST_NUMBER(3.1416, "3.1416");
    TEST_NUMBER(1E10, "1E10");
    TEST_NUMBER(1e10, "1e10");
    TEST_NUMBER(1E+10, "1E+10");
    TEST_NUMBER(1E-10, "1E-10");
    TEST_NUMBER(-1E10, "-1E10");
    TEST_NUMBER(-1e10, "-1e10");
    TEST_NUMBER(-1E+10, "-1E+10");
    TEST_NUMBER(-1E-10, "-1E-10");
    TEST_NUMBER(1.234E+10, "1.234E+10");
    TEST_NUMBER(1.234E-10, "1.234E-10");
    TEST_NUMBER(0.0, "1e-10000");
}

#define TEST_STRING(expect, json)\
    do{\
        lept_value v;\
        lept_init(&v);\
        EXPECT_EQ_INT(LEPT_PARSE_OK,lept_parse(&v,json));\
        EXPECT_EQ_INT(LEPT_STRING,lept_get_type(&v));\
        EXPECT_EQ_INT(expect, lept_get_string(&v), lept_get_string_length(&v));\
        lept_free(&v);\
    } while(0)

static void test_parse_string()
{
    TEST_STRING("","\"\"");
    TEST_STRING("hello","\"hello\"");
    TEST_STRING("hello\nworld","\"hello\\nworld\"");
    TEST_STRING("hello\0world","\"hello\\u0000world\"");
    TEST_STRING("\x24","\"\\u0024\"");
    TEST_STRING("\xC2\xA2","\"\\u00A2\"");
    TEST_STRING("\xE2\x82\xAC","\"\\u20AC\"");
    TEST_STRING("\xF0\x9D\x84\x9E", "\"\\uD834\\uDD1E\"");
}    

static test_parse_array()
{   size_t i,j;
    lept_value v;
    
    lept_init(&v);
    EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, "[ ]"));
    EXPECT_EQ_INT(LEPT_ARRAY,lept_get_type(&v));
    EXPECT_EQ_INT(0, lept_get_array_size(&v));
    lept_free(&v);


    lept_init(&v);
    EXPECT_EQ_INT(LEPT_PARSE_OK,lept_parse(&v, "[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]"));
    EXPECT_EQ_INT(LEPT_ARRAY, lept_get_type(&v));
    EXPECT_EQ_INT(4,lept_get_array_size(&v));
    for (i=0; i< 4; i++)
    {
        lept_value* a = lept_get_array_element(&v,i);
        EXPECT_EQ_INT(LEPT_ARRAY,lept_get_type(a));
        EXPECT_EQ_INT(i,lept_get_array_size(a));
        for(j=0; j < i; j++)
        {
            lept_value* e= lept_get_array_element(a,j);
            EXPECT_EQ_INT(LEPT_NUMBER,lept_get_type(&e));
            EXPECT_EQ_INT((double)j, lept_get_number(&e));
        }
    }
    lept_free(&v);
}

static void test_parse_object() {
    lept_value v;
    size_t i;

    lept_init(&v);
    EXPECT_EQ_INT(LEPT_PARSE_OK,lept_parse(&v,"{ }"));
    EXPECT_EQ_INT(LEPT_OBJECT,lept_get_type(&v));
    EXPECT_EQ_SIZE_T(0,lept_get_object_size(&v));
    lept_free(&v);

    lept_init(&v);
    EXPECT_EQ_INT(LEPT_PARSE_OK,lept_parse(&v,
        "{"
        "\"n\" : null ,"
        "\"f\" : false,"
        "\"t\" : true,"
        "\"i\" : 123 ,"
        "\"s\" : \"abc\","
        "\"a\" : [1,2,4],"
        "\"o\" : {\"1\" : 1, \"2\": 2,\"3\":3}"
        "}"
        ));
    EXPECT_EQ_INT(LEPT_OBJECT,lept_get_type(&v));
    EXPECT_EQ_SIZE_T(7,lept_get_object_size(&v));
    EXPECT_EQ_STRING("n",lept_get_object_key(&v,0),lept_get_object_key_length(&v, 0));
    EXPECT_EQ_INT(LEPT_NULL,lept_get_type(lept_get_object_value(&v,0)));
    EXPECT_EQ_STRING("f",lept_get_object_key(&v,1),lept_get_object_key_length(&v,1));
    EXPECT_EQ_INT(LEPT_FALSE,lept_get_type(lept_parse_objct_value(&v,1)));
    EXPECT_EQ_STRING("t", lept_get_object_key(&v, 2), lept_get_object_key_length(&v, 2));
    EXPECT_EQ_INT(LEPT_TRUE, lept_get_type(lept_get_object_value(&v, 2)));
    EXPECT_EQ_STRING("i",lept_get_object_key(&v,3),lept_get_object_key_length(&v,3));
    EXPECT_EQ_INT(LEPT_NUMBER,lept_get_type(lept_get_object_value(&v,3)));
    EXPECT_EQ_DOUBLE(123.0, lept_get_number(lept_get_object_value(&v, 3)));
    EXPECT_EQ_STRING("s", lept_get_object_key(&v, 4), lept_get_object_key_length(&v, 4));
    EXPECT_EQ_INT(LEPT_STRING, lept_get_type(lept_get_object_value(&v, 4)));
    EXPECT_EQ_STRING("abc", lept_get_string(lept_get_object_value(&v, 4)), lept_get_string_length(lept_get_object_value(&v, 4)));
    EXPECT_EQ_STRING("a", lept_get_object_key(&v, 5), lept_get_object_key_length(&v, 5));
    EXPECT_EQ_INT(LEPT_ARRAY, lept_get_type(lept_get_object_value(&v, 5)));
}


#define TEST_ERROR(error, json)\
    do {\
        lept_value v;\
        lept_init(&v);\
        v.type = LEPT_FALSE;\
        EXPECT_EQ_INT(error,lept_parse(&v,json));\
        EXPECT_EQ_INT(LEPT_NULL,lept_get_type(&v));\
        lept_free(&v);\
    } while(0)

static void test_parse_root_not_singular() 
{
    TEST_ERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "null x");


    
    /* invalid number */
    TEST_ERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "0123"); /* after zero should be '.' or nothing */
    TEST_ERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "0x0");
    TEST_ERROR(LEPT_PARSE_ROOT_NOT_SINGULAR, "0x123");

}

static void test_parse_expect_value()
{
    TEST_ERROR(LEPT_PARSE_EXPECT_VALUE, "");
    TEST_ERROR(LEPT_PARSE_EXPECT_VALUE, " ");
   
}

static void test_parse_invalid_value()
{
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "nuxx");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "scc");
     /* invalid number */
   
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "+0");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "+1");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, ".123"); /* at least one digit before '.' */
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "1.");   /* at least one digit after '.' */
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "INF");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "inf");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "NAN");
    TEST_ERROR(LEPT_PARSE_INVALID_VALUE, "nan");

}

static void test_parse_number_too_big()
{
    
    TEST_ERROR(LEPT_PARSE_NUMBER_TOO_BIG, "1e309");
    TEST_ERROR(LEPT_PARSE_NUMBER_TOO_BIG, "-1e309");
    
}


static void test_parse()
{
	test_parse_null();
    test_parse_true();
    test_parse_false();
    test_parse_number();
    test_parse_invalid_value();
    test_parse_root_not_singular();
}


int main()
{
	test_parse();
	printf("%d/%d (%3.2f) passed\n", test_pass, test_count, test_pass*100.0 / test_count );
	return main_ret;
}