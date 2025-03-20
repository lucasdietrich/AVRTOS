#![no_std]
#![no_main]

extern crate alloc;

use alloc::boxed::Box;
use alloc::vec::Vec;
use avrtos::kernel::{Kernel, KernelParams};
use avrtos::{arduino_hal, println};

const CAPACITY: u16 = 8;

fn print_heap_stats() {
    let (total, used, free) = avrtos::kalloc::heap_stats();
    println!(
        "Heap stats: total: {}, used: {}, free: {}",
        total, used, free
    );
}

#[arduino_hal::entry]
fn main() -> ! {
    let _kernel = Kernel::init_with_params(KernelParams::default()).unwrap();

    println!("Hello, world!");

    let mut v: Vec<u16> = Vec::with_capacity(CAPACITY as usize);

    for i in 0..CAPACITY {
        v.push(i);
    }

    for i in v.iter() {
        println!("{}", i);
    }

    print_heap_stats();

    struct Foo {
        a: u32,
        b: u32,
    }

    let foo = Foo { a: 42, b: 43 };
    let boxed = Box::new(foo);
    println!("Boxed: a: {} b: {}", boxed.a, boxed.b);

    print_heap_stats();

    loop {}
}
