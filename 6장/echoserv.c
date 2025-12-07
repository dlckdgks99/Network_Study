// Xinetd에 등록되어 에코 요청 서비스를 함
// 이 코드는 서버코드가 아닌 xinetd가 이 코드를 서버처럼 실행시켜주는 코드

#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <netinet/tcp.h>

int main(){
    int cnt=0;
    char line[1024];

    setvbuf(stdout,NULL,_IOLBF,0);
    while(fgets(line,sizeof(line),stdin)!=NULL){
        printf("%3i : %s",++cnt,line);
    }
    return 0;
}