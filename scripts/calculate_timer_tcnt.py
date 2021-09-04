from typing import List

F_CPU = 16_000_000

class Timer:
    def __init__(self, n: int, prescalers: List[int], resolution: int = 8):
        self.n = n
        self.prescalers = prescalers

        assert resolution in [8, 16]
        self.resolution = resolution
        self.max_tcnt = (1 << resolution) - 1

    def frequency(self, tcnt: int, prescaler: int, f_cpu: int = F_CPU) -> float:
        assert prescaler in self.prescalers
        assert 0 <= tcnt <= self.max_tcnt

        return f_cpu / (prescaler * (self.max_tcnt + 1 - tcnt))
    
    def period(self, tcnt: int, prescaler: int, f_cpu: int = F_CPU) -> float:
        return self.get_frequency(tcnt, prescaler, f_cpu)

    def tcnt(self, f_target: float, prescaler: int = 256, f_cpu: int = F_CPU):
        tcnt = (1 + self.max_tcnt) - f_cpu / (f_target * prescaler)

        if tcnt < 0:
            raise Exception()(f"TCNT {tcnt} > {self.max_tcnt} MAX TCNT => Need to increase prescaler")

        return tcnt

    def int_to(self, tcnt: int):
        assert tcnt <= self.max_tcnt

        return self.max_tcnt - tcnt

    def best_tcnt(self, f_target: float, f_cpu: int = F_CPU):
        for prescaler in sorted(self.prescalers):
            try:
                t = self.tcnt(f_target, prescaler, f_cpu)
                return (prescaler, t)
            except Exception as e:
                continue
        return (0, 0)

    def __repr__(self):
        return f"timer{self.n} : {self.resolution:2} bits, prescalers = " + ", ".join(str(prescaler) for prescaler in self.prescalers)

if __name__ == "__main__":
    timers = [
        Timer(0, [1, 8, 64, 256, 1024], 8),
        Timer(1, [1, 8, 64, 256, 1024], 16),
        Timer(2, [1, 8, 32, 64, 128, 256, 1024], 8)
    ]

    # for timer in timers:
    #     print(timer)

    def defines_for(timer: Timer, values):
        ret = ""
        first = True
        for period in values:
            f = 1 / (period / 1000.0)

            p, t = timer.best_tcnt(f, F_CPU)

            t = int(round(t, 0))

            if t == 0 and p == 0:
                raise Exception(f"Cannot have this frequency = {f:.3f} (period = {1/f:.6f} s) with {timer} !")

            # print(f"period = {period*100} µs frequency = {f:.3f} Hz : prescaler = {p} tcnt = {t} calculated frequency = {timers[0].frequency(t, p)}")

            if (first):
                ret += f"#if KERNEL_TIME_SLICE == {period}\n"
                first = False
            else:
                ret += f"#elif KERNEL_TIME_SLICE == {period}\n"

            ret += f"#   define SYSCLOCK_TIMER_INTTO        {timer.int_to(t)}\n"
            ret += f"#   define SYSCLOCK_TIMER_TCNT         {t}\n"
            ret += f"#   define SYSCLOCK_TIMER_TCNTL        {hex(t & 0xFF)}\n"
            ret += f"#   define SYSCLOCK_TIMER_TCNTH        {hex(t >> 8)}\n"
            ret += f"#   define SYSCLOCK_TIMER_FREQ         {f}\n"
            ret += f"#   define SYSCLOCK_TIMER_PRESCALER    SYSCLOCK_TIMER_PRESCALER_{p}\n"
        
        ret += "#endif\n"

        return ret


if __name__ == "__main__":
    
    ret0 = defines_for(timers[0], range(1, 16 + 1))
    with open("./tmp/t0.txt", "w+") as fp:
        fp.write(ret0)

    ret1 = defines_for(timers[1], list(range(1, 16 + 1)) + [20, 25, 50, 100, 200, 250, 500, 1000, 2000, 2500, 3000, 4000])
    with open("./tmp/t1.txt", "w+") as fp:
        fp.write(ret1)

    ret2 = defines_for(timers[2], range(1, 16 + 1))
    with open("./tmp/t2.txt", "w+") as fp:
        fp.write(ret2)