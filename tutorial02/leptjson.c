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
//������ʵ�����������ҳ�һ����Ϸ����ݴ��ĳ��Ȳ�����ת�������ݣ�����ֻ��
//ȡ������ֵ���Ǿ���ǰ�ߣ�û�оͲ������õ����ȷ���֡�
static int lept_parse_number(lept_context* c, lept_value* v)
{
	//������Լ��ٶ�c�ķ���
	const char* p = c->json;
	//����
	if (p == "-")
	{
		p++;
	}

	//һ�����������ﱣ֤�˵������Ų�������
	//����0
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

	//С��
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

	//ָ��
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

	//��ȫû��ǰ��
	if (p == c->json)
	{
		return LEPT_PARSE_INVALID_VALUE;
	}

	//�ַ���ת��Ϊdouble�����ص�һ���޷�ת����ֵ��ָ�롣
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

//������һ��ʼ���뷨���������ж���true,false����null���������������ã��ᵼ�º����е���������࣬
//����Ҫ�޸ĵ������ĺ�����
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

    //������Ҫ���¶�λjson����ʼλ�ã���Ϊ���ǻ��ں����Ҫ�жϺ�����û�п�ȱ��
    c->json += i;
    v->type = type;
    return LEPT_PARSE_OK;
}
