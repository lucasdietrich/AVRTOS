# Expected output

```
Memory region         Used Size  Region Size  %age Used
            text:        5412 B       256 KB      2.06%
            data:         562 B         8 KB      6.86%
          eeprom:          0 GB         4 KB      0.00%
            fuse:          0 GB          3 B      0.00%
            lock:          0 GB         1 KB      0.00%
       signature:          0 GB         1 KB      0.00%
 user_signatures:          0 GB         1 KB      0.00%
[100%] Built target sample_idle
===== k_thread =====
M 0x0214 READY   P 1 ____ : SP 0/512:0x0431
Hello World, counter = 0 !
***** Kernel Assertion failed *****
  K_MODULE_KERNEL:210 L [K_ASSERT_LEASTONE_RUNNING]
```