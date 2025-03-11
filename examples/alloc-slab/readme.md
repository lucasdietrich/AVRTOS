# Expected result:

```
[3/4] cd /home/lucas/AVRTOS/build/examples/alloc-slab && /usr/local/bin/qemu-system-avr -M mega2560 -bios /home/lucas/AVRTOS/build/examples/alloc-slab/sample_alloc_slab -nographic -icount auto
k_slab_init (buf: 0x310): 0
total: 256 used: 0 free: 256
k_slab_alloc(): 0x400
total: 256 used: 16 free: 240
k_slab_alloc(): 0x3f0
total: 256 used: 32 free: 224
k_slab_alloc(): 0x3e0
total: 256 used: 48 free: 208
k_slab_alloc(): 0x3d0
total: 256 used: 64 free: 192
k_slab_alloc(): 0x3c0
total: 256 used: 80 free: 176
k_slab_alloc(): 0x3b0
total: 256 used: 96 free: 160
k_slab_alloc(): 0x3a0
total: 256 used: 112 free: 144
k_slab_alloc(): 0x390
total: 256 used: 128 free: 128
k_slab_alloc(): 0x380
total: 256 used: 144 free: 112
k_slab_alloc(): 0x370
total: 256 used: 160 free: 96
k_slab_alloc(): 0x360
total: 256 used: 176 free: 80
k_slab_alloc(): 0x350
total: 256 used: 192 free: 64
k_slab_alloc(): 0x340
total: 256 used: 208 free: 48
k_slab_alloc(): 0x330
total: 256 used: 224 free: 32
k_slab_alloc(): 0x320
total: 256 used: 240 free: 16
k_slab_alloc(): 0x310
total: 256 used: 256 free: 0
k_slab_alloc(): 0
total: 256 used: 256 free: 0
k_slab_free(0x400)
total: 256 used: 240 free: 16
k_slab_free(0x3f0)
total: 256 used: 224 free: 32
k_slab_free(0x3e0)
total: 256 used: 208 free: 48
k_slab_free(0x3d0)
total: 256 used: 192 free: 64
k_slab_free(0x3c0)
total: 256 used: 176 free: 80
k_slab_free(0x3b0)
total: 256 used: 160 free: 96
k_slab_free(0x3a0)
total: 256 used: 144 free: 112
k_slab_free(0x390)
total: 256 used: 128 free: 128
k_slab_free(0x380)
total: 256 used: 112 free: 144
k_slab_free(0x370)
total: 256 used: 96 free: 160
k_slab_free(0x360)
total: 256 used: 80 free: 176
k_slab_free(0x350)
total: 256 used: 64 free: 192
k_slab_free(0x340)
total: 256 used: 48 free: 208
k_slab_free(0x330)
total: 256 used: 32 free: 224
k_slab_free(0x320)
total: 256 used: 16 free: 240
k_slab_free(0x310)
total: 256 used: 0 free: 256
QEMU: Terminated
```