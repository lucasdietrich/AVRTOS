#![no_std]
#![no_main]
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(dead_code)]

use avrtos::duration::Duration;
use avrtos::kernel::{sleep, yeet, Kernel};
use avrtos::signal::Signal;
use avrtos::thread::Priority;
use avrtos::traits::SyncPrimitive;
use avrtos::{arduino_hal, println, thread};

#[arduino_hal::entry]
fn main() -> ! {
    let kernel = Kernel::init().unwrap();

    let signal = Signal::new().unwrap();

    match signal.release(0x77) {
        Ok(()) => println!("Signal released"),
        Err(err) => println!("Error releasing signal: {:?}", err),
    }

    match signal.acquire(Duration::from_secs(1)) {
        Ok(value) => println!("Signal acquired: {}", value),
        Err(err) => println!("Error acquiring signal: {:?}", err),
    }

    signal.reset();

    match signal.acquire(Duration::from_secs(1)) {
        Ok(value) => println!("Signal acquired: {}", value),
        Err(err) => println!("Error acquiring signal: {:?}", err),
    }

    // let _handle = thread::spawn(0x200, Priority::Cooperative, b'1', move || loop {

    // })
    // .unwrap();

    loop {
        yeet();
    }
}
