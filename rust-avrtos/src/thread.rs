use core::{ffi::c_void, mem::MaybeUninit, ptr::null_mut};

use alloc::{boxed::Box, vec::Vec};
use avrtos_sys::{self as ll, k_thread, k_thread_start};

use crate::{print, println, THREAD_PRIO_COOP, THREAD_PRIO_PREEMPT};

pub struct Stats {
    stack_size: u16,
    stack_usage: u16,
}

pub struct JoinHandle {
    thread: Thread,
}

impl JoinHandle {
    pub fn join(self) {}

    pub fn as_raw_ptr(&mut self) -> *mut k_thread {
        &raw mut self.thread.inner
    }

    pub fn print_canaries(&mut self)  {
       unsafe {
            ll::k_print_stack_canaries(self.as_raw_ptr());
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
    inner: k_thread,
}

impl Thread {
    fn new(
        stack_size: u16,
        prio: Priority,
        symbol: u8,
        func: Box<dyn FnOnce()>,
    ) -> Option<Thread> {
        let prio = match prio {
            Priority::Preemptive => THREAD_PRIO_COOP,
            Priority::Cooperative => THREAD_PRIO_PREEMPT,
        };

        // Allocate stack
        let mut stack: Vec<u8> = Vec::try_with_capacity(stack_size as usize).ok()?;
        let stack_ptr = stack.as_mut_ptr(); // TODO What is the lifetime of this ptr ??

        let func_ptr = Box::into_raw(Box::new(func)); // TODO lifetime ??

        // Rust entry
        unsafe extern "C" fn entry_wrapper(main: *mut c_void) {
            println!("entry_wrapper main: {:?}", main);

            // The heck
            let boxed_func = Box::from_raw(main as *mut Box<dyn FnOnce()>);
            boxed_func()
        }

        let thread = unsafe {
            // Where is this allocated ?? On stack, this should be allocated elseware
            let mut thread_storage = MaybeUninit::<ll::k_thread>::uninit();

            let result = ll::k_thread_create(
                thread_storage.as_mut_ptr(),
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

            let result = k_thread_start(thread_storage.as_mut_ptr());
            if result != 0 {
                return None;
            }

            thread_storage.assume_init()
        };

        Some(Thread { inner: thread })
    }
}

// TODO add return value
pub fn spawn<F>(stack_size: u16, prio: Priority, symbol: u8, func: F) -> Option<JoinHandle>
where
    F: FnOnce(),
    F: Send,
    F: 'static,
{
    // Create entry function
    let main = Box::new(move || {
        println!("main");
        func();
    });

    let thread = Thread::new(stack_size, prio, symbol, main)?;

    Some(JoinHandle { thread })
}
