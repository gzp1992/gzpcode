#include "ftp.h"
//第一次链接服务器
void connect_url(const char *url){
    socketfd = ftp_connect(url,21);
    if(socketfd<0){
        printf("链接失败,请检查域名是否正确\n");
        socketfd=0;
        exit(-1);
    }
    else{
        //判断链接结果
        if(connect_recv(socketfd)<0){
            printf("ftp> 链接服务器失败,请于管理员联系\n");
            exit(-1);
        }
        else{
            printf("ftp> 链接成功\n");
            //客户端处于状态的标志位
            flag=1;
        }
    }
}
