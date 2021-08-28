#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

int num_diners, max_capacity, in_count, out_count, customer_count=0;
pthread_mutex_t enter_mutex, exit_mutex;
pthread_cond_t in_cond, out_cond,rest_enter, rest_exit;
void print_entered(int num)
{
  printf("Diner %d entered restaurant!\n", num);
}

void print_exited(int num)
{
  printf("Diner %d exited restaurant!\n", num);
}

void *diner_func(void *data)
{
  int thread_id = *((int *)data);
  //write entering logic here
  pthread_mutex_lock(&enter_mutex);

  pthread_cond_wait(&in_cond, &enter_mutex);

  print_entered(thread_id);
  in_count++;
  customer_count++;
  if (in_count == max_capacity || customer_count == num_diners)
  {
    in_count = 0;
    pthread_cond_signal(&rest_enter);
  }

  pthread_mutex_unlock(&enter_mutex);

  //write exiting logic here
  pthread_mutex_lock(&exit_mutex);
  pthread_cond_wait(&out_cond, &exit_mutex);
  print_exited(thread_id);
  out_count++;
  if (out_count == max_capacity || customer_count == num_diners)
  {
    out_count = 0;
    pthread_cond_signal(&rest_exit);
  }
  pthread_mutex_unlock(&exit_mutex);

  return 0;
}

int main(int argc, char *argv[])
{

  int *diner_thread_id;
  pthread_t *diner_thread;

  int i, j;

  if (argc < 3)
  {
    printf("./restaurant #total_diners #max_capacity e.g. ./exe 10 5\n");
    exit(1);
  }
  else
  {
    num_diners = atoi(argv[1]);
    max_capacity = atoi(argv[2]);
  }

  diner_thread_id = (int *)malloc(sizeof(int) * num_diners);
  diner_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_diners);

  for (i = 0; i < num_diners; i++)
    diner_thread_id[i] = i;

  pthread_mutex_init(&enter_mutex, NULL);
  pthread_mutex_init(&exit_mutex, NULL);
  pthread_cond_init(&in_cond, NULL);
  pthread_cond_init(&out_cond, NULL);
  pthread_cond_init(&rest_exit, NULL);
  pthread_cond_init(&rest_enter, NULL);

  for (i = 0; i < num_diners; i++)
  {
    pthread_create(&diner_thread[i], NULL, diner_func, (void *)&diner_thread_id[i]);
  }

  //write restaurant logic here
  while (1)
  {
    if(customer_count == num_diners){
      break;
    }
    pthread_mutex_lock(&enter_mutex);
    if (in_count == 0)
    {
      for (j = 0; j < max_capacity; j++)
      {
        pthread_cond_signal(&in_cond);
      }
    }
    pthread_cond_wait(&rest_enter, &enter_mutex);

    sleep(1);

    pthread_mutex_unlock(&enter_mutex);
    pthread_mutex_lock(&exit_mutex);
    if (out_count == 0)
    {
      for (j = 0; j < max_capacity; j++)
      {
        pthread_cond_signal(&out_cond);
      }
    }
    pthread_cond_wait(&rest_exit, &exit_mutex);
    pthread_mutex_unlock(&exit_mutex);
    
    sleep(1);
    printf("\n");
  }

  pthread_mutex_destroy(&enter_mutex);
  pthread_mutex_destroy(&exit_mutex);
  pthread_cond_destroy(&in_cond);
  pthread_cond_destroy(&out_cond);
  pthread_cond_destroy(&rest_exit);
  pthread_cond_destroy(&rest_enter);
  free(diner_thread);
  free(diner_thread_id);
  return 0;
}