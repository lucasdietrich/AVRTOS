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

    let mut counter = 5;

    let handle = thread::spawn(0x300, Priority::Cooperative, b'1', move || loop {
        println!("loop: {}", counter);
        sleep(Duration::from_secs(1));

        if counter == 0 {
            break;
        }

        counter -= 1;
    })
    .unwrap();

    handle.join(Duration::FOREVER).unwrap();

    println!("done");

    loop {
        yeet();
    }
}
