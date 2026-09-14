# Kernel Scheduler Statistics Analyzer

A Linux kernel module that collects and analyzes real-time scheduler statistics including CPU utilization, context switches, and task states. This project provides insights into kernel scheduler behavior through procfs interfaces and user-space analysis tools.

## The Module Currently Provides

• **Real-time scheduler statistics collection** - Captures CPU scheduling data and performance metrics
• **Context switch tracking** - Monitors context switches between tasks  
• **Per-CPU utilization analysis** - Tracks CPU usage patterns
• **procfs interface** - Exports scheduler data for user-space tools
• **Thread performance monitoring** - Analyzes individual thread scheduling behavior

## Topology / Data Flow

The module follows a layered architecture:

```
Kernel Scheduler Events
         ↓
  [ksched_module] - Hook into scheduler events
         ↓
 Kernel Memory - Store statistics
         ↓
 [/proc/ksched/] - Export data interface
         ↓
[scheduler_analyzer] - Read and display results
```

Data flows from the kernel through procfs to user-space tools for analysis and visualization.

## Processing Flow

1. Kernel hooks capture scheduler events (context switches, task state changes, CPU utilization)
2. Module collects and aggregates statistics in kernel memory
3. Data is organized by CPU core and task identifier
4. procfs interface exports the collected metrics for user-space access
5. User-space analyzer reads data, parses statistics, and displays formatted results
6. Raw counters and statistics can be reset through procfs write interface

## Repository Files

• `kernel/ksched_module.c` - Kernel module implementation with scheduler hooks, data collection, and procfs handlers
• `user/scheduler_analyzer.c` - User-space program to read and display collected statistics
• `test/thread_test.c` - Benchmark and test suite for scheduler analysis
• `include/ksched.h` - Shared header definitions between kernel module and user tools
• `Makefile` - Builds kernel module and user-space tools against the running kernel
• `.gitignore` - Excludes build artifacts and kernel object files

## Requirements

Run this project on Linux. The current environment is suitable for editing, but building and loading the module requires a Linux kernel environment such as a native Linux host or VM.

You need:

• A running Linux kernel with matching kernel headers
• `make` and a C compiler (gcc or clang)
• Root privileges or `sudo` access for module loading and inspection
• Kernel support for loadable modules, Kprobes hooks, and procfs
• Standard C libraries for user-space tool compilation

## Build and Run

### 1. Build the module and tools

```bash
make
```

This produces `kernel/ksched_module.ko` (the compiled kernel module) and `scheduler_analyzer` (user-space analysis tool).

### 2. Load the kernel module

```bash
sudo insmod kernel/ksched_module.ko
```

Verify the module is loaded:

```bash
lsmod | grep ksched
```

### 3. Run the scheduler analyzer

```bash
./scheduler_analyzer
```

The tool reads collected statistics from `/proc/ksched/` and displays analysis results.

### 4. View kernel statistics through procfs

```bash
cat /proc/ksched/stats      # Display scheduler statistics
cat /proc/ksched/tasks      # Show monitored tasks
```

### 5. Run the test suite

```bash
./test_threads
```

This executes benchmark tests to generate scheduler activity and verify statistics collection.

### 6. Inspect kernel messages

```bash
dmesg | tail -30
```

Kernel messages from the scheduler module appear here.

### 7. Unload the module

```bash
sudo rmmod ksched_module
```

Or use the provided Makefile target:

```bash
make unload
```

## Make Targets

```bash
make              # Build the kernel module and user-space tools
make clean        # Remove build artifacts and kernel object files
make unload       # Unload the kernel module (requires root)
```

## Limitations

• The module is designed for educational purposes and demonstrates kernel instrumentation concepts
• Statistics collection has a performance overhead; results should be interpreted accordingly
• The procfs interface is read-only for statistics; dynamic configuration is limited
• Module supports monitoring a configurable maximum number of tasks
• High-frequency scheduling events may result in some data loss at high loads
• Not intended for production environments

## About

This project explores Linux kernel scheduling internals through direct kernel instrumentation. It demonstrates Linux kernel module development, Kprobes hooks, procfs interface implementation, and real-time statistics collection in kernel space. Useful for kernel developers, system administrators, and anyone interested in understanding Linux scheduler behavior and kernel architecture.

## Future Enhancements

• Real-time graph plotting and visualization of metrics
• Historical data logging to disk
• Per-process CPU affinity tracking
• Integration with Linux tracing tools (ftrace, perf)
• Remote monitoring via network socket interface
• Enhanced filtering and aggregation capabilities

## Related Topics

[kernel-module](https://github.com/topics/kernel-module) • [linux-kernel](https://github.com/topics/linux-kernel) • [scheduler](https://github.com/topics/scheduler) • [statistics](https://github.com/topics/statistics) • [procfs](https://github.com/topics/procfs) • [performance-monitoring](https://github.com/topics/performance-monitoring) • [system-programming](https://github.com/topics/system-programming)
