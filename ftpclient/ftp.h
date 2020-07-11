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
extern int socketfd;//socket������
//flag�ж��Ƿ��Ѿ����ӳɹ�,0��δ���ӣ�1�������ӣ�δ��¼��2���������ѵ�¼
extern int flag ;
extern char path[50] ;
void init(void);
//���ӷ�����
int ftp_connect(const char* ,int );
//����
int connect_recv(int);
//�˳�
void ftp_quit(int);
//��½
int ftp_login(int);
//�ϵ�����
int ftp_rest(int);
//��ȡ�������汾
int ftp_syst(int);
//����ģʽ pasv�ӿ�
int ftp_pasv(int);
//��ȡ�б�
int ftp_list(int,const char *);
//����Ŀ¼
int ftp_mkdir(int,const char *);
//�л�Ŀ¼
int ftp_cd(int,const char *);
//ɾ���ļ�
int ftp_rm(int,const char *);
//ɾ��Ŀ¼
int ftp_rmdir(int,const char *);
//�鿴��ǰ����·��
int ftp_pwd(int);
//�����ֲ�
int ftp_help(void);
//��ȡ����ģʽ.iΪ�����ƴ���
int ftp_type(int);
//����
int ftp_get(int,const char *);
//�ϴ�����
int ftp_up(int,const char *);

#endif



