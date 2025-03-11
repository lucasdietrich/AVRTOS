#![no_std]
#![no_main]

use core::mem;

use avrtos::kernel::{Kernel, KernelInitParams};
use avrtos::sys::{k_sleep_1s, rust_sleep};
use avrtos::thread::JoinHandle;
use avrtos::{arduino_hal, println, thread};

struct Context {
    i: u32,
}

#[arduino_hal::entry]
fn main() -> ! {
    let kernel = Kernel::init(KernelInitParams::default()).unwrap();

    let mut ctx = Context {
        i: 0
    };

    let mut join_handle_1 = thread::spawn(512, thread::Priority::Cooperative, b'1', move || loop {
        ctx.i += 1;
        println!("i: {}", ctx.i);
        unsafe { k_sleep_1s() };
    })
    .unwrap();

    // TODO fix: two threads does not work at the same time today, dont know why ?
    // let mut join_handle_2 = thread::spawn(1024, thread::Priority::Preemptive, b'2', move || {
    //     let mut val: u64 = 0;

    //     loop {
    //         val += 1;

    //         if (val & 0xFFFFF) == 0 {
    //             println!("val: {:x}", val);
    //         }
    //         kernel.yeet();
    //     }
    // }).unwrap();

    println!("Threads started");
    println!("sizeof join_handle_1 {} thread: {:?}", mem::size_of::<JoinHandle>(), join_handle_1.as_raw_ptr());
    // println!("sizeof join_handle_2 {} thread: {:?}", mem::size_of::<JoinHandle>(), join_handle_2.as_raw_ptr());

    loop {
        join_handle_1.print_canaries();
        // join_handle_2.print_canaries();

        unsafe {
            k_sleep_1s();
        }
    }
}
