//1초 간격으로 두 개의 스레드가 돌아가면서 count 출력

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

void *thrfunc(void *arg);
int counting=0;
phtread_mutex_t count_lock;
phtread_mutextattr_t mutext_attr;

int main(int argc, char **argv){
    phtread_t tid[2];
    int i,status;
    pthread_mutexattr_init(&mutex_attr);
}