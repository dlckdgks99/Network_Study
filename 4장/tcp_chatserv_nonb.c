//넌블록킹(non-blocking) I/O 기반의 채팅 서버

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/file.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define MAXLINE 511
#define MAX_SOCK 1024

char *EXIT_STRING ="exit";
char *START_STRING="Connected to chat_server\n";
int maxfdp1;
int num_chat=0;
int clisock_list[MAX_SOCK];
int listen_sock;

void addClient(int s, struct sockaddr_in *newcliaddr);
void removeClient(int);
int set_nonblock(int sockfd);
int is_nonblock(int sockfd);
int tcp_listen(int host,int port, int backlog);
void errquit(char *mesg){
    perror(mesg);
    exit(1);
}

int main(int argc,char *argv[]){
    char buf[MAXLINE];
    int i,j,nbyte,count, addrlen;
    int accp_sock,clilen;
    struct sockaddr_in cliaddr;

    if(argc !=2){
        printf("사용법 %s port \n",argv[0]);
        exit(0);
    }

    listen_sock=tcp_listen(INADDR_ANY,atoi(argv[1]),5);
    if(listen_sock==-1){
        errquit("tcp_listen fail");
    }
    if(set_nonblock(listen_sock)==-1){
        errquit("set_nonblock fail");
    }

    for(count=0;;count++){
        if(count==100000){
            putchar('.');
            fflush(stdout); 
            count=0;
        }

        addrlen=sizeof(cliaddr);
        accp_sock=accept(listen_sock,(struct sockaddr *)&cliaddr, &clilen);
        if(accp_sock==-1 &&errno!=EWOULDBLOCK){//당장 접속한 클라이언트가 없음  , 1.“데이터 없음(EWOULDBLOCK)”이면 -> 정상적인 상황이므로 무시하고 넘어감 2.다른 errno라면 -> 진짜 에러라서 서버 종료
            errquit("accept fail");
        }
        else if(accp_sock>0){ //새로운 클라이언트가 딱 지금 접속함 
            clisock_list[num_chat]=accp_sock;
            if(is_nonblock(accp_sock)!=0 &&set_nonblock(accp_sock)<0){
                errquit("set_nonblock fail");
            }
            addClient(accp_sock,&cliaddr);
            send(accp_sock,START_STRING,strlen(START_STRING),0);
            printf("%d번째 사용자 추가. \n",num_chat);
        }

        //클라이언트가 보낸 메시지를 모든 클라이언트에게 방송
        for (i=0;i<num_chat;i++){
            errno=0;
            nbyte=recv(clisock_list[i],buf,MAXLINE,0);
            if(nbyte==0){// nbyte==0 -> 클라이언트가 연결 종료 
                removeClient(i);
                continue;;
            }
            else if(nbyte==-1 &&errno ==EWOULDBLOCK){// 정상 상황 skip
                continue;
            }

            //nbyte>0면 정상적으로 데이터 수신
            if(strstr(buf,EXIT_STRING)!=NULL){  //exit가 있는지 확인
                removeClient(i);
                continue;
            }

            buf[nbyte]=0;
            for(j=0;j<num_chat;j++){ //출력
                send(clisock_list[j],buf,nbyte,0);
            }
            printf("%s\n",buf);
        }
    }
}

void addClient(int s, struct sockaddr_in *newcliaddr){
    char buf[20];
    inet_ntop(AF_INET,&newcliaddr->sin_addr,buf,sizeof(buf));
    printf("new client: %s\n",buf);
    clisock_list[num_chat]=s;
    num_chat++;
}

void removeClient(int i){
    close(clisock_list[i]);
    if(i!=num_chat-1){
        clisock_list[i]=clisock_list[num_chat-1];
    }
    num_chat--;
    printf("채팅 참가자 1명 탈퇴. 현재 참가자 수 =%d\n",num_chat);
}


//소켓이 nonblock인지 확인
int is_nonblock(int sockfd){ //val & O_NONBLOCK으로 플래그 확인
    int val;
    val=fcntl(sockfd,F_GETFL,0);
    if(val & O_NONBLOCK){
        return 0;
    }
    return -1;
}

//소켓을 nonblock으로 설정
int set_nonblock(int sockfd){
    int val;
    val=fcntl(sockfd,F_GETFL,0);//커널에 “이 fd(sockfd)에 설정돼 있는 상태 플래그들 val에 저장
    if(fcntl(sockfd,F_SETFL,val|O_NONBLOCK)==-1){  // val | O_NONBLOCK ->  기존 플래그는 그대로 두고, 거기에 non-block 플래그만 추가하는 것. 이 소켓을 향한 모든 I/O 호출이 “블록하지 않는 방식”으로 바뀜.
        return -1;
    }
    return 0;
}

int tcp_listen(int host, int port, int backlog){
    int sd;
    struct sockaddr_in servaddr;

    sd=socket(AF_INET,SOCK_STREAM,0);
    if(sd==-1){
        perror("socket fail");
        exit(1);
    }

    bzero((char *)&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(host); //host → network long,  4바이트, IP 주소
    servaddr.sin_port=htons(port); //host → network short, 2바이트, 포트 번호
    if (bind(sd, (struct sockaddr *) &servaddr,sizeof(servaddr))<0){
        perror("bind fail");
        exit(1);
    }
    listen(sd,backlog);
    return sd;
}