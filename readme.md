
# Todo

- IDLE state when there are no more thread to execute

- change 
```asm
    lds r30, k_thread           ; load current thread structure addr in X
    lds r31, k_thread + 1
```
to
```
    lds ZL, lo8(k_thread)       ; load current thread structure addr in X
    lds ZH, hi8(k_thread)
```

Save SREG in thread structure rather than in stack

when clearing/setting interrupt flag, set flag at it was before (storing it in the thread structure the time being : no need for stack)

test k_sched_lock/unlock

minimize IDLE thread stack -> try to remove the thread

create signal/semaphore

when remving event from tqueue, set .next to NULL 
having next = NULL mean not in tqueue (and not in runqueue)

renamed project to AVRTOS