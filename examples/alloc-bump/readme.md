# Expected result:

```
lucas@zgw:~/AVRTOS$ make run_qemu 
"ninja" -C build run_qemu 
ninja: Entering directory `build'
[0/1] cd /home/lucas/AVRTOS/build/examples/alloc-bump && /usr/local/bin/qemu-system-avr -M mega2560 -bios /home/lucas/AVRTOS/build/examples/alloc-bump/sample_alloc_global -nographic -icount auto
bump_init (buf: 0x2fc): 0
bump_alloc: 0x2fc
bump_alloc: 0x30c
bump_alloc: 0x30e
bump_alloc: 0x31e
bump_alloc: 0x320
bump_alloc: 0x330
bump_alloc: 0x340
total: 256 used: 84 free: 172
bump_alloc: 0x350
bump_alloc: 0
```