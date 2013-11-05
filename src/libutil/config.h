/*
    The MIT License (MIT)

    Copyright (c) 2013 Vasiliy Soshnikov

    Permission is hereby granted, free of charge, to any person obtaining a copy of
    this software and associated documentation files (the "Software"), to deal in
    the Software without restriction, including without limitation the rights to
    use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
    the Software, and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions:
    
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
    FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
    COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
    IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
    CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef LIBUTIL_CONFIG_H
#define LIBUTIL_CONFIG_H 1

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


enum config_val_type {
    CONF_VAR_LONG = 0,
    CONF_VAR_STRING,
    CONF_VAR_BOOL,
    CONF_VAR_DOUBLE
};


struct config_val {
    union {
        double d;
        long l;
        char * str; // Free via caller
        bool b;
    } v;
};

void
config_file(char const * filename);


void
config_eval(char const * expr);


bool
config_get(enum config_val_type t, struct config_val * val,
    char const * key, char * error, size_t size);


struct config_val
config_get_or_die(enum config_val_type t, char const * key);


bool
config_open_section(const char * key, char * error, size_t size);


void
config_open_section_or_die(char const * key);


bool
config_close_section(char * error, size_t size);


void
config_close_section_or_die(void);


void
config_destroy(void);


#define CONF_GET_STRING_OR_DIE(key) \
    config_get_or_die(CONF_VAR_STRING, key).v.str


#define CONF_GET_BOOL_OR_DIE(key) \
    config_get_or_die(CONF_VAR_BOOL, key).v.b


#define CONF_GET_DOUBLE_OR_DIE(key) \
    config_get_or_die(CONF_VAR_DOUBLE, key).v.d


#define CONF_GET_LONG_OR_DIE(key) \
    config_get_or_die(CONF_VAR_LONG, key).v.l


#define config_open_sections_or_die(section, ...)            \
({                                                           \
    const char *__sections[] = { section, __VA_ARGS__ };     \
    for (unsigned int __i = 0;                               \
        __i < sizeof(__sections) / sizeof(*__sections[0]);   \
        ++__i)                                               \
    config_open_section_or_die(__sections[__i]);             \
})


#define config_close_sections_or_die(count)                 \
({                                                          \
    for (int __i = 0; __i < (count); ++__i)                 \
        config_close_section_or_die();                      \
})


#ifdef __cplusplus
}      /* extern "C" */
#endif  


#endif  

