def gen_registers(n: int = 32, instruction: str = "push", reverse: bool = False):
    if reverse:
        rangei = range(n - 1, -1, -1)
    else:
        rangei = range(n)

    for i in rangei:
        print(f"{instruction} r{i}")

if __name__ == "__main__":
    gen_registers(32, "push", False);
    gen_registers(32, "pop", True);