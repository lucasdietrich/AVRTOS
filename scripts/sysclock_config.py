from calculate_timer_tcnt import timers, Timer

for tim in timers:
    print(tim)
    for pres in tim.prescalers:
        min_us = tim.min_period_ns(pres) / 1000.0
        max_us = tim.max_period_us(pres)
        print(f"\tprescaler = {pres} period min = {min_us} us max = {max_us} us")

# select best prescaler
def gen_preproc_pres_selection_to_c(timer: Timer):
    c = ""

    first = True
    
    for prescaler in timer.prescalers:
        ifelif = "if" if first else "elif"

        max_us = timer.max_period_us(prescaler)
        

        c += f"#{ifelif} KERNEL_SYSCLOCK_PERIOD_US <= {max_us}\n"
        c += f"#\tdefine K_SYSCLOCK_TIMER_PRESCALER K_SYSCLOCK_TIMER_PRESCALER_{prescaler}\n"
        c += f"#\tdefine K_SYSCLOCK_TIMER_TCNT \\\n\t\t((1LLU + {timer.max_tcnt}LLU) - (((uint64_t)F_CPU)*KERNEL_SYSCLOCK_PERIOD_US)/({prescaler}000000LLU))\n"
        c += f"#\tdefine K_SYSCLOCK_TIMER_TCNTL ((uint8_t) K_SYSCLOCK_TIMER_TCNT)\n"
        c += f"#\tdefine K_SYSCLOCK_TIMER_TCNTH ((uint8_t) (K_SYSCLOCK_TIMER_TCNT >> 8))\n"

        first = False

    c += "#endif\n"

    print(c)


if __name__ == "__main__":
    print("\n"*100)
    gen_preproc_pres_selection_to_c(timers[0])
    print("\n"*5)
    gen_preproc_pres_selection_to_c(timers[1])
    print("\n"*5)
    gen_preproc_pres_selection_to_c(timers[2])