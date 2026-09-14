#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>


static void *worker(void *arg)
{
    long id = (long)arg;

    printf("Thread %ld started\n", id);

    volatile unsigned long long counter = 0;

    while (1) {

        counter++;

        /*
         * Prevent compiler from optimizing
         * everything away.
         */
        if (counter == 1000000000ULL) {

            printf("Thread %ld still running\n",
                   id);

            counter = 0;
        }
    }

    return NULL;
}


int main(void)
{
    pthread_t t1;
    pthread_t t2;
    pthread_t t3;


    printf("Test process started\n");

    printf("Creating 3 worker threads...\n");


    pthread_create(&t1,
                   NULL,
                   worker,
                   (void *)1);


    pthread_create(&t2,
                   NULL,
                   worker,
                   (void *)2);


    pthread_create(&t3,
                   NULL,
                   worker,
                   (void *)3);


    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);


    return 0;
}