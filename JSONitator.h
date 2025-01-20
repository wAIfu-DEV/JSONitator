#pragma once

/**
 * @author w-AI-fu_DEV
 * @date 2025-01-18
 * @license MIT
 */

#if !defined(JSON_PARSER_DEBUG)
#define JSON_PARSER_DEBUG 0
#endif

#if !defined(JSON_PARSER_SILENT)
#define JSON_PARSER_SILENT 0
#endif

#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"
#include "ctype.h"
#include "string.h"
#include "math.h"

typedef uint8_t VALUE_TYPE;

const VALUE_TYPE VAL_OBJECT = 0;
const VALUE_TYPE VAL_ARRAY = 1;
const VALUE_TYPE VAL_NUMBER = 2;
const VALUE_TYPE VAL_STRING = 3;
const VALUE_TYPE VAL_BOOL = 4;
const VALUE_TYPE VAL_NULL = 5;

const char *__VAL_TO_STR[] = {
    [VAL_OBJECT] = "object",
    [VAL_ARRAY] = "array",
    [VAL_NUMBER] = "number",
    [VAL_STRING] = "string",
    [VAL_BOOL] = "bool",
    [VAL_NULL] = "null",
};

const char *__LIST_SEQ = "\"\\\b\f\n\r\t";
const char *__LIST_ESC = "\"\\/bfnrt";

char __ESC_TO_SEQ[117] = {
    ['\"'] = '\"',
    ['\\'] = '\\',
    ['/'] = '/',
    ['b'] = '\b',
    ['f'] = '\f',
    ['n'] = '\n',
    ['r'] = '\r',
    ['t'] = '\t',
};

const char *__SEQ_TO_ESC[93] = {
    ['\"'] = "\\\"",
    ['\\'] = "\\\\",
    ['\b'] = "\\b",
    ['\f'] = "\\f",
    ['\n'] = "\\n",
    ['\r'] = "\\r",
    ['\t'] = "\\t",
};

const uint64_t __MAX_ITER = ((uint64_t)0) - 1;

typedef struct json
{
    void *value;
    VALUE_TYPE type;
} JSON;

typedef struct json_obj
{
    char **fields;
    JSON **values;
    uint64_t length;
} JSON_OBJECT;

typedef struct json_array
{
    JSON **elements;
    uint64_t length;
} JSON_ARRAY;

typedef double JSON_NUMBER;

typedef struct string_seg
{
    char *str;
    uint64_t len;
} __STRING_SEGMENT;

typedef struct builder
{
    __STRING_SEGMENT *segments;
    uint64_t len;
} __BUILDER;

errno_t __parse_any_value(JSON *self, const char *s, uint64_t *i);
errno_t __parse_object(JSON *self, const char *s, uint64_t *i);
errno_t __parse_array(JSON *self, const char *s, uint64_t *i);

void __print(const char *s)
{
    if (JSON_PARSER_SILENT)
        return;

    __printf("JSONparser: %s\n", s);
}

