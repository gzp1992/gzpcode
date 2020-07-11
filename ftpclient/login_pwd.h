#ifndef __LOGIN_PWD_H__
#define __LOGIN_PWD_H__
#define ECHOFLAGS (ECHO | ECHOE | ECHOK | ECHONL)
//函数set_disp_mode用于控制是否开启输入回显功能
//如果option为0，则关闭回显，为1则打开回显
int set_disp_mode(int,int);
//函数getpasswd用于获得用户输入的密码，并将其存储在指定的字符数组中
int getpasswd(char *, int);
#endif

