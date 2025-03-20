use core::{
    cell::UnsafeCell,
    ffi::c_void,
    mem::{self},
    pin::Pin,
};

use alloc::{boxed::Box, vec::Vec};
use avrtos_sys::{self as ll};

use crate::{print, println, THREAD_PRIO_COOP, THREAD_PRIO_PREEMPT};

pub struct Stats {
    stack_size: u16,
    stack_usage: u16,
}

pub struct JoinHandle {
    thread: Pin<Box<Thread>>,
}

impl JoinHandle {
    pub fn join(self) {}

    pub fn get_thread_ptr(&mut self) -> *mut ll::k_thread {
        self.thread.inner.get()
    }

    pub fn print_canaries(&mut self) {
        unsafe {
            ll::k_print_stack_canaries(self.get_thread_ptr());
        }
    }

    pub fn stats(&self) -> Stats {
        todo!()
    }
}

// static mut STACK: [u8; 1024] = [0; 1024];

pub enum Priority {
    Preemptive,
    Cooperative,
}

struct Thread {
    inner: UnsafeCell<ll::k_thread>,
}

impl Thread {
    fn new(
        stack_size: u16,
        prio: Priority,
        symbol: u8,
        func: Box<dyn FnOnce()>,
    ) -> Option<Pin<Box<Thread>>> {
        let prio = match prio {
            Priority::Preemptive => THREAD_PRIO_COOP,
            Priority::Cooperative => THREAD_PRIO_PREEMPT,
        };

        // Allocate stack
        let mut stack: Vec<u8> = Vec::try_with_capacity(stack_size as usize).ok()?;
        let stack_ptr = stack.as_mut_ptr(); // TODO What is the lifetime of this ptr ??

        let func_ptr = Box::into_raw(Box::new(func)); // TODO lifetime ??

        // Rust entry
        unsafe extern "C" fn entry_wrapper(func: *mut c_void) {
            // The heck ??
            let boxed_func = Box::from_raw(func as *mut Box<dyn FnOnce()>);
            let ret = boxed_func(); // TODO how to get a return value ??
        }

        unsafe {
            // thread_ptr should not move in memory at all costs !!!!!
            // otherwise this would invalidate all doubly linked nodes tied to it
            // (i.e. objects waitqueue and kernel whole runqueue)
            let thread: Pin<Box<Thread>> = Box::pin(mem::zeroed());
            let thread_ptr = thread.inner.get();

            let result = ll::k_thread_create(
                thread_ptr,
                Some(entry_wrapper),
                stack_ptr as *mut c_void,
                stack_size,
                prio,
                func_ptr as *mut c_void,
                symbol as i8,
            );
            if result != 0 {
                return None;
            }

            let result = ll::k_thread_start(thread_ptr);
            if result != 0 {
                return None;
            }

            Some(thread)
        }
    }
}

// TODO add return value
pub fn spawn<F>(stack_size: u16, prio: Priority, symbol: u8, func: F) -> Option<JoinHandle>
where
    F: FnOnce(),
    F: Send,
    F: 'static,
{
    // Box the closure
    let main = Box::new(move || {
        func();
    });

    let thread = Thread::new(stack_size, prio, symbol, main)?;

    Some(JoinHandle { thread })
}
