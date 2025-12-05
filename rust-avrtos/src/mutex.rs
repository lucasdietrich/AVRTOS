use core::cell::UnsafeCell;

use avrtos_sys as ll;

pub struct Mutex<T: Sized> {
    mutex: ll::k_mutex,
    data: UnsafeCell<T>,
}