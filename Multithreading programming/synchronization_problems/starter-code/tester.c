#include <semaphore.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

sem_t mutex;
int counter = 0; 
  
void* thread(void* arg)
{
    //wait
    int i;
    printf("\nEntered..\n");
    
    for(i=0;i<1000; i++)
        counter++;
    printf("%d", counter);
    //signal
    printf("\nJust Exiting...\n");
}

int main(){
    sem_init(&mutex, 0, 1);
    pthread_t threads[10] ;
    int i;
    for(i=0;i<10;i++){
        sleep(1);
        pthread_create(&threads[i], NULL, thread, NULL);
    }

    sem_destroy(&mutex);
    printf("\n final print:%d", counter);
    return 0;
}