void __printf(const char *format, ...)
{
    if (JSON_PARSER_SILENT)
        return;

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

int __is_whitespace(char c)
{
    if (JSON_PARSER_DEBUG)
        __print("__is_whitespace");

    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

errno_t __init_object(JSON_OBJECT *self)
{
    if (JSON_PARSER_DEBUG)
        __print("__init_object");

    self->fields = malloc(sizeof(char *));

    if (self->fields == NULL)
    {
        __print("Failed to alloc fields in object.");
        return 1;
    }

    self->values = malloc(sizeof(JSON *));

    if (self->values == NULL)
    {
        __print("Failed to alloc values in object.");
        return 1;
    }

    self->fields[0] = NULL;
    self->values[0] = NULL;
    self->length = 0;

    return 0;
}

int __str_contains_c(const char *s, char c)
{
    if (JSON_PARSER_DEBUG)
        __print("__str_contains_c");

    uint64_t i = 0;
    while (s[i] != '\0')
    {
        if (s[i] == c)
            return 1;
        ++i;
    }
    return 0;
}

int __str_comp(const char *x, const char *y)
{
    if (JSON_PARSER_DEBUG)
        __print("__str_comp");

    for (uint64_t i = 0; i < (uint64_t)-1; ++i)
    {
        if (!x[i] || !y[i])
        {
            return (x[i] == y[i]);
        }

        if (x[i] != y[i])
            return 0;
    }
    return 0;
}

uint64_t __str_len(const char *s)
{
    if (JSON_PARSER_DEBUG)
        __print("__str_len");

    if (s == NULL)
        return 0;

    uint64_t i = 0;

    if (s[0] == '\0')
    {
        return 0;
    }

    while (s[i] != '\0')
    {
        if (i == __MAX_ITER)
        {
            __print("__MAX_ITER reached in __str_len");
            return 0;
        }
        ++i;
    }
    return i;
}

uint64_t __number_len(const char *s)
{
    if (JSON_PARSER_DEBUG)
        __print("__number_len");

    if (s == NULL)
        return 0;

    uint64_t i = 0;

    if (s[0] == '\0')
    {
        return 0;
    }

    while (s[i] != '\0' && (isdigit(s[i]) || s[i] == '.' || s[i] == '-'))
    {
        if (i == __MAX_ITER)
        {
            __print("__MAX_ITER reached in __number_len");
            return 0;
        }
        ++i;
    }
    return i;
}

errno_t __str_copy_alloc(const char *s, char **buff)
{
    if (JSON_PARSER_DEBUG)
        __print("__str_copy_alloc");

    if (s == NULL)
    {
        __print("s is NULL in call to __str_copy_alloc");
        (*buff) = malloc(sizeof(char));

        if ((*buff) == NULL)
        {
            __print("Failed to allocate empty string in __str_copy_alloc");
            return 1;
        }

        (*buff)[0] = '\0';
        return 1;
    }

    uint64_t len = __str_len(s);
    (*buff) = malloc(sizeof(char) * (len + 1));

    if ((*buff) == NULL)
    {
        __print("Failed to allocate string in __str_copy_alloc");
        return 1;
    }

    if (len == 0)
    {
        (*buff)[0] = '\0';
        return 0;
    }

    uint64_t i = 0;

    while (s[i] != 0)
    {
        if (i == __MAX_ITER)
        {
            __print("__MAX_ITER reached in __str_copy_alloc");
            (*buff)[0] = '\0';
            return 1;
        }

        (*buff)[i] = s[i];
        ++i;
    }

    (*buff)[i] = '\0';
    return 0;
}

errno_t __append_object_entry(JSON_OBJECT *self, const char *field, JSON *value)
{
    if (JSON_PARSER_DEBUG)
        __print("__append_object_entry");

    self->fields = realloc(self->fields, sizeof(char *) * (self->length + 2));

    if (self->fields == NULL)
    {
        __print("Failed to reallocate fields in __append_object_entry");
        return 1;
    }

    self->values = realloc(self->values, sizeof(JSON *) * (self->length + 2));

    if (self->values == NULL)
    {
        __print("Failed to reallocate values in __append_object_entry");
        return 1;
    }

    errno_t res = __str_copy_alloc(field, &self->fields[self->length]);

    if (res != 0)
    {
        __print("Failed to field name in __append_object_entry");
        return 1;
    }

    if (JSON_PARSER_DEBUG)
    {
        __printf("JSONparser: Added field %s to object.\n", field);
    }

    self->fields[self->length + 1] = NULL;

    self->values[self->length] = value;
    self->fields[self->length + 1] = NULL;

    self->length += 1;
    return 0;
}

uint64_t __unparsed_str_len(const char *s)
{
    if (JSON_PARSER_DEBUG)
        __print("__unparsed_str_len");

    int is_escaped = 0;

    uint64_t i = 0;

    while (!(s[i] == '"' && !is_escaped))
    {
        is_escaped = 0;

        if (i == __MAX_ITER)
        {
            __print("__MAX_ITER reached in __unparsed_str_len");
            return 0;
        }

        if (s[i] == '\0')
        {
            __print("Failed to get length of string, found EOF while counting.");
            return 0;
        }

        if (s[i] == '\\')
        {
            is_escaped = 1;
        }

        ++i;
    }

    return i;
}

char *__parse_string(const char *s, uint64_t *i)
{
    if (JSON_PARSER_DEBUG)
        __print("__parse_string");

    (*i) += 1; // Add opening quote to i

    uint64_t len = __unparsed_str_len(&s[*i]);
    char *parsed = malloc(sizeof(char) * (len + 1));

    uint64_t si = *i;
    uint64_t pi = 0;
    int is_escaped = 0;

    while (!(s[si] == '"' && !is_escaped))
    {
        if (si == __MAX_ITER)
        {
            __print("__MAX_ITER reached in __parse_string");
            return NULL;
        }

        if (is_escaped)
        {
            if (s[si] == '\0' || !__str_contains_c(__LIST_ESC, s[si]))
            {
                __printf("JSONparser: Found invalid escaped character '\\%c' inside string.", s[si] ? s[si] : '0');
                return NULL;
            }

            char seq = __ESC_TO_SEQ[s[si]];

            parsed[si - pi - (*i)] = seq;
            ++si;

            is_escaped = 0;
            continue;
        }

        is_escaped = 0;

        if (s[si] == '\0')
        {
            __print("Failed to parse string: found EOF during parsing.");
            return NULL;
        }

        if (s[si] == '\\')
        {
            is_escaped = 1;
            ++si;
            ++pi;
            continue;
        }

        parsed[si - pi - (*i)] = s[si];
        ++si;
    }
    parsed[si - pi - (*i)] = '\0';

    (*i) += len + 1; // we add length of str + last closing quote
    return parsed;
}

errno_t __consume_colon(const char *s, uint64_t *i)
{
    if (JSON_PARSER_DEBUG)
        __print("__consume_colon");

    while (s[*i] != '\0')
    {
        if ((*i) == __MAX_ITER)
        {
            __print("__MAX_ITER reached in __consume_colon");
            return 1;
        }

        if (__is_whitespace(s[*i]))
        {
            ++(*i);
            continue;
        }

        if (s[*i] == ':')
        {
            ++(*i);
            return 0;
        }

        __printf("JSONparser: Expected character ':' but found unexpected '%c' instead at position %lld.\n", s[*i], *i);
        return 1;
    }

    __print("Found EOF while expecting character ':'");
    return 1;
}

errno_t __parse_number(JSON *self, const char *s, uint64_t *i)
{
    if (JSON_PARSER_DEBUG)
        __print("__parse_number");

    if (s[0] == '\0')
    {
        __print("Failed to parse object: found EOF during parsing in __parse_number");
        return 1;
    }

    uint64_t len = __number_len(&s[*i]);

    char *digits_buff = malloc(sizeof(char) * (len + 1));

    if (digits_buff == NULL)
    {
        __print("Failed to allocate digits string in __parse_number");
        return 1;
    }

    for (uint64_t j = 0; j < len; ++j)
    {
        digits_buff[j] = s[*i + j];
    }
    digits_buff[len] = '\0';

    JSON_NUMBER *num_ptr = malloc(sizeof(JSON_NUMBER));
    if (num_ptr == NULL)
    {
        __print("Failed to allocate memory for digits buffer in __parse_number.");
        free(digits_buff);
        digits_buff = NULL;
        return 1;
    }

    errno = 0;
    (*num_ptr) = (double)strtod(digits_buff, NULL);
    if (errno != 0)
    {
        __print("Failed to parse number using strtold.");
        free(digits_buff);
        digits_buff = NULL;
        return 1;
    }

    self->type = VAL_NUMBER;
    self->value = num_ptr;

    (*i) += len;
    free(digits_buff);
    digits_buff = NULL;
    return 0;
}

errno_t __parse_any_value(JSON *self, const char *s, uint64_t *i)
{
    if (JSON_PARSER_DEBUG)
        __print("__parse_any_value");

    if (s[0] == '\0')
    {
        __print("Failed to parse object: found EOF during parsing in __parse_any_value");
        return 1;
    }

    while (s[*i] != '\0')
    {
        if ((*i) == __MAX_ITER)
        {
            __print("__MAX_ITER reached in __parse_any_value");
            return 1;
        }

        if (__is_whitespace(s[*i]))
        {
            ++(*i);
            continue;
        }

        if (s[*i] == 'n')
        {
            if (s[(*i) + 1] == 'u' && s[(*i) + 2] == 'l' && s[(*i) + 3] == 'l')
            {
                self->type = VAL_NULL;
                self->value = NULL;
                (*i) += 4;
                return 0;
            }
            else
            {
                __print("Failed to parse null in __parse_any_value");
                return 1;
            }
        }

        if (s[*i] == 't')
        {
            if (s[(*i) + 1] == 'r' && s[(*i) + 2] == 'u' && s[(*i) + 3] == 'e')
            {
                int *boolean = malloc(sizeof(int));

                if (boolean == NULL)
                {
                    __print("Failed to allocate bool in __parse_any_value");
                    return 1;
                }

                (*boolean) = 1;

                self->type = VAL_BOOL;
                self->value = boolean;
                (*i) += 4;
                return 0;
            }
            else
            {
                __print("Failed to parse true in __parse_any_value");
                return 1;
            }
        }

        if (s[*i] == 'f')
        {
            if (s[(*i) + 1] == 'a' && s[(*i) + 2] == 'l' && s[(*i) + 3] == 's' && s[(*i) + 4] == 'e')
            {
                int *boolean = malloc(sizeof(int));

                if (boolean == NULL)
                {
                    __print("Failed to allocate bool in __parse_any_value");
                    return 1;
                }

                (*boolean) = 0;

                self->type = VAL_BOOL;
                self->value = boolean;
                (*i) += 5;
                return 0;
            }
            else
            {
                __print("Failed to parse true in __parse_any_value");
                return 1;
            }
        }

        if (s[*i] == '{')
        {
            self->type = VAL_OBJECT;
            errno_t err = __parse_object(self, s, i);

            if (err != 0)
            {
                __print("Failed to parse object in __parse_any_value");
                return 1;
            }
            return 0;
        }

        if (s[*i] == '[')
        {
            self->type = VAL_ARRAY;
            errno_t err = __parse_array(self, s, i);

            if (err != 0)
            {
                __print("Failed to parse array in __parse_any_value");
                return 1;
            }
            return 0;
        }

        if (s[*i] == '"')
        {
            char *str = __parse_string(s, i);

            if (str == NULL)
            {
                __print("Failed to parse string value in __parse_any_value");
                return 1;
            }

            self->type = VAL_STRING;
            self->value = str;
            return 0;
        }

        if (isdigit(s[*i]) || s[*i] == '.' || s[*i] == '-')
        {
            self->type = VAL_NUMBER;

            errno_t err = __parse_number(self, s, i);
            if (err != 0)
            {
                __print("Failed to parse number in __parse_any_value");
                return 1;
            }
            return 0;
        }

        __printf("JSONparser: Found unexpected '%c' character while parsing value.\n", s[*i]);
        return 1;
    }

    return 0;
}

errno_t __parse_object(JSON *self, const char *s, uint64_t *i)
{
    if (JSON_PARSER_DEBUG)
        __print("__parse_object");

    ++(*i);

    if (s[0] == '\0')
    {
        __print("Failed to parse object: found EOF during parsing in __parse_object");
        return 1;
    }

    JSON_OBJECT *obj = malloc(sizeof(JSON_OBJECT));
    errno_t res = __init_object(obj);

    if (res != 0)
    {
        free(obj);
        obj = NULL;
        __print("Failed to allocate object in __parse_object");
        return 1;
    }

    self->type = VAL_OBJECT;
    self->value = obj;

    while (s[*i] != '\0')
    {
        if ((*i) == __MAX_ITER)
        {
            __print("__MAX_ITER reached in __parse_object");
            goto clean_obj_entries;
        }

        if (__is_whitespace(s[*i]))
        {
            ++(*i);
            continue;
        }

        if (s[*i] == '}')
        {
            ++(*i);
            return 0;
        }

        if (s[*i] == '"')
        {
            char *field = __parse_string(s, i);

            if (field == NULL)
            {
                __print("Failed to parse object, could not parse field string.");
                goto clean_obj_entries;
            }

            int res = __consume_colon(s, i);
            if (res != 0)
            {
                __print("Failed to parse object, incomplete entry missing the ':' character.");
                goto clean_field;
            }

            JSON *value = malloc(sizeof(JSON));

            if (value == NULL)
            {
                __print("Failed to allocate value in __parse_object");
                goto clean_field;
            }

            errno_t err = __parse_any_value(value, s, i);

            if (err != 0)
            {
                __print("Failed to parse value in __parse_object");
                goto clean_value;
            }

            err = __append_object_entry(obj, field, value);

            if (err != 0)
            {
                __print("Failed to parse value in __parse_object");
                goto clean_value;
            }

            while (__is_whitespace(s[*i]))
            {
                ++(*i);
            }

            if (s[*i] == ',')
            {
                free(field);
                field = NULL;

                ++(*i);
                continue;
            }
            else if (s[*i] != '}')
            {
                __printf("JSONparser: Expected ',', or '}', but found '%c' instead at position %lld.\n", s[*i], *i);
                goto clean_value;
            }

            free(field);
            field = NULL;

            ++(*i);
            return 0;

        clean_value:
            free(value);
            value = NULL;
        clean_field:
            free(field);
            field = NULL;
        clean_obj_entries:
            free(obj->fields);
            obj->fields = NULL;
            free(obj->values);
            obj->values = NULL;
        clean_obj:
            free(obj);
            obj = NULL;
            return 1;
        }

        __printf("JSONparser: Found unexpected '%c' character while parsing object.\n", s[*i]);
        free(obj);
        obj = NULL;
        return 1;
    }

    __print("Found EOF when parsing object.");
    free(obj);
    obj = NULL;
    return 1;
};

errno_t __init_array(JSON_ARRAY *self)
{
    if (JSON_PARSER_DEBUG)
        __print("__init_array");

    self->elements = malloc(sizeof(JSON *));
    if (self->elements == NULL)
    {
        __print("Failed to allocate elements in array.");
        return 1;
    }

    self->length = 0;
    return 0;
}

errno_t __append_array_element(JSON_ARRAY *array, JSON *value)
{
    if (JSON_PARSER_DEBUG)
        __print("__append_array_element");

    array->elements = realloc(array->elements, sizeof(JSON *) * (array->length + 1));
    if (array->elements == NULL)
    {
        __print("Failed to reallocate elements in array.");
        return 1;
    }

    array->elements[array->length] = value;
    array->length += 1;
    return 0;
}

errno_t __parse_array(JSON *self, const char *s, uint64_t *i)
{
    if (JSON_PARSER_DEBUG)
        __print("__parse_array");

    (*i)++;

    if (s[0] == '\0')
    {
        __print("Failed to parse array: found EOF during parsing in __parse_array");
        return 1;
    }

    JSON_ARRAY *array = malloc(sizeof(JSON_ARRAY));

    if (array == NULL)
    {
        __print("Failed to allocate array in __parse_array");
        return 1;
    }

    errno_t err = __init_array(array);
    if (err != 0)
    {
        free(array);
        array = NULL;
        return 1;
    }

    self->type = VAL_ARRAY;
    self->value = array;

    while (s[*i] != '\0')
    {
        if ((*i) == __MAX_ITER)
        {
            __print("__MAX_ITER reached in __parse_array");
            goto clean_arr_elems;
        }

        if (__is_whitespace(s[*i]))
        {
            ++(*i);
            continue;
        }

        if (s[*i] == ']')
        {
            ++(*i);
            return 0;
        }

        JSON *value = malloc(sizeof(JSON));

        if (value == NULL)
        {
            __print("Failed to allocate JSON for element in array.");
            goto clean_arr_elems;
        }

        errno_t err = __parse_any_value(value, s, i);
        if (err != 0)
        {
            __print("Failed to parse value in array.");
            goto clean_value;
        }

        err = __append_array_element(array, value);
        if (err != 0)
        {
            goto clean_value;
        }

        while (__is_whitespace(s[*i]))
        {
            ++(*i);
        }

        if (s[*i] == ',')
        {
            ++(*i);
            continue;
        }

        if (s[*i] == ']')
        {
            ++(*i);
            return 0;
        }

        __printf("JSONparser: Unexpected character '%c' while parsing array.\n", s[*i]);

    clean_value:
        free(value);
        value = NULL;
    clean_arr_elems:
        free(array->elements);
        array->elements = NULL;
    clean_arr:
        free(array);
        array = NULL;
        return 1;
    }

    __print("Found EOF while parsing array.");
    free(array->elements);
    array->elements = NULL;
    free(array);
    array = NULL;
    return 1;
}

/**
 * @brief Parses a JSON string and returns a `JSON` struct pointer
 *
 * @param s json string (s is not modified)
 * @return NULL | JSON* (memory owned, you need to free it using `json_free`)
 */
JSON *json_parse(const char *s)
{
    if (JSON_PARSER_DEBUG)
        __print("json_parse");

    if (s == NULL)
        return NULL;

    uint64_t i = 0;

    while (1)
    {
        if (i == __MAX_ITER)
        {
            __print("__MAX_ITER reached in json_parse");
            return NULL;
        }

        if (s[i] == '\0')
        {
            __print("Found EOF in json_parse");
            return NULL;
        }

        if (s[i] == '{')
        {
            JSON *obj = malloc(sizeof(JSON));

            if (obj == NULL)
            {
                __print("Failed to alloc object in json_parse");
                return NULL;
            }

            obj->type = VAL_OBJECT;

            errno_t err = __parse_object(obj, s, &i);

            if (err != 0)
            {
                __print("Failed to parse outer object.");
                goto clean_obj;
            }

            return obj;

        clean_obj:
            free(obj);
            obj = NULL;
            return NULL;
        }

        if (s[i] == '[')
        {
            JSON *arr = malloc(sizeof(JSON));

            if (arr == NULL)
            {
                __print("Failed to alloc array in json_parse");
                return NULL;
            }

            arr->type = VAL_ARRAY;
            errno_t err = __parse_array(arr, s, &i);

            if (err != 0)
            {
                __print("Failed to parse outer array.");
                goto clean_arr;
            }

            return arr;

        clean_arr:
            free(arr);
            arr = NULL;
            return NULL;
        }

        if (__is_whitespace(s[i]))
        {
            ++i;
            continue;
        }
        else
        {
            __printf("JSONparser: Expected characters ['{','[',' ','\\n','\\r','\\t'] but got unexpected '%c' instead at position %lld\n", s[i], i);
            return NULL;
        }
    }
    __print("Somehow you escaped an infinite loop, how did we get here??");
    return NULL;
}

/**
 * @brief Get a value in object using field name
 *
 * @param self JSON struct of type VAL_OBJECT
 * @param field name of field
 * @return NULL | JSON* (do not free, if you wish to free the memory, free the highest parent instead using json_free)
 */
JSON *json_object_get(JSON *self, const char *field)
{
    if (JSON_PARSER_DEBUG)
        __print("json_object_get");

    if (self->type != VAL_OBJECT)
    {
        __print("Cannot use json_object_get on object that is not of type VAL_OBJECT.");
        return NULL;
    }

    JSON_OBJECT *obj = self->value;

    for (uint64_t i = 0; i < obj->length; ++i)
    {
        if (__str_comp(field, obj->fields[i]))
        {
            return obj->values[i];
        }
    }
    __printf("JSONparser: json_object_get failed to find field \"%s\" in object.\n", field);
    return NULL;
}

/**
 * @brief Get the item at index in array
 *
 * @param self JSON struct of type VAL_ARRAY
 * @param index
 * @return NULL | JSON* (do not free, if you wish to free the memory, free the highest parent instead using json_free)
 */
JSON *json_array_get(JSON *self, uint64_t index)
{
    if (JSON_PARSER_DEBUG)
        __print("json_array_get");

    if (self->type != VAL_ARRAY)
    {
        __print("Cannot use json_array_get on object that is not of type VAL_ARRAY.");
        return NULL;
    }

    JSON_ARRAY *arr = self->value;

    if (index >= arr->length)
    {
        __printf("JSONparser: in json_get_array, tried to access index %llu, which is out of bounds of array.", index);
        return NULL;
    }

    return arr->elements[index];
}

JSON *json_get_deep(JSON *self, uint64_t fields_amount, const char *fields[fields_amount])
{
    if (JSON_PARSER_DEBUG)
        __print("json_get_deep");

    if (fields_amount == 0)
    {
        __print("json_get_deep called with 0 fields to access.");
        return NULL;
    }

    const char *access = fields[0];

    if (access == NULL)
    {
        __print("argument fields[0] of json_get_deep is NULL.");
        return NULL;
    }

    if (self->type == VAL_OBJECT)
    {
        JSON *val = json_object_get(self, access);

        if (val == NULL)
        {
            __print("Failed to get value from object access.");
            return NULL;
        }

        if (fields_amount == 1)
        {
            return val;
        }
        else
        {
            return json_get_deep(val, fields_amount - 1, &fields[1]);
        }
    }
    else if (self->type == VAL_ARRAY)
    {
        uint64_t index = strtoull(access, NULL, 10);
        JSON *val = json_array_get(self, index);

        if (val == NULL)
        {
            __print("Failed to get value from array access.");
            return NULL;
        }

        if (fields_amount == 1)
        {
            return val;
        }
        else
        {
            return json_get_deep(val, fields_amount - 1, &fields[1]);
        }
    }
    else
    {
        __print("Type of JSON value cannot be accessed.");
        return NULL;
    }
}

/**
 * @brief Returns a string representation of a JSON value type.
 *
 * @param type Obtained from json->type
 * @return NULL | const char* (read-only memory, do not free)
 */
const char *json_type_to_str(VALUE_TYPE type)
{
    if (type > VAL_NULL)
    {
        return NULL;
    }
    return __VAL_TO_STR[type];
}

void __builder_append(__BUILDER *builder, char *segment)
{
    if (JSON_PARSER_DEBUG)
        __print("__builder_append");

    __STRING_SEGMENT seg = {
        .str = segment,
        .len = __str_len(segment),
    };

    if (builder->len == 0)
    {
        builder->segments = malloc(sizeof(__STRING_SEGMENT));
        builder->len = 1;
        builder->segments[0] = seg;
    }
    else
    {
        builder->segments = realloc(builder->segments, sizeof(__STRING_SEGMENT) * (builder->len + 1));
        builder->len += 1;
        builder->segments[builder->len - 1] = seg;
    }
}

char *__builder_get_str(__BUILDER *builder)
{
    if (JSON_PARSER_DEBUG)
        __print("__builder_get_str");

    if (builder == NULL || builder->len == 0)
    {
        char *result = malloc(sizeof(char));

        if (result == NULL)
        {
            __print("Failed to allocate memory for empty string in __builder_get_str");
            return NULL;
        }

        (*result) = '\0';
        return result;
    }

    uint64_t total_len = 0;

    for (uint64_t i = 0; i < builder->len; ++i)
    {
        total_len += builder->segments[i].len;
    }

    char *result = malloc(sizeof(char) * (total_len + 1));

    if (result == NULL)
    {
        __print("Failed to allocate memory for string in __builder_get_str");
        return NULL;
    }

    uint64_t offset = 0;
    for (uint64_t i = 0; i < builder->len; ++i)
    {
        memcpy(result + offset, builder->segments[i].str, builder->segments[i].len);
        offset += builder->segments[i].len;
    }

    result[total_len] = '\0';
    return result;
}

char *__escape_string(const char *value)
{
    if (JSON_PARSER_DEBUG)
        __print("__escape_string");

    uint64_t len = __str_len(value);
    uint64_t capacity = len + 1;
    char *escaped = malloc(sizeof(char) * capacity);

    if (!escaped)
    {
        __print("Memory allocation failed in __escape_string");
        return NULL;
    }

    uint64_t index = 0;
    for (uint64_t i = 0; i < len; ++i)
    {
        char c = value[i];

        if (c != '\0' && __str_contains_c(__LIST_SEQ, c))
        {
            const char *esc_seq = __SEQ_TO_ESC[(unsigned char)c];
            uint64_t esc_len = 2;

            if (index + esc_len >= capacity)
            {
                capacity *= 2;
                char *temp = realloc(escaped, sizeof(char) * capacity);
                if (!temp)
                {
                    __print("Memory reallocation failed in __escape_string");
                    free(escaped);
                    escaped = NULL;
                    return NULL;
                }
                escaped = temp;
            }

            escaped[index] = esc_seq[0];
            escaped[index + 1] = esc_seq[1];
            index += esc_len;
        }
        else
        {
            if (index + 1 >= capacity)
            {
                capacity *= 2;
                char *temp = realloc(escaped, sizeof(char) * capacity);
                if (!temp)
                {
                    __print("Memory reallocation failed in __escape_string");
                    free(escaped);
                    escaped = NULL;
                    return NULL;
                }
                escaped = temp;
            }
            escaped[index++] = c;
        }
    }

    escaped[index] = '\0';
    return escaped;
}

/**
 * @brief Stringifies a JSON struct as well as all its descendants.
 *
 * @param json JSON struct (obtained from json_parse)
 * @return NULL | char* (memory owned, you need to free it)
 */
char *json_stringify(JSON *json)
{
    if (JSON_PARSER_DEBUG)
        __print("json_stringify");

    __BUILDER builder = {
        .len = 0,
        .segments = NULL,
    };

    switch (json->type)
    {
    case VAL_OBJECT:
    {
        __builder_append(&builder, "{");

        JSON_OBJECT *obj = json->value;

        const uint64_t cleaner_size = obj->length * 2;
        char *to_clean[cleaner_size];

        for (uint64_t i = 0; i < obj->length; ++i)
        {
            // create dud STRING value object to parse as json string
            JSON *field_obj = malloc(sizeof(JSON));
            field_obj->type = VAL_STRING;
            field_obj->value = obj->fields[i];

            char *string_field = json_stringify(field_obj);
            if (string_field == NULL)
            {
                __print("Failed to stringify field in object entry.");
                free(builder.segments);
                builder.segments = NULL;
                return NULL;
            }

            free(field_obj);
            field_obj = NULL;

            __builder_append(&builder, string_field);
            __builder_append(&builder, ":");

            char *string_val = json_stringify(obj->values[i]);
            if (string_val == NULL)
            {
                __print("Failed to stringify value in object entry.");
                free(builder.segments);
                builder.segments = NULL;
                return NULL;
            }

            __builder_append(&builder, string_val);

            to_clean[i] = string_field;
            to_clean[(uint64_t)(cleaner_size / 2) + i] = string_val;

            if (i != obj->length - 1)
            {
                __builder_append(&builder, ",");
            }
        }
        __builder_append(&builder, "}");

        char *result = __builder_get_str(&builder);
        free(builder.segments);
        builder.segments = NULL;

        for (uint64_t i = 0; i < cleaner_size; ++i)
        {
            free(to_clean[i]);
            to_clean[i] = NULL;
        }
        return result;
    }
    case VAL_ARRAY:
    {
        __builder_append(&builder, "[");

        JSON_ARRAY *arr = json->value;

        char *to_clean[arr->length];

        for (uint64_t i = 0; i < arr->length; ++i)
        {
            char *string_val = json_stringify(arr->elements[i]);
            if (string_val == NULL)
            {
                __print("Failed to stringify value in array.");
                free(builder.segments);
                builder.segments = NULL;
                return NULL;
            }

            to_clean[i] = string_val;

            __builder_append(&builder, string_val);

            if (i != arr->length - 1)
            {
                __builder_append(&builder, ",");
            }
        }
        __builder_append(&builder, "]");

        char *result = __builder_get_str(&builder);
        free(builder.segments);
        builder.segments = NULL;

        for (uint64_t i = 0; i < arr->length; ++i)
        {
            free(to_clean[i]);
            to_clean[i] = NULL;
        }
        return result;
    }
    case VAL_BOOL:
    {
        int *boolean = json->value;
        __builder_append(&builder, (*boolean) ? "true" : "false");
        char *result = __builder_get_str(&builder);
        free(builder.segments);
        builder.segments = NULL;
        return result;
    }
    case VAL_NUMBER:
    {
        JSON_NUMBER *number = json->value;
        JSON_NUMBER num_val = *number;

        double diff = fabs(round(num_val) - num_val);

        char buff[512];

        if (diff < 0.00001)
        {
            errno_t err = _i64toa_s((int64_t)num_val, buff, 512, 10);
            if (err != 0)
            {
                __print("Failed to stringify number.");
                return NULL;
            }

            __builder_append(&builder, buff);
        }
        else
        {
            _snprintf_s(buff, 512, 512, "%.12g", (double)num_val);

            char *decimal_point = strchr(buff, '.');
            if (decimal_point != NULL)
            {
                char *end = buff + strlen(buff) - 1;
                while (end > decimal_point && *end == '0')
                {
                    *end = '\0';
                    end--;
                }
                if (*end == '.')
                {
                    *end = '\0';
                }
            }

            __builder_append(&builder, buff);
        }

        char *result = __builder_get_str(&builder);
        free(builder.segments);
        builder.segments = NULL;
        return result;
    }
    case VAL_STRING:
    {
        __builder_append(&builder, "\"");

        char *escaped_string = __escape_string(json->value);
        if (escaped_string == NULL)
        {
            __print("Failed to escape string.");
            return NULL;
        }
        __builder_append(&builder, escaped_string);
        __builder_append(&builder, "\"");
        char *result = __builder_get_str(&builder);
        free(builder.segments);
        builder.segments = NULL;
        free(escaped_string);
        escaped_string = NULL;
        return result;
    }
    case VAL_NULL:
    {
        __builder_append(&builder, "null");
        char *result = __builder_get_str(&builder);
        free(builder.segments);
        builder.segments = NULL;
        return result;
    }
    default:
        return NULL;
    }
    return NULL;
}

/**
 * @brief Prints the JSON struct + a newline characters. If you do not want the
 * newline character, use json_stringify
 *
 * @param json JSON struct (obtained from json_parse)
 */
void json_print(JSON *json)
{
    char *s = json_stringify(json);

    if (s == NULL)
    {
        printf("<error>\n");
        return;
    }

    printf("%s\n", s);
    free(s);
}

/**
 * @brief Frees the JSON struct as well as all its descendants.
 * @param json JSON struct (obtained from json_parse)
 */
void json_free(JSON *json)
{
    if (JSON_PARSER_DEBUG)
        __print("json_free");

    switch (json->type)
    {
    case VAL_OBJECT:
    {
        JSON_OBJECT *obj = json->value;

        for (uint64_t i = 0; i < obj->length; ++i)
        {
            free(obj->fields[i]);
            obj->fields[i] = NULL;
            json_free(obj->values[i]);
            obj->values[i] = NULL;
        }
        free(obj->fields);
        obj->fields = NULL;
        free(obj->values);
        obj->values = NULL;
        free(obj);
        obj = NULL;
        break;
    }
    case VAL_ARRAY:
    {
        JSON_ARRAY *arr = json->value;

        for (uint64_t i = 0; i < arr->length; ++i)
        {
            json_free(arr->elements[i]);
            arr->elements[i] = NULL;
        }
        free(arr->elements);
        arr->elements = NULL;
        free(arr);
        arr = NULL;
        break;
    }
    case VAL_BOOL:
    case VAL_NUMBER:
    case VAL_STRING:
    {
        free(json->value);
        json->value = NULL;
        break;
    }
    case VAL_NULL:
    {
        break;
    }
    default:
        break;
    }

    free(json);
    return;
}

/**
 * @brief Get the char* value out of a JSON struct of type VAL_STRING
 *
 * @param json JSON struct (obtained from json_parse)
 * @return NULL | char* (memory not owned, do not free)
 */
char *json_value_string(JSON *json)
{
    if (json->type != VAL_STRING)
    {
        __print("Tried to get string value out of JSON struct not of type VAL_STRING");
        return NULL;
    }

    return (char *)json->value;
}

/**
 * @brief Get a pointer to bool(int) value out of a JSON struct of type VAL_BOOL
 *
 * @param json JSON struct (obtained from json_parse)
 * @return NULL | int* (memory not owned, do not free)
 */
int *json_value_bool(JSON *json)
{
    if (json->type != VAL_BOOL)
    {
        __print("Tried to get bool value out of JSON struct not of type VAL_BOOL");
        return NULL;
    }

    return (int *)json->value;
}

/**
 * @brief Get a pointer to double value out of a JSON struct of type VAL_NUMBER
 *
 * @param json JSON struct (obtained from json_parse)
 * @return NULL | double* (memory not owned, do not free)
 */
double *json_value_number(JSON *json)
{
    if (json->type != VAL_NUMBER)
    {
        __print("Tried to get number value out of JSON struct not of type VAL_NUMBER");
        return NULL;
    }

    return (double *)json->value;
}

/**
 * @brief No real need for this one, just use NULL instead lol
 * @param json JSON struct (obtained from json_parse)
 * @return NULL
 */
void *json_value_null(JSON *json)
{
    return NULL;
}

/**
 * @brief Check if JSON struct is of type VAL_NULL
 *
 * @param json JSON struct (obtained from json_parse)
 * @return bool(int)
 */
int json_is_null(JSON *json)
{
    return json->type == VAL_NULL;
}

/**
 * @brief Get a pointer to the JSON_OBJECT value out of a JSON struct of type VAL_OBJECT
 *
 * @param json JSON struct (obtained from json_parse)
 * @return NULL | JSON_OBJECT* (memory not owned, do not free)
 */
JSON_OBJECT *json_value_object(JSON *json)
{
    if (json->type != VAL_OBJECT)
    {
        __print("Tried to get object value out of JSON struct not of type VAL_OBJECT");
        return NULL;
    }

    return (JSON_OBJECT *)json->value;
}

/**
 * @brief Get a pointer to the JSON_ARRAY value out of a JSON struct of type VAL_ARRAY
 *
 * @param json JSON struct (obtained from json_parse)
 * @return NULL | JSON_ARRAY* (memory not owned, do not free)
 */
JSON_ARRAY *json_value_array(JSON *json)
{
    if (json->type != VAL_ARRAY)
    {
        __print("Tried to get array value out of JSON struct not of type VAL_ARRAY");
        return NULL;
    }

    return (JSON_ARRAY *)json->value;
}