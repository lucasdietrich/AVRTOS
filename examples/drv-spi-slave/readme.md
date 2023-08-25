# SPI Slave Configuration

For my experiment, I used an Arduino Mega 2560 as the slave.

## Performance Metrics

To view the metrics of the SPI Slave driver, activate the `MEASURE_STATS` macro.

The table below presents sample metrics at various send intervals from the
master. The significant number of errors at a shorter send interval is due to
the SPI slave skipping a few bytes because of the CPU load. Consequently,
collecting these metrics can adversely affect performance.

| Interval from Master | Performance Stats                 |
| -------------------- | --------------------------------- |
| 10                   | Data rate: 74.559 B/s, err: 1836  |
| 15                   | Data rate: 54.234 B/s, err: 50    |
| 20                   | Data rate: 42.571 B/s, err: 2     |
| 25                   | Data rate: 35.036 B/s, err: 0     |

With the metrics feature turned off, the data rate can easily surpass 100 kB/s.
