all: eventScheduler

eventScheduler: eventScheduler.c MinHeap.c MinHeap.h
	gcc -o eventScheduler eventScheduler.c MinHeap.c -lpthread -lrt

clean:
	rm -rf rescheduler
