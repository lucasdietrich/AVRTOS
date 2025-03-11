# Expected result

```
lucas@zgw:~/AVRTOS$ make run_qemu 
"ninja" -C build run_qemu 
ninja: Entering directory `build'
[0/1] cd /home/lucas/AVRTOS/build/examples/alloc-global && /usr/local/bin/qemu-system-avr -M mega2560 -bios /home/lucas/AVRTOS/build/examples/alloc-global/sample_alloc_bump -nographic -icount auto
[ total: 511 used: 000 free: 511 ]      k_malloc(128): 0x336
[ total: 511 used: 128 free: 383 ]      k_malloc(128): 0x3b6
[ total: 511 used: 256 free: 255 ]      k_malloc(128): 0x436
[ total: 511 used: 384 free: 127 ]      k_malloc(128): 0
[ total: 511 used: 384 free: 127 ]      k_malloc(64): 0x4b6
[ total: 511 used: 448 free: 063 ]      k_malloc(64): 0
[ total: 511 used: 448 free: 063 ]      k_malloc(32): 0x4f6
[ total: 511 used: 480 free: 031 ]      k_malloc(32): 0
[ total: 511 used: 480 free: 031 ]      k_malloc(16): 0x516
[ total: 511 used: 496 free: 015 ]      k_malloc(16): 0
[ total: 511 used: 496 free: 015 ]      k_malloc(8): 0x526
[ total: 511 used: 504 free: 007 ]      k_malloc(8): 0
[ total: 511 used: 504 free: 007 ]      k_malloc(4): 0x52e
[ total: 511 used: 508 free: 003 ]      k_malloc(4): 0
[ total: 511 used: 508 free: 003 ]      k_malloc(2): 0x532
[ total: 511 used: 510 free: 001 ]      k_malloc(2): 0
[ total: 511 used: 510 free: 001 ]      k_malloc(1): 0x534
[ total: 511 used: 511 free: 000 ]      k_malloc(1): 0
[ total: 511 used: 511 free: 000 ]      k_malloc(0): 0
Total allocates: 511
[ total: 511 used: 000 free: 511 ]      Cleared
```