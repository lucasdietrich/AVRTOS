use avrtos_sys::{k_sleep, k_timeout_t};

pub fn sleep_broken() {
    // Doing this is highly BROKEN and I don't know why !!!
    let timeout = k_timeout_t { value: 0 };
    unsafe { k_sleep(timeout) }
}
