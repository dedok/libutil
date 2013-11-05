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

#include "libutil/config.h"

#include "libutil/log.h"
#include "libutil/util.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <string.h>


/*
    Private API
 */

static lua_State * lua = NULL;
static void * mem_ = NULL;


static
int
atpanic(lua_State *lua)
{
    DIE("config error : %s", lua_tostring(lua, -1));
    return 0;
}


static inline
void
init(void)
{
    if (!lua) {
        lua = MEM(luaL_newstate());
        lua_atpanic(lua, atpanic);
        lua_pushcfunction(lua, luaopen_base);
        lua_call(lua, 0, 0);
    }
}


static inline
bool
find_value(char const * key, char * error, size_t size)
{
    static const size_t return_len = sizeof("return ") - 1;
    size_t key_len = strlen(key);

    char buf[return_len + key_len];
    memcpy(buf, "return ", return_len);
    memcpy(buf + return_len, key, key_len + 1);

    if (luaL_dostring(lua, buf) != 0)
        lua_error(lua);

    if (lua_isnil(lua, -1)) {
        snprintf(error, size,
                 "config error : key '%s' not found", key);
        return false;
    }

    return true;
}


static inline
void
find_value_or_die(char const * key)
{
    char error[255];
    CHECK(find_value(key, &error[0], sizeof(error)/sizeof(error[0])), == false,
          DIE, "%s", error);
}


static inline
bool
get_bool(char const * key, bool * val, char * error, size_t size)
{
    if (unlikely(!find_value(key, error, size)))
        return false;

    if (unlikely(!lua_isboolean(lua, -1))) {
        snprintf(error, size, "config error : key '%s' is not a boolean", key);
        return false;
    }

    *val = lua_toboolean(lua, -1);
    lua_pop(lua, 1);

    return true;
}


static inline
bool
get_bool_or_die(char const * key)
{
    bool res;
    char error[255];
    CHECK(get_bool(key, &res, &error[0], sizeof(error)/sizeof(error[0])),
            == false,
         DIE, "%s", error);
    return res;
}


static inline
bool
get_long(char const * key, long * val, char * error, size_t size)
{
    if (unlikely(!find_value(key, error, size)))
        return false;

    if (unlikely(!lua_isnumber(lua, -1))) {
        snprintf(error, size, "config error : key '%s' is not a boolean", key);
        return false;
    }

    double value = lua_tonumber(lua, -1);
    *val = value;
    if (*val != value) {
        snprintf(error, size, "config error : key '%s': '%s' is not an integer",
              key, lua_tostring(lua, -1));
        return false;
    }

    lua_pop(lua, 1);

    return true;
}


static inline
long
get_long_or_die(char const * key)
{
    long res;
    char error[255];
    CHECK(get_long(key, &res, &error[0], sizeof(error)/sizeof(error[0])),
            == false,
         DIE, "%s", error);
    return res;
}


static inline
double
get_double(char const * key, double * val, char * error, size_t size)
{
    if (unlikely(!find_value(key, error, size)))
        return false;

    if (unlikely(!lua_isnumber(lua, -1))) {
        snprintf(error, size, "config error : key '%s' is not a number", key);
        return false;
    }

    *val = lua_tonumber(lua, -1);
    lua_pop(lua, 1);

    return true;
}


static inline
double
get_double_or_die(char const * key)
{
    double res;
    char error[255];
    CHECK(get_double(key, &res, &error[0], sizeof(error)/sizeof(error[0])),
            == false,
         DIE, "%s", error);
    return res;
}


static inline
char *
get_str(char const * key, char * error, size_t size)
{
    char * res;

    if (unlikely(!find_value(key, error, size)))
        return NULL;

    if (unlikely(!lua_isstring(lua, -1))) {
        snprintf(error, size, "config error : key '%s' is not a string", key);
        return NULL;
    }

    res = MEM(strdup(lua_tostring(lua, -1)));
    lua_pop(lua, 1);

    return res;
}


static inline
char *
get_str_or_die(char const * key)
{
    char error[255];
    return CHECK(get_str(key, &error[0], sizeof(error)/sizeof(error[0])),
                    == NULL, DIE, "%s", error);
}


/*
    Public API
 */

void
config_file(char const * file)
{
    init();
    if (luaL_dofile(lua, file) != 0)
        lua_error(lua);
}


void
config_eval(char const * expr)
{
    init();
    if (luaL_dostring(lua, expr) != 0)
        lua_error(lua);
}


bool
config_get(enum config_val_type t, struct config_val * val,
        char const * key, char * error, size_t size)
{
    bool ok = true;
    *val = (struct config_val) { .v = { 0 } };
    switch (t) {
        case CONF_VAR_LONG : ok = get_long(key, &val->v.l, error, size); break;
        case CONF_VAR_STRING :
            val->v.str = get_str(key,error, size);
            if (unlikely(val->v.str == NULL)) ok = false;
            break;
        case CONF_VAR_BOOL : ok = get_bool(key, &val->v.b, error, size); break;
        case CONF_VAR_DOUBLE :
            ok = get_double(key, &val->v.d, error, size); break;
        default :
            DIE("config error : unknown/unsupported type '%i'", t);
    } // switch

    return ok;
}


struct config_val
config_get_or_die(enum config_val_type t, char const * key)
{
    char error[255];
    struct config_val v = { .v = { 0 } };
    memset(error, 0, sizeof(error)/sizeof(error[0]));
    if (!config_get(t, &v, key, &error[0], sizeof(error)/sizeof(error[0])))
          DIE("%s", error);
    return v;
}


bool
config_open_section(const char * key, char * error, size_t size)
{
    lua_pushthread(lua);

    if (unlikely(!find_value(key, error, size)))
        return false;

    if (unlikely(!lua_istable(lua, -1))) {
        snprintf(error, size, "config error : key '%s' is not a table", key);
        return false;
    }

    lua_createtable(lua, 0, 1);
    lua_getfenv(lua, -3);
    lua_setfield(lua, -2, "__index");
    lua_setmetatable(lua, -2);
    lua_setfenv(lua, -2);
    lua_pop(lua, 1);

    return true;
}


void
config_open_section_or_die(const char * key)
{
    char error[255];
    CHECK(config_open_section(key, &error[0], sizeof(error)/sizeof(error[0])),
            == false,
          DIE, "%s", error);
}


bool
config_close_section(char * error, size_t size)
{
    lua_pushthread(lua);
    lua_getfenv(lua, -1);
    if (!lua_istable(lua, -1) || ! lua_getmetatable(lua, -1)) {
        snprintf(error, size, "config error : no open section to close");
        return false;
    }

    lua_pushnil(lua);
    lua_setmetatable(lua, -3);
    lua_getfield(lua, -1, "__index");
    lua_setfenv(lua, -4);
    lua_pop(lua, 3);

    return true;
}


void
config_close_section_or_die(void)
{
    char error[255];
    CHECK(config_close_section(&error[0], sizeof(error)/sizeof(error[0])),
            == false,
          DIE, "%s", error);
}


void
config_destroy(void)
{
    if (lua) {
        lua_close(lua);
        lua = NULL;
        free(mem_);
    }
}

