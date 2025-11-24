# 3.1 프로세스의 이해

### 프로세스의 정의
- 프로세스란 실행중인 프로그램
- 사용자 모드 또는 커널 모드중 하나에서 실행
- 프로세스의 상태
  - 실행(running) 상태 : 프로세스가 CPU 서비스를 받을 수 있는 상태
  - 블록(waiting) 상태 : 입출력 처리나 어떤 조건을 기다리는 상태
  - 중단(stopped) 상태 : 프로세스 실행이 잠시 중단된 상태
  - 좀비(zombie) 상태 : 프로세스의 실행은 끝났으나 아직 종료 상태를 부모 프로세스에게 보내지 못하고 있는 상태
  - ps명령어 : 현재 실행중인 프로세스의 상태를 확인

### 프로세스의 메모리 배치
- 메모리 영역
  - 프로세스 이미지라고 부름 : 프로그램 실행에 필요한 어셈블러 코드와 각종 변수들이 저장
  - 어떤 프로세스가 사용하는 메모리 영역을 다른 프로세스가 접근할 수 없음

| 메모리 영역 | 메모리 내용 및 변수들 |
| ----------- | ---------------------- |
| 환경변수와 명령행 인자 영역 | *environ 내용 |
| 스택(stack) | argc, argv, auto_var, reg_var, auto_ptr |
| 힙(heap) | molloc()이 할당한 10 바이트 |
|데이터 영역 | uninit_global_var,static_var, init_global_var=3 |
| 코드 영역 | 어셈블리된 프로그램 코드 |
 
 - 환경 변수 : 프로세스 실행과 관련된 정보들
 - 코드 영역 : 프로그램의 어셈블리 코드가 저장되며 데이터 영역에는 초기화된 또는 초기화 되지 않은 데이터들이 저장
 - 스택 영역 : 현재 호출되어 실행중인 함수의 코드와 환경 정보들이 저장
 - 자동 변수 : 함수 내부에서 임수로 사용하는 변수
 - 힙 영역 : 함수가 리턴되어도 메모리가 사라지지 않는 영역, molloc() - free()

### 프로세스의 생성과 종료
- fork() : 새로운 프로세스를 만들기 위한 함수
  - 부모 프로세스, 자식 프로세스
  - 자식 프로세스의 fork()의 리턴값은 항상 0이고, 부모 프로세스의 fork()의 리턴 값은 자식 프로세스의 PID 값이 된다
  - fork() 실행 실패시 -1 리턴
- exec() : 프로세스가 하던 작업을 종료하고 다른 작업을 하도록 전환할 때 exec 계열의 함수를 사용
  - 현재 실행 중이던 프로세스의 이미지를 새로운 프로세스의 이미지로 교체
  - execl("/bin/ls", "ls", "test.txt",NULL);
    - int execl(const char *path, const char *arg, ...);
    - int execv(const char *path, char *const argv[]);
    - int execle(const char *path, const char *arg, ...,char * const envp[]);
    - int execve(const char *path, char * const argv[], char * const envp[]);
    - int execlp(const char *file, const char *arg, ...);
    - int execvp(const char *file, char * const argv[]);
    - l : list 형태의 인자를 취한다는 의미
    - v : vector 형태의 인자를 취한다는 의미
    - p : 명령을 PATH에서 검색하겠다는 의미
    - e : 환경변수 인자를 받는 함수임을 의미

  - 프로세스의 환경변수 : 현재 작업 디렉토리명과 같은 정보를 환경변수라고함
    - FILE * file =fopen("temp","w"); : 
    - char *getenv(const char * name); : 환경변수의 값을 알아오기 위한 함수, 하나의 환경변수 값만 얻어옴
    - char **environ을 사용하면 모든 환경변수를 출력 ex) extern char **environ;
  - 프로세스의 종류
    - exit() : 자신을 종료시키는 데 사용
    - flush : 버퍼(buffer)에 쌓여 있던 데이터를 바로 비워서 즉시 출력 및 전송하도록 강제로 밀어내는 동작
    - atexit(함수) 
# 🤔 3장에 대한 생각
- fork()
- exec()
  - 예제 코드에서 실행 결과가 책과 다르게 나았지만 그 이유는 OS 스케줄러에 따라 달라지기 때문에 그런것이다.
