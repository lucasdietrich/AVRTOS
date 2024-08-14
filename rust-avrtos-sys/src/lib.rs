#![no_std]
#![allow(non_upper_case_globals)]
#![allow(non_camel_case_types)]
#![allow(non_snake_case)]
#![allow(dead_code)]

mod bindings;

pub use bindings::*;

pub fn sleep(ms: u32) {
    let timeout = k_timeout_t { value: ms };
    unsafe { k_sleep(timeout)}
}

pub fn sleep_1s() {
    unsafe { k_sleep_1s() }
}

pub fn my_yield() {
    unsafe {
        let key = z_rust_irq_lock();
        z_yield();
        z_rust_irq_unlock(key);
    }
}
