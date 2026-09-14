#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>

#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/sched/cputime.h>

#include <linux/rcupdate.h>
#include <linux/slab.h>

#include "../include/ksched.h"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kernel Scheduler Analyzer");
MODULE_DESCRIPTION("Kernel Scheduler Statistics Analyzer");
MODULE_VERSION("1.0");


/*
 * ---------------------------------------------------------
 * ioctl handler
 * ---------------------------------------------------------
 */
static long ksched_ioctl(struct file *file,
                         unsigned int cmd,
                         unsigned long arg)
{
    struct ksched_request request;
    struct ksched_task *kernel_buffer;

    struct task_struct *process;
    struct task_struct *thread;

    unsigned int count = 0;

    if (cmd != KSCHED_GET_STATS)
        return -ENOTTY;


    /*
     * Copy request from user space
     */
    if (copy_from_user(&request,
                       (void __user *)arg,
                       sizeof(request))) {

        return -EFAULT;
    }


    /*
     * Validate capacity
     */
    if (request.capacity == 0)
        return -EINVAL;

    if (request.capacity > KSCHED_MAX_TASKS)
        request.capacity = KSCHED_MAX_TASKS;


    /*
     * Allocate kernel buffer
     */
    kernel_buffer = kcalloc(request.capacity,
                            sizeof(struct ksched_task),
                            GFP_KERNEL);

    if (!kernel_buffer)
        return -ENOMEM;


    /*
     * -----------------------------------------------------
     * Traverse processes and threads
     * -----------------------------------------------------
     *
     * for_each_process()
     *      gets every process/thread-group leader
     *
     * for_each_thread()
     *      gets the threads belonging to that process
     */
    rcu_read_lock();

    for_each_process(process) {

        /*
         * Add the process itself.
         */
        if (count < request.capacity) {

            struct ksched_task *entry =
                &kernel_buffer[count];

            u64 user_time;
            u64 system_time;

            memset(entry, 0, sizeof(*entry));

            entry->tid = task_pid_nr(process);
            entry->tgid = task_tgid_nr(process);

            get_task_comm(entry->name, process);

            entry->type = KSCHED_TYPE_PROCESS;

            /*
             * Get adjusted user/system CPU time.
             */
            task_cputime_adjusted(process,
                                   &user_time,
                                   &system_time);

            entry->cpu_time_ns =
                user_time + system_time;

            /*
             * Context switches.
             */
            entry->voluntary_switches =
                process->nvcsw;

            entry->involuntary_switches =
                process->nivcsw;

            entry->context_switches =
                process->nvcsw +
                process->nivcsw;

            count++;
        }


        /*
         * Now inspect threads belonging
         * to this process.
         */
        for_each_thread(process, thread) {

            if (count >= request.capacity)
                break;

            {
                struct ksched_task *entry =
                    &kernel_buffer[count];

                u64 user_time;
                u64 system_time;

                memset(entry, 0, sizeof(*entry));

                entry->tid = task_pid_nr(thread);
                entry->tgid = task_tgid_nr(thread);

                get_task_comm(entry->name, thread);

                entry->type = KSCHED_TYPE_THREAD;

                task_cputime_adjusted(thread,
                                       &user_time,
                                       &system_time);

                entry->cpu_time_ns =
                    user_time + system_time;

                entry->voluntary_switches =
                    thread->nvcsw;

                entry->involuntary_switches =
                    thread->nivcsw;

                entry->context_switches =
                    thread->nvcsw +
                    thread->nivcsw;

                count++;
            }
        }

        if (count >= request.capacity)
            break;
    }

    rcu_read_unlock();


    /*
     * -----------------------------------------------------
     * Copy statistics to user-space buffer
     * -----------------------------------------------------
     */
    if (copy_to_user(
            (void __user *)(uintptr_t)request.buffer,
            kernel_buffer,
            count * sizeof(struct ksched_task))) {

        kfree(kernel_buffer);
        return -EFAULT;
    }


    /*
     * Tell user space how many entries were returned.
     */
    request.count = count;


    if (copy_to_user(
            (void __user *)arg,
            &request,
            sizeof(request))) {

        kfree(kernel_buffer);
        return -EFAULT;
    }


    kfree(kernel_buffer);

    return 0;
}


/*
 * ---------------------------------------------------------
 * File operations
 * ---------------------------------------------------------
 */
static const struct file_operations ksched_fops = {

    .owner = THIS_MODULE,

    .unlocked_ioctl = ksched_ioctl,

#ifdef CONFIG_COMPAT
    .compat_ioctl = ksched_ioctl,
#endif
};


/*
 * ---------------------------------------------------------
 * Misc device
 * ---------------------------------------------------------
 */
static struct miscdevice ksched_device = {

    .minor = MISC_DYNAMIC_MINOR,

    .name = KSCHED_DEVICE_NAME,

    .fops = &ksched_fops,

    .mode = 0666,
};


/*
 * ---------------------------------------------------------
 * Module initialization
 * ---------------------------------------------------------
 */
static int __init ksched_init(void)
{
    int result;

    printk(KERN_INFO
           "KSCHED: Loading Kernel Scheduler Analyzer\n");

    result = misc_register(&ksched_device);

    if (result != 0) {

        printk(KERN_ERR
               "KSCHED: Failed to register device\n");

        return result;
    }

    printk(KERN_INFO
           "KSCHED: Device /dev/%s created\n",
           KSCHED_DEVICE_NAME);

    return 0;
}


/*
 * ---------------------------------------------------------
 * Module cleanup
 * ---------------------------------------------------------
 */
static void __exit ksched_exit(void)
{
    misc_deregister(&ksched_device);

    printk(KERN_INFO
           "KSCHED: Kernel Scheduler Analyzer unloaded\n");
}


module_init(ksched_init);
module_exit(ksched_exit);