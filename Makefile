obj-m += kernel/ksched_module.o

KDIR := /home/yuvhashree/wsl-kernel
PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	gcc -Wall -Wextra -O2 \
		-I./include \
		user/scheduler_analyzer.c \
		-o scheduler_analyzer

test:
	gcc -Wall -Wextra -O2 -pthread \
		test/thread_test.c \
		-o thread_test

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f scheduler_analyzer
	rm -f thread_test
