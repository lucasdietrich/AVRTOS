use core::fmt::Write;

use avrtos_sys as ll;

pub struct Serial;

impl Write for Serial {
    fn write_str(&mut self, s: &str) -> core::fmt::Result {
        unsafe {
            ll::serial_send(s.as_ptr() as *const i8, s.len() as u16);
        }
        Ok(())
    }
}
