#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>

void *thrfunc(void *arg);
void cancel_and_join(pthread_t tid);

int max_loop = 20000000;
int cnt = 0;
pthread_t curthd;

int main(int argc, char **argv){
    pthread_t tid;
    int status;
    struct timespec micro_sec = {0, 100000};

    printf("** PTHREAD_CANCEL_DISABLE\n");

    status = pthread_create(&tid, NULL, thrfunc, NULL);
    if(status != 0){
        printf("thread create error: %s\n", strerror(status));
        exit(1);
    }

    nanosleep(&micro_sec, NULL);
    cancel_and_join(tid);
    return 0;
}

void *thrfunc(void *arg){
    int status;
    curthd = pthread_self();

    status = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if(status != 0){
        printf("pthread_setcancelstate fail: %s\n", strerror(status));
        exit(1);
    }
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);
    for(cnt = 1; cnt < max_loop; cnt++){
        if(cnt % 50000 == 0){
            pthread_testcancel(); // DISABLE이면 여기서도 취소 안 됨
        }
    }
    return NULL;
}

void cancel_and_join(pthread_t tid){
    void *result;
    int status;

    status = pthread_cancel(tid);
    if(status != 0){
        printf("pthread_cancel error: %s\n", strerror(status));
        exit(1);
    }

    status = pthread_join(tid, &result);
    if(status != 0){
        printf("pthread_join error: %s\n", strerror(status));
        exit(1);
    }

    if(result == PTHREAD_CANCELED){
        printf("[Thread ID=%lu] thread is canceled\n", (unsigned long)curthd);
    } else {
        printf("[Thread ID=%lu] thread is not canceled\n", (unsigned long)curthd);
    }
    printf("총 %d번의 루프중 %d 번의 루프를 돌았음\n", max_loop, cnt);
}
