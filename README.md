# RTScheduler

## A simple real-time scheduler for embedded applications

What do I mean by real-time scheduler in this instance? When you schedule a task to run in 5ms, you know that it will run in exactly 5ms. This scheduler does not offer pre-emptive multi-tasking as you would expect from Unix, Windows or an RTOS, only one task will run at a time (therefore each task must complete as quickly as possible).

The scheduler relies on a real-time clock clounter for operation, typically this is provided by a timer interrupt (a common feature on microprocessors and microcontrollers). A timer interrupt that increments the clock counter every 1ms (1000 timer per second) is a good starting point.

The scheduler is written in C, is reliable long-term, and has a small footprint in terms of memory usage and code space. Tasks are defined by the user program as functions, each task function must be of the form:

    void task_name(PTASKPARM p);

The PTASKPARM type is defined as a void * so you must cast to/from the actual type of your parameter (e.g. a ptr to a structure). Each task must have a user defined unique ID that is passed to the scheduler API to identify the task.

Each task function should do what it needs to do as quickly as possible, it should never wait for something to happen, e.g. in a loop (use interrupts to signal events or define a task that runs repeatedly to check for a condition).

The scheduler API is quite simple, the sequence of events on start-up of your application should be:

1. Set up the real-time clock interrupt
2. Call initScheduler(int num_tasks) to initialise the task structures
3. Register your tasks using the registerTask() function
4. Schedule any tasks you want to run off the bat using the scheduleTask() function
5. The very last call you should make is to the schedule() function, the actual scheduling loop (it **never** returns)

## The API

### void initScheduler(int size);

Initialises the scheduler task structure. This must be called before any other scheduler API functions.

Parameters:

                int size                  number of task descriptors to allocate, if this is <= 0 or > MAX_TASKS then
                                          the number of task descriptors is set to MAX_TASKS (defined in scheduler.h).

### void registerTask(uint16_t taskID, void (* run)(PTASKPARM));

Registers a task function with the scheduler.

Parameters:     

                uint16_t    taskID         A user-defined unique identifier for the task
                void        (* run)        A pointer to the actual task function of the form
                                           void task_name(PTASKPARM p);
                                         
### void deregisterTask(uint16_t taskID);

De-registers a previously registered task with the scheduler, freeing up the task definition if it is no longer required. If the taskID is not registered with the scheduler, it does nothing.

Parameters:     

                uint16_t    taskID          The user-defined unique identifier for the task, 
                                            as supplied to registerTask()

### void scheduleTask(uint16_t taskID, timer_t time, PTASKPARM p)

Schedules the task to run in the specified **time** in the future. A task must be registered using registerTask() before it can be scheduled.

Parameters:     

                uint16_t    taskID          The user-defined unique identifier for the task, 
                                            as supplied to registerTask()
                timer_t     time            Delay before the task runs, this will be in ms, 
                                            if the real-time clock resolution is ms.
                PTASKPARM   p               A pointer to the task parameter to pass to the 
                                            task, can be NULL.

### void rescheduleTask(uint16_t taskID, PTASKPARM p)

Re-schedules the task to run again after the same delay as specified in the scheduleTask() call. Useful for calling at the end of the task itself to force it to run again (after the specified delay).

Parameters:     

                uint16_t    taskID          The user-defined unique identifier for the task, 
                                            as supplied to registerTask()
                PTASKPARM   p               A pointer to the task parameter to pass to the 
                                            task, can be NULL.

### void unscheduleTask(uint16_t taskID);

Un-schedules a task that has previously been scheduled, e.g. this will cancel the scheduled run.

Parameters:     

                uint16_t    taskID          The user-defined unique identifier for the task, 
                                            as supplied to registerTask()

### void schedule()

The main scheduler loop, this will loop forever waiting for tasks to be scheduled, it never returns. This must be the last function called from main().

## Example

I have included an example that runs on an Atmega microcontroller (as found on Arduino boards), I have tested on an Arduino Nano and Arduino Uno, both have the Atmega328p microcontroller (16Mhz, 2K RAM, 32K Flash ROM). The example registers one task that blinks the on-board LED every second on the Arduino (on for 50ms, off for 950ms).

### Building the example

I have included a makefile that uses the avr tool chain (avr-gcc and related tools) to compile and build the target .hex file. The target can be written to the Atmega device's Flash ROM using the avrdude tool.

## Have fun!
