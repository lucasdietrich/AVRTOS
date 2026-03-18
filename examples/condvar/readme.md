make run_qemu 
"ninja" -C build run_qemu 
ninja: Entering directory `build'
[3/4] Linking C executable examples/condvar/sample_condvar
Memory region         Used Size  Region Size  %age Used
            text:        5576 B       256 KB      2.13%
            data:        1068 B         8 KB     13.04%
          eeprom:           0 B         4 KB      0.00%
            fuse:           0 B          3 B      0.00%
            lock:           0 B         1 KB      0.00%
       signature:           0 B         1 KB      0.00%
 user_signatures:           0 B         1 KB      0.00%
readelf: Warning: Section '.noinit' was not dumped because it does not exist
[3/4] cd /home/lucas/AVRTOS/build/examples/condvar && /usr/local/bin/qemu-s.../lucas/AVRTOS/build/examples/condvar/sample_condvar -nographic -icount auto
P: produced 1
B: consumed 1
P: produced 2
A: consumed 2
P: produced 3
B: consumed 3
P: produced 4
A: consumed 4
P: produced 5
B: consumed 5
P: produced 6
A: consumed 6