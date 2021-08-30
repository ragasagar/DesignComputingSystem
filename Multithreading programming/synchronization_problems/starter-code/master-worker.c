#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include <semaphore.h>

int item_to_produce, curr_buf_size, item_to_consume;
int total_items, max_buf_size, num_workers, num_masters;

int in = 0, out = 0;
int *buffer;
sem_t full, empty;
sem_t mutex;
void print_produced(int num, int master)
{
  printf("Produced %d by master %d\n", num, master);
}

void print_consumed(int num, int worker)
{

  printf("Consumed %d by worker %d\n", num, worker);
}

//produce items and place in buffer
//modify code below to synchronize correctly
void *generate_requests_loop(void *data)
{
  int thread_id = *((int *)data);

  while (1)
  {
    sem_wait(&empty);
    sem_wait(&mutex);
    if (item_to_produce >= total_items)
    {
      sem_post(&mutex);
      sem_post(&full);
      break;
    }
    buffer[in] = item_to_produce;
    in = (in + 1) % max_buf_size;
    print_produced(item_to_produce, thread_id);
    item_to_produce++;
    sem_post(&mutex);
    sem_post(&full);
  }
  return 0;
}

//write function to be run by worker threads
//ensure that the workers call the function print_consumed when they consume an item
void *consume_requests_loop(void *data)
{
  int thread_id = *((int *)data);

  while (1)
  {

    sem_wait(&full);
    sem_wait(&mutex);
    if (item_to_consume >= total_items)
    {
      sem_post(&mutex);
      sem_post(&empty);
      break;
    }
    print_consumed(buffer[out], thread_id);
    out = (out + 1) % max_buf_size;
    item_to_consume++;
    sem_post(&mutex);
    sem_post(&empty);
  }
}


int main(int argc, char *argv[])
{
  int *master_thread_id;
  int *worker_thread_id;
  pthread_t *master_thread;
  pthread_t *worker_thread;
  item_to_produce = 0;
  curr_buf_size = 0;

  int i;

  if (argc < 5)
  {
    printf("./master-worker #total_items #max_buf_size #num_workers #masters e.g. ./exe 10000 1000 4 3\n");
    exit(1);
  }
  else
  {
    num_masters = atoi(argv[4]);
    num_workers = atoi(argv[3]);
    total_items = atoi(argv[1]);
    max_buf_size = atoi(argv[2]);
  }

  //semaphore initialization
  sem_init(&empty, 0, max_buf_size);
  sem_init(&full, 0, 0);
  sem_init(&mutex, 0, 1);

  buffer = (int *)malloc(sizeof(int) * max_buf_size);
  item_to_consume = 0;

  //create master producer threads
  master_thread_id = (int *)malloc(sizeof(int) * num_masters);
  master_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_masters);
  for (i = 0; i < num_masters; i++)
    master_thread_id[i] = i;

  //create worker threads
  worker_thread_id = (int *)malloc(sizeof(int) * num_workers);
  worker_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_workers);
  for (i = 0; i < num_workers; i++)
    worker_thread_id[i] = i;

  for (i = 0; i < num_masters; i++)
    pthread_create(&master_thread[i], NULL, generate_requests_loop, (void *)&master_thread_id[i]);

  //create worker consumer threads
  for (i = 0; i < num_workers; i++)
    pthread_create(&worker_thread[i], NULL, consume_requests_loop, (void *)&worker_thread_id[i]);

  //wait for all threads to complete
  for (i = 0; i < num_masters; i++)
  {
    pthread_join(master_thread[i], NULL);
    printf("master %d joined\n", i);
  }

  for (i = 0; i < num_workers; i++)
  {
    pthread_join(worker_thread[i], NULL);
    printf("consumer %d joined\n", i);
  }

  /*----Deallocating Buffers---------------------*/
  sem_destroy(&empty);
  sem_destroy(&full);
  sem_destroy(&mutex);
  free(buffer);
  free(master_thread_id);
  free(master_thread);

  return 0;
}
