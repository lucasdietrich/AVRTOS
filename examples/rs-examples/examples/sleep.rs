#![no_std]
#![no_main]
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(dead_code)]

use avrtos::duration::Duration;
use avrtos::kernel::{sleep, yeet, Kernel, KernelParams};
use avrtos::{arduino_hal, println};

const boot: &'static str = "avrtos starting\n\x00";
const msg: &'static str = "\n\x00";

#[arduino_hal::entry]
fn main() -> ! {
    let kernel = Kernel::init_with_params(KernelParams::default()).unwrap();

    loop {
        // Doing this is highly BROKEN and I don't know why !!!
        // let mut timeout = k_timeout_t { value: 255 };
        // println!("timeout: {:?}", timeout.value);

        sleep(Duration::from_secs(1));
        yeet();
    }

    drop(kernel);
    loop {}
}
