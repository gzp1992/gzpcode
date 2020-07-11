#ifndef __FTP_H__
#define __FTP_H__
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
extern int socketfd;//socket描述符
//flag判断是否已经链接成功,0：未连接，1：已连接，未登录，2：已连接已登录
extern int flag ;
extern char path[50] ;
void init(void);
//链接服务器
int ftp_connect(const char* ,int );
//接受
int connect_recv(int);
//退出
void ftp_quit(int);
//登陆
int ftp_login(int);
//断点续传
int ftp_rest(int);
//获取服务器版本
int ftp_syst(int);
//被动模式 pasv接口
int ftp_pasv(int);
//获取列表
int ftp_list(int,const char *);
//创建目录
int ftp_mkdir(int,const char *);
//切换目录
int ftp_cd(int,const char *);
//删除文件
int ftp_rm(int,const char *);
//删除目录
int ftp_rmdir(int,const char *);
//查看当前所在路径
int ftp_pwd(int);
//帮助手册
int ftp_help(void);
//获取传输模式.i为二进制传输
int ftp_type(int);
//下载
int ftp_get(int,const char *);
//上传数据
int ftp_up(int,const char *);

#endif



