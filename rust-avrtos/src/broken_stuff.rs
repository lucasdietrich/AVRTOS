use avrtos_sys::{k_sleep, k_ticks_t};

#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct k_timeout_BROKEN_t {
    pub value: k_ticks_t,
}

pub fn sleep_broken() {
    // Doing this is highly BROKEN and I don't know why !!!
    let timeout = k_timeout_BROKEN_t { value: 0 };
    
    // DO something with timeout
}
