#![no_main]
#![no_std]
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(dead_code)]
#![feature(core_intrinsics)]

use core::{ffi::c_char, intrinsics::unreachable};

use avrtos::arduino_hal;
use avrtos::sys::{serial_init_baud, serial_print, z_avrtos_init};

const boot: &'static str = "avrtos starting\n\x00";

#[arduino_hal::entry]
fn main() -> ! {
    unsafe {
        serial_init_baud(115200);
        serial_print(boot.as_ptr() as *const c_char);
    }

    loop {}
}
