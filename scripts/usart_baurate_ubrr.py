F_CPU = 16e6

synchronous = True
factor = 16 if not synchronous else 8

baudrates = [
        2400,
        4800,
        9600,
        14400,
        19200,
        28800,
        38400,
        57600,
        76800,
        115200,
        230400,
        250000,
        500000,
        1000000,
]

for baurate in baudrates:
        ubrr = int(F_CPU / factor / baurate - 1)
        print("{:>8} {:>8}".format(baurate, ubrr))