// 채팅 참가자 관리, 채팅 메시지 수신 및 방송

#include <stdio.h> //표준 입출력: printf, perror, puts 
#include <stdlib.h> //exit, atoi, malloc 같은 유틸 함수
#include <string.h> //memset, strstr, strlen 같은 문자열 함수
#include <strings.h>///bzero 같은 BSD 계열 함수
#include <fcntl.h> //파일/디스크립터 관련 함수/상수
#include <sys/socket.h> //소켓 관련 기본 함수: socket, bind, listen, accept, send, recv
#include <sys/file.h>
#include <netinet/in.h>//struct sockaddr_in, htons, htonl 등 IP/포트 관련 구조체와 함수
#include <arpa/inet.h> //inet_ntop, inet_pton 같은 IP 문자열 <-> 이진 변환
#include <unistd.h>//close, read, write, sleep 
#include <sys/select.h> //select, fd_set, FD_SET, FD_ZERO, FD_ISSE

#define MAXLINE 511 //메시지 최대 크기
#define MAX_SOCK 1024 //최대 클라이언트 수

char *EXIT_STRING = "exit"; 
char *START_STRING = "Connected to chat_server \n";

int maxfdp1; 
int num_chat=0;
int clisock_list[MAX_SOCK];
int listen_sock;

void addClient(int s, struct sockaddr_in *newcliaddr); //새로 접속한 클라이언트 fd를 리스트에 추가
int getmax(); //listen_sock와 clisock_list 중 가장 큰 fd 찾기
void removeClient(int s); //채팅 탈퇴 처리 함수
int tcp_listen(int host, int port, int backlog); // 소켓 생성 및 listen 해주는 헬퍼함수
void errquit(char *mesg){
    perror(mesg);
    exit(1);
}

int main(int argc, char * argv[]){
    struct sockaddr_in cliaddr; //클라이언트 주소 정보 저장용
    char buf[MAXLINE+1];
    int i, j, nbyte,accp_sock;
    int addrlen=sizeof(struct sockaddr_in); //accept() 호출 시 주소 길이
    fd_set read_fds; //select()에서 읽기 이벤트 감시할 fd 구조체
    if(argc != 2){
        printf("사용법 : %s port\n",argv[0]);
        exit(0);
    }

    //tcp_listen(host,port,backlog) 함수호출
    listen_sock = tcp_listen(INADDR_ANY,atoi(argv[1]),5); // 코드를 깔끔하게하고, 반복되는 작업을 캡슐화히기위해 tcp_listen 함수 사용
                                                          // 모든 IP(0.0.0.0)에서 들어오는 연결 허용, 문자열 포트를 정수로 변환, listen 큐 크기
    while(1){
        FD_ZERO(&read_fds); //fd 집합을 비우기
        FD_SET(listen_sock,&read_fds);//새로운 연결이 들어오는지 감시.
        for(i=0;i<num_chat;i++){ //이미 접속한 모든 클라이언트들의 소켓을 감시 목록에 추가 -> 어느 클라이언트가 메시지를 보냈는지 확인하기 위해서
            FD_SET(clisock_list[i],&read_fds);
        }
        maxfdp1=getmax()+1; //select가 감시해야 할 파일 디스크립터 중 가장 큰 번호 + 1 값을 계산해서 select의 첫 번째 인자로 넣는다
        puts("wait for client");
        if(select(maxfdp1,&read_fds,NULL,NULL,NULL)<0){
            errquit("select fail");
        }
        if(FD_ISSET(listen_sock,&read_fds)){// select 이후에 listen_sock이 “읽기 가능” 상태라는 건→ 새 연결 요청(connect)이 들어왔다는 뜻
            accp_sock=accept(listen_sock,(struct sockaddr *)&cliaddr,&addrlen);
            if(accp_sock==-1){
                errquit("accept fail");
            }
        addClient(accp_sock,&cliaddr);
        send(accp_sock,START_STRING,strlen(START_STRING),0);
        printf("%d번째 사용자 추가.\n",num_chat);
        }

    //클라이언트가 보낸 메시지를 모든 클라이언트에게 발송
        for(i=0;i<num_chat;i++){
            if(FD_ISSET(clisock_list[i],&read_fds)){//i번째 클라이언트 소켓에서 “읽을 데이터가 있음”을 의미
                nbyte=recv(clisock_list[i],buf,MAXLINE,0);
                if(nbyte<=0){
                    removeClient(i);
                    continue;
                }
                buf[nbyte]=0; //문자열로 처리하기 위해 끝에 '\0' 추가
                if(strstr(buf,EXIT_STRING)!=NULL){
                    removeClient(i);
                    continue;
                }
                for (j=0;j<num_chat;j++){ //현재 접속한 모든 클라이언트에게 같은 메시지를 보내서 방송(broadcast
                    send(clisock_list[j],buf,nbyte,0);
                }
                printf("%s\n",buf);
            }
        }
    }
    return 0;

}

// 새로운 채팅 관리자 처리
void addClient(int s, struct sockaddr_in *newcliaddr){
    char buf[20];
    inet_ntop(AF_INET,&newcliaddr->sin_addr,buf,sizeof(buf)); //클라이언트 IP 주소를 사람이 읽을 수 있는 문자열로 변환
    printf("new client: %s\n",buf);

    clisock_list[num_chat]=s; //배열 끝에 fd 추가
    num_chat++;
}

//채팅 탈퇴 처리
void removeClient(int s){
    close(clisock_list[s]);
    if(s!=num_chat-1){
        clisock_list[s]=clisock_list[num_chat-1];//삭제할 인덱스가 배열의 마지막이 아니면 -> 마지막 원소를 현재 위치로 가져와서 “구멍 메우기”.
    }
        num_chat--;
        printf("채팅 참가자 1명 탈퇴. 현재 참가자 수 = %d\n",num_chat);
    
}

//최대 소켓번호 찾기 ->listen_sock과 clisock_list[i] 중 가장 큰 fd를 찾음
int getmax(){
    int max=listen_sock;
    int i;
    for(i=0;i<num_chat;i++){
        if(clisock_list[i]>max){
            max=clisock_list[i];
        }
    }
    return max;
}

//listen 소켓 생성 및 listen
int tcp_listen(int host,int port, int backlog){
    int sd;
    struct sockaddr_in servaddr;

    sd=socket(AF_INET,SOCK_STREAM,0);
    if(sd==-1){
        perror("socket fail");
        exit(1);
    }
    //servaddr 구조체의 내용 세팅
    bzero((char *)&servaddr,sizeof(servaddr)); //servaddr 구조체 전체 메모리를 0으로 초기화
    servaddr.sin_family=AF_INET;//IPv4 사용
    servaddr.sin_addr.s_addr=htonl(host); //모든 IP에서의 접속을 받아들임.
    servaddr.sin_port=htons(port);
    if(bind(sd,(struct sockaddr * )&servaddr, sizeof(servaddr))<0){
        perror("bind fail");
        exit(1);
    }
    listen(sd,backlog);
    return sd;
}