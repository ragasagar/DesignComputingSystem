#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>


void print_entered(int num)
{
  printf("Diner %d entered restaurant!\n", num);
}


void print_exited(int num)
{
  printf("Diner %d exited restaurant!\n", num);
}


void* diner_func(void* data)
{
    int thread_id = *((int *)data);

    //write entering logic here
    print_entered(thread_id);


    //write exiting logic here
    print_exited(thread_id);

    return 0;

}


  
int main(int argc, char *argv[])
{


      int *diner_thread_id;
      pthread_t *diner_thread;
            
      int i;
      
       if (argc < 3) {
        printf("./restaurant #total_diners #max_capacity e.g. ./exe 10 5\n");
        exit(1);
      }
      else {
        num_diners = atoi(argv[1]);
        max_capacity = atoi(argv[2]); 
      }

       diner_thread_id = (int *)malloc(sizeof(int) * num_diners);
       diner_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_diners);


      //write restaurant logic here
      

      

        return 0;
}