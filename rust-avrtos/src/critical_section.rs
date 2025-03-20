use avrtos_sys::{z_irq_lock, z_irq_unlock};

pub struct Cs {
    key: u8,

    // From
    // https://github.com/rust-embedded/critical-section/blob/main/src/lib.rs
    _not_send_nor_sync: core::marker::PhantomData<*mut ()>,
}

impl Cs {
    pub unsafe fn new() -> Self {
        let key = z_irq_lock();
        Self {
            key,
            _not_send_nor_sync: core::marker::PhantomData,
        }
    }
}

impl Drop for Cs {
    fn drop(&mut self) {
        unsafe {
            z_irq_unlock(self.key);
        }
    }
}

#[inline(always)]
pub fn critical_section<F, R>(f: F) -> R
where
    F: FnOnce() -> R,
{
    let _cs = unsafe { Cs::new() };
    f()
}
