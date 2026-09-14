#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>

#include "../include/ksched.h"


#define DEVICE "/dev/ksched_stats"


static void print_line(void)
{
    printf("-------------------------------------------------------------------------------\n");
}


static void print_header(void)
{
    printf("\n");
    printf("===============================================================================\n");
    printf("              KERNEL SCHEDULER STATISTICS ANALYZER\n");
    printf("===============================================================================\n");

    printf("%-8s %-18s %-10s %-12s %-12s %-12s\n",
           "TID",
           "TASK NAME",
           "TYPE",
           "CPU TIME",
           "CONTEXT SW",
           "TGID");

    print_line();
}


int main(void)
{
    int fd;

    struct ksched_task *tasks;

    struct ksched_request request;

    unsigned int i;

    unsigned long long total_context_switches = 0;

    unsigned int highest_cpu_index = 0;


    /*
     * Allocate space for task information.
     */
    tasks = calloc(KSCHED_MAX_TASKS,
                   sizeof(struct ksched_task));

    if (!tasks) {

        perror("calloc");
        return 1;
    }


    /*
     * Open kernel device.
     */
    fd = open(DEVICE, O_RDWR);

    if (fd < 0) {

        perror("open /dev/ksched_stats");

        printf("\n");
        printf("Make sure the kernel module is loaded.\n");

        free(tasks);

        return 1;
    }


    /*
     * Prepare ioctl request.
     */
    memset(&request, 0, sizeof(request));

    request.buffer =
        (unsigned long long)(uintptr_t)tasks;

    request.capacity = KSCHED_MAX_TASKS;


    /*
     * Ask kernel for scheduler statistics.
     */
    if (ioctl(fd,
              KSCHED_GET_STATS,
              &request) < 0) {

        perror("ioctl");

        close(fd);
        free(tasks);

        return 1;
    }


    printf("\n");

    printf("Number of tasks returned: %u\n",
           request.count);


    print_header();


    /*
     * Display every task.
     */
    for (i = 0; i < request.count; i++) {

        double cpu_ms;

        const char *type;


        cpu_ms =
            tasks[i].cpu_time_ns / 1000000.0;


        if (tasks[i].type == KSCHED_TYPE_PROCESS)
            type = "Process";
        else
            type = "Thread";


        printf("%-8u %-18s %-10s %-12.2f %-12llu %-12u\n",

               tasks[i].tid,

               tasks[i].name,

               type,

               cpu_ms,

               (unsigned long long)
                   tasks[i].context_switches,

               tasks[i].tgid);


        /*
         * Total context switches.
         */
        total_context_switches +=
            tasks[i].context_switches;


        /*
         * Find highest CPU-consuming task.
         */
        if (tasks[i].cpu_time_ns >
            tasks[highest_cpu_index].cpu_time_ns) {

            highest_cpu_index = i;
        }
    }


    print_line();


    /*
     * Summary.
     */
    printf("\n");

    printf("SUMMARY\n");

    printf("----------------------------------------\n");

    printf("Total Tasks          : %u\n",
           request.count);

    printf("Total Context Switches: %llu\n",
           total_context_switches);

    if (request.count > 0) {

        printf("Highest CPU Time     : %s\n",
               tasks[highest_cpu_index].name);

        printf("Highest CPU TID      : %u\n",
               tasks[highest_cpu_index].tid);
    }


    printf("\n");

    close(fd);

    free(tasks);

    return 0;
}