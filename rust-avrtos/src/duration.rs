use avrtos_sys::{k_ticks_t, k_timeout_t, z_ms_to_ticks};

pub struct Duration(u32);

impl Duration {
    pub const FOREVER: Duration = Duration(u32::MAX);
    pub const NO_DELAY: Duration = Duration(0);

    pub fn from_millis(millis: u32) -> Self {
        Duration(millis)
    }

    pub fn from_micros(micros: u32) -> Self {
        Duration(micros / 1000)
    }

    pub fn from_secs(secs: u32) -> Self {
        Duration(secs * 1000)
    }

    pub fn as_millis(&self) -> u32 {
        self.0
    }

    pub fn as_ticks(&self) -> k_ticks_t {
        unsafe {
            z_ms_to_ticks(self.0)
        }
    }
}

// TODO broken !!!!
impl Into<k_timeout_t> for Duration {
    fn into(self) -> k_timeout_t {
       self.as_ticks()
    }
}
