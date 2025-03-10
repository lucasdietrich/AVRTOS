use core::{ffi::c_void, mem::MaybeUninit};

use avrtos_sys as kk;

use crate::{THREAD_PRIO_COOP, THREAD_PRIO_PREEMPT};

pub struct JoinHandle {
    thread: kk::k_thread,
}

static mut STACK: [u8; 1024] = [0; 1024];

pub enum Priority {
    Preemptive,
    Cooperative,
}

pub fn spawn<F, T>(context: &mut T, prio: Priority, symbol: char, func: F) -> Option<JoinHandle>
where
    F: FnOnce(&mut T) + Send + 'static,
{
    let prio =         match prio {
        Priority::Preemptive => THREAD_PRIO_COOP,
        Priority::Cooperative => THREAD_PRIO_PREEMPT,
    };

    let thread = unsafe {

        unsafe extern "C" fn entry_wrapper(arg: *mut c_void) {
            let func = arg as *mut dyn FnOnce();
            let func = unsafe { &mut *func };
            func();
        }

        let main = move || {
            func(context);
        };

        let mut thread_storage = MaybeUninit::<kk::k_thread>::uninit();

        let result = kk::k_thread_create(
            thread_storage.as_mut_ptr(),
            Some(entry_wrapper),
            &mut STACK as *mut u8 as *mut c_void,
            STACK.len() as u16,
            prio,
            context as *mut _ as *mut c_void,
            symbol as i8,
        );

        if result != 0 {
            return None;
        }

        thread_storage.assume_init()
    };

    Some(JoinHandle { thread })
}
