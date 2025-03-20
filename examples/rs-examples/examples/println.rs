#![no_std]
#![no_main]

use avrtos::kernel::{Kernel, KernelParams};
use avrtos::{arduino_hal, println};

#[arduino_hal::entry]
fn main() -> ! {
    let _kernel = Kernel::init_with_params(KernelParams::default()).unwrap();

    println!("Hello, world!");

    loop {}
}
