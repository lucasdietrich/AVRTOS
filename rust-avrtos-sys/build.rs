use std::{env, path::PathBuf};

const AVR_GCC_DEFAULT: &str = "avr-gcc";
const AVR_GCC_FLTO: bool = false; // Depending on the version, the LTO is by default enabled or not

const LINKER_SCRIPT: &'static str = "architecture/avr/avrtos-avr6.xn";

const SOURCES: &[&str] = &[
    "../src/avrtos/arch/arch_atomic.S",
    "../src/avrtos/arch/arch_utils.S",
    "../src/avrtos/arch/debug_utils.S",
    "../src/avrtos/arch/init_sections.S",
    "../src/avrtos/arch/thread_switch_preempt.S",
    "../src/avrtos/devices/tcn75.c",
    "../src/avrtos/devices/mcp2515.c",
    "../src/avrtos/drivers/exti.c",
    "../src/avrtos/drivers/gpio.c",
    "../src/avrtos/drivers/i2c.c",
    "../src/avrtos/drivers/spi.c",
    "../src/avrtos/drivers/timer.c",
    "../src/avrtos/drivers/usart.c",
    "../src/avrtos/dstruct/debug.c",
    "../src/avrtos/dstruct/dlist.c",
    "../src/avrtos/dstruct/slist.c",
    "../src/avrtos/dstruct/tqueue.c",
    "../src/avrtos/dstruct/tdqueue.c",
    "../src/avrtos/alloc/default.c",
    "../src/avrtos/alloc/bump.c",
    "../src/avrtos/alloc/slab.c",
    "../src/avrtos/misc/led.c",
    "../src/avrtos/misc/serial.c",
    "../src/avrtos/assert.c",
    "../src/avrtos/atomic.c",
    "../src/avrtos/canaries.c",
    "../src/avrtos/debug.c",
    "../src/avrtos/event.c",
    "../src/avrtos/fault.c",
    "../src/avrtos/fifo.c",
    "../src/avrtos/flags.c",
    "../src/avrtos/idle.c",
    "../src/avrtos/init.c",
    "../src/avrtos/kernel.c",
    "../src/avrtos/mem_slab.c",
    "../src/avrtos/msgq.c",
    "../src/avrtos/mutex.c",
    "../src/avrtos/prng.c",
    "../src/avrtos/ring.c",
    "../src/avrtos/semaphore.c",
    "../src/avrtos/signal.c",
    "../src/avrtos/stack_sentinel.c",
    "../src/avrtos/stdout.c",
    "../src/avrtos/sysclock.c",
    "../src/avrtos/timer.c",
    "../src/avrtos/workqueue.c",
    "../src/avrtos/rust_helpers.c",
];

const HEADERS: &[&str] = &[
    "../src/avrtos/devices/tcn75.h",
    "../src/avrtos/devices/mcp2515.h",
    "../src/avrtos/devices/mcp2515_priv.h",
    "../src/avrtos/drivers.h",
    "../src/avrtos/drivers/exti.h",
    "../src/avrtos/drivers/gpio.h",
    "../src/avrtos/drivers/i2c.h",
    "../src/avrtos/drivers/i2c_defs.h",
    "../src/avrtos/drivers/spi.h",
    "../src/avrtos/drivers/timer.h",
    "../src/avrtos/drivers/timer_defs.h",
    "../src/avrtos/drivers/usart.h",
    "../src/avrtos/dstruct/debug.h",
    "../src/avrtos/dstruct/dlist.h",
    "../src/avrtos/dstruct/slist.h",
    "../src/avrtos/dstruct/tqueue.h",
    "../src/avrtos/dstruct/tdqueue.h",
    "../src/avrtos/misc/led.h",
    "../src/avrtos/misc/serial.h",
    "../src/avrtos/alloc/api.h",
    "../src/avrtos/alloc/alloc.h",
    "../src/avrtos/alloc/bump.h",
    "../src/avrtos/alloc/slab.h",
    "../src/avrtos/alloc/alloc_private.h",
    "../src/avrtos.h",
    "../src/avrtos/assert.h",
    "../src/avrtos/atomic.h",
    "../src/avrtos/avrtos.h",
    "../src/avrtos/avrtos_arduinoide_conf.h",
    "../src/avrtos/avrtos_conf.h",
    "../src/avrtos/canaries.h",
    "../src/avrtos/defines.h",
    "../src/avrtos/debug.h",
    "../src/avrtos/errno.h",
    "../src/avrtos/event.h",
    "../src/avrtos/fault.h",
    "../src/avrtos/fifo.h",
    "../src/avrtos/flags.h",
    "../src/avrtos/idle.h",
    "../src/avrtos/init.h",
    "../src/avrtos/kernel.h",
    "../src/avrtos/kernel_private.h",
    "../src/avrtos/logging.h",
    "../src/avrtos/mem_slab.h",
    "../src/avrtos/msgq.h",
    "../src/avrtos/mutex.h",
    "../src/avrtos/prng.h",
    "../src/avrtos/ring.h",
    "../src/avrtos/semaphore.h",
    "../src/avrtos/signal.h",
    "../src/avrtos/stack_sentinel.h",
    "../src/avrtos/io.h",
    "../src/avrtos/sys.h",
    "../src/avrtos/sysclock.h",
    "../src/avrtos/timer.h",
    "../src/avrtos/types.h",
    "../src/avrtos/workqueue.h",
    "../src/avrtos/rust_helpers.h",
];

