# Kernel Scheduler Statistics Analyzer

A Linux kernel module that collects and analyzes detailed scheduler statistics including task states, CPU utilization, context switches, and scheduling events. This project provides real-time insights into kernel scheduler behavior through procfs interfaces and user-space analysis tools.

The module currently provides:

• **Real-time scheduler statistics collection** - Gathers CPU scheduling data, task states, and performance metrics
• **Context switch tracking** - Monitors and counts context switches between tasks
• **Per-CPU utilization analysis** - Tracks CPU usage patterns and idle time
• **User-space data interface** - Exports scheduler data through procfs for user-space analysis
• **Thread performance monitoring** - Analyzes individual thread scheduling behavior
• **Customizable data collection** - Configurable statistics gathering with procfs control interface

## Architecture

The project consists of three main components:

1. **Kernel Module** (`kernel/ksched_module.c`) - Core scheduler data collection and hook management
2. **User-Space Analyzer** (`user/scheduler_analyzer.c`) - Reads and processes scheduler statistics
3. **Test Suite** (`test/thread_test.c`) - Benchmark tests for scheduler behavior analysis

### Data Flow

```
┌─────────────────────────────────────┐
│   Kernel Scheduler Events           │
└────────────┬────────────────────────┘
             │
             ▼
┌─────────────────────────────────────┐
│   ksched_module (Kernel Module)     │
│   - Hook into scheduler events      │
│   - Collect statistics              │
│   - Store metrics in-kernel         │
└────────────┬────────────────────────┘
             │
             ▼
┌─────────────────────────────────────┐
│   /proc/ksched/ (procfs Interface)  │
│   - Export collected data           │
│   - Provide read/write control      │
└────────────┬────────────────────────┘
             │
             ▼
┌─────────────────────────────────────┐
│   scheduler_analyzer (User Tool)    │
│   - Parse scheduler statistics      │
│   - Display formatted results       │
│   - Perform analysis                │
└─────────────────────────────────────┘
```

## Repository Files

- `kernel/ksched_module.c` - Main kernel module implementation with scheduler hooks, data collection, and procfs handlers
- `user/scheduler_analyzer.c` - User-space program to read and display collected statistics
- `test/thread_test.c` - Benchmark and stress-test suite for scheduler analysis
- `include/ksched.h` - Shared header definitions between kernel module and user-space tools
- `Makefile` - Builds both the kernel module and user-space tools against the running kernel
- `.gitignore` - Excludes build artifacts and kernel object files

## Requirements

Run this project on Linux. The module requires:

- A running Linux kernel with matching kernel headers
- `make` and a C compiler (gcc or clang)
- Root privileges or `sudo` access for module loading
- Kernel support for loadable modules, Netfilter/Kprobes hooks, and procfs
- For user-space tool: standard C libraries

## Build and Run

### 1. Build the module and user tools

```bash
make
```

This produces:
- `kernel/ksched_module.ko` - The compiled kernel module
- `scheduler_analyzer` - User-space analysis tool

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

### 4. Run the test suite

```bash
./test_threads
```

This executes benchmark tests to generate scheduler activity and verify statistics collection.

### 5. Inspect runtime statistics

View collected data through procfs:

```bash
cat /proc/ksched/stats      # Display scheduler statistics
cat /proc/ksched/tasks      # Show monitored tasks
sudo cat /proc/ksched/info  # View module information
```

### 6. Unload the module

```bash
sudo rmmod ksched_module
```

Or use the provided cleanup:

```bash
make clean
make unload
```

## Make Targets

```bash
make              # Build the kernel module and user-space tools
make clean        # Remove build artifacts and object files
make unload       # Unload the kernel module (requires root)
make help         # Display all available make targets
```

## Collected Metrics

The module collects the following statistics:

- **Task Count** - Total number of processes/threads being monitored
- **Context Switches** - Number of CPU context switches per measurement period
- **CPU Utilization** - Per-core CPU usage percentage
- **Task States** - Distribution of tasks in Running, Sleeping, Waiting states
- **Scheduling Latency** - Time between task wakeup and actual CPU assignment
- **Load Average** - System load metrics

## Procfs Interface

The module exports data through `/proc/ksched/`:

- `/proc/ksched/stats` - Raw scheduler statistics (readable)
- `/proc/ksched/tasks` - List of monitored tasks with their states (readable)
- `/proc/ksched/reset` - Reset counters (writable; any write clears stats)

Kernel messages can be viewed with:

```bash
dmesg | tail -30
```

## Limitations

- VLAN configuration is kernel-module-specific and cannot be changed at runtime
- Statistics collection has a performance overhead; results should be interpreted accordingly
- The procfs interface reads only; dynamic configuration is limited
- Module supports monitoring up to a configurable maximum number of tasks
- High-frequency data collection may miss some rapid scheduling events
- This is an educational kernel module, not intended for production use

## Testing

The test suite (`test/thread_test.c`) provides:

- Thread spawn and termination benchmarks
- Context switch stress testing
- CPU affinity testing
- Scheduling latency measurement
- Load generation for statistical validation

Run tests with:

```bash
sudo ./test_threads --verbose
sudo ./test_threads --iterations 1000
```

## Troubleshooting

**Module fails to load:**
```bash
dmesg | tail -20  # Check kernel error messages
sudo dmesg | grep -i ksched
```

**No data in procfs:**
- Verify module is loaded: `lsmod | grep ksched`
- Check permissions: `ls -la /proc/ksched/`
- Ensure test workload is running to generate scheduler events

**Permission denied errors:**
- User-space tool must read procfs: `sudo ./scheduler_analyzer`
- Module loading requires root: `sudo insmod`

## About

This project explores kernel scheduling internals through direct kernel instrumentation. It demonstrates:

- Linux kernel module development
- Kprobes and tracepoint hooks
- procfs interface implementation
- Kernel data structures and scheduling algorithms
- Real-time statistics collection in kernel space

Useful for kernel developers, system administrators, and anyone interested in understanding Linux scheduler behavior.

## Future Enhancements

- Real-time graph plotting of metrics
- Historical data logging to file
- Remote monitoring via network interface
- Per-process CPU affinity tracking
- Integration with Linux tracing tools (ftrace, perf)

## License

This project is provided as-is for educational and research purposes.

## Related Topics

[kernel-module](https://github.com/topics/kernel-module) • [linux-kernel](https://github.com/topics/linux-kernel) • [scheduler](https://github.com/topics/scheduler) • [statistics](https://github.com/topics/statistics) • [procfs](https://github.com/topics/procfs) • [performance-monitoring](https://github.com/topics/performance-monitoring) • [system-programming](https://github.com/topics/system-programming)
