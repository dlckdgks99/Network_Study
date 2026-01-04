// 스레드 동기화 문제 (경쟁 조건 )의 발생

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_THR 2
void *thrfunc(void *arg);
void prn_data(long who);
int who_run = -1;

int main(int argc, char **argv){
    pthread_t tid[MAX_THR];
    int i, status;
    for(i=0; i<MAX_THR; i++){
        if((status=pthread_create(&tid[i],NULL, &thrfunc, NULL))!=0){
            printf("thread crette error: %s\n",strerror(status));
            exit(0);
        }
    }
    pthread_join(tid[0],NULL);
    return 0;
}

void *thrfunc(void *arg){
    while(1){
        if(who_run==-1){
            prn_data(pthread_self());
        }
    }
    return NULL;
}

void prn_data(long me){
    who_run=me;
    if(who_run !=pthread_self()){
        printf("Error : %ld스레드 실행중 who_run=%d\n",me,who_run);
    }
    who_run=-1;
}