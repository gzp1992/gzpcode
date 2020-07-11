#include "ftp.h"
void reget(void){
    FILE *p_file = fopen(path,"rb");
    if(!p_file){
        printf("no file\n");
        return  ;
    }
    fseek(p_file,0,SEEK_END);
    long p_offset = ftell(p_file);
    printf("p_offset=%ld\n",p_offset);
    fclose(p_file);
    p_file = NULL;
//断点续传
    char sendline[1024]={};
    char recvline[1024]={};
    sprintf(sendline,"REST %ld\r\n",p_offset);
    printf("send REST\n");
    int sendaddr = send(socketfd,sendline,strlen(sendline),0);
    if(sendaddr== -1){
        return  ;
    }
    int recvaddr = recv(socketfd,recvline,sizeof(recvline),0);
    if(recvaddr== -1){
        return  ;	
    }
   if(strncmp(recvline,"350",3)==0){		//350		 文件行为暂停
      //  return ;
    printf("send REST\n");
    
   }
    
    else{
        return  ;
    }
    //char path[23]={};
    //设置传输类型
    if(ftp_type(socketfd)<0){
        printf("ftp> 二进制文件传输启动失败,将使用默认传输\n");	
    }
    //设置pasv接口
    int pasv_sock = ftp_pasv(socketfd);
    if(pasv_sock<0) {
        printf("ftp> 设置pasv接口失败\n");
        printf("ftp> 下载结束,请稍后重试\n");
        return ;
    }
    //发送指令
    sprintf(sendline,"RETR %s\r\n",path);
    printf("send RETR\n");
    sendaddr = send(socketfd,sendline,strlen(sendline),0);
    if(sendaddr<0){
        close(pasv_sock);
        printf("ftp> 发送指令失败\n");
        printf("ftp> 下载结束,请稍后重试\n");
        return  ;
    }
    recvaddr = recv(socketfd,recvline,512,0);
    if(recvaddr<0){
        close(pasv_sock);
        printf("ftp> 接受指令失败\n");
        printf("ftp> 下载结束,请稍后重试\n");
        return  ;
    }
    if(strncmp(recvline,"300",3)>0){
        close(pasv_sock);
        printf("ftp> %s文件不存在\n",path);
        printf("ftp> 下载结束,请稍后重试\n");
        return  ;//文件不存在
    }
    int fd = open(path,O_WRONLY|O_APPEND);
    if(fd == -1 ) {
        printf("ftp> open lose下载结束,请稍后重试\n");
        return  ;
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
            return  ;
        }
    }
    close(pasv_sock);
    close(fd);
    //向服务器接受数据
    memset(recvline,0,sizeof(recvline));
    recvaddr = recv(socketfd,recvline,512,0);
    recvline[recvaddr]=0;
    if(strncmp(recvline,"300",3)>0){
        printf("ftp> %s文件传输成功,但为接受到服务器数据\n",path);
        printf("ftp> 下载结束,请检查是否下载是否正确\n");
        return  ;
    }
    printf("ftp> 下载成功结束.\n");
}


void reput(void){
    char sendline[512]={};
    char recvline[512]={};
    //设置传输模式
    //在本地创建该文件
    int fd = open(path,O_RDONLY);
    if(fd == -1){
        printf("ftp> 上传结束,文件不存在\n");
        return ;
    }
    sprintf(sendline,"SIZE %s\r\n",path);
    int sendaddr = send(socketfd,sendline,strlen(sendline),0);
    if(sendaddr== -1){
        return  ;
    }
    int recvaddr = recv(socketfd,recvline,512,0);
    if(recvaddr== -1){
        return  ;	
    }
    char rcv1[16] = {0};
    char rcv2[16] = {0};
    sscanf(recvline,"%s %s",rcv1,rcv2);
    long int size = (long)atoi(rcv2);
    memset(sendline,0,sizeof(sendline));
    memset(recvline,0,sizeof(recvline));
    sprintf(sendline,"REST %ld\r\n",size);
    sendaddr = send(socketfd,sendline,strlen(sendline),0);
    if(sendaddr== -1){
        return  ;
    }
    recvaddr = recv(socketfd,recvline,sizeof(recvline),0);
    if(recvaddr== -1){
        return  ;	
    }
    lseek(fd,size,SEEK_SET); 



    //设置传输类型
    if(ftp_type(socketfd)<0){
        printf("ftp> 二进制文件传输启动失败,将使用默认传输\n");	
    }
    //设置pasv接口
    int pasv_sock = ftp_pasv(socketfd);
    if(pasv_sock<0) {
        close(fd);
        printf("ftp> 设置pasv接口失败\n");
        printf("ftp> 上传结束,请稍后重试\n");
        return ;
    }
    //发送指令
    sprintf(sendline,"STOR %s\r\n",path);
    sendaddr = send(socketfd,sendline,strlen(sendline),0);
    if(sendaddr<0){
        close(pasv_sock);
        close(fd);
        printf("ftp> 发送指令失败\n");
        printf("ftp> 上传结束,请稍后重试\n");
        return ;
    }
    recvaddr = recv(socketfd,recvline,512,0);
    if(recvaddr<0){
        close(pasv_sock);
        close(fd);
        printf("ftp> 接受指令失败\n");
        printf("ftp> 上传结束,请稍后重试\n");
        return ;
    }
    if(strncmp(recvline,"300",3)>0){
        close(pasv_sock);
        close(fd);
        printf("ftp> 与服务器链接失败\n");
        printf("ftp> 上传结束,请稍后重试\n");
        return ;
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
            return ;
        }
    }
    close(pasv_sock);
    close(fd);
    //想服务器接受数据
    memset(recvline,0,sizeof(recvline));
    recvaddr = recv(socketfd,recvline,512,0);
    recvline[recvaddr]=0;
    if(strncmp(recvline,"300",0)>0){
        printf("ftp> 文件理论上传成功,但未接受到服务器数据\n");
        printf("ftp> 上传结束,请检查上传文件完整性\n");
        return ;
    }
    printf("ftp> 文件上传成功\n");


}