const INCLUDES: &[&str] = &[
    "../src/",
    "../src/avrtos",
    "../src/avrtos/arch",
    "../src/avrtos/devices",
    "../src/avrtos/drivers",
    "../src/avrtos/dstruct",
    "../src/avrtos/misc",
];

const INCLUDE_WORLD_FILE: &str = "../src/avrtos_all.h";

pub struct CompileEnv {
    compiler_path: String,
}

fn compile_avrtos_sources(cenv: CompileEnv) {
    let mut build = cc::Build::new();

    let build = build
        .compiler(cenv.compiler_path)
        .files(SOURCES.iter())
        .includes(INCLUDES.iter())
        .static_flag(true)
        .no_default_flags(true)
        // mcu
        .flag("-mmcu=atmega2560")
        // standard flags
        .flag("-Wall")
        .flag("-fno-fat-lto-objects")
        .flag("-funsigned-char")
        .flag("-funsigned-bitfields")
        .flag("-fpack-struct")
        .flag("-fshort-enums")
        .flag("-fdata-sections")
        .flag("-ffunction-sections")
        .flag("-fno-split-wide-types")
        .flag("-fno-tree-scev-cprop")
        // .flag("-flto")
        .flag("-Wstrict-prototypes")
        .flag("-std=gnu11");

    if AVR_GCC_FLTO {
        build.flag("-flto");
    }

    // debug infos
    build
        // .flag("-gstrict-dwarf")
        // disable strict warnings
        .flag("-Wno-expansion-to-defined")
        .flag("-Wno-implicit-fallthrough")
        .flag("-Wno-sign-compare")
        .flag("-Wno-type-limits")
        // target config flags
        .flag("-DF_CPU=16000000UL")
        // Default rust config
        .flag("-DCONFIG_RUST=1");

    // build
    //     .flag("-Wl,--gc-sections")
    //     .flag("-Wl,-Map,map.out");

    #[cfg(feature = "qemu")]
    {
        build.flag("-D__QEMU__");
    }
    if cfg!(feature = "debug") {
        build.flag("-g");
        build.flag("-gdwarf-3");
    } else {
        build.flag("-Os");
    }

    build.ar_flag("v");

    build.compile("avrtos");

    for source in SOURCES {
        println!("cargo:rerun-if-changed={}", source);
    }

    for header in HEADERS {
        println!("cargo:rerun-if-changed={}", header);
    }
}

fn bindgen() {
    // Tell cargo to rerun the build script if the header changes
    println!("cargo:rerun-if-changed={}", INCLUDE_WORLD_FILE);

    let bindings = bindgen::Builder::default()
        .header(INCLUDE_WORLD_FILE)
        .layout_tests(false)
        .use_core()
        .formatter(bindgen::Formatter::Rustfmt)
        .default_enum_style(bindgen::EnumVariation::ModuleConsts)
        .size_t_is_usize(false)
        .clang_arg("-I/usr/avr/include")
        .clang_arg("-I../src")
        .clang_arg("--target=avr")
        .clang_arg("-mmcu=atmega2560")
        .clang_arg("-Os")
        .clang_arg("-D__AVR_3_BYTE_PC__")
        .clang_arg("-DF_CPU=16000000UL")
        .clang_arg("-DCONFIG_RUST=1");

    let allowlist_functions = [
        "k_.*",
        "z_.*",
        "dlist_.*",
        "slist.*",
        "tqueue_.*",
        "led_.*",
        "serial_.*",
        "atomic_.*",
        "__fault",
        "__assert",
        "print_slist",
        "print_dlist",
        "print_tqueue",
        "gpio_.*",
        "i2c_.*",
        "spi_.*",
        "ll_.*",
        "timer_.*",
        "timer8_.*",
        "timer16_.*",
        "usart_.*",
        "exti_.*",
        "tcn75_.*",
    ];
    let allowlist_types = [
        "EXTI_Ctrl_Device",
        "PCI_Ctrl_Device",
        "SPI_Device",
        "timer2_prescaler_t",
        "timer_prescaler_t",
        "timer16_interrupt_t",
        "z_callsaved_ctx",
        "z_callused_ctx",
        "z_intctx",
        "K_.*",
        "Z_.*",
    ];

    let bindings = allowlist_functions
        .iter()
        .fold(bindings, |b, func| b.allowlist_function(func));

    let bindings = allowlist_types
        .iter()
        .fold(bindings, |b, ty| b.allowlist_type(ty));

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .generate()
        .expect("Unable to generate bindings")
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");
}

fn main() {
    #[cfg(feature = "avrtos-linker-script")]
    {
        println!("cargo:rustc-link-arg=-T{}", LINKER_SCRIPT);
    }

    // add link args ???
    // println!("cargo:rustc-flag=-Wl,--gc-sections");
    // println!("cargo:rustc-flag=-Wl,-Map,map.out");
    // println!("link-arg=-Wl,--verbose");

    println!("cargo:rustc-link-lib=static=avrtos");

    // set AVRTOS_AVR_GCC_PATH if exists
    let compiler_path = match std::env::var("AVRTOS_AVR_GCC_PATH") {
        Ok(path) => path,
        Err(_) => AVR_GCC_DEFAULT.to_string(),
    };

    let cenv = CompileEnv { compiler_path };

    compile_avrtos_sources(cenv);

    bindgen();
}
