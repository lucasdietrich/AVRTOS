#![no_std]
#![no_main]
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(dead_code)]

use avrtos::{arduino_hal, println};
use avrtos::duration::Duration;
use avrtos::kernel::{sleep, yeet, Kernel, KernelParams};

#[arduino_hal::entry]
fn main() -> ! {
    let kernel = Kernel::init_with_params(KernelParams::default()).unwrap();

    let mut timeout = 1000_u32;

    loop {
        let duration = Duration::from_millis(timeout);
        println!("Sleeping for {} ms ({} ticks)", duration.as_millis(), duration.as_ticks());
        sleep(duration);

        timeout += 1000;

        if timeout > 10000 {
            timeout = 1000;
        }
    }
}
