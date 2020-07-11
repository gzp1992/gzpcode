#include "ftp.h"
#include "t_func.h"
void connect_url_2(void){
    char host[50] = {};
    printf("ftp> url:");
    scanf("%s",host);
    scanf("%*[^\n]");
    scanf("%*c");
    socketfd = ftp_connect(host,21);
    if(socketfd<0){
        printf("链接失败,请检查域名是否正确\n");
        return;
    }
    else{
        if(connect_recv(socketfd)<0){
            printf("ftp> 链接服务器失败,请于管理员联系\n");
            return;
        }
        else{
            printf("ftp> 链接成功\n");
            flag=1;
            t_login();
        }
    }

}
void t_url(void){
    if(flag==1||flag==2){
        printf("ftp> 是否放弃之前的链接(y/n):");
        char c=0;
        scanf("%c",&c);
        scanf("%*[^\n]");
        scanf("%*c");
        if(c=='n'||c=='N'){
            return;
        }
        else if(c=='y'||c=='Y'){  
            flag=0;
            close(socketfd);
            connect_url_2();
        }
        else{
            printf("输入错误\n");
            return;
        }
    }
    else {
        connect_url_2();
    }
} 
void t_quit(void){
    ftp_quit(socketfd);
}

void t_login(void){
    if(flag==0){
        printf("ftp> 没有链接网址，请输入url命令进行网址链接\n");
        return;
    }
    else if(flag==2){
        printf("ftp> 已经登陆，无需在登陆\n");
        return;
    }
    switch(ftp_login(socketfd)){
        case -1:
            printf("ftp> 用户名错误\n");
            return;
        case -2:
            printf("ftp> 密码出错\n");
            return;
        case 0:
            printf("ftp> 登陆成功\n");
            flag=2;
            return;
        default:
            printf("ftp> 请检查网络环境,并重启客户端\n");
            return;
    }
}

void t_syst(void){
    if(flag<2){
        printf("ftp> 请先登陆服务器\n");
        return;
    }
    if(ftp_syst(socketfd)<0){
        printf("ftp> 请检查网络环境,并重试\n");
    }
}	
void t_ls(void){
    if(flag<2){
        printf("ftp> 请先登陆服务器\n");
        return;
    }
    if(ftp_list(socketfd,".")<0){
        printf("ftp> 请检查网络,并重试\n");
    }
}
void t_mkdir(void){
    if(flag<2){
        printf("ftp> 请先登陆服务器\n");
        return;
    }
    if((ftp_mkdir(socketfd,path))<0){
        printf("ftp> 请检查网络,并重试\n");
    }
}
void t_cd(void){
    if(flag<2){
        printf("ftp> 请先登陆服务器\n");
        return;
    }
    if((ftp_cd(socketfd,path))<0){
        printf("ftp> 请检查网络,并重试\n");
    }
}
void t_rm(void){
    if(flag<2){
        printf("ftp> 请先登陆服务器\n");
        return;
    }
    ftp_rm(socketfd,path);
}
void t_rmdir(void){
    if(flag<2){
        printf("ftp> 请先登陆服务器\n");
        return;
    }
    if((ftp_rmdir(socketfd,path))<0){
        printf("ftp> 请检查网络,并重试\n");
    }
}
void t_pwd(void){
    if(flag<2){
        printf("ftp> 请先登陆服务器\n");
        return;
    }
    if((ftp_pwd(socketfd))<0){
        printf("ftp> 请检查网络,并重试\n");
    }
}
void t_help(void){
    ftp_help();
}
void t_get(void){
    if(flag<2){
        printf("ftp> 请先登陆服务器\n");
        return;
    }
    ftp_get(socketfd,path);
}
void t_put(void){
    if(flag<2){
        printf("ftp> 请先登陆服务器\n");
        return;
    }
    ftp_up(socketfd,path);
}
void t_clear(void){
    system("clear");
}
void t_n(void){
}
void t_r(void){
}
void t_0(void){
}





