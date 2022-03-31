#ifndef LEPTJSON_H__
#define LEPTJSON_H__

typedef enum {
  LEPT_NULL,
  LEPT_TRUE,
  LEPT_FALSE,
  LEPT_ARRAY,
  LEPT_OBJECT,
  LEPT_STRING,
  LEPT_NUMBER
} lept_type;

typedef struct {
  lept_type type;
} lept_value;

int lept_parse(lept_value* v, const char* json);

enum {
  LEPT_PARSE_OK = 0,
  LEPT_PARSE_EXPECT_VALUE,
  LEPT_PARSE_INVALID_VALUE,
  LEPT_PARSE_ROOT_NOT_SINGULAR,
};

lept_type lept_get_type(const lept_value* v);

#endif
