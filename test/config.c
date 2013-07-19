#include "libutil/headers.h"

#include <stdio.h>
#include <string.h>


#define CHECK_(a, b, ...) CHECK(a, b, DIE, __VA_ARGS__)


static void
conf_common_test(int argc, char ** argv) 
{
    static char * str = "simple = { b = true, str = \"str\", l = 100, d = 0.01 }";
    static char * str_1 = "simple = { b = false, str = \"str\", l = -1, d = 0.01 }";
    
    // test : file | eval
    {
    
    if (argc > 1)
        config_file(argv[1]);
    else 
        config_eval(str);
   
    config_open_section("simple");
    CHECK_(config_get(CONF_VAR_BOOL, "b").b, == false);
    
    char * ret_str = CHECK_(config_get(CONF_VAR_STRING, "str").str, == NULL);
    CHECK_(strcmp(ret_str, "str"), != 0);
    free(ret_str);

    CHECK_(config_get(CONF_VAR_LONG, "l").l, != 100);
    CHECK_(config_get(CONF_VAR_DOUBLE, "d").d, != 0.01);
    config_close_section();

    config_destroy();
    
    }
    {
    
    // test : eval -> eval
    config_eval(str);
    config_eval(str_1);
    config_open_section("simple");
    CHECK_(config_get(CONF_VAR_BOOL, "b").b, == true);
    
    char * ret_str = CHECK_(config_get(CONF_VAR_STRING, "str").str, == NULL);
    CHECK_(strcmp(ret_str, "str"), != 0);
    free(ret_str);

    CHECK_(config_get(CONF_VAR_LONG, "l").l, != -1);
    CHECK_(config_get(CONF_VAR_DOUBLE, "d").d, != 0.01);
    config_close_section();

    config_destroy();
    
    }

    // test : open sections
    {
    // TODO impl this 
    }

    // test : array of params
    {
    // TODO tmpl this
    }
}


int
main(int argc, char ** argv)
{
    conf_common_test(argc, argv);
    return EXIT_SUCCESS;
}

