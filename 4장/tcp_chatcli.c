//서버에 접속한 후 키보드의 입력을 서버로 전달하고, 서버로부터 오는 메시지를 화면에 출력

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAXLINE 1000
#define NAME_LEN 20

char * EXIT_STRING="exit";

//소켓 생성 및 서버 연결, 생성된 소켓 리턴
int tcp_connect(int af, char *servip, unsigned short port);//IPv4 주소(AF_INET), IP, 포트를 받아서 서버와 TCP 연결을 생성하는 함수.
void errquit(char *mesg){
    perror(mesg);
    exit(1);
}
int main(int argc,char *argv[]){
    char bufall[MAXLINE+NAME_LEN];//[이름]:메시지 전체가 저장될 버퍼
    char *bufmsg; //이름 뒤의 메시지 부분만 가리키기 위한 포인터
    int maxfdp1,s,namelen;
    fd_set read_fds; //select에서 감시할 fd 집합
    if(argc !=4){
        printf("사용법 : %s server_ip port name \n",argv[0]);
        exit(0);
    }
    sprintf(bufall,"[%s]:",argv[3]); //이름을 [Name]: 형태로 만들기
    namelen=strlen(bufall);
    bufmsg=bufall+namelen;
    s=tcp_connect(AF_INET,argv[1],atoi(argv[2]));
    if(s==-1){
        errquit("tcp_connect fail");
    }
    puts("서버에 접속되었습니다.");
    maxfdp1=s+1; //소켓 s와 stdin(0)을 감시할 예정이므로:
    FD_ZERO(&read_fds);

    while(1){
        FD_SET(0,&read_fds);//0 = 키보드
        FD_SET(s,&read_fds);//s = 서버 소켓
        if(select(maxfdp1,&read_fds,NULL,NULL,NULL)<0){ //키보드를 눌렀거나 서버가 메시지를 보냈거나 둘 중 하나라도 발생하면 select가 깨어남
            errquit("select fail");
        }
        if(FD_ISSET(s,&read_fds)){
            int nbyte;
            if((nbyte=recv(s,bufmsg,MAXLINE,0))>0){
                bufmsg[nbyte]=0;
                printf("%s \n",bufmsg);
            }
        }
        if(FD_ISSET(0,&read_fds)){
            if(fgets(bufmsg,MAXLINE,stdin)){
                if(send(s,bufall,namelen+strlen(bufmsg),0)<0){
                    puts("Error : Write error on socket.");
                }
                if(strstr(bufmsg,EXIT_STRING)!=NULL){
                    puts("Good bye.");
                    close(s);
                    exit(0);
                }
            }
        }
    }
}
int tcp_connect(int af,char * servip, unsigned short port){
    struct sockaddr_in servaddr;
    int s;
    if((s=socket(af,SOCK_STREAM,0))<0){
        return -1;
    }
    bzero((char*)&servaddr,sizeof(servaddr));
    servaddr.sin_family=af;
    inet_pton(AF_INET,servip, &servaddr.sin_addr);
    servaddr.sin_port=htons(port);

    // 연결 요청
    if(connect(s,(struct sockaddr *)&servaddr,sizeof(servaddr))<0){
        return -1;
    }
    return s;
}

