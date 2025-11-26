# 5.1 소켓 옵션 변경
### 소켓 옵션 변경 함수
- getsockopt() : extern int getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlen);  ,
  - 소켓에 현재 지정되어 있는 옵션 값을 알아냄
  - fd: 옵션을 조회할 소켓 번호
  - level: 옵션 레벨 (SOL_SOCKET, IPPROTO_IP, IPPROTO_TCP 등)
  - optname: 조회할 옵션 이름
  - optval: 옵션 값을 저장할 버퍼(포인터)
  - optlen: optval의 크기를 나타내는 변수(입력 + 출력)

- setsockopt() : extern int setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen);
  - 소켓 옵션을 변경함
  - fd: 옵션을 설정할 소켓 번호
  - level: 옵션 레벨 (SOL_SOCKET, IPPROTO_IP, IPPROTO_TCP 등)
  - optname: 설정할 옵션 이름
  - optval: 설정할 옵션 값이 저장된 버퍼(포인터)
  - optlen: optval의 크기(바이트 단위)

# 5.2 소켓 옵션 종류
### SO_KEEPALIVE
- TCP 연결이 정상적으로 지속되고 있는지를 주기적으로 확인해 주는 기능
- 세 가지 응답
  - 상대방이 ACK을 보낸다
    - TCP 연결이 정상적으로 동작 중
  - 상대방이 RST 에러를 보내온다
    - 상대방 호스트가 꺼진 후 재부팅 된 상태임을 나타내므로 TCP는 소켓을 닫으며 이때 에러 코드는 'EECONNRESET'
  - 아무 응답 없다
    - 질문을 몇 번 더 보낸본 후 연결을 종료
### SO_LINGER
- close()를 호출한 이후에 상대방에서도 정상적으로 종료 절차가 이루어졌는지를 확인하기 위해서 사용
- SO_LINGER 옵션을 지정하면 close()는 옵션에서 지정한 linger 시간이 먼저 타임아웃되면 ETIMEDOUT 에러가 발생
- shutdown()
  - int shutdown(int s, int how) : int shutdown(소켓번호, 소켓을 통한 양방향 스트림에 대한 동작 지정)
  - SHUT_WR : 송신 스트림만 닫는다, “나는 더 이상 보낼 데이터 없지만 너가 보내는 데이터는 계속 recv 할 수 있음”
  - SHUT_RD : 수신 스트림만 닫는다, “너가 보내는 데이터는 안 읽지만 나는 계속 send는 할 수 있음”
  - SHUT_RDWR : 송신 및 수신 양방향 스트림을 모두 닫는다, "둘 다 안해"
    
### SO_RCVBUF와 SO_SNDBUF
- 소켓의 송신버퍼와 수신버퍼의 크기를 변경
- 클라이언트의 경우 소켓 버퍼의 크기는 connect()호출 이전에, 서버는 listen() 함수를 호출하기 전에 각각 변경해야함 -> 3-way 핸드쉐이크를 할 때 상대방과 협의하여 버퍼 크기를 결정하며 이후에는 버퍼 크기를 바꿀 수 없기에

### SO_REUSERADDR
- 소켓주소 재사용 옵션은 동일한 소켓주소를 여러 프로세스에서 또한 한 프로세스 내의 여러 소켓에서 중복하여 사용하는 것을 허용하는 옵션
- 소켓주소 재상용 옵션이 필요한 경우
  - TIME-WAIT 상태에서의 주소 재사용
    - CTRL-C로 강제 종료시킨 후 즉시 다시 실행시켜보면 "Address in use"라는 에러가 발생해 서버 프로그램을 실행시킬 수 없다 -> active close를 하면 TCP가 TIME-WAIT 상태에서 2MSL 시간동안 기다려야하기 때문
    - 만일 TIME-WAIT 상태에 있는 포트번호를 바로 재사용할 필요가 있다면 소켓주소 재사용 옵션을 설정
    - bind()를 호출하기 전에 SO_RESUSEADDR 옵션을 지정
    - 서버 프로그램에서 TCP가 TIME-WAIT 상태에 동안에도 서비스를 즉시 재시작할 수 있도록 하기 위해서 소켓주소 재사용 옵션이 사용
  - 자식 프로세스가 서버인 경우
    - TCP 서버 프로그램에서 자식 프로세스가 서비스 처리를 담당하고 있는 도중에 부모 프로세스가 종료되었다가 다시 시작하게 되면 "포트번호가 사용중" 에러가 발생을 피하기 위해 사용
  - 멀티홈 서버의 경우
    - 멀티홈(호스트가 두 개 이상의 랜에 접속된 경우 이 호스트는 두 개의 IP주소를 갖는것)
    - 멀티홈 호스트에서 두 개 이상의 IP 주소가 같은 포트번호를 사용하는 것이 필요한 경우가 있는데 이를 위해 소켓주소 재사용 옵션 설정 사용
  - 완정 중복 바인딩
    - 여러 프로세스에서 동일한 IP 주소와 동일한 포트번호를 중복하여 bind()하는 것 => UDP 소켓에서만 사용 가능
    - ex)멀티캐스트 데이터그램 수신
