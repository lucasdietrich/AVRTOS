CPU_FREQ = 16_000_000


def scl_freq(twbr: int, prescaler: int):
    return CPU_FREQ / (16 + 2 * twbr * prescaler)

def twbr_calc(prescaler: int, scl_freq: int):
    return (CPU_FREQ / (prescaler * scl_freq) - 16) / 2


if __name__ == "__main__":
    for twbr in range(0, 256):
        for prescaler in [1, 4, 16, 64]:
            freq = scl_freq(twbr, prescaler)
            if freq <= 400_000:
                print(f"TWBR={twbr}, prescaler={prescaler}, freq={freq}")


    print(twbr_calc(1, 400_000))
    print(twbr_calc(4, 400_000))
    print(twbr_calc(16, 400_000))
    print(twbr_calc(64, 400_000))