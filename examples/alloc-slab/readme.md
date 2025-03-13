# Expected result:

```
[3/4] cd /home/lucas/AVRTOS/build/examples/alloc-slab && /usr/local/bin/qemu-system-avr -M mega2560 -bios /home/lucas/AVRTOS/build/examples/alloc-slab/sample_alloc_slab -nographic -icount auto
slab_init (buf: 0x310): 0
total: 256 used: 0 free: 256
slab_alloc(): 0x400
total: 256 used: 16 free: 240
slab_alloc(): 0x3f0
total: 256 used: 32 free: 224
slab_alloc(): 0x3e0
total: 256 used: 48 free: 208
slab_alloc(): 0x3d0
total: 256 used: 64 free: 192
slab_alloc(): 0x3c0
total: 256 used: 80 free: 176
slab_alloc(): 0x3b0
total: 256 used: 96 free: 160
slab_alloc(): 0x3a0
total: 256 used: 112 free: 144
slab_alloc(): 0x390
total: 256 used: 128 free: 128
slab_alloc(): 0x380
total: 256 used: 144 free: 112
slab_alloc(): 0x370
total: 256 used: 160 free: 96
slab_alloc(): 0x360
total: 256 used: 176 free: 80
slab_alloc(): 0x350
total: 256 used: 192 free: 64
slab_alloc(): 0x340
total: 256 used: 208 free: 48
slab_alloc(): 0x330
total: 256 used: 224 free: 32
slab_alloc(): 0x320
total: 256 used: 240 free: 16
slab_alloc(): 0x310
total: 256 used: 256 free: 0
slab_alloc(): 0
total: 256 used: 256 free: 0
slab_free(0x400)
total: 256 used: 240 free: 16
slab_free(0x3f0)
total: 256 used: 224 free: 32
slab_free(0x3e0)
total: 256 used: 208 free: 48
slab_free(0x3d0)
total: 256 used: 192 free: 64
slab_free(0x3c0)
total: 256 used: 176 free: 80
slab_free(0x3b0)
total: 256 used: 160 free: 96
slab_free(0x3a0)
total: 256 used: 144 free: 112
slab_free(0x390)
total: 256 used: 128 free: 128
slab_free(0x380)
total: 256 used: 112 free: 144
slab_free(0x370)
total: 256 used: 96 free: 160
slab_free(0x360)
total: 256 used: 80 free: 176
slab_free(0x350)
total: 256 used: 64 free: 192
slab_free(0x340)
total: 256 used: 48 free: 208
slab_free(0x330)
total: 256 used: 32 free: 224
slab_free(0x320)
total: 256 used: 16 free: 240
slab_free(0x310)
total: 256 used: 0 free: 256
QEMU: Terminated
```