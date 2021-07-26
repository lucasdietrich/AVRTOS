
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