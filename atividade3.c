#include <semaphore.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>

bool isTobaco = false;
bool isPaper = false;
bool isMatch = false;
pthread_mutex_t mutex;
sem_t agent_sem; 
sem_t tobaco_sem;
sem_t matches_sem;
sem_t paper_sem;
sem_t tobacco;
sem_t matches;
sem_t paper;


void *agent_a(){
    while (true){
        sem_wait(&agent_sem);
        sem_post(&tobaco_sem);
        sem_post(&paper_sem);
        printf("Sent tobaco and paper to table\n");
    }
}

void *agent_b(){
    while (true){
        sem_wait(&agent_sem);
        sem_post(&paper_sem);
        sem_post(&matches_sem);
        printf("Sent matches and paper to table\n");
    }
}
void *agent_c(){
    while (true){
        sem_wait(&agent_sem);
        sem_post(&matches_sem);
        sem_post(&tobaco_sem);
        printf("Sent matches and tobaco to table\n");
    }
}

void *tobacco_pusher(){
    while (true)
    {
        sem_wait(&tobaco_sem);
        pthread_mutex_lock(&mutex);
        if (isPaper){
            isPaper = false;
            sem_post(&matches);
        }
        else if (isMatch){
            isMatch = false;
            sem_post(&paper);
        }else {
            isTobaco = true;
        }
        pthread_mutex_unlock(&mutex);
    }
}

void *matches_pusher(){
    while (true)
    {
        sem_wait(&matches_sem);
        pthread_mutex_lock(&mutex);
        if (isTobaco){
            isTobaco = false;
            sem_post(&paper);
        }
        else if (isPaper){
            isPaper = false;
            sem_post(&tobacco);
        }else {
            isMatch = true;
        }
        pthread_mutex_unlock(&mutex);
    }
}


void *paper_pusher(){
    while (true)
    {
        sem_wait(&paper_sem);
        pthread_mutex_lock(&mutex);
        if (isTobaco){
            isTobaco = false;
            sem_post(&matches);
        }
        else if (isMatch){
            isMatch = false;
            sem_post(&tobacco);
        }else {
            isPaper = true;
        }
        pthread_mutex_unlock(&mutex);
    }
}

void *matches_thread(){
    while (true)
    {
        sem_wait(&matches);
        printf("Matches made a cigar\n");
        sem_post(&agent_sem);
        sleep(rand() % 5 + 5);
    }
    
}

void *tobacco_thread(){
    while (true)
    {
        sem_wait(&tobacco);
        printf("Tobacco made a cigar\n");
        sem_post(&agent_sem);
        sleep(rand() % 5 + 5);
    }
    
}
void *paper_thread(){
    while (true)
    {
        sem_wait(&paper);
        printf("Paper made a cigar\n");
        sem_post(&agent_sem);
        sleep(rand() % 5 + 5);
    }
    
}

int main(){
    sem_init(&agent_sem, 0, 0);
    sem_init(&tobaco_sem, 0, 0);
    sem_init(&paper_sem, 0, 0);
    sem_init(&matches_sem, 0, 0);
    sem_init(&tobacco, 0, 0);
    sem_init(&paper, 0, 0);
    sem_init(&matches, 0, 0);
    pthread_mutex_init(&mutex,NULL);
    
    pthread_t threads[9];
    pthread_create(&threads[0],NULL,agent_a,0);
    pthread_create(&threads[1],NULL,agent_b,0);
    pthread_create(&threads[2],NULL,agent_c,0);
    pthread_create(&threads[3],NULL,matches_thread,0);
    pthread_create(&threads[4],NULL,tobacco_thread,0);
    pthread_create(&threads[5],NULL,paper_thread,0);
    pthread_create(&threads[6],NULL,matches_pusher,0);
    pthread_create(&threads[7],NULL,paper_pusher,0);
    pthread_create(&threads[8],NULL,tobacco_pusher,0);
    sleep(2);
    
    sem_post(&agent_sem);
    pthread_join(threads[0], NULL);
}
