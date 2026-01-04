// 조건변수를 통해서 두 스레드간에 통신하는 예
//메인 스레드는 조건변수 대기,자식 스레드는 sleep_time초 후 조건변수 시그널을 보내서 메인을 깨dna
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>

typedef struct _complex{
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int value;
} thread_control_t;

thread_control_t data = {
    PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_COND_INITIALIZER,
    0
};

void thr_errquit(char *msg,int errcode){
    printf("%s : %s\n", msg, strerror(errcode));
    pthread_exit(NULL);
}

void *wait_thread(void *arg); //스레드로 실행될 함수 프로토타입(선언). pthread_create에 넘기기 위해 필요.
int sleep_time;

int main(int argc,char **argv){
    int status;
    pthread_t wait_thr;
    struct timespec timeout;

    if(argc != 2){
        printf("사용법 : cond 5 (자식 스레드 sleep 시간)\n");
        exit(1);
    }

    sleep_time = atoi(argv[1]);

    /* 자식 스레드 생성 */
    if((status = pthread_create(&wait_thr, NULL, wait_thread, NULL)) != 0){
        printf("pthread_create fail : %s\n", strerror(status));
        exit(1);
    }

    timeout.tv_sec = time(NULL) + 3;
    timeout.tv_nsec = 0;

    if((status = pthread_mutex_lock(&data.mutex)) != 0){
        printf("pthread_mutex_lock fail : %s\n", strerror(status));
        exit(1);
    }

    if(data.value == 0){
        status = pthread_cond_timedwait(&data.cond, &data.mutex, &timeout);
        if(status == ETIMEDOUT)
            printf("Condition wait timeout\n");
        else
            printf("Wait on condition...\n");
    }

    if(data.value == 1)
        printf("Condition was signaled.\n");

    if((status = pthread_mutex_unlock(&data.mutex)) != 0){
        printf("pthread_mutex_unlock fail : %s\n", strerror(status));
        exit(1);
    }

    if((status = pthread_join(wait_thr, NULL)) != 0){
        printf("pthread_join fail : %s\n", strerror(status));
        exit(1);
    }

    return 0;
}

void *wait_thread(void *arg){
    int status;

    sleep(sleep_time);

    if((status = pthread_mutex_lock(&data.mutex)) != 0)
        thr_errquit("pthread_mutex_lock failure", status);

    data.value = 1;

    if((status = pthread_cond_signal(&data.cond)) != 0)
        thr_errquit("pthread_cond_signal failure", status);

    if((status = pthread_mutex_unlock(&data.mutex)) != 0)
        thr_errquit("pthread_mutex_unlock failure", status);

    return NULL;
}
