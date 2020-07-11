#include "login_pwd.h"
#include "ftp.h"
//链接服务器
int ftp_connect(const char* host,int port){
    char buf[512]={};//接受服务器发送来的消息
    //创建socket设备
    int s = socket(AF_INET,SOCK_STREAM,0);
    if(s<0) return -3;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    //主机字节序换成网络字节序
    addr.sin_port = htons(port);
    //用域名或者主机名获取IP地址，返回值是struct hostent *，IP地址以网络字节序存储
    struct hostent* server = gethostbyname(host);
    if(!server) return -1;
    //（目标地址，源地址，长度 ）
    memcpy(&addr.sin_addr.s_addr,server->h_addr,server->h_length);
    //connect，与服务器申请建立链接
    if(connect(s,(struct sockaddr*)&addr,sizeof(addr))==-1) return -2;
    return s;
}
//接受服务器信息，判断链接服务器IP和端口是否成功
int connect_recv(int sockfd){
    char buf[512]={};
    int len = recv(sockfd,buf,512,0);
    buf[len] = 0;
    if(strncmp(buf,"220",3)==0) return 0; //220              服务就绪
    //失败，关闭socket
    close(sockfd);
    return -1;
}
//退出
void ftp_quit(int sockfd){
    printf("客户端准备退出...\n");
    sleep(1);
    send(sockfd,"QUIT\r\n",6,0);
    close(sockfd);
    exit(0);
}
//登陆
int ftp_login(int sockfd){
    char name[32]={};
    char pass[32]={};
    char sendline[1024]={};
    char recvline[1024]={};
    printf("ftp> name:");
    scanf("%s",name);
    scanf("%*[^\n]");
    scanf("%*c");
    //ftp协议格式字符串存到数组里
    sprintf(sendline,"USER %s\r\n",name);//固定格式
    int sendaddr = send(sockfd,sendline,strlen(sendline),0);
    if(sendaddr<0){return -3;}
    int recvaddr=recv(sockfd,recvline,sizeof(recvline),0);
    if(strncmp(recvline,"331",3)==0){		//331		 要求密码
        printf("ftp> pass:");
    }
    else{return -1;}
    //首先关闭输出回显，这样输入密码时就不会显示输入的字符信息
    set_disp_mode(STDIN_FILENO,0);
    //调用getpasswd函数获得用户输入的密码
    getpasswd(pass, sizeof(pass));  
    //恢复回显
    set_disp_mode(STDIN_FILENO,1);
    //将数组初始化为0
    memset(sendline,0,sizeof(sendline));
    memset(recvline,0,sizeof(recvline));
    sprintf(sendline,"PASS %s\r\n",pass);
    sendaddr = send(sockfd,sendline,strlen(sendline),0);
    if(sendaddr<0){return -2;}
    recvaddr=recv(sockfd,recvline,sizeof(recvline),0);
    if(strncmp(recvline,"230",3)==0){
        return 0;
    }
    else{
        return -2;
    }
    return -3;
}
//断点续传
int ftp_rest(int sockfd){
    char sendline[1024]={};
    char recvline[1024]={};
    sprintf(sendline,"REST 0\r\n");
    int sendaddr = send(sockfd,sendline,strlen(sendline),0);
    if(sendaddr==-1){
        return -2;
    }
    int recvaddr = recv(sockfd,recvline,sizeof(recvline),0);
    if(recvaddr==-1){
        return -2;	
    }
    if(strncmp(recvline,"350",3)==0){		//350		 文件行为暂停
        return 0;
    }
    else{
        return -1;
    }
}
//获取服务器版本
int ftp_syst(int sockfd){
    char sendline[1024]={};
    char recvline[1024]={};
    strcat(sendline,"SYST\r\n");
    int sendaddr=send(sockfd,sendline,strlen(sendline),0);
    if(sendaddr==-1){
        return -2;
    }
    int recvaddr = recv(sockfd,recvline,sizeof(recvline),0);
    if(recvaddr==-1){
        return -2;
    }
    if(strncmp(recvline,"215",3)==0){		//215              系统类型回复
        printf("%s",recvline);
        return 0;
    }
    else return -1;
}
//被动模式 pasv接口
int ftp_pasv(int sockfd){
    char sendline[1024]={};
    char recvline[1024]={};
    int addr[6];//服务器会将PASV接口的IP地址传回莱，要链接过去
    strcat(sendline,"PASV\r\n");
    int sendaddr = send(sockfd,sendline,strlen(sendline),0);
    if(sendaddr==-1){
        return -2;
    }
    int recvaddr = recv(sockfd,recvline,sizeof(recvline),0);
    if(recvaddr==-1){
        return -2;
    }
    //忽略（前的数据
    sscanf(recvline,"%*[^(](%d,%d,%d,%d,%d,%d)",&addr[0],&addr[1],&addr[2],&addr[3],&addr[4],&addr[5]);
    memset(sendline,0,sizeof(sendline));
    memset(recvline,0,sizeof(recvline));
    sprintf(sendline,"%d.%d.%d.%d",addr[0],addr[1],addr[2],addr[3]);
    //								计算端口公式
    return ftp_connect(sendline,addr[4]*256+addr[5]);
}
//获取列表
int ftp_list(int sockfd,const char *path){
    char sendline[512]={};
    char recvline[512]={};
    //被动模式
    int sock_pacv = ftp_pasv(sockfd);
    if(sock_pacv<0) return -1;
    sprintf(sendline,"LIST %s\r\n",path);
    int sendaddr = send(sockfd,sendline,strlen(sendline),0);
    if(sendaddr<0) return -1;
    int recvaddr = recv(sockfd,recvline,512,0);
    if(recvaddr<0) return -1;
    memset(recvline,0,sizeof(recvline));
    int len = 0;
    while((len=recv(sock_pacv,recvline,512,0))>0){
        printf("%s",recvline);
        memset(recvline,0,sizeof(recvline));
    }
    close(sock_pacv);
    memset(recvline,0,sizeof(recvline));
    recvaddr = recv(sockfd,recvline,512,0);
    if(strncmp(recvline,"226",3)==0){
        return 1;
    }
    else return -1;
}
//创建目录
int ftp_mkdir(int sockfd,const char *path){
    //char path[32]={};
    char sendline[512]={};
    char recvline[512]={};
    sprintf(sendline,"MKD %s\r\n",path);
    int sendaddr = send(sockfd,sendline,strlen(sendline),0);
    if(sendaddr<0) return -1;
    int recvaddr = recv(sockfd,recvline,512,0);
    if(recvaddr<0) return -1;
    if(strncmp(recvline,"257",3)==0){
        return 1;
    }
    return 0;
}
//切换目录
int ftp_cd(int sockfd,const char *path){
    char sendline[512]={};
    char recvline[512]={};
    sprintf(sendline,"CWD %s\r\n",path);
    int sendaddr = send(sockfd,sendline,strlen(sendline),0);
    if(sendaddr<0) return -1;
    int recvaddr = recv(sockfd,recvline,512,0);
    if(recvaddr<0) return -1;
    if(strncmp(recvline,"200",3)!=0){
        return 1;
    }
    return 0;
}
//删除文件
int ftp_rm(int sockfd,const char *path){
    char sendline[512]={};
    char recvline[512]={};
    sprintf(sendline,"DELE %s\r\n",path);
    int sendaddr = send(sockfd,sendline,strlen(sendline),0);
    if(sendaddr<0){
        printf("ftp> 发送DELE指令出错\n");
        printf("ftp> 删除结束\n");
        return 0;	
    }
    int recvaddr = recv(sockfd,recvline,512,0);
    if(recvaddr<0){
        printf("ftp> 接收DELE指令出错\n");
        printf("ftp> 删除结束\n");
        return 0;	
    }
    if(strncmp(recvline,"250",3)==0){
        printf("ftp> 删除成功\n");
        return 0;	
    }
    printf("ftp> 删除失败\n");
    printf("ftp> 删除结束\n");
}
//删除目录
int ftp_rmdir(int sockfd,const char *path){
    char sendline[512]={};
    char recvline[512]={};
    sprintf(sendline,"RMD %s\r\n",path);
    int sendaddr = send(sockfd,sendline,strlen(sendline),0);
    if(sendaddr<0){
        printf("ftp> 发送RMD指令出错\n");
        printf("ftp> 删除结束\n");
        return 0;	
    }
    int recvaddr = recv(sockfd,recvline,512,0);
    if(recvaddr<0){
        printf("ftp> 接收RMD指令出错\n");
        printf("ftp> 删除结束\n");
        return 0;	
    }
    if(strncmp(recvline,"250",3)==0){
        printf("ftp> 删除成功\n");
        return 0;	
    }
    printf("ftp> 删除失败\n");
    printf("ftp> 删除结束\n");
}
//查看当前所在路径
int ftp_pwd(int sockfd){
    char sendline[512]={};
    char recvline[512]={};
    sprintf(sendline,"PWD\r\n");
    int sendaddr = send(sockfd,sendline,strlen(sendline),0);
    if(sendaddr<0) return -1;
    int recvaddr = recv(sockfd,recvline,512,0);
    if(recvaddr<0) return -1;
    if(strncmp(recvline,"257",3)==0){
        char* p = recvline+5;
        printf("ftp> ");
        while((*p)!='"'){
            printf("%c",*(p++));
        }
        printf("\n");
    }
    else return -1;
}
//帮助手册
int ftp_help(void){
    printf("FTP功能命令介绍\n");
    printf("help   --------->  帮助菜单\n");
    printf("quit   --------->  退出\n");
    printf("url    --------->  链接网址\n");
    printf("login  --------->  登陆\n");
    printf("syst   --------->  获取服务器版本\n");
    printf("ls     --------->  获取服务器当前列表\n");
    printf("mkdir path  ---->  创建目录\n");
    printf("rm path    ----->  删除文件\n");
    printf("rmdir path ----->  删除目录\n");
    printf("pwd    --------->  获取当前路径\n");
    printf("put path-------->  上传文件\n");
    printf("get path-------->  下载文件\n");
    printf("cd path -------->  切换目录\n");
    printf("clear  --------->  清屏\n");

}
//获取传输模式.i为二进制传输
int ftp_type(int sockfd){
    char sendline[512]={};
    char recvline[512]={};
    sprintf(sendline,"TYPE I\r\n");
    int sendaddr = send(sockfd,sendline,strlen(sendline),0);
    if(sendaddr<0) return -1;
    int recvaddr = recv(sockfd,recvline,512,0);
    if(recvaddr<0) return -1;
    if(strncmp(recvline,"200",3)==0){
        printf("使用二进制传输数据\n");
    }
    else return -1;
}
//下载
int ftp_get(int sockfd,const char *path){
    //char path[23]={};
    char sendline[512]={};
    char recvline[512]={};
    //设置传输类型
    if(ftp_type(sockfd)<0){
        printf("ftp> 二进制文件传输启动失败,将使用默认传输\n");	
    }
    //设置pasv接口
    int pasv_sock = ftp_pasv(sockfd);
    if(pasv_sock<0) {
        printf("ftp> 设置pasv接口失败\n");
        printf("ftp> 下载结束,请稍后重试\n");
        return 0;
    }
    //发送指令
    sprintf(sendline,"RETR %s\r\n",path);
    int sendaddr = send(sockfd,sendline,strlen(sendline),0);
    if(sendaddr<0){
        close(pasv_sock);
        printf("ftp> 发送指令失败\n");
        printf("ftp> 下载结束,请稍后重试\n");
        return -1;
    }
    int recvaddr = recv(sockfd,recvline,512,0);
    if(recvaddr<0){
        close(pasv_sock);
        printf("ftp> 接受指令失败\n");
        printf("ftp> 下载结束,请稍后重试\n");
        return -1;
    }
    if(strncmp(recvline,"300",3)>0){
        close(pasv_sock);
        printf("ftp> %s文件不存在\n",path);
        printf("ftp> 下载结束,请稍后重试\n");
        return -1;//文件不存在
    }
    int fd = open(path,O_WRONLY|O_CREAT|O_TRUNC,0666);
    if(fd == -1) {
        printf("ftp> 本地文件创建失败\n");
        printf("ftp> 下载结束,请稍后重试\n");
        return -1;
    }
    //开始想pasv接口读取数据
    memset(sendline,0,sizeof(sendline));
    memset(recvline,0,sizeof(recvline));
    while((recvaddr=recv(pasv_sock,recvline,512,0))){
        int write_line = write(fd,recvline,recvaddr);
        if(write_line!=recvaddr){
            close(pasv_sock);
            close(fd);
            printf("ftp>%s 文件中失败\n",path);
            printf("ftp> 下载结束,请稍后重试\n");
            return -1;
        }
    }
    close(pasv_sock);
    close(fd);
    //向服务器接受数据
    memset(recvline,0,sizeof(recvline));
    recvaddr = recv(sockfd,recvline,512,0);
    recvline[recvaddr]=0;
    if(strncmp(recvline,"300",3)>0){
        printf("ftp> %s文件传输成功,但为接受到服务器数据\n",path);
        printf("ftp> 下载结束,请检查是否下载是否正确\n");
        return -1;
    }
    printf("ftp> 下载成功结束.\n");
}
//上传数据
int ftp_up(int sockfd,const char *path){
    //char path[23]={};
    char sendline[512]={};
    char recvline[512]={};
    //设置传输模式
    //在本地创建该文件
    int fd = open(path,O_RDONLY);
    if(fd == -1){
        printf("ftp> 上传结束,文件不存在\n");
        return -1;
    }
    //设置传输类型
    if(ftp_type(sockfd)<0){
        printf("ftp> 二进制文件传输启动失败,将使用默认传输\n");	
    }
    //设置pasv接口
    int pasv_sock = ftp_pasv(sockfd);
    if(pasv_sock<0) {
        close(fd);
        printf("ftp> 设置pasv接口失败\n");
        printf("ftp> 上传结束,请稍后重试\n");
        return 0;
    }
    //发送指令
    sprintf(sendline,"STOR %s\r\n",path);
    int sendaddr = send(sockfd,sendline,strlen(sendline),0);
    if(sendaddr<0){
        close(pasv_sock);
        close(fd);
        printf("ftp> 发送指令失败\n");
        printf("ftp> 上传结束,请稍后重试\n");
        return -1;
    }
    int recvaddr = recv(sockfd,recvline,512,0);
    if(recvaddr<0){
        close(pasv_sock);
        close(fd);
        printf("ftp> 接受指令失败\n");
        printf("ftp> 上传结束,请稍后重试\n");
        return -1;
    }
    if(strncmp(recvline,"300",3)>0){
        close(pasv_sock);
        close(fd);
        printf("ftp> 与服务器链接失败\n");
        printf("ftp> 上传结束,请稍后重试\n");
        return -1;
    }
    //开始想pasv接口写入数据
    memset(sendline,0,sizeof(sendline));
    memset(recvline,0,sizeof(recvline));
    int len = 0;
    while((len=read(fd,recvline,512))>0){
        sendaddr = send(pasv_sock,recvline,len,0);
        if(sendaddr!=len){
            close(pasv_sock);
            close(fd);
            printf("ftp> 文件传输失败\n");
            printf("ftp> 上传结束,请稍后重试\n");
            return -1;
        }
    }
    close(pasv_sock);
    close(fd);
    //想服务器接受数据
    memset(recvline,0,sizeof(recvline));
    recvaddr = recv(sockfd,recvline,512,0);
    recvline[recvaddr]=0;
    if(strncmp(recvline,"300",0)>0){
        printf("ftp> 文件理论上传成功,但未接受到服务器数据\n");
        printf("ftp> 上传结束,请检查上传文件完整性\n");
        return -1;
    }
    printf("ftp> 文件上传成功\n");
}



