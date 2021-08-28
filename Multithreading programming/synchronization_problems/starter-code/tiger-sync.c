#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include <semaphore.h>

#define MALE 0
#define FEMALE 1
#define MATE 0
#define MATCHMAKER 1

int num_females, num_males, num_matchmakers, num_childern, num_tigers;
pthread_mutex_t male_mutex, female_mutex, matchmaker_mutex, mating_mutex;
pthread_cond_t mating_wait, mating_complete;
void print_born(int type, int gen)
{
    char *gender, type;
    switch (gen)
    {
    case MALE:
        gender = "male";
        break;
    case FEMALE:
        gender = "female";
        break;
    }
    switch (type)
    {
    case MATE:
        type = "matchmaker";
        break;
    case MATCHMAKER:
        type = "mate";
        break;
    }
    if (type == 0)
    {
        printf("Tiger with role: {%s} born", type);
    }
    else
    {
        printf("Tiger with role: {%s as %s} born", type, gender);
    }
}

void print_mating_begin(int num)
{
    printf("Mating begins with {%s} children to be born", num);
}

void print_tiger_leave(int gender)
{
    char *gender;
    switch (gender)
    {
    case MALE:
        gender = "male";
        break;
    case FEMALE:
        gender = "female";
        break;
    }
    printf("Tiger with gender: {%s} leaves arena\n", gender);
}

void *male(void *data)
{
    pthread_mutex_lock(&male_mutex);
    num_males++;
    print_born(0, 0);
    pthread_mutex_unlock(&male_mutex);
    return 0;
}
void *female(void *data)
{
    pthread_mutex_lock(&female_mutex);
    num_females++;
    print_born(0, 0);
    pthread_mutex_unlock(&male_mutex);
    return 0;
}
void *matchmaker(void *data)
{
    pthread_mutex_lock(&matchmaker_mutex);
    num_matchmakers++;
    print_born(0, 0);
    pthread_mutex_unlock(&matchmaker_mutex);
    return 0;
}
void mating_occur()
{
    int children_to_born, num_mate;
    int i;
    pthread_t *children_thread;
    while(1){
        children_to_born = 1;
        num_mate=0;

        pthread_mutex_lock(&mating_mutex);
        while(num_females == 0 || num_males == 0 || num_matchmakers == 0){
            pthread_cond_wait(&mating_wait, &mating_mutex);
        }

        mating_occur(num_childern);

        children_to_born = (rand() % (num_childern)) + 1;
        num_mate = (rand() % (children_to_born + 1)) + 1;
        children_thread = (pthread_t *)malloc(sizeof(pthread_t) * children_to_born);
        for(i = 0; i< num_mate ; i++){
            switch (rand()%2)
            {
            case 0:
                pthread_create(&children_thread[i], NULL, male, NULL);
                break;
            
            default:
                pthread_create(&children_thread[i], NULL, female, NULL);
                break;
            }
        }
        for(i=num_mate; i< children_to_born; i++){
            pthread_create(&children_thread[i], NULL, matchmaker, NULL);
        }


    }
}
int main(int argc, char *argv[])
{
    int num_females, num_males, num_matchmakers, num_childern, num_tigers;
    if (argc < 5)
    {
        printf("./tiger-sync #num_females #num_males #num_matchmakers #num_children #num_tigers e.g. ./exe 10 10 10 5 1000\n");
        exit(1);
    }
    else
    {
        num_females = atoi(argv[1]);
        num_males = atoi(argv[2]);
        num_matchmakers = atoi(argv[3]);
        num_childern = atoi(argv[4]);
        num_tigers = atoi(argv[5]);
    }
}
