# Event-Rescheduler
My second project for Operating Systems that practiced using semaphores and timers.

How to run: 
1. Make
2. ./eventScheduler
3. Put input in (my testplan.txt shows how a typical run goes).

Program Implementation: 
You probably don't feel like reading this so I am going to make it as short as possible. 

I made a struct to define the attributes of a event and basically used that to store in the minheap and be extracted and such. For the threads I did what Russell said. Create a main thread that spawns the scheduler and reader threads. The scheduler thread prints out the string and the readerThread takes in input. 

Reader Thread: takes in input, gets currentTime, updates attributes of event instance, then is added to the minHeap. After that I extract the most min element and retrieve the relativeSpan so I can reset the timer after first deactivating it. 

Scheduler Thread: waits using a semaphore until the timeout handler is invoked. After that I get the current time deactivate the timer and start extracting the min element from the minheap until it is empty or the events left are less than the current time. Otherwise update the relativeSpan to reset the timer.

The semaphores and signal handles are set up properly to perform actions in appropriate order in the threads. It is basically just Russells code from class. 

