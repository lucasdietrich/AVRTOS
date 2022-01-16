from calculate_timer_tcnt import Timer, F_CPU, timers

for timer in timers:
    print(timer)
    for f, p in zip(timer.frequencies(), timer.periods()):
        print(f"prescaler = {f[0]}  freq = {f[1]} Hz  period = {p[1]} us")

print("Timer 1 : prescaler = 256 , TCNT = 0 : freq = {0:.3f} Hz -> {1} us".format(
      timers[1].frequency(0, 256), timers[1].period_us(0, 256)))
