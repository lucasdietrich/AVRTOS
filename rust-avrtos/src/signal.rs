use core::{cell::UnsafeCell, mem, pin::Pin};

use crate::{duration::Duration, error::OsErr, traits::SyncPrimitive};
use alloc::boxed::Box;
use avrtos_sys as ll;

pub struct Signal {
    inner: Pin<Box<UnsafeCell<ll::k_signal>>>,
}

impl Signal {
    pub fn new() -> Result<Self, OsErr> {
        unsafe {
            let signal_storage: Pin<Box<UnsafeCell<ll::k_signal>>> = Box::pin(mem::zeroed());

            let signal = ll::k_signal_init(signal_storage.get());

            match signal {
                0 => Ok(Self {
                    inner: signal_storage,
                }),
                errno => Err(OsErr::from_code(errno).unwrap()),
            }
        }
    }

    fn get_ptr(&self) -> *mut ll::k_signal {
        self.inner.get()
    }

    pub fn reset(&self) {
        unsafe {
            let signal_ptr = self.get_ptr();

            (*signal_ptr).flags = 0;
        }
    }
}

impl SyncPrimitive for Signal {
    type Swap = u8;

    fn acquire(&self, duration: Duration) -> Result<Self::Swap, OsErr> {
        unsafe {
            let ret = ll::k_poll_signal(self.get_ptr(), duration.into());

            match ret {
                0 => Ok(0),
                errno => Err(OsErr::from_code(errno).unwrap()),
            }
        }
    }

    fn release(&self, released: Self::Swap) -> Result<(), Self::Swap> {
        unsafe {
            let ret = ll::k_signal_raise(self.get_ptr(), released);

            match ret {
                0 => Ok(()),
                _ => Err(released),
            }
        }
    }
}
