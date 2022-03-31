#include <assert.h>
#include <errno.h> /* errno, ERANGE */
#include <math.h>  /* HUGE_VAL */
#include <stddef.h>
#include <stdlib.h> /* NULL, strtod() */

#include "leptjson.h"

#define EXPECT(c, ch)         \
  do {                        \
    assert(*c->json == (ch)); \
    c->json++;                \
  } while (0)

#define ISDIGIT(ch) ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch) ((ch) >= '1' && (ch) <= '9')

typedef struct {
  const char* json;
} lept_context;

/* ws = *(%x20 / %x09 / %x0A / %x0D) */
static void lept_parse_whitespace(lept_context* c) {
  const char* p = c->json;
  while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
    p++;

  c->json = p;
}

static int lept_parse_literal(lept_context* c,
                              lept_value* v,
                              const char* literal,
                              lept_type type) {
  size_t i;
  EXPECT(c, literal[0]);
  for (i = 0; literal[i + 1]; i++) {
    if (c->json[i] != literal[i + 1]) {
      return LEPT_PARSE_INVALID_VALUE;
    }
  }

  c->json += i;
  v->type = type;
  return LEPT_PARSE_OK;
}

/* null  = "null" */
static int lept_parse_null(lept_context* c, lept_value* v) {
  return lept_parse_literal(c, v, "null", LEPT_NULL);
}

/* t ➔ true */
static int lept_parse_true(lept_context* c, lept_value* v) {
  return lept_parse_literal(c, v, "true", LEPT_TRUE);
}

/* f ➔ false */
static int lept_parse_false(lept_context* c, lept_value* v) {
  return lept_parse_literal(c, v, "false", LEPT_FALSE);
}

/* number = [ "-" ] int [ frac ] [ exp ] */
/* int = "0" / digit1-9 *digit */
/* frac = "." 1*digit */
/* exp = ("e" / "E") ["-" / "+"] 1*digit */
static int lept_parse_number(lept_context* c, lept_value* v) {
  const char* p = c->json;
  if (*p == '-') p++;

  if (*p == '0') {
    p++;
  } else {
    if (!ISDIGIT1TO9(*p)) return LEPT_PARSE_INVALID_VALUE;

    for (p++; ISDIGIT(*p); p++)
      ;
  }

  if (*p == '.') {
    p++;
    if (!ISDIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;

    for (p++; ISDIGIT(*p); p++)
      ;
  }

  if (*p == 'E' || *p == 'e') {
    p++;

    if (*p == '-' || *p == '+') p++;

    if (!ISDIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;

    for (p++; ISDIGIT(*p); p++)
      ;
  }

  errno = 0;
  v->n = strtod(c->json, NULL);
  if (errno == ERANGE && (v->n == HUGE_VAL || v->n == -HUGE_VAL))
    return LEPT_PARSE_NUMBER_TOO_BIG;
  c->json = p;
  v->type = LEPT_NUMBER;
  return LEPT_PARSE_OK;
}

static int lept_parse_value(lept_context* c, lept_value* v) {
  switch (*c->json) {
    case 'n':
      return lept_parse_null(c, v);
    case 't':
      return lept_parse_true(c, v);
    case 'f':
      return lept_parse_false(c, v);
    case '\0':
      return LEPT_PARSE_EXPECT_VALUE;
    default:
      return lept_parse_number(c, v);
  }
}

/* ws json ws */
int lept_parse(lept_value* v, const char* json) {
  lept_context c;
  int ret;
  assert(v != NULL);
  c.json = json;
  v->type = LEPT_NULL;
  lept_parse_whitespace(&c);
  if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK) {
    if (*c.json != '\0') return LEPT_PARSE_ROOT_NOT_SINGULAR;
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
