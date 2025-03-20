#![no_std]
#![no_main]
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(dead_code)]

use core::ffi::c_char;

use avrtos::duration::Duration;
use avrtos::kernel::{sleep, Kernel};
use avrtos::sys::{led_init, led_toggle, serial_hex16, serial_print};
use avrtos::{arduino_hal, println};

const boot: &'static str = "avrtos starting\n\x00";
const msg: &'static str = "\n\x00";

#[arduino_hal::entry]
fn main() -> ! {
    let kernel = Kernel::init().unwrap();
    unsafe {
        serial_print(boot.as_ptr() as *const c_char);
        led_init();
    }

    let mut counter = 0;

    loop {
        counter += 1;

        unsafe {
            led_toggle();
            serial_hex16(counter);
            serial_print(msg.as_ptr() as *const c_char);
        }

        sleep(Duration::from_secs(1));
    }

    drop(kernel);
    loop {}
}
