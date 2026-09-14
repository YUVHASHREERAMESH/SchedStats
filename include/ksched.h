#ifndef KSCHED_H
#define KSCHED_H

#include <linux/types.h>

#define KSCHED_DEVICE_NAME "ksched_stats"
#define KSCHED_MAX_TASKS   10
#define KSCHED_COMM_LEN    16

#define KSCHED_TYPE_PROCESS 0
#define KSCHED_TYPE_THREAD  1

struct ksched_task {
    __u32 tid;
    __u32 tgid;

    char name[KSCHED_COMM_LEN];

    __u8 type;
    __u8 reserved[3];

    __u64 cpu_time_ns;
    __u64 context_switches;
    __u64 voluntary_switches;
    __u64 involuntary_switches;
};

/*
 * User provides:
 *   buffer = address of userspace array
 *   capacity = number of entries available
 *
 * Kernel returns:
 *   count = number of entries written
 */
struct ksched_request {
    __u64 buffer;
    __u32 capacity;
    __u32 count;
};

#define KSCHED_IOCTL_MAGIC 'K'

#define KSCHED_GET_STATS \
    _IOWR(KSCHED_IOCTL_MAGIC, 1, struct ksched_request)

#endif
