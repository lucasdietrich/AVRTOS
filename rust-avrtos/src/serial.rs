use core::fmt::Write;

use avrtos_sys::serial_send;

pub struct Serial;

impl Write for Serial {
    fn write_str(&mut self, s: &str) -> core::fmt::Result {
        unsafe {
            serial_send(s.as_ptr() as *const i8, s.len() as u16);
        }
        Ok(())
    }
}
