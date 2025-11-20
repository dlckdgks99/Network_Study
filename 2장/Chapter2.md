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

  -소켓 사용절차
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
### TCP 클라이언트 예제 프로ㅓ그램
- daytime 클라이언트
