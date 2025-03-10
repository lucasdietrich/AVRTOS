#![no_std]
#![no_main]

use avrtos::kernel::{Kernel, KernelInitParams};
use avrtos::{arduino_hal, println};

#[arduino_hal::entry]
fn main() -> ! {
    let _kernel = Kernel::init(KernelInitParams::default()).unwrap();

    println!("Hello, world!");

    loop {}
}
