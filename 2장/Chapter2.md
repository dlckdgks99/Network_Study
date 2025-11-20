# 2.1 소켓 개요

### 소켓 정의
- 소켓번호
- 포트번호
  - /etc/services 
  - $sudo netstat -tulnp : 나의 서버에서 실제로 열린 포트 확인

### 소켓 사용법
- 소켓개설
  1. 통신에 사용할 프로토콜(TCP or UDP)
  2. 자신의 IP주소
  3. 자신의 포트번호
  4. 상대방의 IP주소
  5. 상대방의 포트번호

- C 프로그램 환경
- 소켓주소 구조체
  - 소켓 주소 : 주소체계, IP주소, 포트번호
  - sockaddr : 소켓 주소를 담을 구조
    - 2바이트의 address family와 14 바이트의 주소(IP 주소 + 포트번호)로 구성
  - sockaddr_in :
    - 4 바이트의 IP 주소와 2 바이트의 포트번호를 구분하여 액세스할 수 있는 인터넷 전용 소켓주소 구조체
    - sockaddr과의 호환성을 위하여 8바이트의 빈칸 sin_zere[8]을 사용하여 sockaddr_in 구조체의 전체 크기를 16바이트로 맞춤

  - 소켓 사용절차
    - 서버
        - socket()
        - bind()
        - listen()
        - accept()
        - send()
        - recv
    - 클라이언트
        - socket()
        - connect()
        - recv()
        - send()
        - close()

# 2.2 인터넷 주소변환
### 바이트 순서
- 호스트 바이트 순서 : 컴퓨터가 내부 메모리에 숫자를 저장하는 순서
  - little-endian : 하위 바이트가 메모리에 먼저 저장
  - big-endian : 상위 바이트가 메모리에 먼저 저장
- 네트워크 바이트 순서 : 포트번호나 IP 주소와 같은 정보를 바이트 단위로 네트워크로 전송하는 순서, high-order 바이트부터 전송
  
- Unsigned short interger 변환 (2바이트 크기)
  - htons() : host-to network 바이트 변환
  - ntohs() : network-to-host 바이트 변환
- Unsigned long interger 변환(4바이트 크기)
  - htonl() : host-to network 바이트 변환
  - ntohl() : network-to-host 바이트 변환
    
