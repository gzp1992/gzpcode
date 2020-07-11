#include <string.h>
#include "t_cmd.h"
#include "t_func.h"
#include "resume.h"
t_cmd cmd_list[]=
{
    {"url", t_url},
    {"quit", t_quit},
    {"login", t_login},
    {"syst", t_syst},
    {"ls", t_ls},
    {"mkdir", t_mkdir},
    {"cd", t_cd},
    {"rm", t_rm},
    {"rmdir", t_rmdir},
    {"pwd", t_pwd},
    {"help", t_help},
    {"get", t_get},
    {"put", t_put},
    {"clear", t_clear},
    {"\n", t_n},
    {"\r", t_r},
    {"\0", t_0},
    {"reget", reget},
    {"reput", reput}
};

t_cmd *find_cmd(const char *user_cmd)
{
    int i = 0;

    for(; i<sizeof(cmd_list)/sizeof(cmd_list[0]);i++)
    {
        if(strcmp(user_cmd, cmd_list[i].name)==0)
        {
            return &(cmd_list[i]);
        }
    }
    return (t_cmd *)0;
}
