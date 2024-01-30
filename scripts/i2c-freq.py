import pprint
import enum

class Prescaler(enum.IntEnum):
    I2C_PRESCALER_1 = 1
    I2C_PRESCALER_4 = 4
    I2C_PRESCALER_16 = 16
    I2C_PRESCALER_64 = 64

CPU_FREQ_DEFAULT = 16_000_000

def scl_freq(twbr: int, prescaler: int, cpu_freq: int = CPU_FREQ_DEFAULT):
    return cpu_freq / (16 + 2 * twbr * prescaler)

def twbr_calc(prescaler: int, scl_freq: int, cpu_freq: int = CPU_FREQ_DEFAULT):
    return (cpu_freq / (prescaler * scl_freq) - 16) / 2

def gen_couples(cpu_freq: int = CPU_FREQ_DEFAULT):
    couples = []
    for prescaler in Prescaler:
        for twbr in range(0, 256):
            freq = scl_freq(twbr, prescaler, cpu_freq)
            freq10 = round(freq, 0)
            if freq <= 400_000 and abs(freq - freq10) < 0.0000001:
                # if freq is already in the list, skip it
                if any([x[2] == freq for x in couples]):
                    continue
                couples.append((twbr, prescaler, int(freq)))
    couples.sort(key=lambda x: x[2])
    return couples

if __name__ == "__main__":


    couples = gen_couples(8_000_000)
    # for twbr, prescaler, freq in couples:
    #     print(f"#define I2C_CONF_{freq} I2C_CONFIG_INIT({prescaler.name}, 0x{twbr:02X}u)")
    # for twbr, prescaler, freq in couples:
    #     print(f"#define I2C_CONF_{freq} I2C_CONFIG_INIT({prescaler.name}, I2C_CALC_TWBR({prescaler.name}, {freq}))")
    for twbr, prescaler, freq in couples:
        print(f"#define I2C_CONF_{freq} I2C_CONFIG_INIT_FREQ({prescaler.name}, {freq})")
    # pprint.pprint(couples)

    # print(twbr_calc(1, 400_000))
    # print(twbr_calc(4, 400_000))
    # print(twbr_calc(16, 400_000))
    # print(twbr_calc(64, 400_000))