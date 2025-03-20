#![no_std]
#![feature(try_with_capacity)]

pub use avrtos_sys as sys;

pub use arduino_hal;

pub mod broken_stuff;
pub mod critical_section;
pub mod duration;
pub mod error;
pub mod kernel;
pub mod mutex;
pub mod panic;
pub mod serial;
pub mod signal;
pub mod stdio;
pub mod thread;
pub mod traits;

#[cfg(feature = "alloc")]
pub mod kalloc;

#[cfg(feature = "alloc")]
extern crate alloc;

const THREAD_STATE_POS: u8 = 0;
const THREAD_SCHED_LOCKED_POS: u8 = 2;
const THREAD_PRIO_POS: u8 = 3;
const THREAD_TIMER_EXPIRED_POS: u8 = 5;
const THREAD_PEND_CANCELED_POS: u8 = 6;
const THREAD_WAKEUP_SCHED_POS: u8 = 7;

const THREAD_PRIO_COOP: u8 = 2 << THREAD_PRIO_POS;
const THREAD_PRIO_PREEMPT: u8 = 0 << THREAD_PRIO_POS;
