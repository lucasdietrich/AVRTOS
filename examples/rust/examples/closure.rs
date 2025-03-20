#![no_std]
#![no_main]

extern crate alloc;

use alloc::boxed::Box;
use alloc::vec::Vec;
use avrtos::kernel::{Kernel, KernelParams};
use avrtos::{arduino_hal, println};

#[arduino_hal::entry]
fn main() -> ! {
    let _kernel = Kernel::init_with_params(KernelParams::default()).unwrap();

    println!("Hello, world!");

    let mut z: u16 = 0;

    let mut closure = |x: u16| {
        z += x;
        z
    };

    for i in 0..10 {
        println!("{}", closure(i));
    }

    loop {}
}
