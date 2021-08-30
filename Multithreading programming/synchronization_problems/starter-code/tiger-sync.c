#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#define MALE 0
#define FEMALE 1
#define MATCHMAKER 2

int num_females, num_males, num_matchmakers, num_children, num_tigers, count_tiger = 0;
int mating_begin;
pthread_mutex_t male_mutex, female_mutex, matchmaker_mutex, mating_mutex, born_mutex;
pthread_cond_t mating_wait;
void print_born(int tiger_type)
{
    char *gender, *type;
    switch (tiger_type)
    {
    case MALE:
        printf("Tiger with role: {mate as male} born\n");
        break;
    case FEMALE:
        printf("Tiger with role: {mate as female} born\n");
        break;
    case MATCHMAKER:
        printf("Tiger with role: {matchmaker} born\n");
    }
}

void print_mating_begin(int num)
{
    printf("\nMating begins with {%d} children to be born\n", num);
}

void print_tiger_leave(int gen)
{
    switch (gen)
    {
    case MALE:
        printf("Tiger with gender: {male} leaves arena\n");
        break;
    case FEMALE:
        printf("Tiger with gender: {female} leaves arena\n");
        break;
    }
}

void print_tiger_population()
{
    printf("Tiger population: male:%d, female: %d , matchmaker: %d, total_count:%d\n", num_males, num_females, num_matchmakers, count_tiger);
}

void *male(void *data)
{
    pthread_mutex_lock(&male_mutex);
    num_males++;
    print_born(0);
    pthread_cond_signal(&mating_wait);
    pthread_mutex_unlock(&male_mutex);
    return 0;
}
void *female(void *data)
{
    pthread_mutex_lock(&female_mutex);
    num_females++;
    print_born(1);
    pthread_cond_signal(&mating_wait);
    pthread_mutex_unlock(&female_mutex);
    return 0;
}
void *matchmaker(void *data)
{
    pthread_mutex_lock(&matchmaker_mutex);
    num_matchmakers++;
    print_born(2);
    pthread_cond_signal(&mating_wait);
    pthread_mutex_unlock(&matchmaker_mutex);
    return 0;
}
void mating_occur(int children_to_born)
{
    int i;
    pthread_t *children_thread;
    int *children_thread_id;
    mating_begin = 1;
    int value = 1;

    print_mating_begin(children_to_born);

    children_thread = (pthread_t *)malloc(sizeof(pthread_t) * children_to_born);
    children_thread_id = (int *)malloc(sizeof(int) * children_to_born);

    for (i = 0; i < children_to_born; i++)
    {

        count_tiger++;
        if (count_tiger == num_tigers)
        {
            break;
        }
        value = rand() % (3) + 0;

        switch (value)
        {
        case MALE:
            children_thread_id[i] = MALE;
            pthread_create(&children_thread[i], NULL, male, (void *)&children_thread_id[i]);
            break;

        case FEMALE:
            children_thread_id[i] = FEMALE;
            pthread_create(&children_thread[i], NULL, female, (void *)&children_thread_id[i]);
            break;
        case MATCHMAKER:
            children_thread_id[i] = MATCHMAKER;
            pthread_create(&children_thread[i], NULL, matchmaker, (void *)&children_thread_id[i]);
            break;
        }
    }

    for (i = 0; i < children_to_born; i++)
    {
        pthread_join(children_thread[i], NULL);
        if (children_thread_id[i] == 0 || children_thread_id[i] == 1)
        {
            print_tiger_leave(children_thread_id[i]);
        }
    }

    num_males--;
    num_females--;
    num_matchmakers--;
    free(children_thread_id);
    free(children_thread);
    mating_begin = 0;
}
int main(int argc, char *argv[])
{
    pthread_t *mating_thread;
    int i, children_to_born = 1;
    if (argc < 6)
    {
        printf("./tiger-sync #num_females #num_males #num_matchmakers #num_children #num_tigers e.g. ./exe 10 10 10 5 1000\n");
        exit(1);
    }
    else
    {
        num_females = atoi(argv[1]);
        num_males = atoi(argv[2]);
        num_matchmakers = atoi(argv[3]);
        num_children = atoi(argv[4]);
        num_tigers = atoi(argv[5]);
    }
    pthread_mutex_init(&male_mutex, NULL);
    pthread_mutex_init(&female_mutex, NULL);
    pthread_mutex_init(&matchmaker_mutex, NULL);
    pthread_mutex_init(&mating_mutex, NULL);
    pthread_cond_init(&mating_wait, NULL);
    pthread_mutex_init(&born_mutex, NULL);

    while (1)
    {

        pthread_mutex_lock(&mating_mutex);
        while ((num_females <= 0 || num_males <= 0 || num_matchmakers <= 0))
        {
            pthread_cond_wait(&mating_wait, &mating_mutex);
        }

        children_to_born = (rand() % (num_children)) + 1;
        mating_occur(children_to_born);
        pthread_mutex_unlock(&mating_mutex);
        print_tiger_population();
        sleep(1);
    }

    pthread_mutex_destroy(&male_mutex);
    pthread_mutex_destroy(&female_mutex);
    pthread_mutex_destroy(&matchmaker_mutex);
    pthread_mutex_destroy(&mating_mutex);
    pthread_cond_destroy(&mating_wait);
    pthread_mutex_destroy(&born_mutex);
    return 0;
}
