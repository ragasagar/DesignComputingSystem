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

int count_tiger, need_female = 0, need_male = 0, need_matchmaker = 0;
int num_females, num_males, num_matchmakers, num_tigers, num_children;
int count = 0;
pthread_mutex_t mating_lock;
pthread_cond_t male_wait, female_wait, matchmaker_wait, male_exit, female_exit, match_exit, mating_start;
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
    printf("Mating begins with {%d} children to be born\n", num);
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
void tiger_stats()
{
    printf("Total tiger: %d, male: %d, female: %d, matchmaker: %d\n", count_tiger, num_males, num_females, num_matchmakers);
}
void *male(void *data)
{
    int thread_id = *((int *)data);
    pthread_mutex_lock(&mating_lock);
    pthread_cond_wait(&male_wait, &mating_lock);
    need_male = 1;
    count++;
    pthread_cond_signal(&mating_start);
    pthread_cond_wait(&male_exit, &mating_lock);
    need_male = 0;
    print_tiger_leave(thread_id);
    pthread_mutex_unlock(&mating_lock);
    return 0;
}
void *female(void *data)
{
    int thread_id = *((int *)data);
    pthread_mutex_lock(&mating_lock);
    pthread_cond_wait(&female_wait, &mating_lock);
    need_female = 1;
    count++;
    pthread_cond_signal(&mating_start);
    pthread_cond_wait(&female_exit, &mating_lock);
    need_female = 0;
    print_tiger_leave(thread_id);
    pthread_mutex_unlock(&mating_lock);
    return 0;
}
void *matchmaker(void *data)
{

    pthread_mutex_lock(&mating_lock);
    pthread_cond_wait(&matchmaker_wait, &mating_lock);
    need_matchmaker = 1;
    count++;
    pthread_cond_signal(&mating_start);
    pthread_cond_wait(&match_exit, &mating_lock);
    need_matchmaker = 0;
    pthread_mutex_unlock(&mating_lock);
    return 0;
}

void *mating_occur(void *data)
{
    pthread_t *children_thread;
    int *children_thread_id;
    int value;
    int i, children_to_born = 1;
    while (1)
    {
        if (count_tiger >= num_tigers)
        {
            // tiger_stats();
            break;
        }
        pthread_mutex_lock(&mating_lock);
        pthread_cond_signal(&male_wait);
        pthread_cond_signal(&female_wait);
        pthread_cond_signal(&matchmaker_wait);

        while (count < 3)
        {   if(num_females==0 || num_males == 0 || num_matchmakers==0)
                break;
            pthread_cond_wait(&mating_start, &mating_lock);
        }

        if (need_male == 0 || need_female == 0 || need_matchmaker == 0)
        {

            sleep(5);
            // tiger_stats();
            break;
        }

        children_to_born = (rand() % (num_children)) + 1;

        print_mating_begin(children_to_born);

        children_thread = (pthread_t *)malloc(sizeof(pthread_t) * children_to_born);
        children_thread_id = (int *)malloc(sizeof(int) * children_to_born);

        for (i = 0; i < children_to_born; i++)
        {

            count_tiger++;
            value = rand() % (3) + 0;
            print_born(value);
            switch (value)
            {

            case MALE:
                num_males++;
                children_thread_id[i] = MALE;
                pthread_create(&children_thread[i], NULL, male, (void *)&children_thread_id[i]);
                break;

            case FEMALE:
                num_females++;
                children_thread_id[i] = FEMALE;
                pthread_create(&children_thread[i], NULL, female, (void *)&children_thread_id[i]);
                break;

            case MATCHMAKER:
                num_matchmakers++;
                children_thread_id[i] = MATCHMAKER;
                pthread_create(&children_thread[i], NULL, matchmaker, (void *)&children_thread_id[i]);
                break;
            }
            if(count_tiger == num_tigers){
                break;
            }
        }

        count = 0;
        //exiting areana starts here
        num_females--;
        num_males--;
        num_matchmakers--;
        pthread_cond_signal(&male_exit);
        pthread_cond_signal(&female_exit);
        pthread_cond_signal(&match_exit);
        pthread_mutex_unlock(&mating_lock);
        sleep(1);
    }
}
int main(int argc, char *argv[])
{
    pthread_t mating_thread, *female_threads, *male_threads, *matchmaker_threads;
    int i, children_to_born = 1, male_flag = 0, female_flag = 1;
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
        count_tiger = num_matchmakers + num_males + num_males;
    }
    pthread_mutex_init(&mating_lock, NULL);
    pthread_cond_init(&male_wait, NULL);
    pthread_cond_init(&female_wait, NULL);
    pthread_cond_init(&matchmaker_wait, NULL);
    pthread_cond_init(&match_exit, NULL);
    pthread_cond_init(&male_wait, NULL);
    pthread_cond_init(&female_wait, NULL);
    pthread_cond_init(&mating_start, NULL);

    female_threads = (pthread_t *)malloc(sizeof(pthread_t) * num_females);
    male_threads = (pthread_t *)malloc(sizeof(pthread_t) * num_males);
    matchmaker_threads = (pthread_t *)malloc(sizeof(pthread_t) * num_matchmakers);

    for (i = 0; i < num_females; i++)
    {
        pthread_create(&female_threads[i], NULL, female, (void *)&female_flag);
    }
    for (i = 0; i < num_males; i++)
    {
        pthread_create(&male_threads[i], NULL, male, (void *)&male_flag);
    }
    for (i = 0; i < num_matchmakers; i++)
    {
        pthread_create(&matchmaker_threads[i], NULL, matchmaker, NULL);
    }
    pthread_create(&mating_thread, NULL, mating_occur, NULL);
    pthread_join(mating_thread, NULL);

    pthread_mutex_destroy(&mating_lock);
    pthread_cond_destroy(&male_wait);
    pthread_cond_destroy(&female_wait);
    pthread_cond_destroy(&matchmaker_wait);
    pthread_cond_destroy(&match_exit);
    pthread_cond_destroy(&male_wait);
    pthread_cond_destroy(&female_wait);
    pthread_cond_destroy(&mating_start);
    free(female_threads);
    free(male_threads);
    free(matchmaker_threads);
    return 0;
}
