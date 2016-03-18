-include make.conf

LIBLITMUS ?= ../liblitmus

include ${LIBLITMUS}/inc/config.makefile

vpath %.c src/

CPPFLAGS += -Iinclude/ -Iinc/

ALL = mybase_task task_trace

LDLIBS += -lm

.PHONY: all clean

all: ${ALL}

clean:
	rm -f ${ALL} *.o *.d

obj-mybase_task:=mybase_task.o
mybase_task: ${obj-mybase_task} 

obj-task_trace:=task_trace.o tracing.o
task_trace: ${obj-task_trace} 

Exp001: mybase_task
	cp mybase_task /mnt/NFS_Share/ModeManagerBins/Exp001/Dom1
	cp mybase_task /mnt/NFS_Share/ModeManagerBins/Exp001/Dom2
	cp mybase_task /mnt/NFS_Share/ModeManagerBins/Exp001/Dom3

Exp002: task_trace
	cp task_trace /mnt/NFS_Share/ModeManagerBins/Exp002/Dom1
