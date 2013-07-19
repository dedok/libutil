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


static void *
allocator(void * UNUSED_VAR(u), void * m, size_t UNUSED_VAR(s), size_t ns)
{
    return realloc(m, ns);
}


static int
atpanic(lua_State *lua)
{
    DIE("config error : %s", lua_tostring(lua, -1));
    return 0;
}


static inline void
init(void)
{
    if (!lua) {
        lua = MEM(lua_newstate(allocator, NULL));
        lua_atpanic(lua, atpanic);
        lua_pushcfunction(lua, luaopen_base);
        lua_call(lua, 0, 0);
    }
}


static inline void
find_value(char const * key)
{
    static const size_t return_len = sizeof("return ") - 1;
    size_t key_len = strlen(key);

    char buf[return_len + key_len];
    memcpy(buf, "return ", return_len);
    memcpy(buf + return_len, key, key_len + 1);

    if (luaL_dostring(lua, buf) != 0)
        lua_error(lua);
    
    if (lua_isnil(lua, -1))
        DIE("config error : key '%s' not found", key);
}


static inline bool
get_bool(char const * key)
{
    find_value(key);
    if (!lua_isboolean(lua, -1))
        DIE("config error : key '%s' is not a boolean", key);

    bool const res = lua_toboolean(lua, -1);
    lua_pop(lua, 1);

    return res;
}


static inline long
get_long(char const * key)
{
    find_value(key);
    if (!lua_isnumber(lua, -1))
        ERROR("config error : key '%s' is not a number", key);

    double value = lua_tonumber(lua, -1);
    long res = value;
    if (res != value)
        ERROR("config error : key '%s': '%s' is not an integer", key, lua_tostring(lua, -1));
    lua_pop(lua, 1);

    return res;
}


static inline double
get_double(char const * key)
{
    find_value(key);
    if (!lua_isnumber(lua, -1))
        ERROR("config error : key '%s' is not a number", key);

    double res = lua_tonumber(lua, -1);
    lua_pop(lua, 1);

    return res;
}


static inline char *
get_str(char const * key)
{
    find_value(key);
    if (!lua_isstring(lua, -1))
        ERROR("config error : key '%s' is not a string", key);

    char * res = MEM(strdup(lua_tostring(lua, -1)));

    lua_pop(lua, 1);

    return res;
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


struct config_val 
config_get(enum config_val_type t, char const * key) 
{
    struct config_val v = { .l = 0, .str = NULL, .b = false, .d = 0.0 };
    switch (t) {  
        case CONF_VAR_LONG :
            return v = (struct config_val) { .l = get_long(key) };
        case CONF_VAR_STRING :
            return v = (struct config_val) { .str = get_str(key) };
        case CONF_VAR_BOOL :
            return v = (struct config_val) { .b = get_bool(key) };
        case CONF_VAR_DOUBLE :
            return v = (struct config_val) { .d = get_double(key) };
        default :
            DIE("config error : unknown/unsupported type '%i'", t);
    } // switch

    // This will never happen
    return v;
}


void
config_open_section(const char *key)
{
    lua_pushthread(lua);
    find_value(key);
    if (!lua_istable(lua, -1))
        DIE("config error : key '%s' is not a table", key);

    lua_createtable(lua, 0, 1);
    lua_getfenv(lua, -3);
    lua_setfield(lua, -2, "__index");
    lua_setmetatable(lua, -2);
    lua_setfenv(lua, -2);
    lua_pop(lua, 1);
}


void
config_close_section(void)
{
    lua_pushthread(lua);
    lua_getfenv(lua, -1);
    if (!lua_istable(lua, -1) || ! lua_getmetatable(lua, -1))
        DIE("config error : no open section to close");

    lua_pushnil(lua);
    lua_setmetatable(lua, -3);
    lua_getfield(lua, -1, "__index");
    lua_setfenv(lua, -4);
    lua_pop(lua, 3);
}


void
config_destroy(void)
{
    if (lua) {
        lua_close(lua);
        lua = NULL;
    }
}

