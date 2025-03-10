#![no_std]

pub use avrtos_sys as sys;

pub use arduino_hal;

pub mod kernel;
pub mod panic;
pub mod serial;
pub mod stdio;
pub mod thread;

const THREAD_STATE_POS: u8 = 0;
const THREAD_SCHED_LOCKED_POS: u8 = 2;
const THREAD_PRIO_POS: u8 = 3;
const THREAD_TIMER_EXPIRED_POS: u8 = 5;
const THREAD_PEND_CANCELED_POS: u8 = 6;
const THREAD_WAKEUP_SCHED_POS: u8 = 7;

const THREAD_PRIO_COOP: u8 = 2 << THREAD_PRIO_POS;
const THREAD_PRIO_PREEMPT: u8 = 0 << THREAD_PRIO_POS;