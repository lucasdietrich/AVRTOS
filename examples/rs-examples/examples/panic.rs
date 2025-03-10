#![no_std]
#![no_main]
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(dead_code)]

use avrtos::arduino_hal;
use avrtos::kernel::Kernel;
use avrtos::kernel::KernelInitParams;
#[arduino_hal::entry]
fn main() -> ! {
    let kernel = Kernel::init(KernelInitParams {
        serial_baud: Some(115200),
    });

    panic!("This is a panic message");
}