- 바이트 순서 확인 예
  - $getservbyname("echo,"udp")

### IP 주소 변환
- 인터넷 주소 표현 방법
  - 도메인 네임
  - IP 주소
  - dotted decimal
- 인터넷 주소 표현법을 상호변환해 주는 네 개의 주소변환 함수
  - gethostbyname()
    -IP 주소를 포함하고 있는 구조체 in_addr의 포인터와 이 주소의 길이, 주소타입을 입력하여 해당 호스트의 정보를 가지고 있는 hostent 구조체의 포인터를 리턴
  - gethostbyaddr()
  - inet_pton()
  - inet_ntop()

# 2.3 TCP 클라이언트 프로그램
### TCP 클라이언트 프로그램 작성 절차
1. socket() : 소켓 개설
  - tcp 소켓 개설 : SOCK_STREAM으로 선택
  - 사용할 트랜스포트 프로토콜, 자신의 IP 주소와 포트번호, 상대방의 IP주소와 포트번호 등 다섯 개 정보가 지정되어야함
2. sockaddr_in 구성 : 서버의 IP주소와 포트 번호 지정
3. connect() : 서버로 연결 요구
  - 3-way 핸드쉐이크 시작
  - 서버와 연걸이 되려면 서버측에서는 listen()과 accpet()를 호출해 두고있어야함 
4. send(),recv() : 데이터 송수신
  - 데이터를 전송할 소켓번호(s)
  - 송신할 데이터 버퍼(buf)
  - 전송할 데이터 크기(length)를 지정해야한다.
  - 
5. close()
  - 소켓의 사용을 마치기위한 함수 
### TCP 클라이언트 예제 프로그램 
- daytime 클라이언트 : port 번호 13번
  - casting의 이유 : struct sockaddr_in → struct sockaddr 로 형 변환해서 소켓 함수에 전달하는 것
  - bzero : 어떤 포인터가 가리키는 구조체의 내용을 모두 0으로 변환
    -초기화하지 않으면 구조체 내부에 쓰레기 값이 남아 있어서 connect() 등에서 예상치 못한 동작 오류가 발생할 수 있음
    
- TCP 에코 클라이언트 : port 번호 7번
  - 보안상 echo 기능을 하지않아 연결실패
  - fget 함수 : 스트링 문자열을 입력받는 함수
    - fgets(문자열저장버퍼, 버퍼의 크기, 입력스트림포인터)
  - write함수
  - read 함수
    
- 포트번호 배정
  - TCP : connect() 호출 성공 후 배정
  - UDP : 첫 번째 메시지를 보내는 sendto()함수가 성공한 후에 배정
  - getsockname() : 자신의 호스트에 있는 소켓 정보를 알아내는 함수
  - 보안 취약점 때문에 대부분의 Linux 배포판에서 echo(7) 비활성화
    
# TCP 서버 프로그래밍
### TCP 서버 프로그램 작성 절차
- socket(), 소켓의 생성
  - socket(PF_INET,SOCK_STREAM,0)
  - 그 응용 프로그램 내에서 유일한 번호인 소켓번호를 배정받음 
- bind()
  - 소켓번호와 소켓주소(자신의 IP주소+자신의 포트번호)를 연결하는 역할
  - int bind(int s, struct sockaddr *addr, int len) = int bind(소켓번호, 서버 자신의 소켓주소 구조체 포인터, *addr 구조체의 크기)
  - INADDR_ANY : 서버가 자신의 IP 주소를 자동으로 가져다 쓸 때 사용
- listen()
  - socket()함수에 의해 생성되는 소켓은 기본적으로 능동적 소켓, 수동적 소켓으로 변환하려면 listen 호출
  - int listen(int s,int backlog) = int listen(소켓번호, 연결을 기다리는 클라이언트의 최대 수)
- accept()
  - 서버는 listen을 호출한 이후 클라이언트와 설정된 연결들을 실제로 받아들이기 위해 사용
  - int accept(int s, struct sockaddr *addr, int *addrlen) = int accept(소켓번호, 연결요청을 한 클라이언트의 소켓주소 구조체, *addr 구조체 크기의 포인터)
- TCP의 3-way 핸드쉐이크
  - 클라이언트가 connect()를 호출하면 커널은 SYN(X)를 전송
  - 이를 수신한 서버는 ACK(X+1), SYN(Y)로 응답
  - 클라이언트의 커널이 다시 ACK(Y+1) 보냄
  - 이는 서버측 listen이 호출되어야한다
  - listen이 호출되어있다면 서버는 accept()를 호출하여 accept 큐에서 하나의 연결을 꺼내가면 그 때 클라이언트의 connect()함수가 리턴되고 송수신 가능해진다.
 
### TCP 에코 서버 프로그램

# UDP 프로그램
### UDP 프로그램 작성 절차
#  🤔 2장에 대한 생각
1. IP 주소 변환 4가지에 대해 각각 차이 알기
  - gethostbyname()
  - gethostbyaddr()
  - inet_pton()
  - inet_ntop()
2. CASTING
  - struct sockaddr_in → struct sockaddr 로 형 변환해서 소켓 함수에 전달하는 것
  - bind, connect, accept 소켓함수들은 sockaddr만 인자로 받고있기때문에 sockaddr_in을 sockaddr로 캐스팅하는것!
3. bzero =meeset
  - sockaddr_in의 값들을 처음에 0으로 초기화하지않으면 쓰레값으로인해 소켓함수에서 예상치 못한 오류가 발
3. PORT 지원 X
  - daytime, echo 등 보안문제로 인해 OS가 막혀있다. 그래서 연결해 실패했다 => 예전에는 자동으로 서버가 돌아갔지만 현재는 막혀있어 연결이 실패한다. 새로운 포트를 가진 서버를 구축해 연결을 해야한다
  - 2000년에는 echo, daytime 서버가 자동으로 돌고 있었고 지금은 서버가 꺼져 있으니 직접 서버를 만들어서 실행해야함
4. servaddr.sin_addr.s_addr = htonl(INADDR_ANY) : 서버측에서 어떤 ip든 들어와라, 서버만사용
5. inet_pton(AF_INET, "192.168.0.10", &servaddr.sin_addr) : 특정한 IP만 들어오고 연결, 서버 및 클라이언트 사