### 기타 옵션
- SO_OOBINLINE : 상대방이 보낸 대역외(OOB: Out of Band) 데이터를 일반 데이터의 수신버퍼와 같이 저장되록하여 동등한 순서로 처리되도록 하는 옵션
- SO_RCVLOWAT : 송수신버퍼의 최소량 설정 옵션, 최소량 데이터 크기를 변경
- O_SNDLOWAT : 송수신버퍼의 최소량 설정 옵션, select()함수가 쓰기조건으로 리턴하기 위해서 필요한 송신버퍼의 최소한의 여유 공간 크기를 지정
- SO_DONTROUTE : 데이터그램 송신 시에 시스템이 배정하는 라우팅 경로를 사용하지 않도록 함 
- SO_BROADCAST : 브로드캐스트 메시지를 보낼 수 있는지를 결정, UDP소켓에서만 사용 가능
- TCP_NODELAY : TCP 에서는 Nagle's 알고리즘이 디폴트로 실행되고있는데 이 옵션을 선택하면 실행되지않음
- TCP_MAXSEG : TCP의 최대 세그먼트 크기(MSS) 값을 읽거나 변경할 때 사용

# 5.3 멀티캐스트 프로그래밍
### 멀티캐스트 개요
- 멀티캐스트 : 송신측에서 하나의 데이터그램을 송신하면 이것이 네트워크 내에서 복제되어 다수 호스트로 전송되는 기능
- broadcast VS multicast
  - 브로드캐스트는 한 LAN 내에서만, 멀티캐스트는 LAN 내에서 뿐 아니라 인터넷으로 연결되어있는 임의의 호스트들 중에 특정 멀티캐스트 그룹에 가입되어 있는 호스트들을 대상으로 데이터그램을 동시에 보냄
- 멀티캐스트 그룹 : 멀티캐스트 데이터그램을 수신하고자 하는 호스트들의 집합
  - 멀티캐스트 그룹을 구분하기 위해 클래스 D의 IP주소를 그룹 주소로 사용
  - 클래스 D의 IP주소는 224.0.0.0 ~ 239.255.255.255 사이의 값을 가짐, 이 그룹주소를 목적지로 하여 UDP 데이터그램을 전송
  - 멀티캐스트 서비스 시 중간의 라우터들이 MRP(Multicast Route)를 실행하고 있어야함, 어떤 호스트가 멀티캐스트 그룹에 가입하려면 그 호스트는 인접한 라우터에세 IGMP 메시지를 보내야함

### 멀티캐스트 데이터그램 송수신
- 송신 시, 1. 해당 멀티캐스트 그룹 IP주소 2. 멀티캐스트 그룹에서 공통으로 사용하는 포트번호 지정 3. UDP 데이터그램 전송
- 48비트의 MAC 주소를 보냄
- 소켓 옵션 지정
  - IP_ADD_MEMBERSHIP : 멀티캐스트 그룹에서 가입
  - IP_DROP_MEMBERSHIP : 멀티캐스트 그룹에서 탈퇴                 
  - IP_MULTICAST_LOOP : 멀티캐스트 데이터그램의 LOOPBACK 허용 여부 지정
  - IP_MULTICAST_TTL : 멀티캐스트 데이터그램의 TTL 값 지정
  - IP_MULTICAST_IF : 멀티캐스트 데이터그램의 전송용 인터페이스 지정
- ip_mreq, 그룹주소
  - ip_mreq : 멀티캐스트 그룹에 가입 또는 그룹에서 탈퇴하기 위해서는 멀티캐스트 그룹주소를 나타내는 구조체
    - 멀티캐스트 그룹을 나타내는 클래스 D의 IP주소와 이 멀티캐스트 그룹과 연결된 자신의 인터페이스(IP주소)가 포함

# 5.4 send()와 recv()의 옵션 지정
- MSG_PEEK : 데이터를 읽기만 하고 버퍼에서 삭제하지 않음, 현재 버퍼에 얼마나 많은 데이터가 있는지를 알아내는 데 사용
- MSG_WAITALL : recv()호출 시 지정된 크기의 데이터를 수신하기 전까지는 리턴하지 않고 대기
- MSG_OOB : 대역외 데이터를 송신 또는 수신할 때 사용
# 번외
### gcc VS cc 차이
| 명령 | 의미                       | 실제 컴파일러         | 특징                               |
| ---- | -------------------------- | ---------------------- | ---------------------------------- |
| gcc  | GNU C 컴파일러 직접 호출  | GCC                    | 강력한 경고/최적화, 가장 많이 사용 |
| cc   | 시스템 기본 C 컴파일러 호출 | GCC 또는 Clang 등     | OS에 따라 달라서 환경 의존적       |


# 🤔 5장에 대한 나의 생각
- 도메인 주소 변환함수[gethostbyname(), gethostbyaddr()] 와 소켓 옵션 변경함수 [getsockopt(), setsockopt()] 헷갈렸었음
- broadcast VS multicast
- multicast.c 코드 오류발생
  - note: ‘snprintf’ output between 2 and 1034 bytes into a destination of size 1024, snprintf(line, sizeof(line), "%s %s", name, message); : 단순 오류 컴파일은 성공
  - mreq 구조체 선언이안됨 : mreq 구조체 문제 찾는중
