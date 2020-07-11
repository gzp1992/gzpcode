#ifndef __T_CMD_H__
#define __T_CMD_H__
extern int flag;
extern int socketfd;
extern char path[50];
typedef struct cmd
{
    char *name;
    void (*pfunc)(void);
}t_cmd;
t_cmd *find_cmd(const char *);

#endif
