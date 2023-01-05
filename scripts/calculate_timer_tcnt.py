from typing import List

F_CPU = 16_000_000


class Timer:
    def __init__(self, n: int, prescalers: List[int], resolution: int = 8):
        self.n = n
        self.prescalers = prescalers

        assert resolution in [8, 16]
        self.resolution = resolution
        self.max_tcnt = (1 << resolution) - 1

    def valid_tcnt(self, tcnt: int):
        return 0 <= tcnt <= self.max_tcnt

    def frequencies(self):
	    return [(prescaler, self.frequency(0, prescaler)) for prescaler in self.prescalers]

    def periods(self):
        return [(prescaler, self.period_us(0, prescaler)) for prescaler in self.prescalers]

    def frequency(self, tcnt: int, prescaler: int, f_cpu: int = F_CPU) -> float:
        assert prescaler in self.prescalers
        assert 0 <= tcnt <= self.max_tcnt

        return f_cpu / (prescaler * (self.max_tcnt + 1 - tcnt))

    def max_period_of_all(self, f_cpu: int = F_CPU):
        return self.max_period_us(max(self.prescalers), f_cpu)
        
    def max_period_us(self, prescaler: int, f_cpu: int = F_CPU) -> int:
        return self.period_us(0, prescaler, f_cpu)
    
    def min_period_us(self, prescaler: int, f_cpu: int = F_CPU) -> int:
        return self.period_us(self.max_tcnt, prescaler, f_cpu)

    def min_period_ns(self, prescaler: int, f_cpu: int = F_CPU) -> int:
        assert prescaler in self.prescalers

        f_cpu_ghz = f_cpu / 1_000_000_000

        return int((prescaler) / f_cpu_ghz)

    def period_us(self, tcnt: int, prescaler: int, f_cpu: int = F_CPU) -> int:
        assert prescaler in self.prescalers
        assert 0 <= tcnt <= self.max_tcnt

        f_cpu_mhz = f_cpu // 1_000_000

        return int((prescaler * (self.max_tcnt + 1 - tcnt)) / f_cpu_mhz)

    def tcnt_from_freq(self, f_target: float, prescaler: int = 256, f_cpu: int = F_CPU):
        tcnt = (1 + self.max_tcnt) - f_cpu / (f_target * prescaler)

        if tcnt < 0:
            raise Exception()(
                f"TCNT {tcnt} > {self.max_tcnt} MAX TCNT => Need to increase prescaler")

        return tcnt

    def tcnt_from_period(self, period_us: int, prescaler: int = 256, f_cpu: int = F_CPU):
        tcnt = (1 + self.max_tcnt) - f_cpu * period_us / (prescaler * 1_000_000)

        if self.valid_tcnt(tcnt):
            return tcnt
        else:
            raise Exception(f"Invalid TCNT : {tcnt}")

    def int_to_tcnt(self, tcnt: int):
        assert tcnt <= self.max_tcnt

        return self.max_tcnt - tcnt

    def best_tcnt(self, f_target: float, f_cpu: int = F_CPU):
        return self.best_tcnt_from_freq(f_target, f_cpu)

    def best_tcnt_from_freq(self, f_target: float, f_cpu: int = F_CPU):
        for prescaler in sorted(self.prescalers):
            try:
                t = self.tcnt_from_freq(f_target, prescaler, f_cpu)
                return (prescaler, t)
            except Exception as e:
                continue
        return (0, 0)

    def best_tcnt_from_period(self, period_us: int, f_cpu: int = F_CPU):
        for prescaler in sorted(self.prescalers):
            try:
                t = self.tcnt_from_period(period_us, prescaler, f_cpu)
                return (prescaler, t)
            except Exception as e:
                continue
        return (0, 0)

    def __repr__(self):
        return f"timer{self.n} : {self.resolution:2} bits, prescalers = " + ", ".join(str(prescaler) for prescaler in self.prescalers)

    def get_special_periods(self, f_cpu: int = F_CPU):
        l = [1] + [self.max_period_of_all(f_cpu)]
        for pres in self.prescalers:
            l.append(self.max_period_us(pres, f_cpu))
        return sorted(l)



timers = [
    Timer(0, [1, 8, 64, 256, 1024], 8),
    Timer(1, [1, 8, 64, 256, 1024], 16),
    Timer(2, [1, 8, 32, 64, 128, 256, 1024], 8)
]

if __name__ == "__main__":

    # for timer in timers:
    #     print(timer)

    def defines_for(timer: Timer, periods_us_list):
        ret = ""
        first = True
        for period_us in periods_us_list:

            p, t = timer.best_tcnt_from_period(period_us, F_CPU)

            period_us = timer.period_us(t, p)

            t = int(round(t, 0))

            if t == 0 and p == 0:
                raise Exception(
                    f"Cannot have this periods = {period_us} us with {timer} !")

            ifelif = "if" if first else "elif"

            ret += f"#{ifelif} CONFIG_KERNEL_SYSCLOCK_PERIOD_US == {period_us}\n"
            ret += f"#\tdefine K_SYSCLOCK_TIMER_PRESCALER    K_SYSCLOCK_TIMER_PRESCALER_{p}\n"
            
            ret += f"#\tdefine K_SYSCLOCK_TIMER_TCNT         {t}\n"
            ret += f"#\tdefine K_SYSCLOCK_TIMER_TCNTL        {hex(t & 0xFF)}\n"
            ret += f"#\tdefine K_SYSCLOCK_TIMER_TCNTH        {hex(t >> 8)}\n"
            ret += f"#\tdefine K_SYSCLOCK_TCNT_ISNULL        {'1' if t == 0 else '0'}\n"

            first = False

        ret += "#endif\n"

        return ret


if __name__ == "__main__":

    for tim in timers:
        print(tim.max_period_of_all())

    t0r = sorted(set(timers[0].get_special_periods() + list(range(100, 16384, 100))))
    t1r = sorted(set(timers[1].get_special_periods() + list(range(100, 10000, 100)) + list(range(10000, 100000, 1000)) + list(range(100000, 4194304, 10000))))
    t2r = sorted(set(timers[2].get_special_periods() + list(range(100, 16384, 100))))

    ret0 = defines_for(timers[0], t0r)
    with open("./tmp/t0.txt", "w+") as fp:
        fp.write(ret0)

    ret1 = defines_for(timers[1], t1r)
    with open("./tmp/t1.txt", "w+") as fp:
        fp.write(ret1)

    ret2 = defines_for(timers[2], t2r)
    with open("./tmp/t2.txt", "w+") as fp:
        fp.write(ret2)
