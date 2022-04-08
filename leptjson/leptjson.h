#ifndef LEPTJSON_H__
#define LEPTJSON_H__

#include <stddef.h>

#define lept_init(v) \
  do { (v)->type = LEPT_NULL; } while (0)

#define lept_set_null(v) lept_free(v)

typedef enum {
  LEPT_NULL,
  LEPT_TRUE,
  LEPT_FALSE,
  LEPT_ARRAY,
  LEPT_OBJECT,
  LEPT_STRING,
  LEPT_NUMBER
} lept_type;

typedef struct lept_value lept_value;

struct lept_value {
  lept_type type;
  union {
    struct {
      lept_value* e;
      size_t size;
    } a; /* array */

    struct {
      char* s;
      size_t len;
    } s;

    double n;
  } u;
};

enum {
  LEPT_PARSE_OK = 0,
  LEPT_PARSE_EXPECT_VALUE,
  LEPT_PARSE_INVALID_VALUE,
  LEPT_PARSE_ROOT_NOT_SINGULAR,
  LEPT_PARSE_NUMBER_TOO_BIG,
  LEPT_PARSE_MISS_QUOTATION_MARK,
  LEPT_PARSE_INVALID_STRING_ESCAPE,
  LEPT_PARSE_INVALID_STRING_CHAR,
  LEPT_PARSE_INVALID_UNICODE_HEX,
  LEPT_PARSE_INVALID_UNICODE_SURROGATE
};

int lept_parse(lept_value* v, const char* json);

int lept_get_boolean(const lept_value* v);

void lept_set_boolean(lept_value* v, int b);

lept_type lept_get_type(const lept_value* v);

double lept_get_number(const lept_value* v);

void lept_set_number(lept_value* v, double b);

const char* lept_get_string(const lept_value* v);

size_t lept_get_string_length(const lept_value* v);

void lept_set_string(lept_value* v, const char* s, size_t len);

void lept_set_null(lept_value* v);

size_t lept_get_array_size(const lept_value* v);

lept_value* lept_get_array_element(const lept_value* v, size_t index);

void lept_free(lept_value* v);

#endif
