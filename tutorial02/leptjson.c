#include "leptjson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL, strtod() */
#include <stdbool.h>

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)

typedef struct {
    const char* json;
}lept_context;

static void lept_parse_whitespace(lept_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

bool Is1to9(char* a)
{
	if (*a >= '1' && *a <= '9')
	{
		return true;
	}
	return false;
}

bool Is0to9(char* a)
{
	if (*a >= '0' && *a <= '9')
	{
		return true;
	}
	return false;
}
//这里其实就是我们在找出一个最长合法数据串的长度并把它转换成数据，所以只有
//取到正常值我们就往前走，没有就不走来拿到最长正确数字。
static int lept_parse_number(lept_context* c, lept_value* v)
{
	//这里可以减少对c的访问
	const char* p = c->json;
	//负数
	if (p == "-")
	{
		p++;
	}

	//一般整数（这里保证了单独符号不成立）
	//单独0
	if (*p == '0')
	{
		p++;
	}
	// 1- 9
	else

		if (!Is1to9(p))
		{
			return LEPT_PARSE_INVALID_VALUE;
		}
		else
		{
			p++;
			for (; Is0to9(p); p++)
			{
			}
		}

	//小数
	if (*p == '.')
	{
		p++;
		if (!Is0to9(p))
		{
			return LEPT_PARSE_INVALID_VALUE;
		}
		else
		{
			for (; Is0to9(p); p++)
			{
			}
		}
	}

	//指数
	if (*p == 'E' || *p == 'e')
	{
		p++;
		if (*p == '+' || *p == '-')
		{
			if (!Is0to9(p))
			{
				return LEPT_PARSE_INVALID_VALUE;
			}
			else
			{
				for (; Is0to9(p); p++)
				{
				}
			}
		}
		else
		{
			return LEPT_PARSE_INVALID_VALUE;
		}
	}

	//完全没有前进
	if (p == c->json)
	{
		return LEPT_PARSE_INVALID_VALUE;
	}

	//字符串转换为double，返回第一个无法转换的值的指针。
	v->n = strtod(c->json, NULL);
	//if (c->json == end)
	//	return LEPT_PARSE_INVALID_VALUE;
	c->json = p;
	v->type = LEPT_NUMBER;
	return LEPT_PARSE_OK;
}

static int lept_parse_value(lept_context* c, lept_value* v) {
    switch (*c->json) 
	{
        case 't':  return lept_parse_literal(c, v, "true", LEPT_TRUE);
        case 'f':  return lept_parse_literal(c, v, "false", LEPT_FALSE);
        case 'n':  return lept_parse_literal(c, v, "null", LEPT_NULL);
        default:   return lept_parse_number(c, v);
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
    }
}

int lept_parse(lept_value* v, const char* json) {
    lept_context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    v->type = LEPT_NULL;
    lept_parse_whitespace(&c);
    if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK) {
        lept_parse_whitespace(&c);
        if (*c.json != '\0') {
            v->type = LEPT_NULL;
            ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}

lept_type lept_get_type(const lept_value* v) {
    assert(v != NULL);
    return v->type;
}

double lept_get_number(const lept_value* v) {
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->n;
}

//这里我一开始的想法是在里面判断是true,false还是null，但这种做法不好，会导致函数承担的任务过多，
//还需要修改调用它的函数。
static int lept_parse_literal(lept_context* c, lept_value* v, const char* literal, lept_type type)
{
    EXPECT(c, literal[0]);
    size_t i = 0;
    for ( ; literal[i + 1] != '\0'; i++)
    {
        if (literal[i+ 1] != c-> json[i])
        {
            return LEPT_PARSE_INVALID_VALUE;
        }
    }

    //这里需要重新定位json的起始位置，因为我们还在后边需要判断后面有没有空缺等
    c->json += i;
    v->type = type;
    return LEPT_PARSE_OK;
}
