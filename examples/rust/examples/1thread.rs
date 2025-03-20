#![no_std]
#![no_main]
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(dead_code)]

use avrtos::duration::Duration;
use avrtos::kernel::{sleep, yeet, Kernel};
use avrtos::thread::Priority;
use avrtos::{arduino_hal, println, thread};

#[arduino_hal::entry]
fn main() -> ! {
    let kernel = Kernel::init().unwrap();

    let _handle = thread::spawn(0x200, Priority::Cooperative, b'1', || loop {
        println!("loop");
        sleep(Duration::from_secs(1));
    })
    .unwrap();

    loop {
        yeet();
    }
}
