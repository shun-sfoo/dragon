#include <assert.h>
#include <errno.h> /* errno, ERANGE */
#include <math.h>  /* HUGE_VAL */
#include <stddef.h>
#include <stdlib.h> /* NULL, strtod() */
#include <string.h>

#include "leptjson.h"

#ifndef LEPT_PARSE_STACK_INIT_SIZE
#define LEPT_PARSE_STACK_INIT_SIZE 256
#endif

#define EXPECT(c, ch)         \
  do {                        \
    assert(*c->json == (ch)); \
    c->json++;                \
  } while (0)

#define ISDIGIT(ch) ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch) ((ch) >= '1' && (ch) <= '9')

#define PUTC(c, ch)                                    \
  do {                                                 \
    *(char*)lept_context_push(c, sizeof(char)) = (ch); \
  } while (0)

typedef struct {
  const char* json;
  char* stack;
  size_t size, top;
} lept_context;

static void* lept_context_push(lept_context* c, size_t size) {
  void* ret;
  if (c->top + size >= c->size) {
    if (c->size == 0) c->size = LEPT_PARSE_STACK_INIT_SIZE;

    while (c->top + size >= c->size)
      c->size += c->size >> 1;

    c->stack = (char*)realloc(c->stack, c->size);
  }
  ret = c->stack + c->top;
  c->top += size;
  return ret;
}

static void* lept_context_pop(lept_context* c, size_t size) {
  assert(c->top >= size);
  return c->stack += (c->top -= size);
}

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
  v->u.n = strtod(c->json, NULL);
  if (errno == ERANGE && (v->u.n == HUGE_VAL || v->u.n == -HUGE_VAL))
    return LEPT_PARSE_NUMBER_TOO_BIG;
  c->json = p;
  v->type = LEPT_NUMBER;
  return LEPT_PARSE_OK;
}

static int lept_parse_string(lept_context* c, lept_value* v) {
  size_t head = c->top, len;
  assert(c != NULL);
  const char* p;
  EXPECT(c, '\"');
  p = c->json;
  for (;;) {
    char ch = *p++;
    switch (ch) {
      case '\"':
        len = c->top - head;
        lept_set_string(v, (const char*)lept_context_pop(c, len), len);
        c->json = p;
        return LEPT_PARSE_OK;
      case '\n':
        PUTC(c, '\\');
        break;
      case '\0':
        c->top = head;
        return LEPT_PARSE_MISS_QUOTATION_MARK;
      default:
        PUTC(c, ch);
    }
  }
}

static int lept_parse_value(lept_context* c, lept_value* v) {
  switch (*c->json) {
    case 'n':
      return lept_parse_null(c, v);
    case 't':
      return lept_parse_true(c, v);
    case 'f':
      return lept_parse_false(c, v);
    case '\"':
      return lept_parse_string(c, v);
    case '\0':
      return LEPT_PARSE_EXPECT_VALUE;
    default:
      return lept_parse_number(c, v);
  }
}

lept_type lept_get_type(const lept_value* v) {
  assert(v != NULL);
  return v->type;
}

double lept_get_number(const lept_value* v) {
  assert(v != NULL && v->type == LEPT_NUMBER);
  return v->u.n;
}

void lept_free(lept_value* v) {
  assert(v != NULL);
  if (v->type == LEPT_STRING) free(v->u.s.s);
  v->type = LEPT_NULL;
}

void lept_set_string(lept_value* v, const char* s, size_t len) {
  assert(v != NULL && (s != NULL || len == 0));
  lept_free(v);
  v->u.s.s = (char*)malloc(len + 1);
  memcpy(v->u.s.s, s, len);
  v->u.s.s[len] = '\0';
  v->u.s.len = len;
  v->type = LEPT_STRING;
}

void lept_set_boolean(lept_value* v, int b) {
  assert(v != NULL);
  v->type = b ? LEPT_TRUE : LEPT_FALSE;
}

int lept_get_boolean(const lept_value* v) {
  assert(v != NULL || v->type == LEPT_FALSE || v->type == LEPT_TRUE);
  return v->type == LEPT_TRUE;
}

char* lept_get_string(const lept_value* v) {
  assert(v != NULL);
  return v->u.s.s;
}

size_t lept_get_string_length(const lept_value* v) {
  assert(v != NULL && v->type == LEPT_STRING);
  return v->u.s.len;
}

void lept_set_number(lept_value* v, double d) {
  assert(v != NULL && v->type == LEPT_NUMBER);
  v->u.n = d;
}

/* ws json ws */
int lept_parse(lept_value* v, const char* json) {
  lept_context c;
  int ret;
  assert(v != NULL);
  c.json = json;
  c.stack = NULL;
  c.size = c.top = 0;
  lept_init(v);
  v->type = LEPT_NULL;
  lept_parse_whitespace(&c);
  if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK) {
    if (*c.json != '\0') return LEPT_PARSE_ROOT_NOT_SINGULAR;
  }

  assert(c.top == 0);
  free(c.stack);
  return ret;
}
