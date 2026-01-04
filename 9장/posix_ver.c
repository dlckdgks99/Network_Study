#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
    long version = sysconf(_SC_VERSION);
    printf("Posix version : %ld\n",version);
    if (version>=199506L){
        printf("이 시스템에서는 Posix 라이브러리 사용이 가능합니다");
    }
    else{
        printf("이 시스템에서는 posix1003.1c 스레드를 지원하지 않습니다");
    }
    return 0;
    
}