#include "ftp.h"
#include "t_cmd.h"
#include "t_func.h"
#include "connect_url.h"
#define LEN (50)
//socket设备
int socketfd = 0;
//flag判断是否已经链接成功,0：未连接，1：已连接，未登录，2：已连接已登录
int flag = 0;
//保存操作的文件名
char path[LEN]={0};
int main(int argc,char *argv[]){
    t_cmd *ptr = NULL;
    //存放键盘输入的内容
    char buf[LEN] = {0};
    //存放输入的命令
    char command[LEN]={0};
    if(argc!=2){
        printf("输入格式错误，请重新运行程序:t_ftp  <ip>\n");
        return -1;
    }
    connect_url(argv[1]);//第一次链接服务器
    ftp_help();//输出帮助列表
    t_login();//登陆 
    while(1){
        setbuf(stdin,NULL);//清空输入缓冲区
        printf("ftp>");
        memset(command,0,LEN);//清空数组内容
        memset(path,0,LEN);
        fgets(buf,LEN,stdin);//将输入的命令存到数组里
        if(strlen(buf)==LEN-1 && buf[LEN-2]!='\n'){
            scanf("%*[^\n]");
            scanf("%*c");
        }
        buf[strlen(buf)-1] = '\0';
        sscanf(buf,"%s %s",command,path);//从数组提取命令
        ptr = find_cmd(command);//匹配支持的命令
        if(ptr){//成功
            ptr->pfunc();
        }
        else{
            printf("此功能未上线，try------help\n");
        }
    }
    return 0;
}
