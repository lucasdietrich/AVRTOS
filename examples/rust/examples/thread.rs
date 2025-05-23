#![no_std]
#![no_main]

extern crate alloc;

use core::mem;

use avrtos::duration::Duration;
use avrtos::kernel::{sleep, yeet, Kernel, KernelParams};
use avrtos::thread::JoinHandle;
use avrtos::{arduino_hal, println, thread};

use alloc::{boxed::Box, vec::Vec};

// struct Context {
//     i: u32,
// }

#[arduino_hal::entry]
fn main() -> ! {
    let kernel = Box::new(Kernel::init_with_params(KernelParams::default()).unwrap());

    // let mut ctx = Context { i: 0 };
    let mut i = 0;

    let mut join_handle_1 = thread::spawn(620, thread::Priority::Cooperative, b'1', move || loop {
        i += 1;
        println!("Thread1: i: {}", i);
        sleep(Duration::from_secs(1));
    })
    .unwrap();

    let mut join_handle_2 = thread::spawn(620, thread::Priority::Preemptive, b'2', move || {
        let mut val: u64 = 0;

        loop {
            val += 1;

            if (val & 0xFFFFF) == 0 {
                println!("Thread2: val: {:x}", val);
            }
            yeet();
        }
    })
    .unwrap();

    let mut join_handle_3 = thread::spawn(620, thread::Priority::Cooperative, b'3', move || {
        println!("Thread 3: single operation then exit");
        sleep(Duration::from_secs(1));
    })
    .unwrap();

    println!("Threads started");
    println!(
        "sizeof join_handle_1 {} thread: {:?}",
        mem::size_of::<JoinHandle>(),
        join_handle_1.get_thread_ptr()
    );
    println!(
        "sizeof join_handle_2 {} thread: {:?}",
        mem::size_of::<JoinHandle>(),
        join_handle_2.get_thread_ptr()
    );
    println!(
        "sizeof join_handle_3 {} thread: {:?}",
        mem::size_of::<JoinHandle>(),
        join_handle_3.get_thread_ptr()
    );

    loop {
        join_handle_1.print_canaries();
        join_handle_2.print_canaries();
        join_handle_3.print_canaries();

        sleep(Duration::from_secs(1));
    }
}
