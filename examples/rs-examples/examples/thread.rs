#![no_std]
#![no_main]

use avrtos::kernel::{Kernel, KernelInitParams};
use avrtos::thread::Priority;
use avrtos::{arduino_hal, println, thread};

#[arduino_hal::entry]
fn main() -> ! {
    let _kernel = Kernel::init(KernelInitParams::default()).unwrap();

    let mut i = 0;

    println!("Starting thread");

    let join_handle= thread::spawn(&mut i, Priority::Cooperative, 'r', move |i| {
        loop {
            *i += 1;
            println!("Hello, world!");
        }
    }).unwrap();

    println!("Thread started");

    loop {}
}
